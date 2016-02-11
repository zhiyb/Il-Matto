#ifndef SPIFLASH_H
#define SPIFLASH_H

#define SPIFLASH_PORT	B
#define SPIFLASH_WP	_BV(2)
#define SPIFLASH_RESET	_BV(3)
#define SPIFLASH_CS	_BV(4)
#define SPIFLASH_MOSI	_BV(5)
#define SPIFLASH_MISO	_BV(6)
#define SPIFLASH_SCK	_BV(7)

#ifndef SPIFLASH_SIZE
#define SPIFLASH_SIZE	(16UL * 1024UL * 1024UL)
#endif

#define SPIFLASH_PAGE_SIZE	256

namespace SPIFlash
{
	void hwInit();
	void assert(bool e);
	unsigned char spiTransfer(const unsigned char data = 0);

	void init();
	void reset();
#ifdef SPIFLASH_TEST
	void test();
#endif
	
	bool checkBusy();

	uint32_t readJID();
	// Read status register n (1 <= n <= 3)
	uint8_t readStatus(uint8_t n);
	void writeStatus(uint8_t n, uint8_t v);
	void readStart(const uint32_t address);
	static inline uint8_t readByte() {return spiTransfer();}
	static inline uint16_t readWord()
	{
		uint16_t data = (uint16_t)spiTransfer() << 8;
		return data | (uint16_t)spiTransfer();
	}
	static inline uint16_t readDWord()
	{
		uint32_t data = (uint32_t)spiTransfer() << 24;
		data |= (uint32_t)spiTransfer() << 16;
		data |= (uint32_t)spiTransfer() << 8;
		return data | (uint32_t)spiTransfer();
	}
	static inline void readStop() {assert(false);}

	void erase4k(const uint32_t address);
	void erase32k(const uint32_t address);
	void erase64k(const uint32_t address);
	void eraseChip();

	void programStart(const uint32_t address);
	static inline void writeByte(const uint8_t v) {spiTransfer(v);}
	static inline void writeWord(const uint16_t v)
	{
		spiTransfer(v >> 8);
		spiTransfer(v);
	}
	static inline void writeDWord(const uint32_t v)
	{
		spiTransfer(v >> 24);
		spiTransfer(v >> 16);
		spiTransfer(v >> 8);
		spiTransfer(v);
	}
	static inline void programStop() {assert(false);}
	void writeData(uint32_t addr, char *data, uint32_t length);
}

#endif
