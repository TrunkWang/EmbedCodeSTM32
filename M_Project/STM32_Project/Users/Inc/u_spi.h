#ifndef _U_SPI_H_
#define _U_SPI_H_


#define HARD_SPI


#ifdef SOFT_SPI
void uuu_spi_init(void );
unsigned short uuu_spi_readdata(unsigned char devicev  );
#endif

#ifdef HARD_SPI
void uuu_spi_init(void );
void uuu_spi_enable(void );
void uuu_spi_disable(void );
unsigned char uuu_spi_readwrite(unsigned char datav);

unsigned short uuu_spi_readdata(unsigned char devicev  );
#endif

#endif


