#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#ifdef SPIFLASH_TEST
#include <stdio.h>
#include <tft.h>
#endif
#include <macros.h>
#include "spiflash.h"

#define SF_DDR	CONCAT_E(DDR, SPIFLASH_PORT)
#define SF_PORT	CONCAT_E(PORT, SPIFLASH_PORT)
#define SF_PIN	CONCAT_E(PIN, SPIFLASH_PORT)

void __attribute__((__weak__)) SPIFlash::hwInit()
{
	SF_PORT |= SPIFLASH_WP | SPIFLASH_RESET | SPIFLASH_CS | SPIFLASH_MOSI | SPIFLASH_MISO | SPIFLASH_SCK;
	SF_DDR |= SPIFLASH_WP | SPIFLASH_RESET | SPIFLASH_CS | SPIFLASH_MOSI | SPIFLASH_SCK;
	SF_DDR &= ~(SPIFLASH_MISO);

	SPCR = _BV(SPE) | _BV(MSTR);
	SPSR = _BV(SPI2X);
}

void __attribute__((__weak__)) SPIFlash::assert(bool e)
{
	if (e)
		SF_PORT &= ~SPIFLASH_CS;
	else
		SF_PORT |= SPIFLASH_CS;
}

unsigned char __attribute__((__weak__)) SPIFlash::spiTransfer(const unsigned char data)
{
	SPDR = data;
	while (!(SPSR & _BV(SPIF)));
	return SPDR;
}

#define CMD_WRITE_ENABLE	0x06
#define CMD_WRITE_DISABLE	0x04	// Automatic reset
#define CMD_READ_STATUS_1	0x05
#define CMD_WRITE_STATUS_1	0x01
#define CMD_READ_STATUS_2	0x35
#define CMD_WRITE_STATUS_2	0x31
#define CMD_READ_STATUS_3	0x15
#define CMD_WRITE_STATUS_3	0x11
#define CMD_READ_DATA		0x03	// 24-bit address
#define CMD_FAST_READ		0x0b	// 24-bit address + 8-bit dummy
#define CMD_PAGE_PROGRAM	0x02	// 24-bit address, max. 1 page (256 bytes) boundary
#define CMD_SECTOR_ERASE	0x20	// 24-bit address (4k bytes sector)
#define CMD_BLOCK_ERASE_32K	0x52	// 24-bit address (32k bytes block)
#define CMD_BLOCK_ERASE_64K	0xd8	// 24-bit address (64k bytes block)
#define CMD_CHIP_ERASE		0xc7
#define CMD_ERASE_SUSPEND	0x75
#define CMD_ERASE_RESUME	0x7a
#define CMD_POWER_DOWN		0xb9
#define CMD_RELEASE_POWER_DOWN	0xab	// 3 dummy bytes + read device ID
#define CMD_READ_MID		0x90	// 24-bit address (0) + read manufacturer + device ID
#define CMD_READ_UID		0x4b	// 4 dummy bytes + 64-bit serial number
#define CMD_READ_JID		0x9f	// JEDEC ID (Manufacturer ID + memory type + capacity)
#define CMD_READ_SFDP		0x5a	// Serial flash discoverable parameter
#define CMD_ERASE_SECURITY	0x44	// Erase security registers: 24-bit address
#define CMD_PROGRAM_SECURITY	0x42	// 24-bit address + data bytes
#define CMD_READ_SECURITY	0x48	// 24-bit address + 8-bit dummy
#define CMD_LOCK		0x36	// 24-bit address
#define CMD_UNLOCK		0x39	// 24-bit address
#define CMD_READ_LOCK		0x3d	// 24-bit address + read 8-bit lock value
#define CMD_GLOBAL_LOCK		0x7e
#define CMD_GLOBAL_UNLOCK	0x98
#define CMD_ENABLE_RESET	0x66
#define CMD_RESET_DEVICE	0x99	// In sequence with CMD_ENABLE_RESET

#define PAGE_SIZE	SPIFLASH_PAGE_SIZE

namespace SPIFlash
{
	static uint8_t statusAddress(uint8_t n)
	{
		switch (n) {
		case 2:
			return CMD_READ_STATUS_2;
		case 3:
			return CMD_READ_STATUS_3;
		}
		return CMD_READ_STATUS_1;
	}

	static uint32_t spiTransfer24(const uint32_t data = 0)
	{
		uint32_t out;
		out = (uint32_t)spiTransfer(data >> 16) << 16;
		out |= (uint32_t)spiTransfer(data >> 8) << 8;
		out |= (uint32_t)spiTransfer(data);
		return out;
	}

	bool checkBusy()
	{
		assert(true);
		spiTransfer(CMD_READ_STATUS_1);
		uint8_t status = spiTransfer();
		assert(false);
		return status & 0x01;
	}

	static void writeEnable()
	{
		assert(true);
		spiTransfer(CMD_WRITE_ENABLE);
		assert(false);
	}
	
	void writeData(uint32_t addr, char *data, uint32_t length)
	{
		if (addr % PAGE_SIZE) {
			programStart(addr);
			uint8_t i = PAGE_SIZE - addr % PAGE_SIZE;
			i = length > i ? i : length;
			length -= i;
			while (i--)
				spiTransfer(*data++);
			programStop();
			addr = (addr & 0xffffff00) + 0x0100;
		}
		while ((length & 0xffffff00) != 0) {
			programStart(addr);
			uint8_t i = 0;
			do {
				spiTransfer(*data++);
			} while (++i != 0);
			programStop();
			length -= 0x0100;
			addr += 0x0100;
		}
		uint8_t len = length;
		if (len) {
			programStart(addr);
			do
				spiTransfer(*data++);
			while (--len);
			programStop();
		}
	}
	
	uint8_t readStatus(uint8_t n)
	{
		assert(true);
		spiTransfer(statusAddress(n));
		n = spiTransfer();
		assert(false);
		return n;
	}
	
	void writeStatus(uint8_t n, uint8_t v)
	{
		writeEnable();
		assert(true);
		spiTransfer(statusAddress(n) - 0x04);
		spiTransfer(v);
		assert(false);
	}
}

void __attribute__((__weak__)) SPIFlash::init()
{
	hwInit();
}

void SPIFlash::reset()
{
	assert(true);
	spiTransfer(CMD_ENABLE_RESET);
	assert(false);
	assert(true);
	spiTransfer(CMD_RESET_DEVICE);
	assert(false);
	_delay_us(30);
}

uint32_t SPIFlash::readJID()
{
	while (checkBusy());
	assert(true);
	spiTransfer(CMD_READ_JID);
	uint32_t jid = spiTransfer24();
	assert(false);
	return jid;
}

void SPIFlash::readStart(const uint32_t address)
{
	while (checkBusy());
	assert(true);
	spiTransfer(CMD_FAST_READ);
	spiTransfer24(address);
	spiTransfer();
}

void SPIFlash::erase4k(const uint32_t address)
{
	while (checkBusy());
	writeEnable();
	assert(true);
	spiTransfer(CMD_SECTOR_ERASE);
	spiTransfer24(address);
	assert(false);
}

void SPIFlash::erase32k(const uint32_t address)
{
	while (checkBusy());
	writeEnable();
	assert(true);
	spiTransfer(CMD_BLOCK_ERASE_32K);
	spiTransfer24(address);
	assert(false);
}

void SPIFlash::erase64k(const uint32_t address)
{
	while (checkBusy());
	writeEnable();
	assert(true);
	spiTransfer(CMD_BLOCK_ERASE_64K);
	spiTransfer24(address);
	assert(false);
}

void SPIFlash::eraseChip()
{
	while (checkBusy());
	writeEnable();
	assert(true);
	spiTransfer(CMD_CHIP_ERASE);
	assert(false);
}

void SPIFlash::programStart(const uint32_t address)
{
	while (checkBusy())
		_delay_us(10);
	writeEnable();
	assert(true);
	spiTransfer(CMD_PAGE_PROGRAM);
	spiTransfer24(address);
}

#ifdef SPIFLASH_TEST
void SPIFlash::test()
{
#if 1
	tft::setFont(10, 16);
	uint32_t jid = readJID();
	printf("JEDEC ID: 0x%6lx\n", jid);
#endif

#if 1
	_delay_ms(500);
	puts("Reading to TFT...");
	readStart(0);
	tfthw::all();
	tfthw::memWrite();
	for (uint32_t addr = 0; addr < SPIFLASH_SIZE; addr++)
		tfthw::write(readByte());
	readStop();
	puts("Done!");
#endif

#if 1
	_delay_ms(500);
	puts("Erasing chip...");
	eraseChip();
	while (checkBusy());
	printf("Filling rand(%u)...\n", RAND_MAX);
	for (uint32_t page = 0; page < SPIFLASH_SIZE >> 8; page++) {
		programStart(page << 8);
		printf("%lu", page);
		tft::x = 0;
		uint8_t i = 0;
		do {
			writeWord(rand());
			i += 2;
		} while (i != 0);
		programStop();
	}
	puts("\nDone!");
#endif
}
#endif
