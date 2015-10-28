#include <string.h>
#include <rfm12_config.h>
#include <rfm12.h>
#include "tcp_layer.h"
#include "mac_layer.h"

struct buffer_t
{
	uint8_t dst;
	uint8_t src;
	uint8_t buffer[0];
};

//static uint8_t txBuffer[RFM12_TX_BUFFER_SIZE];

uint8_t *payloadBuffer()
{
	struct buffer_t *ptr = (struct buffer_t *) rf_tx_buffer.buffer;
	return ptr->buffer;
}

uint8_t send(uint8_t addr, uint8_t length)
{
	struct buffer_t *ptr = (struct buffer_t *) rf_tx_buffer.buffer;
	ptr->dst = addr;
	ptr->src = MACHINE_ADDRESS;
	return rfm12_start_tx(MAC_CSMA, length + sizeof(struct buffer_t));
}

uint8_t write(uint8_t addr, uint8_t length, uint8_t *buffer)
{
	struct buffer_t *ptr = (struct buffer_t *) rf_tx_buffer.buffer;
	ptr->dst = addr;
	ptr->src = MACHINE_ADDRESS;
	memcpy(ptr->buffer, buffer, length);
	return rfm12_start_tx(MAC_CSMA, length + sizeof(struct buffer_t));
}

uint8_t available()
{
	if (rfm12_rx_status() != STATUS_COMPLETE)
		return 0;
	uint8_t len = rfm12_rx_len();
	if (len <= 2)	// Discard empty packages
		goto free;
	struct buffer_t *ptr = (struct buffer_t *) rfm12_rx_buffer();
	if (ptr->dst != MACHINE_ADDRESS)
		goto free;
	return len - sizeof(struct buffer_t);
free:
	rfm12_rx_clear();
	return 0;
}

uint8_t read(uint8_t *addr, uint8_t *buffer)
{
	uint8_t len = rfm12_rx_len() - sizeof(struct buffer_t);
	if (len == 0)
		return 0;
	if (rfm12_rx_status() != STATUS_COMPLETE)
		return 0;
	//uint8_t type = rfm12_rx_type();
	struct buffer_t *ptr = (struct buffer_t *) rfm12_rx_buffer();
	*addr = ptr->src;
	memcpy(buffer, ptr->buffer, len);
	rfm12_rx_clear();
	return len;
}
