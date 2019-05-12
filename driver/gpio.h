#ifndef __GPIO_H__
#define __GPIO_H__


#define LED1_MAP GPIO_Pin_9
#define LED2_MAP GPIO_Pin_8
#define LED3_MAP GPIO_Pin_7

////////////////////////////////////////////////// 
#define LED1 PBout(9)// 
#define LED2 PBout(8)// 
#define LED3 PBout(7)// 


void led_init (void);
void led_alarm (uint16_t flash_num, uint16_t flash_time, uint16_t cycle);
void led_output (uint16_t led_ctr);


#endif


