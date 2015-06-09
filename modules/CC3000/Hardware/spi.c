/*****************************************************************************
*
*  spi.c - CC3000 Host Driver Implementation.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

//*****************************************************************************
//
//! \addtogroup link_buff_api
//! @{
//
//*****************************************************************************
#include "hci.h"
#include "spi.h"
#include "evnt_handler.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#define READ                    3
#define WRITE                   1

#define HI(value)               (((value) & 0xFF00) >> 8)
#define LO(value)               ((value) & 0x00FF)

#define ASSERT_CS()          (PORTD &= ~SPI_CS)
#define DEASSERT_CS()        (PORTD |= SPI_CS)

#define HEADERS_SIZE_EVNT       (SPI_HEADER_SIZE + 5)

#define SPI_HEADER_SIZE			(5)

#define 	eSPI_STATE_POWERUP 				 (0)
#define 	eSPI_STATE_INITIALIZED  		 (1)
#define 	eSPI_STATE_IDLE					 (2)
#define 	eSPI_STATE_WRITE_IRQ	   		 (3)
#define 	eSPI_STATE_WRITE_FIRST_PORTION   (4)
#define 	eSPI_STATE_WRITE_EOT			 (5)
#define 	eSPI_STATE_READ_IRQ				 (6)
#define 	eSPI_STATE_READ_FIRST_PORTION	 (7)
#define 	eSPI_STATE_READ_EOT				 (8)

typedef struct
{
	gcSpiHandleRx  SPIRxHandler;
	unsigned short usTxPacketLength;
	unsigned short usRxPacketLength;
	unsigned long  ulSpiState;
	unsigned char *pTxPacket;
	unsigned char *pRxPacket;
} tSpiInformation;

tSpiInformation sSpiInformation;

// buffer for 5 bytes of SPI HEADER
unsigned char tSpiReadHeader[] = {READ, 0, 0, 0, 0};

void SpiWriteDataSynchronous(unsigned char *data, unsigned short size);
void SpiWriteAsync(const unsigned char *data, unsigned short size);
void SpiPauseSpi(void);
void SpiResumeSpi(void);
void SSIContReadOperation(void);

// The magic number that resides at the end of the TX/RX buffer (1 byte after
// the allocated size) for the purpose of detection of the overrun. The location
// of the memory where the magic number resides shall never be written. In case
// it is written - the overrun occurred and either receive function or send
// function will stuck forever.
#define CC3000_BUFFER_MAGIC_NUMBER (0xDE)

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//#pragma is used for determine the memory location for a specific variable.                            ///
//__no_init is used to prevent the buffer initialization in order to prevent hardware WDT expiration    ///
// before entering to 'main()'.                                                                         ///
//for every IDE, different syntax exists :          1.   __CCS__ for CCS v5                             ///
//                                                  2.  __IAR_SYSTEMS_ICC__ for IAR Embedded Workbench  ///
// *CCS does not initialize variables - therefore, __no_init is not needed.                             ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
__attribute__((section(".noinit"))) char spi_buffer[CC3000_RX_BUFFER_SIZE];
__attribute__((section(".noinit"))) unsigned char wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];

//*****************************************************************************
//
//!  SpiClose
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Close Spi interface
//
//*****************************************************************************
void SpiClose(void)
{
	if (sSpiInformation.pRxPacket)
		sSpiInformation.pRxPacket = 0;

	// Disable Interrupt...
	tSLInformation.WlanInterruptDisable();
}

//*****************************************************************************
//
//!  SpiOpen
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Open Spi interface
//
//*****************************************************************************
void SpiOpen(gcSpiHandleRx pfRxHandler)
{
	sSpiInformation.ulSpiState = eSPI_STATE_POWERUP;
	sSpiInformation.SPIRxHandler = pfRxHandler;
	sSpiInformation.usTxPacketLength = 0;
	sSpiInformation.pTxPacket = NULL;
	sSpiInformation.pRxPacket = (unsigned char *)spi_buffer;
	sSpiInformation.usRxPacketLength = 0;
	spi_buffer[CC3000_RX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;
	wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;

	// Enable interrupt of WLAN IRQ
	tSLInformation.WlanInterruptEnable();
}

//*****************************************************************************
//
//!  init_spi
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  initializes an SPI interface
//
//*****************************************************************************
int spi_init(void)
{
	SPI_D |= SPI_MOSI | SPI_SCK | SPI_CS | CC3000_EN;
	SPI_D &= ~(SPI_MISO | CC3000_INT);
	SPI_W |= SPI_MOSI | SPI_MISO | SPI_SCK | SPI_CS | CC3000_EN | CC3000_INT;

	UCSR1B = _BV(RXEN1) | _BV(TXEN1);
	UCSR1C = _BV(UMSEL11) | _BV(UMSEL10) | _BV(UCPHA1);
	UBRR1 = 0;

	return(ESUCCESS);
}

//*****************************************************************************
//
//! SpiFirstWrite
//!
//!  @param  ucBuf     buffer to write
//!  @param  usLength  buffer's length
//!
//!  @return none
//!
//!  @brief  enter point for first write flow
//
//*****************************************************************************
long SpiFirstWrite(unsigned char *ucBuf, unsigned short usLength)
{
	// workaround for first transaction
	ASSERT_CS();

	// ~50 micro delay
	_delay_us(50);

	// SPI writes first 4 bytes of data
	SpiWriteDataSynchronous(ucBuf, 4);

	_delay_us(50);

	SpiWriteDataSynchronous(ucBuf + 4, usLength - 4);

	// From this point on - operate in a regular way
	sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

	DEASSERT_CS();

	return(0);
}

//*****************************************************************************
//
//!  SpiWrite
//!
//!  @param  pUserBuffer  buffer to write
//!  @param  usLength     buffer's length
//!
//!  @return none
//!
//!  @brief  Spi write operation
//
//*****************************************************************************
long SpiWrite(unsigned char *pUserBuffer, unsigned short usLength)
{
	unsigned char ucPad = 0;

	// Figure out the total length of the packet in order to figure out if there
	// is padding or not
	if(!(usLength & 0x0001))
		ucPad++;

	pUserBuffer[0] = WRITE;
	pUserBuffer[1] = HI(usLength + ucPad);
	pUserBuffer[2] = LO(usLength + ucPad);
	pUserBuffer[3] = 0;
	pUserBuffer[4] = 0;

	usLength += (SPI_HEADER_SIZE + ucPad);

	// The magic number that resides at the end of the TX/RX buffer (1 byte after
	// the allocated size) for the purpose of detection of the overrun. If the
	// magic number is overwritten - buffer overrun occurred - and we will stuck
	// here forever!
	if (wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER) {
		puts_P(PSTR("CC3000_BUFFER_MAGIC_NUMBER"));
		while (1);
	}

	if (sSpiInformation.ulSpiState == eSPI_STATE_POWERUP)
		while (sSpiInformation.ulSpiState != eSPI_STATE_INITIALIZED);

	if (sSpiInformation.ulSpiState == eSPI_STATE_INITIALIZED) {
		// This is time for first TX/RX transactions over SPI: the IRQ is down -
		// so need to send read buffer size command
		SpiFirstWrite(pUserBuffer, usLength);
	} else {
		// We need to prevent here race that can occur in case 2 back to back
		// packets are sent to the  device, so the state will move to IDLE and once
		//again to not IDLE due to IRQ
		tSLInformation.WlanInterruptDisable();

		while (sSpiInformation.ulSpiState != eSPI_STATE_IDLE);

		sSpiInformation.ulSpiState = eSPI_STATE_WRITE_IRQ;
		sSpiInformation.pTxPacket = pUserBuffer;
		sSpiInformation.usTxPacketLength = usLength;

		// Assert the CS line and wait till SSI IRQ line is active and then
		// initialize write operation
		ASSERT_CS();

		// Re-enable IRQ - if it was not disabled - this is not a problem...
		tSLInformation.WlanInterruptEnable();

		// check for a missing interrupt between the CS assertion and enabling back the interrupts
		if (tSLInformation.ReadWlanInterruptPin() == 0) {
                	SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.usTxPacketLength);

			sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

			DEASSERT_CS();
		}
	}

	// Due to the fact that we are currently implementing a blocking situation
	// here we will wait till end of transaction
	while (eSPI_STATE_IDLE != sSpiInformation.ulSpiState);

	return(0);
}

//*****************************************************************************
//
//!  SpiWriteDataSynchronous
//!
//!  @param  data  buffer to write
//!  @param  size  buffer's size
//!
//!  @return none
//!
//!  @brief  Spi write operation
//
//*****************************************************************************
void SpiWriteDataSynchronous(unsigned char *data, unsigned short size)
{
	while (size--) {
		while (!(TXBufferIsEmpty()));
		UDR1 = *data++;
		while (!(RXBufferIsEmpty()));
		UDR1;
	}
}

//*****************************************************************************
//
//! SpiReadDataSynchronous
//!
//!  @param  data  buffer to read
//!  @param  size  buffer's size
//!
//!  @return none
//!
//!  @brief  Spi read operation
//
//*****************************************************************************
void SpiReadDataSynchronous(unsigned char *data, unsigned short size)
{
	long i = 0;
	unsigned char *data_to_send = tSpiReadHeader;

	for (i = 0; i < size; i++) {
		while (!(TXBufferIsEmpty()));
		//Dummy write to trigger the clock
		UDR1 = data_to_send[0];
		while (!(RXBufferIsEmpty()));
		data[i] = UDR1;
	}
}

//*****************************************************************************
//
//!  SpiReadHeader
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief   This function enter point for read flow: first we read minimal 5
//!	          SPI header bytes and 5 Event Data bytes
//
//*****************************************************************************
void SpiReadHeader(void)
{
	SpiReadDataSynchronous(sSpiInformation.pRxPacket, 10);
}

//*****************************************************************************
//
//!  SpiReadDataCont
//!
//!  @param  None
//!
//!  @return None
//!
//!  @brief  This function processes received SPI Header and in accordance with
//!	         it - continues reading the packet
//
//*****************************************************************************
long SpiReadDataCont(void)
{
	long data_to_recv;
	unsigned char *evnt_buff, type;

	//determine what type of packet we have
	evnt_buff =  sSpiInformation.pRxPacket;
	data_to_recv = 0;
	STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_PACKET_TYPE_OFFSET, type);

	switch(type) {
	case HCI_TYPE_DATA:
		{
			// We need to read the rest of data..
			STREAM_TO_UINT16((char *)(evnt_buff + SPI_HEADER_SIZE),
											 HCI_DATA_LENGTH_OFFSET, data_to_recv);
			if (!((HEADERS_SIZE_EVNT + data_to_recv) & 1))
				data_to_recv++;

			if (data_to_recv)
				SpiReadDataSynchronous(evnt_buff + 10, data_to_recv);
			break;
		}
	case HCI_TYPE_EVNT:
		{
			// Calculate the rest length of the data
			STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE),
											HCI_EVENT_LENGTH_OFFSET, data_to_recv);
			data_to_recv -= 1;

			// Add padding byte if needed
			if ((HEADERS_SIZE_EVNT + data_to_recv) & 1)
				data_to_recv++;

			if (data_to_recv)
				SpiReadDataSynchronous(evnt_buff + 10, data_to_recv);

			sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;
			break;
		}
	}

	return (0);
}

//*****************************************************************************
//
//! SpiPauseSpi
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Spi pause operation
//
//*****************************************************************************
volatile uint8_t spiRunning = 1;
volatile uint8_t interruptRequest = 0;
void SpiPauseSpi(void)
{
	spiRunning = 0;
}

//*****************************************************************************
//
//! SpiResumeSpi
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Spi resume operation
//
//*****************************************************************************
static void isr(void);
void SpiResumeSpi(void)
{
	PCMSK3 |= CC3000_INT;
	spiRunning = 1;
	if (interruptRequest)
		isr();
}

//*****************************************************************************
//
//! SpiTriggerRxProcessing
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Spi RX processing
//
//*****************************************************************************
void SpiTriggerRxProcessing(void)
{
	// Trigger Rx processing
	SpiPauseSpi();
	DEASSERT_CS();

	// The magic number that resides at the end of the TX/RX buffer (1 byte after
	// the allocated size) for the purpose of detection of the overrun. If the
	// magic number is overwritten - buffer overrun occurred - and we will stuck
	// here forever!
	if (sSpiInformation.pRxPacket[CC3000_RX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER)
		while (1);

	sSpiInformation.ulSpiState = eSPI_STATE_IDLE;
	sSpiInformation.SPIRxHandler(sSpiInformation.pRxPacket + SPI_HEADER_SIZE);
}

//*****************************************************************************
//
//!  IntSpiGPIOHandler
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  GPIO A interrupt handler. When the external SSI WLAN device is
//!          ready to interact with Host CPU it generates an interrupt signal.
//!          After that Host CPU has registered this interrupt request
//!          it set the corresponding /CS in active state.
//
//*****************************************************************************
ISR(PCINT3_vect)
{
	// Active low ONLY
	if (SPI_R & CC3000_INT)
		return;
	if (spiRunning == 0)
		interruptRequest = 1;
	else
		isr();
}

static void isr(void)
{
	interruptRequest = 0;
	switch (sSpiInformation.ulSpiState) {
	case eSPI_STATE_POWERUP:
		//This means IRQ line was low call a callback of HCI Layer to inform
		//on event
 		sSpiInformation.ulSpiState = eSPI_STATE_INITIALIZED;
 		break;
	case eSPI_STATE_IDLE:
		sSpiInformation.ulSpiState = eSPI_STATE_READ_IRQ;

		/* IRQ line goes down - we are start reception */
		ASSERT_CS();

		// Wait for TX/RX Compete which will come as DMA interrupt
		SpiReadHeader();

		sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;

		SSIContReadOperation();
		break;
	case eSPI_STATE_WRITE_IRQ:
		SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.usTxPacketLength);

		sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

		DEASSERT_CS();
	}
}

//*****************************************************************************
//
//! SSIContReadOperation
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  SPI read operation
//
//*****************************************************************************
void SSIContReadOperation(void)
{
	// The header was read - continue with the payload read
	if (!SpiReadDataCont()) {
		// All the data was read - finalize handling by switching to the task
		//	and calling from task Event Handler
		SpiTriggerRxProcessing();
	}
}

//*****************************************************************************
//
//! TXBufferIsEmpty
//!
//!  @param
//!
//!  @return returns 1 if buffer is empty, 0 otherwise
//!
//!  @brief  Indication if TX SPI buffer is empty
//
//*****************************************************************************
long TXBufferIsEmpty(void)
{
	return UCSR1A & _BV(UDRE1);
}

//*****************************************************************************
//
//! RXBufferIsEmpty
//!
//!  @param  none
//!
//!  @return returns 1 if buffer is empty, 0 otherwise
//!
//!  @brief  Indication if RX SPI buffer is empty
//
//*****************************************************************************
long RXBufferIsEmpty(void)
{
	// It seems return true when data available
	return UCSR1A & _BV(RXC1);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
