/*
 * common.c
 *
 *  Created on: 8 Aug 2014
 *      Author: ETPL3
 */

#include "common.h"

void Timer0_A_Init(void)
{
	SYSCTL_RCGCTIMER_R = SYSCTL_RCGCTIMER_R0;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER0_BASE, TIMER_A, 120000);			//To count 120x10^3 Clock Pulses in 1ms; For 1sec delay the value must be 120x10^6
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE,TIMER_A);
}

void serial_put_char(uint8_t c)
{
	UARTCharPutNonBlocking(UART0_BASE,c);
}
uint32_t SerialKeyPressed(uint8_t *key, int32_t timeout)
{
	*key = UARTCharGet(UART0_BASE);
	if(*key)
		return 1;
	else return 0;
}

void SerialPutChar(uint8_t c)
{
  UARTCharPut(UART0_BASE, c);
  while ((UART0_FR_R & 0x080) == 0)
  {
  }
}
