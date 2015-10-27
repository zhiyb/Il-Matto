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

static uint8_t txBuffer[RFM12_TX_BUFFER_SIZE];

uint8_t write(uint8_t addr, uint8_t length, uint8_t *buffer)
{
	struct buffer_t *ptr = (struct buffer_t *) txBuffer;
	ptr->dst = addr;
	ptr->src = MACHINE_ADDRESS;
	memcpy(ptr->buffer, buffer, length);
	return rfm12_tx(length + sizeof(struct buffer_t), MAC_CSMA, txBuffer);
}

uint8_t available()
{
	if (rfm12_rx_status() != STATUS_COMPLETE)
		return 0;
	uint8_t len = rfm12_rx_len();
	if (len < 2)
		goto free;
	struct buffer_t *ptr = (struct buffer_t *) rfm12_rx_buffer();
	if (ptr->dst != MACHINE_ADDRESS)
		goto free;
	return 1;
free:
	rfm12_rx_clear();
	return 0;
}

uint8_t read(uint8_t *addr, uint8_t *length, uint8_t *buffer)
{
	if (!available())
		return 0;
	*length = rfm12_rx_len() - sizeof(struct buffer_t);
	//uint8_t type = rfm12_rx_type();
	struct buffer_t *ptr = (struct buffer_t *) rfm12_rx_buffer();
	*addr = ptr->src;
	memcpy(buffer, ptr->buffer, *length);
	rfm12_rx_clear();
	return 1;
}
