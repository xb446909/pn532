#ifndef __UART_H
#define __UART_H

#include <termios.h>
#include <unistd.h>
#include <stdint.h>

typedef struct serial_port_unix {
  int 			    fd; 			// Serial port file descriptor
  struct termios 	termios_backup; 	// Terminal info before using the port
  struct termios 	termios_new; 		// Terminal info during the transaction
}serial_port, *pserial_port;

extern pserial_port uart_open(const char* szPortName);
extern void uart_set_speed(pserial_port sp, uint32_t speed);
extern int uart_send(pserial_port sp, const uint8_t *pbtTx, const size_t szTx, int timeout);
extern int uart_receive(pserial_port sp, uint8_t *pbtRx, const size_t szRx, void *abort_p, int timeout);


#endif /*__UART_H*/
