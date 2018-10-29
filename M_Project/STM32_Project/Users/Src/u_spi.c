
#include "u_spi.h"

#include "stm32f10x.h"

#include "stm32f10x_spi.h"


#define SPI_DELAY_COUNT	(0)

void spi_delay(unsigned int ncount)
{
	int i=0;
	for(i=0;i<ncount;i++);
}

#define SPI_PA_NSS	GPIO_Pin_4

#define SPI_PA_CLK	GPIO_Pin_5

#define SPI_PA_MISO	GPIO_Pin_6

#define SPI_PA_MOSI	GPIO_Pin_7

#ifdef SOFT_SPI

void uuu_spi_init(void )
{
		
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = SPI_PA_NSS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SPI_PA_CLK|SPI_PA_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SPI_PA_MISO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	GPIO_WriteBit(GPIOA, SPI_PA_NSS, Bit_SET);
	GPIO_WriteBit(GPIOA, SPI_PA_CLK, Bit_SET);
	GPIO_WriteBit(GPIOA, SPI_PA_MOSI, Bit_SET);
	
	GPIO_WriteBit(GPIOA, SPI_PA_MISO, Bit_SET);
	
	
}


unsigned short uuu_spi_readdata(unsigned char devicev)
{
	unsigned char datav=0;
	unsigned short getdata=0x00;
	unsigned short ret = 0x00;
	unsigned char  bitv=0x00;
	int i=0;

	GPIO_WriteBit(GPIOA, SPI_PA_NSS, Bit_RESET);
	spi_delay(SPI_DELAY_COUNT);
	

	datav = devicev;
	for(i=0;i<8;i++)
	{
		GPIO_WriteBit(GPIOA, SPI_PA_CLK, Bit_RESET);
		spi_delay(SPI_DELAY_COUNT);
		
		bitv = datav >> 7;
		datav = datav << 1;
		
		if(bitv == 1)
		{
			GPIO_WriteBit(GPIOA, SPI_PA_MOSI, Bit_SET);
		}
		else
		{
			GPIO_WriteBit(GPIOA, SPI_PA_MOSI, Bit_RESET);
		}
		
		GPIO_WriteBit(GPIOA, SPI_PA_CLK, Bit_SET);
		spi_delay(SPI_DELAY_COUNT);
		
	}
	
	GPIO_WriteBit(GPIOA, SPI_PA_CLK, Bit_RESET);
	spi_delay(SPI_DELAY_COUNT);

	getdata = 0;
	for(i=0;i<16;i++)
	{
		GPIO_WriteBit(GPIOA, SPI_PA_CLK, Bit_SET);
		spi_delay(SPI_DELAY_COUNT);
		
		GPIO_WriteBit(GPIOA, SPI_PA_CLK, Bit_RESET);
		spi_delay(SPI_DELAY_COUNT);		
		
		bitv = GPIO_ReadInputDataBit(GPIOA, SPI_PA_MISO);
		if(i<12)
		{
			getdata = getdata << 1;
			if(bitv == Bit_SET)
			{
				getdata = getdata | 0x0001;
			}
			else
			{
				getdata = getdata | 0x0000;
			}
		}
	}
	
	ret = getdata;
	
	GPIO_WriteBit(GPIOA, SPI_PA_NSS, Bit_SET);
	
	return ret;
}


#endif


#ifdef HARD_SPI


void uuu_spi_init(void )
{
	SPI_InitTypeDef spi_struct;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	

	GPIO_InitStructure.GPIO_Pin = SPI_PA_NSS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SPI_PA_CLK|SPI_PA_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SPI_PA_MISO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	spi_struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi_struct.SPI_Mode = SPI_Mode_Master;
	spi_struct.SPI_DataSize = SPI_DataSize_8b;
	spi_struct.SPI_CPOL = SPI_CPOL_Low;
	spi_struct.SPI_CPHA = SPI_CPHA_1Edge;
	spi_struct.SPI_NSS = SPI_NSS_Soft;
	spi_struct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	spi_struct.SPI_FirstBit = SPI_FirstBit_MSB;
	spi_struct.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1,&spi_struct);
	
	SPI_Cmd(SPI1,ENABLE);

}


unsigned short uuu_spi_readdata(unsigned char devicev)
{
	unsigned short ret;
	ret = 0xFFFF;
	return ret;
}


void uuu_spi_enable(void )
{
	GPIO_WriteBit(GPIOA, SPI_PA_NSS, Bit_RESET);
}

void  uuu_spi_disable(void )
{
	GPIO_WriteBit(GPIOA, SPI_PA_NSS, Bit_SET);
}

unsigned char uuu_spi_readwrite(unsigned char datav)
{
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, datav);
	
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}


#endif







