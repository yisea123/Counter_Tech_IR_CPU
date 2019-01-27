#include "main.h"

/**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void led_init (void)
{		
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	/*����GPIOB��GPIOF������ʱ��*/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); 
	/*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	/*������������Ϊ50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*ѡ��Ҫ���Ƶ�GPIOB����*/															   
	GPIO_InitStructure.GPIO_Pin = LED1_MAP;	
	/*���ÿ⺯������ʼ��GPIOB*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);										   
	GPIO_InitStructure.GPIO_Pin = LED2_MAP;	
	/*���ÿ⺯������ʼ��GPIOB*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);									   
	GPIO_InitStructure.GPIO_Pin = LED3_MAP;	
	/*���ÿ⺯������ʼ��GPIOB*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	/* �ر�����led��	*/ 
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

