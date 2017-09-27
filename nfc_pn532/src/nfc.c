#include "nfc.h"
#include "pn532_uart.h"

void nfc_open(const char * connstring)
{
	pn532_uart_open(connstring);
}

