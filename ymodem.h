/*******************************************************************************************************************************************
*
* Project 		: Firmware Upgrade using YMODEM and XMODEM protocol
* Module 		: YMODEM library
* File 			: ymodem.h
* Description 	: This module contains the functions and defines for YMODEM Protocol
* Client 		:
* Author 		:
* Version 		: 0.1
* Date 			: 21st August 2014
* Company 		: Expersis Technologies Pvt. Ltd.
*******************************************************************************************************************************************/

#ifndef YMODEM_H_
#define YMODEM_H_
#include "common.h"

#define ISVALIDDEC(c)  ((c >= '0') && (c <= '9'))
#define CONVERTDEC(c)  (c - '0')

#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)
#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_256_SIZE         (256)
#define PACKET_1K_SIZE          (1024)
#define FILE_NAME_LENGTH		(256)
#define FILE_SIZE_LENGTH        (16)

#define BUFFER_SIZE				128

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  (0x61)  /* 'a' == 0x61, abort by user */

void Ymodem_Receive (uint8_t rxd_byt);
int32_t Ymodem_Write(uint8_t **);
void Ymodem_Reset(void);
#endif /* YMODEM_H_ */
