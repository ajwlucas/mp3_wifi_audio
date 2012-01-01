#ifndef _master_h_
#define _master_h_

void spi_init();
void spi_shutdown();

// SPI master output
// big endian byte order
void spi_out_word(unsigned int data);
void spi_out_short(unsigned short data);
void spi_out_byte(unsigned char data);
void spi_select();
void spi_deselect();

// SPI master input
// big endian byte order
unsigned int spi_in_word();
unsigned short spi_in_short();
unsigned char spi_in_byte();

// SPI ports
#ifndef SPI_MISO
#define SPI_MISO XS1_PORT_1A
#endif
#ifndef SPI_CLK
#define SPI_CLK  XS1_PORT_1C
#endif
#ifndef SPI_MOSI
#define SPI_MOSI XS1_PORT_1D
#endif
// single select line
#define SPI_SS XS1_PORT_1B

// SPI clock frequency is fref/(2*SPI_CLOCK_DIV)
// where fref defaults to 100MHz
#ifndef SPI_CLOCK_DIV
#define SPI_CLOCK_DIV 8
#endif

#endif
