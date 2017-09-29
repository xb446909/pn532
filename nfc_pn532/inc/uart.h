#ifndef __UART_H
#define __UART_H

#include <termios.h>
#include <unistd.h>
#include <stdint.h>

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

typedef enum enum_bool{
	false, 
	true
}bool;

typedef struct _tag_serial_port_unix {
  int 			    fd; 			// Serial port file descriptor
  struct termios 	termios_backup; 	// Terminal info before using the port
  struct termios 	termios_new; 		// Terminal info during the transaction
}serial_port, *pserial_port;

#  define CCLAIMED 0x80000000

pserial_port uart_open(const char* szPortName);
void uart_close(pserial_port sp);
void uart_set_speed(pserial_port sp, uint32_t speed);
int uart_send(pserial_port sp, const uint8_t *pbtTx, const size_t szTx, int timeout);
int uart_receive(pserial_port sp, uint8_t *pbtRx, const size_t szRx, void *abort_p, int timeout);
void uart_flush_input(pserial_port sp);


#endif /*__UART_H*/
