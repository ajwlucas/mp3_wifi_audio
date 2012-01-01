#include <xs1.h>
#include <xclib.h>
#include "spi_driver.h"

extern out buffered port:8 spi_mosi;
extern out buffered port:8 spi_sclk;
extern in buffered port:8 spi_miso;
extern out port spi_ss;

extern clock blk1;
extern clock blk2;

void spi_select()
{
	spi_ss <: 0;
}

void spi_deselect()
{
	spi_ss <: 1;
}

void spi_init()
{
	// configure ports and clock blocks
	configure_clock_rate(blk1, 100, SPI_CLOCK_DIV);
	configure_out_port(spi_sclk, blk1, 0);
	configure_clock_src(blk2, spi_sclk);
	configure_out_port(spi_mosi, blk2, 0);
	configure_in_port(spi_miso, blk2);
	clearbuf(spi_mosi);
	clearbuf(spi_sclk);
	start_clock(blk1);
	start_clock(blk2);
	spi_sclk <: 0xFF;
}

void spi_shutdown()
{
	// need clock ticks in order to stop clock blocks
	spi_sclk <: 0xAA;
	spi_sclk <: 0xAA;
	stop_clock(blk2);
	stop_clock(blk1);
}

unsigned char spi_in_byte()
{
	// MSb-first bit order - SPI standard
	unsigned x;
	clearbuf(spi_miso);
	spi_sclk <: 0xAA;
	spi_sclk <: 0xAA;
	sync(spi_sclk);
	spi_miso :> x;
	return bitrev(x) >> 24;
}

unsigned short spi_in_short()
{
	// big endian byte order
	unsigned short data = 0;
	data |= (spi_in_byte() << 8);
	data |= spi_in_byte();
	return data;
}

unsigned int spi_in_word()
{
	// big endian byte order
	unsigned int data = 0;
	data |= (spi_in_byte() << 24);
	data |= (spi_in_byte() << 16);
	data |= (spi_in_byte() << 8);
	data |= spi_in_byte();
	return data;
}

void spi_out_byte(unsigned char data)
{
	// MSb-first bit order - SPI standard
	unsigned x = bitrev(data) >> 24;
	spi_mosi <: x;
	spi_sclk <: 0xAA;
	spi_sclk <: 0xAA;
	sync(spi_sclk);
	spi_miso :> void;
}

void spi_out_short(unsigned short data)
{
	// big endian byte order
	spi_out_byte((data >> 8) & 0xFF);
	spi_out_byte(data & 0xFF);
}

void spi_out_word(unsigned int data)
{
	// big endian byte order
	spi_out_byte((data >> 24) & 0xFF);
	spi_out_byte((data >> 16) & 0xFF);
	spi_out_byte((data >> 8) & 0xFF);
	spi_out_byte(data & 0xFF);
}
