#include <stdio.h>
#include "uart.h"
#include "logger.h"

pserial_port uart_open(const char* szPortName)
{
    LOG_INFO("Open serial port: %s", szPortName);
    return NULL;
}

void uart_set_speed(pserial_port sp, uint32_t speed)
{
    LOG_INFO("Set serial port speed: %d", speed);
}

int uart_send(pserial_port sp, const uint8_t *pbtTx, const size_t szTx, int timeout)
{
    return 0;
}

int uart_receive(pserial_port sp, uint8_t *pbtRx, const size_t szRx, void *abort_p, int timeout)
{
    return 0;
}


