#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "uart.h"
#include "logger.h"
#include "nfc.h"

#include<fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>

pserial_port uart_open(const char* szPortName)
{
    LOG_INFO("Open serial port: %s", szPortName);
	pserial_port sp = (pserial_port)malloc(sizeof(serial_port));

	if (sp == 0)
		return NULL;

	sp->fd = open(szPortName, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (sp->fd == -1) {
		uart_close(sp);
		return NULL;
	}

	if (tcgetattr(sp->fd, &sp->termios_backup) == -1) {
		uart_close(sp);
		return NULL;
	}
	// Make sure the port is not claimed already
	if (sp->termios_backup.c_iflag & CCLAIMED) {
		uart_close(sp);
		return NULL;
	}
	// Copy the old terminal info struct
	sp->termios_new = sp->termios_backup;

	sp->termios_new.c_cflag = CS8 | CLOCAL | CREAD;
	sp->termios_new.c_iflag = CCLAIMED | IGNPAR;
	sp->termios_new.c_oflag = 0;
	sp->termios_new.c_lflag = 0;

	sp->termios_new.c_cc[VMIN] = 0;     // block until n bytes are received
	sp->termios_new.c_cc[VTIME] = 0;    // block until a timer expires (n * 100 mSec.)

	if (tcsetattr(sp->fd, TCSANOW, &sp->termios_new) == -1) 
	{
		uart_close(sp);
		return NULL;
	}

    return sp;
}

void uart_close(pserial_port sp)
{
	if (sp->fd > 0)
	{
		close(sp->fd);
	}
	free(sp);
}

void uart_set_speed(pserial_port sp, uint32_t speed)
{
    LOG_INFO("Set serial port speed: %d", speed);

	speed_t stPortSpeed = B9600;
	switch (speed) {
	case 9600:
		stPortSpeed = B9600;
		break;
	case 19200:
		stPortSpeed = B19200;
		break;
	case 38400:
		stPortSpeed = B38400;
		break;
#  ifdef B57600
	case 57600:
		stPortSpeed = B57600;
		break;
#  endif
#  ifdef B115200
	case 115200:
		stPortSpeed = B115200;
		break;
#  endif
#  ifdef B230400
	case 230400:
		stPortSpeed = B230400;
		break;
#  endif
#  ifdef B460800
	case 460800:
		stPortSpeed = B460800;
		break;
#  endif
	default:
		LOG_ERROR("Unable to set serial port speed to %d bauds. Speed value must be one of those defined in termios(3).",
			speed);
		return;
	};

	// Set port speed (Input and Output)
	cfsetispeed(&sp->termios_new, stPortSpeed);
	cfsetospeed(&sp->termios_new, stPortSpeed);
	if (tcsetattr(sp->fd, TCSADRAIN, &sp->termios_new) == -1) 
	{
		LOG_ERROR("Unable to apply new speed settings.");
	}

}

int uart_send(pserial_port sp, const uint8_t *pbtTx, const size_t szTx, int timeout)
{
	char szSend[2048];
	strcpy(szSend, "[SEND]");
	char szChar[8];
	memset(szChar, 0, 8);
	for (size_t i = 0; i < szTx; i++)
	{
		sprintf(szChar, " %02x ", pbtTx[i]);
		strcat(szSend, szChar);
	}
	LOG_INFO(szSend);

	if ((int)szTx == write(sp->fd, pbtTx, szTx))
		return 0;
	else
		return -1;
}

int uart_receive(pserial_port sp, uint8_t *pbtRx, const size_t szRx, void *abort_p, int timeout)
{
	int iAbortFd = abort_p ? *((int *)abort_p) : 0;
	int received_bytes_count = 0;
	int available_bytes_count = 0;
	const int expected_bytes_count = (int)szRx;
	int res;
	fd_set rfds;
	do {
	select:
		// Reset file descriptor
		FD_ZERO(&rfds);
		FD_SET(sp->fd, &rfds);

		if (iAbortFd) {
			FD_SET(iAbortFd, &rfds);
		}

		struct timeval timeout_tv;
		if (timeout > 0) {
			timeout_tv.tv_sec = (timeout / 1000);
			timeout_tv.tv_usec = ((timeout % 1000) * 1000);
		}

		res = select(MAX(sp->fd, iAbortFd) + 1, &rfds, NULL, NULL, timeout ? &timeout_tv : NULL);

		if ((res < 0) && (EINTR == errno)) {
			// The system call was interupted by a signal and a signal handler was
			// run.  Restart the interupted system call.
			goto select;
		}

		// Read error
		if (res < 0) {
			LOG_ERROR("Error: %s", strerror(errno));
			return NFC_EIO;
		}
		// Read time-out
		if (res == 0) {
			LOG_ERROR("Timeout!");
			return NFC_ETIMEOUT;
		}

		if (FD_ISSET(iAbortFd, &rfds)) {
			// Abort requested
			LOG_ERROR("Abort!");
			close(iAbortFd);
			return NFC_EOPABORTED;
		}

		// Retrieve the count of the incoming bytes
		res = ioctl(sp->fd, FIONREAD, &available_bytes_count);
		if (res != 0) {
			return NFC_EIO;
		}
		// There is something available, read the data
		res = read(sp->fd, pbtRx + received_bytes_count, MIN(available_bytes_count, (expected_bytes_count - received_bytes_count)));
		// Stop if the OS has some troubles reading the data
		if (res <= 0) {
			return NFC_EIO;
		}
		received_bytes_count += res;

	} while (expected_bytes_count > received_bytes_count);

	char szReceive[2048];
	strcpy(szReceive, "[RECEIVED]");
	char szChar[8];
	memset(szChar, 0, 8);
	for (size_t i = 0; i < szRx; i++)
	{
		sprintf(szChar, " %02x ", pbtRx[i]);
		strcat(szReceive, szChar);
	}
	LOG_INFO(szReceive);

	return NFC_SUCCESS;
}

void uart_flush_input(pserial_port sp)
{
	//msleep(50); // 50 ms

	// This line seems to produce absolutely no effect on my system (GNU/Linux 2.6.35)
	tcflush(sp->fd, TCIFLUSH);
	// So, I wrote this byte-eater
	// Retrieve the count of the incoming bytes
	int available_bytes_count = 0;
	int res;
	res = ioctl(sp->fd, FIONREAD, &available_bytes_count);
	if (res != 0) {
		return;
	}
	if (available_bytes_count == 0) {
		return;
	}
	char *rx = malloc(available_bytes_count);
	if (!rx) {
		perror("malloc");
		return;
	}
	// There is something available, read the data
	if (read(sp->fd, rx, available_bytes_count) < 0) {
		perror("uart read");
		free(rx);
		return;
	}
	LOG_DEBUG("%d bytes have eaten.", available_bytes_count);
	free(rx);
}


