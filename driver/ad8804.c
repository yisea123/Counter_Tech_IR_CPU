#include "main.h"


s_ad8804_env ad8804_env;
uint8_t  DA_V = 0;

int ad8804_init (void)
{
	uint8_t i, DA_V;
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE);	 //使能PB,PE端口时钟
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	
//////////////////////////////////////////////////////////////////////////////////////////////
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
	GPIO_SetBits(GPIOB,GPIO_Pin_2);						 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 
	GPIO_Init(GPIOB, &GPIO_InitStructure);					
	GPIO_SetBits(GPIOB,GPIO_Pin_3);						

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				
	GPIO_SetBits(GPIOB,GPIO_Pin_5); 						
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				
	GPIO_SetBits(GPIOD,GPIO_Pin_7); 						 
	
	
	ad8804_env.da_addr[0] = DA1;
	ad8804_env.da_addr[1] = DA2;
	ad8804_env.da_addr[2] = DA3;
	ad8804_env.da_addr[3] = DA4;
	ad8804_env.da_addr[4] = DA5;
	ad8804_env.da_addr[5] = DA6;
	ad8804_env.da_addr[6] = DA7;
	ad8804_env.da_addr[7] = DA8;
	ad8804_env.da_addr[8] = DA9;
	ad8804_env.da_addr[9] = DA10;
	ad8804_env.da_addr[10] = DA11;
	ad8804_env.da_addr[11] = DA12;
	SHDL = 0;
	SPI_CS = 0;
	SPI_SCK = 0;
	SPI_MOSI = 0;
	SHDL = 1;
	SPI_CS = 1;
	SPI_SCK = 1;
	SPI_MOSI = 1;
	
		//my_println ("system start...");
	DA_V = 0;
	for (i = 0; i < 12; i++){
		ad8804_env.da_value[i] = DA_V;
	}
//	ad8804_env.da_value[11] = 82;
//	ad8804_env.da_value[10] = 141;
//	ad8804_env.da_value[9] = 155;
//	ad8804_env.da_value[8] = 255;
	for (i = 0; i < 12; i++){
		ad8804_write_ch (ad8804_env.da_addr[i], ad8804_env.da_value[i]);
	}
	
	return 0;
}

int ad8804_write_ch (uint8_t ch, uint8_t value)
{
	uint8_t mask;
	SPI_SCK = 0;
	SPI_CS = 0;
	for (mask = 0x08; mask > 0; mask >>= 1){
		if (ch & mask){
			SPI_MOSI = 1;
		}else{
			SPI_MOSI = 0;
		}
		SPI_SCK = 1;
		delay_us(SPI_DELAY);
		SPI_SCK = 0;
		delay_us(SPI_DELAY);
	}
	for (mask = 0x80; mask > 0; mask >>= 1){
		if (value & mask){
			SPI_MOSI = 1;
		}else{
			SPI_MOSI = 0;
		}
		SPI_SCK = 1;
		delay_us(SPI_DELAY);
		SPI_SCK = 0;
		delay_us(SPI_DELAY);
	}
	SPI_CS = 1;
	delay_us(SPI_DELAY);
	return 0;
}


