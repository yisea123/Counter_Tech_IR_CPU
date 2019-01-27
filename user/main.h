#ifndef __MAIN_H__
#define __MAIN_H__


#define U32 unsigned int
#define U16 unsigned short
#define S32 int
#define S16 short int
#define U8  unsigned char
#define	S8  char
	

typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	无符号8位整型变量  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		有符号8位整型变量  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	无符号16位整型变量 */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		有符号16位整型变量 */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	无符号32位整型变量 */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		有符号32位整型变量 */
typedef float          fp32;                    /* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double         fp64;                    /* double precision floating point variable (64bits) 双精度浮点数（64位长度） */
	

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stm32f10x.h>
#include "gpio.h"
#include "timer.h"
#include "sys.h"
#include "AD_Sample.h"
#include "dac.h"
#include "ad8804.h"
#include "delay.h"
#include "usart.h"
#include "my_cmd.h"
#include "includes.h"
#include "ModbusRTU.h"
#include "drive_io.h"
#include "sdio_sdcard.h"
#include "rtc.h"
#include "spi.h"
#include "w25qxx.h" 
#include "log.h"
#include "ff.h"			/* Declarations of FatFs API */
#include "stm32flash.h"
#include "MD5.h"

#define UART_DEBUG	
	
#ifdef UART_DEBUG
#define dbg(format, arg...) printf("\r\nDEBUG: " format "\r\nNick-Cmd:", ## arg)
#else
#define dbg(format, arg...) 
#endif

	
#define __USE_COIN_LOG__
#ifdef __USE_COIN_LOG__
#define LOG(format, arg...) coin_log("" format "", ## arg)
#else
#define LOG(format, arg...)
#endif



#define MY_PRINT	
#ifdef MY_PRINT
#define my_print(format, arg...) printf("" format "", ## arg)
#define my_println(format, arg...) printf("" format "\n", ## arg)
#define cmd(format, arg...) printf("Nick-Cmd:" format "", ## arg)
#else
#define my_println(format, arg...) 
#define cmd(format, arg...) 
#endif

#define ASSERT(i) \
{\
	if (i)\
	{\
		my_println(" ASSERT(%d):Error !!! %s, %d", i, __FILE__, __LINE__);\
		while(i);\
	}\
}

#define LIMIT_VALUE_S(V,MIN,MAX) { \
	if (V < MIN){ \
		V = MIN; \
	}else if (V > MAX){ \
		V = MAX; \
	} \
}
#define LIMIT_VALUE_U(V,MAX) { \
	if (V > MAX){ \
		V = MAX; \
	} \
}


#define DATA_RANGE_CHECK() { \
}

extern OS_EVENT * cmd_msg;			//串口消息队列
extern OS_EVENT * debug_msg;			//串口消息队列
extern OS_EVENT * io_msg;			//IO消息队列
extern OS_EVENT * modbusRTU_msg;			//串口消息队列
extern OS_EVENT * ad8804_msg;			//IO消息队列


extern FATFS *fs[_VOLUMES];  

//START 任务 
//设置任务优先级 
#define START_TASK_PRIO 10 ///开始任务的优先级为最低 
//设置任务堆栈大小 
#define START_STK_SIZE 256 

//GROUP 0***************************
#define INT_GROUP_0 0
#define TIM5_INT_PREEM INT_GROUP_0
#define TIM5_INT_SUB 0
#define ADC1_2_INT_PREEM INT_GROUP_0
#define ADC1_2_INT_SUB 1

//GROUP 1***************************
#define INT_GROUP_1 1
#define DMA1_1_INT_PREEM INT_GROUP_1
#define DMA1_1_INT_SUB 2
#define TIM2_INT_PREEM INT_GROUP_1
#define TIM2_INT_SUB 3

//GROUP 2***************************
#define INT_GROUP_2 2
#define SDIO_INT_PREEM INT_GROUP_2
#define SDIO_INT_SUB 1
#define DMA1_2_INT_PREEM INT_GROUP_2
#define DMA1_2_INT_SUB 1
#define DMA1_4_INT_PREEM INT_GROUP_2
#define DMA1_4_INT_SUB 1
#define EXT0_INT_PREEM INT_GROUP_2
#define EXT0_INT_SUB 1
#define EXT1_INT_PREEM INT_GROUP_2
#define EXT1_INT_SUB 1
#define EXT2_INT_PREEM INT_GROUP_2
#define EXT2_INT_SUB 1
#define EXT3_INT_PREEM INT_GROUP_2
#define EXT3_INT_SUB 1
#define EXT6_INT_PREEM INT_GROUP_2
#define EXT6_INT_SUB 1
#define EXT7_INT_PREEM INT_GROUP_2
#define EXT7_INT_SUB 1
#define EXT8_INT_PREEM INT_GROUP_2
#define EXT8_INT_SUB 1
#define EXT11_INT_PREEM INT_GROUP_2
#define EXT11_INT_SUB 1
#define UART3_RX_INT_PREEM INT_GROUP_2
#define UART3_RX_INT_SUB 2
#define UART1_RX_INT_PREEM INT_GROUP_2
#define UART1_RX_INT_SUB 3
#define UART2_RX_INT_PREEM INT_GROUP_2
#define UART2_RX_INT_SUB 3
#define DMA1_6_INT_PREEM INT_GROUP_2
#define DMA1_6_INT_SUB 3

//GROUP 3***************************
#define INT_GROUP_3 3
#define RTC_INT_PREEM INT_GROUP_3
#define RTC_INT_SUB 0
#define TIM3_INT_PREEM INT_GROUP_3
#define TIM3_INT_SUB 2
#define TIM4_INT_PREEM INT_GROUP_3
#define TIM4_INT_SUB 3
#define TIM7_INT_PREEM INT_GROUP_3
#define TIM7_INT_SUB 3
#define DMA2_3_INT_PREEM INT_GROUP_3
#define DMA2_3_INT_SUB 3

#define PLC_ACK virtual_input[0]

typedef enum SYSTEM_STATUS
{
	RUNNING_OK = 0,
	IR_ADJ,
	COUNTER_ERROR,
	STATUS_ERROR,
	ADC_TIME_ERROR,
	DETECTOR_ERROR
}e_system_status;




extern s_32 sys_run_time;//1000us的精度

#endif
