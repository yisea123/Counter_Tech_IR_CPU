#include "main.h"

int null_temp_value;

void drive_io_init (void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟
	/*开启GPIO外设时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); 
	//////////////////////////////////////////////////////////////////////
	GPIO_DeInit (GPIOE);
	//////////////////////////////////////////////////////////////////////
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*选择要配置的引脚*/															   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
																GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |
																GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
																GPIO_Pin_12 | GPIO_Pin_13;
	/* 关闭所有输出	*/	 
	GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
											GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |
											GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
											GPIO_Pin_12 | GPIO_Pin_13);	
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
}
























