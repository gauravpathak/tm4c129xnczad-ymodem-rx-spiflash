#include "ymodem.h"
#include "common.h"

enum ymodem_state
{
	file_name,
	data_packet,
	end_of_transmission
};

extern volatile uint32_t regaddr;

uint8_t rxd_byt,null_cnt=0,index=0,buff_flg = 0,full=0,full1=0;
static uint8_t data[BUFFER_SIZE],data1[BUFFER_SIZE],*tmp_buff,inc,dec;

static uint32_t file_size[FILE_SIZE_LENGTH],length=0,tmplen=0,cntr = 0;
static volatile uint32_t count=0, sw_buff=0,len = 0,present_state = file_name;
static volatile int32_t packet_no = 0,no_bytes=0;
extern volatile uint8_t tx_flag;

/***********************************************************************************************************************************************
* Function Name	: Ymodem_Receive
* Purpose		: Receives data according to YMODEM protocol byte by byte as per UART Rx Interrupt and fills the respective buffer
* Inputs		: Data byte received from UART0
* Outputs		: None
* Return		: None
* Date Created	: 21st August 2014
* Modified		: No
*******************************************************************************************************************************************************/
void Ymodem_Receive(uint8_t rxd_byt)
{
	switch (rxd_byt)
	{
	case CA:

		break;

	case STX:
		break;

	case EOT:
		if(len > tmplen)
		{
			present_state = end_of_transmission;
		}
		break;

	default:
		break;
	}

	switch(present_state)
	{
	case file_name:
		if(rxd_byt == 0x00)
		{
			null_cnt++;
		}
		else if(rxd_byt == 0xFF)
		{
			dec = rxd_byt;
		}
		if(null_cnt >= 1 && null_cnt <= 5)
		{
			if(null_cnt == 1 && rxd_byt == 0x00)
			{
				inc = rxd_byt;
			}
			else
			{
				if(sw_buff == 0)
				{
					data[cntr] = rxd_byt;
					cntr++;
					if(cntr == BUFFER_SIZE)
					{
						buff_flg = 0xAA;
						cntr = 0;
						sw_buff = 1;
						full = 1;
					}
				}
			}
		}
		else if(null_cnt >= 5)
		{
			buff_flg = 0xAA;
			full = 1;
			tmp_buff = data+PACKET_HEADER;
			if ((inc != (dec ^ 0xff) & 0xff))
			{
				present_state = file_name;
				count = 0;
				serial_put_char(ACK);
			}
			while(*tmp_buff++ != 0x00);
			while(*tmp_buff != ' ')
			{
				file_size[index] = *tmp_buff;
				if(ISVALIDDEC(file_size[index]))
				{
					length = length*10+CONVERTDEC(file_size[index]);
				}
				index++;
				tmp_buff++;
			}
			tmplen = length;
			cntr = 0;
			null_cnt = 0;
			present_state = data_packet;
			serial_put_char(ACK);
			serial_put_char(CRC16);
		}
		break;

	case data_packet:
		if(rxd_byt == STX && count == 0)
		{
			count++;
		}
		else if(count >= 1 && count <= 261)
		{
			if(count == PACKET_SEQNO_INDEX)
			{
				inc = rxd_byt;
				count++;
			}
			else if(count == PACKET_SEQNO_COMP_INDEX)
			{
				dec = rxd_byt;
				count++;
			}
			else if(count >= PACKET_HEADER && count <= 261)
			{
				if(no_bytes < PACKET_1K_SIZE)
				{
					if(sw_buff == 0)
					{
						data[cntr] = rxd_byt;
						cntr++;
						if(cntr == BUFFER_SIZE)
						{
							cntr = 0;
							count = 3;
							sw_buff = 1;
							full = 1;
						}
					}
					else
					{
						data1[cntr] = rxd_byt;
						cntr++;
						if(cntr == BUFFER_SIZE)
						{
							cntr = 0;
							count = 3;
							sw_buff = 0;
							full1 = 1;
						}
					}
				}
				if(no_bytes == PACKET_1K_SIZE)
				{
					packet_no++;
					if ((inc != (dec ^ 0xff) & 0xff))
					{
						present_state = file_name;
						count = 0;
						serial_put_char(ACK);
					}
					if ((inc & 0xff) != (packet_no & 0xff))
					{
						present_state = file_name;
						count = 0;
						serial_put_char(NAK);
					}
					serial_put_char(ACK);
					count = 0;
					no_bytes = -1;
				}
				len++;
				no_bytes++;
			}
		}
		break;

	case end_of_transmission:
		count = 0;
		len = 0;
		length = 0;
		if(sw_buff == 0)
		{
			full1 = 1;
		}
		else
		{
			full = 1;
		}
		if(rxd_byt == STX)
		{
			null_cnt = 0;
			present_state = file_name;
		}
		index = full1 = full = 0;
		no_bytes = 0;
		packet_no = 0;
		serial_put_char(ACK);
		serial_put_char(ACK);
		tx_flag = 1;
		break;

	default:
		break;
	}
}

/***********************************************************************************************************************************************
* Function Name	: Ymodem_Write
* Purpose		: Handles buffer switching for Recieving data from UART0 and writing data on SPI Flash
* Inputs		: Address of the pointer pointing to Array where data is being stored
* Outputs		: Updates the Array pointed to by the pointer provided as argument
* Return		: 1 if buffer 1 or buffer2 is getting accesed; -2 if both buffers got full at same time
* Date Created	: 21st August 2014
* Modified		: No
*******************************************************************************************************************************************************/
int32_t Ymodem_Write(uint8_t **buff)
{
	int32_t return_val = 0;
	if(full == 1 && buff_flg == 0xAA)
	{
		*buff = &data[0];
		full = 0;
		return_val = 1;
	}
	else if(full1 == 1 && buff_flg == 0xAA)
	{
		*buff = &data1[0];
		full1 = 0;
		return_val = 1;
	}
	else if(full == 1 && full1 == 1)
	{
		return_val = -2;;
	}
	return return_val;
}

/***********************************************************************************************************************************************
* Function Name	: Ymodem_Reset
* Purpose		: Resets Ymodem State machine to its original state
* Inputs		: None
* Outputs		: None
* Return		: None
* Date Created	: 21st August 2014
* Modified		: No
*******************************************************************************************************************************************************/
void Ymodem_Reset(void)
{
	count = 0;
	sw_buff = 0;
	buff_flg = 0;
	len = 0;
	tmplen = 0;
	index = full1 = full = 0;
	no_bytes = 0;
	null_cnt = 0;
	packet_no = 0;
	length = 0;
	present_state = file_name;
}

