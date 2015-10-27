#include <avr/io.h>
#include <rfm12_config.h>
#include <rfm12.h>
#include <rfm12_spi.c>
#include <rfm12_hw.h>
#include "mac_layer.h"

//! The tick function implements collision avoidance and initiates transmissions.
/** This function has to be called periodically.
* It will read the rfm12 status register to check if a carrier is being received,
* which would indicate activity on the chosen radio channel. \n
* If there has been no activity for long enough, the channel is believed to be free.
*
* When there is a packet waiting for transmission and the collision avoidance
* algorithm indicates that the air is free, then the interrupt control variables are
* setup for packet transmission and the rfm12 is switched to transmit mode.
* This function also fills the rfm12 tx fifo with a preamble.
*
* \warning Warning, if you do not call this function periodically, then no packet will get transmitted.
* \see rfm12_tx() and rfm12_start_tx()
*/
void mac_csma_0_tick()
{
	uint16_t status;

#if 1
	//start with a channel free count of 16, this is necessary for the ASK receive feature to work
	static uint8_t channel_free_count = 16;	//static local variables produce smaller code size than globals
#endif

	//don't disturb RFM12 if transmitting or receiving
	if (ctrl.rfm12_state != STATE_RX_IDLE) {
		return;
	}

	//disable the interrupt (as we're working directly with the transceiver now)
	//hint: we could be losing an interrupt here, because we read the status register.
	//this applys for the Wakeup timer, as it's flag is reset by reading.
	RFM12_INT_OFF();
	status = rfm12_read(RFM12_CMD_STATUS);
	RFM12_INT_ON();

	//wakeup timer workaround (if we don't restart the timer after timeout, it will stay off.)
#if RFM12_USE_WAKEUP_TIMER
	if (status & (RFM12_STATUS_WKUP)) {
		ctrl.wkup_flag = 1;

		RFM12_INT_OFF();
		//restart the wakeup timer by toggling the bit on and off
		rfm12_data(ctrl.pwrmgt_shadow & ~RFM12_PWRMGT_EW);
		rfm12_data(ctrl.pwrmgt_shadow);
		RFM12_INT_ON();
	}
#endif				/* RFM12_USE_WAKEUP_TIMER */

	//check if we see a carrier
	if (status & RFM12_STATUS_RSSI) {
#if 1
		//yes: reset free counter and return
		channel_free_count = CHANNEL_FREE_TIME;
#endif
		return;
	}
	//no

	//is the channel free long enough ?
#if 1
	if (channel_free_count != 0) {
		//no:
		channel_free_count--;	// decrement counter
		return;
	}
#endif
	//yes: we can begin transmitting

	//do we have something to transmit?
	if (ctrl.txstate == STATUS_OCCUPIED) {	//yes: start transmitting
		//disable the interrupt (as we're working directly with the transceiver now)
		//we won't loose interrupts, as the AVR caches them in the int flag.
		//we could disturb an ongoing reception,
		//if it has just started some cpu cycles ago
		//(as the check for this case is some lines (cpu cycles) above)
		//anyhow, we MUST transmit at some point...
		RFM12_INT_OFF();

		//disable receiver - if you don't do this, tx packets will get lost
		//as the fifo seems to be in use by the receiver

#if RFM12_PWRMGT_SHADOW
		ctrl.pwrmgt_shadow &= ~(RFM12_PWRMGT_ER);	/* disable receiver */
		rfm12_data(ctrl.pwrmgt_shadow);
#else
		rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT);	/* disable receiver */
#endif

		//RFM12BP receiver off
#ifdef RX_LEAVE_HOOK
		RX_LEAVE_HOOK;
#endif

		//calculate number of bytes to be sent by ISR
		//2 sync bytes + len byte + type byte + checksum + message length + 1 dummy byte
		ctrl.num_bytes = rf_tx_buffer.len + 6;

		//reset byte sent counter
		ctrl.bytecount = 0;

		//set mode for interrupt handler
		ctrl.rfm12_state = STATE_TX;

		//RFM12BP transmitter on
#ifdef TX_ENTER_HOOK
		TX_ENTER_HOOK;
#endif

		//fill 2byte 0xAA preamble into data register
		//the preamble helps the receivers AFC circuit to lock onto the exact frequency
		//(hint: the tx FIFO [if el is enabled] is two staged, so we can safely write 2 bytes before starting)
		rfm12_data(RFM12_CMD_TX | PREAMBLE);
		rfm12_data(RFM12_CMD_TX | PREAMBLE);

		//set ET in power register to enable transmission (hint: TX starts now)
#if RFM12_PWRMGT_SHADOW
		ctrl.pwrmgt_shadow |= RFM12_PWRMGT_ET;
		rfm12_data(ctrl.pwrmgt_shadow);
#else
		rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT | RFM12_PWRMGT_ET);
#endif

		//enable the interrupt to continue the transmission
		RFM12_INT_ON();
	}
}
