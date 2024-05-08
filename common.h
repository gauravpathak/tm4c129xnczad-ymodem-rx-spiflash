/*******************************************************************************************************************************************
*
* Project 		: Firmware Upgrade using YMODEM and XMODEM protocol
* Module 		: Common file for include files
* File 			: commom.h
* Description 	: This module contain all the include headers required for project
* Client 		:
* Author 		:
* Version 		: 0.1
* Date 			: 21st August 2014
* Company 		: Expersis Technologies Pvt. Ltd.
*******************************************************************************************************************************************/

#ifndef COMMON_H_
#define COMMON_H_
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"

#include "driverlib/timer.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/ssi.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/udma.h"

#include "inc/tm4c129xnczad.h"
#include "utils/spi_flash.h"

void Timer0_A_Init(void);
void serial_put_char(uint8_t );

#endif /* COMMON_H_ */
