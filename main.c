#include "ymodem.h"
#include "common.h"

volatile uint32_t regaddr = 0x000000,sector = 0;
static volatile uint32_t current_millisec = 700;
int32_t i=0, length=0;
extern int32_t tmplen;
tSPIFlashState p,*SPIState = &p;

volatile uint8_t tx_flag = 0;
uint8_t timeout_expire = 0,*buffer = 0,file_size[10];

extern int xmodemTransmit();

/***********************************************************************************************************************************************
* Function Name	: Timer0_A_IntHandler
* Purpose		: Handles Timer0_A Interrupt
* Inputs		: None
* Outputs		: None
* Return		: None
* Date Created	: 21st August 2014
* Modified		: None
*******************************************************************************************************************************************************/
void Timer0_A_IntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = TimerIntStatus(TIMER0_BASE,true);
	current_millisec--;
	if(current_millisec == 0)
	{
		timeout_expire = 1;
	}
	TimerIntClear(TIMER0_BASE,ui32Status);
}

/***********************************************************************************************************************************************
* Function Name	: SPIFlashHandler
* Purpose		: Handles SSI3 Rx and Tx Interrupt
* Inputs		: None
* Outputs		: None
* Return		: None
* Date Created	: 21st August 2014
* Modified		: None
*******************************************************************************************************************************************************/
void SPIFlashHandler(void)
{
	uint32_t ui32Status;
	ui32Status = SSIIntStatus(SSI3_BASE, true);

	if (ui32Status & SSI_TXFF)
	{
		//wait until transfer finishes
		while (SPIFlashIntHandler(SPIState) != SPI_FLASH_DONE)
		{

		}
		//disable RX interrupt
		SSIIntDisable(SSI3_BASE, SSI_RXFF | SSI_RXTO);
	}

	else if (ui32Status & (SSI_RXFF | SSI_RXTO))
	{
		//wait until transfer finishes
		while (SPIFlashIntHandler(SPIState) != SPI_FLASH_DONE)
		{

		}
		//disable TX interrupt
		SSIIntDisable(SSI3_BASE, SSI_TXFF);
	}
	else
	{

	}
	SSIIntClear(SSI3_BASE, ui32Status);
}

/***********************************************************************************************************************************************
* Function Name	: UARTIntHandler
* Purpose		: Handles UART0 Rx and Tx Interrupt
* Inputs		: None
* Outputs		: None
* Return		: None
* Date Created	: 21st August 2014
* Modified		: None
*******************************************************************************************************************************************************/
void UARTIntHandler(void)
{
	uint32_t ui32Status;
	int8_t rx_byte;

	ui32Status = UARTIntStatus(UART0_BASE, false);						// Get the interrrupt status.

	if(((ui32Status & UART_INT_RX) || (ui32Status & UART_INT_RT)) || ((ui32Status & UART_INT_RX) && (ui32Status & UART_INT_RT)))
	{
		current_millisec = 700;
		rx_byte = (int8_t)UARTCharGetNonBlocking(UART0_BASE);

		Ymodem_Receive(rx_byte);
	}
	// Clear the asserted interrupts.
	UARTIntClear(UART0_BASE, ui32Status);

}
/***********************************************************************************************************************************************
* Function Name	: main
* Purpose		:
* Inputs		: None
* Outputs		: None
* Return		: None
* Date Created	: 21st August 2014
* Modified		: None
*******************************************************************************************************************************************************/
int main(void)
{
	uint8_t buff[BUFFER_SIZE+PACKET_HEADER],*src,mflg=0;
	uint32_t sysclk;
	int32_t val = 0;
	uint8_t pui8_M_ID[2];
	uint16_t pui16_D_ID[2];

	sysclk = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

	SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R0|SYSCTL_RCGCGPIO_R14|SYSCTL_RCGCGPIO_R5|SYSCTL_RCGCGPIO_R8|SYSCTL_RCGCGPIO_R6;
	SYSCTL_RCGCSSI_R = SYSCTL_RCGCSSI_R3;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	UARTEnable(UART0_BASE);
	UARTFIFOEnable(UART0_BASE);

	UARTConfigSetExpClk(UART0_BASE, sysclk, 19200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT|UART_INT_TX);
	IntEnable(INT_UART0);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	GPIOPinConfigure(GPIO_PQ0_SSI3CLK);
	GPIOPinConfigure(GPIO_PQ1_SSI3FSS);
	GPIOPinConfigure(GPIO_PQ2_SSI3XDAT0);
	GPIOPinConfigure(GPIO_PF0_SSI3XDAT1);

	GPIOPinTypeSSI(GPIO_PORTQ_BASE, GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);
	GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0);
	SSIEnable(SSI3_BASE);

	IntEnable(INT_SSI3);

	SPIFlashInit(SSI3_BASE, sysclk,12000000);

	SPIFlashReadID(SSI3_BASE, pui8_M_ID, pui16_D_ID);
	Timer0_A_Init();
	IntMasterEnable();

	while(1)
	{
		if(tx_flag == 0)
		{
			val = Ymodem_Write(&buffer);
			if(val == 1)
			{
				SPIFlashWriteEnable(SSI3_BASE);
				SPIFlashSectorErase(SSI3_BASE,(regaddr+sector));								//Erases Sector of 4KB
				while(SPIFlashReadStatus(SSI3_BASE) != 0);
				SPIFlashWriteEnable(SSI3_BASE);
				SPIFlashPageProgramNonBlocking(SPIState,SSI3_BASE,regaddr,(const uint8_t *)buffer,BUFFER_SIZE,false,0);
				regaddr = (regaddr+BUFFER_SIZE);
				sector = ((sector+4096)-BUFFER_SIZE); 				//Adjusting Erase address to be at 4K boundary
				val = 0;
				if(tx_flag == 1)
				{
					mflg = 0;
					regaddr = 0;
				}
			}
			else if(val == -2)
			{
				UARTCharPutNonBlocking(UART3_BASE,NAK);
			}
			if(timeout_expire == 1)
			{
				if ((UART0_FR_R & 0x010) == 0)
				{
					timeout_expire = 0;
					current_millisec = 0;
				}
				else
				{
					current_millisec = 700;
					timeout_expire = 0;
					regaddr = 0;
					Ymodem_Reset();
					UARTCharPutNonBlocking(UART0_BASE,CRC16);
				}
			}
			else
			{

			}
		}
		if(tx_flag == 1)
		{
			while(SPIFlashReadStatus(SSI3_BASE) != 0);
			SPIFlashRead(SSI3_BASE,regaddr,&buff[PACKET_HEADER],BUFFER_SIZE);
			while(SPIFlashReadStatus(SSI3_BASE) != 0);
			if(mflg == 0)
			{
				src = &buff[PACKET_HEADER];
				while(*src != 0x00)
				{
					src++;
				}
				while(*src != ' ')
				{
					file_size[i] = *src;
					if(ISVALIDDEC(file_size[i]))
					{
						length = length*10+CONVERTDEC(file_size[i]);
					}
					i++;
					src++;
				}
				tmplen = length;
				length = 0;
				sector = 0;
				mflg = 1;
#if (BUFFER_SIZE == 64)
	regaddr = 0x40;
#elif (BUFFER_SIZE 	== 128)
	regaddr = 0x80;
#elif (BUFFER_SIZE 	== 256)
	regaddr = 0x100;
#endif
			}
			else if(mflg == 1)
			{
				xmodemTransmit(buff);
				regaddr += BUFFER_SIZE;
			}
			else
			{

			}
		}
	}

}
