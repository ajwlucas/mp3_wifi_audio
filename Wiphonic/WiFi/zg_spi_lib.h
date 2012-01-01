#ifndef ZG_SPI_LIB_H_
#define ZG_SPI_LIB_H_

void Write16BitRegister(unsigned char regId, unsigned short value);
void Write8BitRegister(unsigned char regId, unsigned char value);
short Read16BitRegister(unsigned char regId);
char Read8BitRegister(unsigned char regId);
void WriteFIFO(unsigned char buf[], int length);
void ReadFIFO(unsigned char buf[], int length);
void WriteTXData(unsigned char buf[], int length);

#endif /*ZG_SPI_LIB_H_*/
