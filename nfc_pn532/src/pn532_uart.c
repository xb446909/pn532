#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pn532_uart.h"
#include "uart.h"

pserial_port pn532_uart_open(const char* connstring)
{
    pserial_port sp = NULL;
    char* pos = strrchr(connstring, ':');
    if (pos != NULL)
    {
        *pos = '\0';
        pos++;
        sp = uart_open(connstring);
        uart_set_speed(sp, atoi(pos));
    }
    else
    {
        sp = uart_open(connstring);
        uart_set_speed(sp, DEFAULT_UART_SPEED);
    }
    if (sp != NULL)
    {
        if (pn532_check_communication(sp) != 0)
        {
            
        }
    }
    return sp;
}

int pn532_check_communication(pserial_port sp)
{
    const uint8_t abtCmd[] = { 0x00, 0x00, 'l', 'i', 'b', 'n', 'f', 'c' };
    const uint8_t abtExpectedRx[] = { 0x00, 'l', 'i', 'b', 'n', 'f', 'c' };
    uint8_t abtRx[sizeof(abtExpectedRx)];
    size_t szRx = sizeof(abtRx);
    int res = 0;
    
    if ((res = pn532_transceive(sp, abtCmd, sizeof(abtCmd), abtRx, szRx, 500)) < 0)
        return res;
    szRx = (size_t) res;
    if ((sizeof(abtExpectedRx) == szRx) && 
            (0 == memcmp(abtRx, abtExpectedRx, sizeof(abtExpectedRx))))
        return 0;
    
    return -1;
}


int pn532_transceive(pserial_port sp, const uint8_t *pbtTx, const size_t szTx, uint8_t *pbtRx, const size_t szRxLen, int timeout)
{
    return 0;
}
