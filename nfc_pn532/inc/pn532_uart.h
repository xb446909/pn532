#ifndef __PN532_UART_H
#define __PN532_UART_H

#define DEFAULT_UART_SPEED    115200  

#include "uart.h"

extern pserial_port pn532_uart_open(const char* connstring);
extern int pn532_check_communication(pserial_port sp);
extern int pn532_transceive(pserial_port sp, const uint8_t *pbtTx, const size_t szTx, uint8_t *pbtRx, const size_t szRxLen, int timeout);

#endif /*__PN532_UART_H*/
