#include <xs1.h>
#include <xclib.h>
#include <print.h>
#include "spi_driver.h"
#include "zg_chip.h"

void WriteFIFO(unsigned char buf[], int length)
{
	spi_select();
	
	for (int i=0; i < length; i++)
	{
		spi_out_byte(buf[i]);
	}
	
	spi_deselect();
	
	spi_select();
	
	// Indicate end of FIFO write
	spi_out_byte(ZG_CMD_WT_FIFO_DONE);
	
	spi_deselect();
}

void WriteTXData(unsigned char buf[], int length)
{
	unsigned char hdr[5];
	
	hdr[0] = ZG_CMD_WT_FIFO_DATA;
	hdr[1] = ZG_MAC_TYPE_TXDATA_REQ;
	hdr[2] = ZG_MAC_SUBTYPE_TXDATA_REQ_STD;
	hdr[3] = 0x00;
	hdr[4] = 0x00;
	
	spi_select();
	
	// ZG header first
	for (int i=0; i < 5; i++)
	{
		spi_out_byte(hdr[i]);
	}
	
	// Keep SS active
	
	for (int i=0; i < length; i++)
	{
		spi_out_byte(buf[i]);
	}
	
	spi_deselect();
	
	spi_select();
	
	// Indicate end of FIFO write
	spi_out_byte(ZG_CMD_WT_FIFO_DONE);
	
	spi_deselect();
}

void ReadFIFO(unsigned char buf[], int length)
{
	spi_select();
	
	// Indicate a read start
	spi_out_byte(ZG_CMD_RD_FIFO);
	
	for (int i=0; i < length; i++)
	{
		buf[i] = spi_in_byte();
	}
	
	spi_deselect();
	
	spi_select();
	
	// Indicate end of FIFO read
	spi_out_byte(ZG_CMD_RD_FIFO_DONE);
	
	spi_deselect();
}

void Write16BitRegister(unsigned char regId, unsigned short value)
{
	spi_select();
	
	// Output the register ID byte then the 16 bit value to write
	spi_out_byte(regId);
	spi_out_short(value);
	
	spi_deselect();
}

void Write8BitRegister(unsigned char regId, unsigned char value)
{
	spi_select();
	
	// Output the register ID byte then the 8 bit value
	spi_out_byte(regId);
	spi_out_byte(value);
	
	spi_deselect();
}

short Read16BitRegister(unsigned char regId)
{
	short value;
	
	regId |= 0x40;
	
	spi_select();
	spi_out_byte(regId);
	value = spi_in_short();
	spi_deselect();
	
	return value;
}

char Read8BitRegister(unsigned char regId)
{
	char value;
	
	regId |= 0x40;
	
	spi_select();
	spi_out_byte(regId);
	value = spi_in_byte();
	spi_deselect();
	
	return value;
}

