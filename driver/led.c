#include "main.h"

/**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void led_init (void)
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	/*开启GPIOB和GPIOF的外设时钟*/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); 
	/*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*选择要控制的GPIOB引脚*/															   
	GPIO_InitStructure.GPIO_Pin = LED1_MAP;	
	/*调用库函数，初始化GPIOB*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);										   
	GPIO_InitStructure.GPIO_Pin = LED2_MAP;	
	/*调用库函数，初始化GPIOB*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);									   
	GPIO_InitStructure.GPIO_Pin = LED3_MAP;	
	/*调用库函数，初始化GPIOB*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	/* 关闭所有led灯	*/ 
	GPIO_ResetBits(GPIOB, LED1_MAP);
	GPIO_SetBits(GPIOB, LED1_MAP); 		
	GPIO_SetBits(GPIOB, LED2_MAP);			 
	GPIO_ResetBits(GPIOB, LED2_MAP);	 
	GPIO_ResetBits(GPIOB, LED3_MAP);	
	GPIO_SetBits(GPIOB, LED3_MAP);	 	 
	LED2 = !LED2;	
}
/*********************************************END OF FILE**********************/
void led_alarm (uint16_t flash_num, uint16_t flash_time, uint16_t cycle)
{
	uint16_t i;
	flash_num <<= 1;
	for (i = 0; i < flash_num; i++){
		LED2 = !LED2;
		delay_ms(flash_time); 
	}
	if (cycle > flash_time){
		delay_ms(cycle - flash_time); 
	}else if (cycle == 0){
	}else{
		delay_ms(1900); 
	}
}
//
void led_output (uint16_t led_ctr)
{
	LED2 = ((led_ctr & 0x01) != 0) ? 0 : 1;
	LED3 = ((led_ctr & 0x02) != 0) ? 0 : 1;
	LED1 = ((led_ctr & 0x04) != 0) ? 0 : 1;
}
//

