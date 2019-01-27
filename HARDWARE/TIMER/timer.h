#ifndef __TIMER_H
#define __TIMER_H

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
#define PMW_WIDTH  100
 

#define SYS_CLK 72
#define TIM2_ARR  999 //1000us
#define TIM2_PSC  71

#define TIM3_ARR  999
#define TIM3_PSC  71

#define TIM4_ARR  999
#define TIM4_PSC  71

#define TIM5_ARR  65535
#define TIM5_PSC  71

//#define TIM6_ARR  31250
#define TIM6_ARR  (((8 * 1000) / 32) - 1) // (((N * 1000) / 32) - 1) N ms 发完一个DAC脉冲
#define TIM6_PSC  71

#define TIM7_ARR  3999
#define TIM7_PSC  71

#define TIM2_INT_CYCLE (((TIM2_ARR + 1) * (TIM2_PSC + 1)) / SYS_CLK) //us
#define TIM3_INT_CYCLE (((TIM3_ARR + 1) * (TIM3_PSC + 1)) / SYS_CLK) //us
#define TIM4_INT_CYCLE (((TIM4_ARR + 1) * (TIM4_PSC + 1)) / SYS_CLK) //us
#define TIM5_INT_CYCLE (((TIM5_ARR + 1) * (TIM5_PSC + 1)) / SYS_CLK) //us
#define TIM6_INT_CYCLE (((TIM6_ARR + 1) * (TIM6_PSC + 1)) / SYS_CLK) //us
#define TIM7_INT_CYCLE (((TIM7_ARR + 1) * (TIM7_PSC + 1)) / SYS_CLK) //us

#define COUNTER_SYSTEM_ONE_SECOND_CTR (((1000/TIM2_INT_CYCLE) * 1000)-1)

#define get_tim5_ticks() (TIM5->CNT)/*(tim5_ticks << 16) + */
#define get_sys_run_time() (sys_run_time.data_hl)

void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM4_PWM_Init(u16 arr,u16 psc);
void TIM5_PWM_Init(u16 arr,u16 psc);
void TIM6_PWM_Init(u16 arr,u16 psc);
void TIM7_PWM_Init(u16 arr,u16 psc);

unsigned long long get_tim5_ticks_old (void);
void refresh_dma1_cycle (void);
void refresh_adc1_cycle (void);


extern unsigned long long tim5_cur_cnt;
extern unsigned long long tim5_pre_cnt;
extern unsigned long long tim5_ticks;

extern uint16_t rand_tmp;
extern u16 tim2_irq_process_time;

#endif
