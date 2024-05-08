#include "common.h"
#include "ymodem.h"

/*Some XMODEM Defines*/
#define CAN  0x18
#define CTRLZ 0x1A
#define DLY_1S 1000
#define MAXRETRANS 25

extern volatile uint8_t tx_flag;
uint8_t packetno = 1,c;

extern volatile uint32_t regaddr;
extern uint32_t SerialKeyPressed(uint8_t *, int32_t);
int32_t tmplen=0,len = 0,retry,bufsz, crc = -1;

extern void SerialPutChar(uint8_t );

/***********************************************************************************************************************************************
* Function Name	: crc16_ccitt
* Purpose		: Calculates 16-bit CRC of the bytes inputted
* Inputs		: Address of the Array where data is stored and no. of bytes in array
* Outputs		: None
* Return		: 16-bit CRC calculated
* Date Created	: 21st August 2014
* Modified		: No
*******************************************************************************************************************************************************/
int crc16_ccitt(char *ptr, int count)
{
	int crc, i;
	crc = 0;
	while (--count >= 0)
	{
		crc = crc ^ (int)*ptr++ << 8;
		for (i = 0; i < 8; ++i)
		{
			if (crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc = crc << 1;
		}
	}
	return (crc & 0xFFFF);
}

/***********************************************************************************************************************************************
* Function Name	: XmodemTransmit
* Purpose		: Transmitts data to UART0 according to Xmodem Protocol into packets of BUFFER_SIZE
* Inputs		: Address of the Array where data is stored
* Outputs		: None
* Return		:  0 for successful packet transmission
* 				  -1 for unsuccessful packet transmission
* 				  -1 for error in transmission
* Date Created	: 21st August 2014
* Modified		: No
*******************************************************************************************************************************************************/
int xmodemTransmit(uint8_t *buffer)
{
	unsigned short ccrc = 1;
	int32_t i;
	uint8_t *buff_ptr;

	bufsz = BUFFER_SIZE;
	*buffer = SOH;
	*(buffer+1) = packetno;
	*(buffer+2) = ~packetno;
//	memset((buffer+3),0,BUFFER_SIZE);
//	memcpy(&xbuff[3],buffer,BUFFER_SIZE);
	if(tmplen > 0)
	{
		if (tmplen == 0)
		{
			SerialPutChar(CTRLZ);
		}
		else
		{
			if (tmplen < bufsz)
				*(buffer+(PACKET_HEADER+tmplen)) = CTRLZ;
		}

		for (retry = 0; retry < MAXRETRANS; ++retry)
		{
			buff_ptr = buffer;
			for (i = 0; i < (BUFFER_SIZE+PACKET_HEADER); ++i)
			{
				SerialPutChar(*buff_ptr);
				buff_ptr++;
			}
			if (crc)
			{
				ccrc = crc16_ccitt((char *)(buffer+PACKET_HEADER), BUFFER_SIZE);
				SerialPutChar((ccrc>>8) & 0xFF);
				SerialPutChar(ccrc & 0xFF);
			}
			else
			{
				unsigned char ccks = 0;
				for (i = 3; i < bufsz+PACKET_HEADER; ++i)
				{
					ccks += *(buffer+i);
				}
				*(buffer+(bufsz+PACKET_HEADER)) = ccks;
			}
			if (SerialKeyPressed(&c,0x100000))
			{
				switch (c)
				{
				case ACK:
					++packetno;
					tmplen = (tmplen-BUFFER_SIZE);
					len += bufsz;
					return 0;
				case CAN:
					if(SerialKeyPressed(&c,0x100000) && c== CAN)
					{
						SerialPutChar(ACK);

						return -1; /* canceled by remote */
					}
					break;
				case NAK:
				default:
					break;
				}
			}
		}
		SerialPutChar(CAN);
		SerialPutChar(CAN);
		SerialPutChar(CAN);

		return -4; /* xmit error */
	}
	else
	{
		for (retry = 0; retry < 10; ++retry)
		{
			SerialPutChar(EOT);
			if (SerialKeyPressed(&c,0x100000) && c == ACK)
			{
				tx_flag = 0;
				regaddr = 0;
				packetno = 1;
				tmplen = 0;
				len = 0;
				break;
			}
		}
		return (c == ACK)?len:-5;
	}
}
