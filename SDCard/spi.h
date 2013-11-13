#ifndef SPI_H
#define SPI_H

class spi
{
public:
	static void init(void);

protected:
	static void send(uint8_t d);
	static uint8_t recv(void);
};

#endif
