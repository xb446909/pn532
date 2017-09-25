#ifndef __UART_H
#define __UART_H

typedef struct serial_port_unix {
  int 			    fd; 			// Serial port file descriptor
  struct termios 	termios_backup; 	// Terminal info before using the port
  struct termios 	termios_new; 		// Terminal info during the transaction
}serial_port, *pserial_port;

pserial_port uart_open(const char* szPortName);


#endif /*__UART_H*/
