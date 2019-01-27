#include "main.h"

//uint16_t RS485_FrameEndFlag;

void uart1_send_data (int ch)
{
    //USART1->SR;  //USART_GetFlagStatus(USART1, USART_FLAG_TC) 解决第一个字符发送失败的问题
	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	if(ch == '\n'){
		USART_SendData(USART1, '\r');
		my_env.roll_count++;
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
    //一个一个发送字符
    USART_SendData(USART1, (unsigned char) ch);
    //等待发送完成
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
}

void uart1_puts(const char *s)
{
	while (*s) {
		uart1_send_data(*s++);
	}
}

#define UART_DMA_BUF_SIZE 1
uint8_t uart1_dma_buf[UART_DMA_BUF_SIZE];

void uart1_dma_config (void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//---------------------串口功能配置---------------------   
	//启动DMA时钟  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
	//DMA1通道4配置  
	DMA_DeInit(DMA1_Channel4);  
	//外设地址  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  
	//内存地址  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&uart1_dma_buf[0]);  
	//dma传输方向单向  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
	//设置DMA在传输时缓冲区的长度  
	DMA_InitStructure.DMA_BufferSize = UART_DMA_BUF_SIZE;  
	//设置DMA的外设递增模式，一个外设  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	//设置DMA的内存递增模式  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	//外设数据字长  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	//内存数据字长  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
	//设置DMA的传输模式  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	//设置DMA的优先级别  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
	//设置DMA的2个memory中的变量互相访问  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel4,&DMA_InitStructure);  
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);     
	
	//DMA发送中断设置  
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_4_INT_PREEM;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_4_INT_SUB;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);    
	//使能通道4  
	//DMA_Cmd(DMA1_Channel4, ENABLE);  
	
	//串口收DMA配置    
    //启动DMA时钟  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
    //DMA1通道5配置  
    DMA_DeInit(DMA1_Channel5);  
    //外设地址  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  
    //内存地址  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)cmd_analyze.rec_buf;  
    //dma传输方向单向  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
    //设置DMA在传输时缓冲区的长度  
    DMA_InitStructure.DMA_BufferSize = CMD_BUF_LEN;  
    //设置DMA的外设递增模式，一个外设  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    //设置DMA的内存递增模式  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //外设数据字长  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //内存数据字长  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
    //设置DMA的传输模式  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    //设置DMA的优先级别  
    DMA_InitStructure.DMA_Priority = DMA_Mode_Circular;  
    //设置DMA的2个memory中的变量互相访问  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
    DMA_Init(DMA1_Channel5,&DMA_InitStructure);  
    //使能通道5  
    DMA_Cmd(DMA1_Channel5,ENABLE);  
    //采用DMA方式接收  
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);  
}
//串口1 DMA方式发送中断  

void DMA1_Channel4_IRQHandler(void)  
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif	
	if(DMA_GetITStatus(DMA1_IT_TC4)){
		//清除标志位  
		DMA_ClearFlag(DMA1_FLAG_TC4);//清除通道4传输完成标志
		DMA_ClearITPendingBit(DMA1_IT_GL4); //清除全部中断标志 
		USART_DMACmd(USART1,USART_DMAReq_Tx, DISABLE); //DISABLE串口1的DMA发送  
		DMA_Cmd(DMA1_Channel4, DISABLE);  

		if (my_env.tty == TTY_MODBUS){
			OSQPost(modbusRTU_msg, (void *) 0xaa);//发送消息
		}else{
			OSQPost(debug_msg, (void *) 0xaa);//发送消息
		}
	}
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 

void start_uart1_dma (u32 _memBaseAddr, uint16_t _size)
{
	/*--------------------------- DMAy Channelx CMAR Configuration ----------------*/
	/* Write to DMA1 Channel4 CMAR */
	DMA1_Channel4->CMAR = _memBaseAddr;
	DMA_SetCurrDataCounter (DMA1_Channel4, _size);
	DMA_Cmd(DMA1_Channel4, ENABLE);  
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //使能串口1的DMA发送   
}

void start_uart3_dma (u32 _memBaseAddr, uint16_t _size)
{
	/*--------------------------- DMAy Channelx CMAR Configuration ----------------*/
	/* Write to DMA1 Channel2 CMAR */
	DMA1_Channel2->CMAR = _memBaseAddr;
	DMA_SetCurrDataCounter (DMA1_Channel2, _size);
	DMA_Cmd(DMA1_Channel2, ENABLE);  
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE); //使能串口1的DMA发送   
}
void uart1_init (void)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART1_RX_INT_PREEM;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART1_RX_INT_SUB;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = UART1_BOUND_RATE;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_DeInit (USART1);
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断 
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE); //开启串口空闲中断 
	USART_Cmd(USART1, ENABLE);                    //使能串口1 
	
	uart1_dma_config ();

}


void stop_uart1_receive (void)
{
	DMA_Cmd(DMA1_Channel5,DISABLE);
	USART_ITConfig(USART1,USART_IT_IDLE, DISABLE); //开启串口空闲中断 
}
void start_uart1_receive (void)
{        
	//设置传输数据长度  
	memset (cmd_analyze.rec_buf, 0, CMD_BUF_LEN);
	DMA_Cmd(DMA1_Channel5,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel5, CMD_BUF_LEN);  
	DMA_Cmd(DMA1_Channel5,ENABLE);
	USART_ITConfig(USART1,USART_IT_IDLE, ENABLE); //开启串口空闲中断 
}


#if EN_USART1_RX   //如果使能了接收
//串口1接收中断     
void USART1_IRQHandler(void)                                 
{     
    uint32_t temp = 0;  
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET){  
			USART_ClearFlag(USART1,USART_IT_IDLE);  
			temp = USART1->SR;  
			temp = USART1->DR; //清USART_IT_IDLE标志  
			temp = temp;
			//DMA_Cmd(DMA1_Channel5,DISABLE);
			OSQPost(cmd_msg, (void *) 0x55);//发送消息	 
    }   
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif 
} 
//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//	__IO u8 Res;
//#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//	OSIntEnter();    
//#endif
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		Res = USART_ReceiveData(USART1);	//读取接收到的数据	
//		fill_rec_buf((char)Res);
//	}else{
//		Res = USART_ReceiveData(USART1);	//读取接收到的数据
//	}
//	Res = USART1->SR;
//	 
//#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//	OSIntExit();  											 
//#endif
//} 
#endif





//




void uart2_dma_config (void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//---------------------串口功能配置---------------------   
	
	//串口收DMA配置    
	//启动DMA时钟  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
	//DMA1通道5配置  
	DMA_DeInit(DMA1_Channel6);  
	//外设地址  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);  
	//内存地址  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)cmd_analyze.emitter_cmd;  
	//dma传输方向单向  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
	//设置DMA在传输时缓冲区的长度  
	DMA_InitStructure.DMA_BufferSize = CMD_BUF_LEN;  
	//设置DMA的外设递增模式，一个外设  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	//设置DMA的内存递增模式  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	//外设数据字长  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	//内存数据字长  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
	//设置DMA的传输模式  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	//设置DMA的优先级别  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
	//设置DMA的2个memory中的变量互相访问  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel6,&DMA_InitStructure); 
		
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE); //传输结束中断

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_6_INT_PREEM;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_6_INT_SUB;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	//使能通道5  
	DMA_Cmd(DMA1_Channel6,ENABLE);  
	//采用DMA方式接收  
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);  
}
//串口2 DMA方式发送中断  
void stop_uart2_receive (void)
{
	DMA_Cmd(DMA1_Channel6,DISABLE);
	USART_ITConfig(USART2,USART_IT_IDLE, DISABLE); //开启串口空闲中断 
}
void start_uart2_receive (void)
{        
	//设置传输数据长度  
	memset (cmd_analyze.emitter_cmd, 0, CMD_BUF_LEN);
	DMA_Cmd(DMA1_Channel6,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel6, CMD_BUF_LEN);  
	DMA_Cmd(DMA1_Channel6,ENABLE);
	USART_ITConfig(USART2,USART_IT_IDLE, ENABLE); //开启串口空闲中断 
}
//串口2配置
void uart2_init (void)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD ,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE); 
  
	//USART2_TX   GPIOD5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PD5
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOD5
   
	//USART2_RX	  GPIOD6初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PD6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOD6 

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART2_RX_INT_PREEM;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART2_RX_INT_SUB;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = UART2_BOUND_RATE;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_DeInit (USART2);
	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断 
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE); //开启串口空闲中断 
	USART_Cmd(USART2, ENABLE);                    //使能串口2
	
	uart2_dma_config ();

}

//串口2
#if EN_USART2_RX   //如果使能了接收
//串口2接收中断     
void USART2_IRQHandler(void)                                 
{     
    uint32_t temp = 0;  
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif    
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  
    {  
			temp = USART2->SR;  
			temp = USART2->DR; //清USART_IT_IDLE标志  
			temp = temp;
			OSQPost(cmd_msg, (void *) 0xaa);//发送消息	 
    }   
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif 
} 
//} 
#endif
//

//串口1 DMA方式接收满中断
void DMA1_Channel6_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(DMA_GetITStatus(DMA1_IT_TC6)){
		//清除标志位  
		DMA_ClearFlag(DMA1_FLAG_TC6);//清除通道4传输完成标志
		DMA_ClearITPendingBit(DMA1_IT_GL6); //清除全部中断标志 
		OSQPost(cmd_msg, (void *) 0xab);//发送消息
	}
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}
//


void uart2_send_data (int ch)
{
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
    //一个一个发送字符
    USART_SendData(USART2, (unsigned char) ch);
    //等待发送完成
    while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
}
void uart2_puts(const char *s)
{
	while (*s) {
		uart2_send_data(*s++);
	}
}


//串口3 DMA方式发送中断  
void DMA1_Channel2_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(DMA_GetITStatus(DMA1_IT_TC2)){
		//清除标志位  
		DMA_ClearFlag(DMA1_FLAG_TC2);//清除通道4传输完成标志
		DMA_ClearITPendingBit(DMA1_IT_GL2); //清除全部中断标志 
		USART_DMACmd(USART3,USART_DMAReq_Tx, DISABLE); //DISABLE串口1的DMA发送  
		DMA_Cmd(DMA1_Channel2, DISABLE);  
		OSQPost(modbusRTU_msg, (void *) 0xaa);//发送消息
	}
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
void uart3_dma_config (void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//---------------------串口功能配置---------------------   
	//启动DMA时钟  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
	//DMA1通道2配置  
	DMA_DeInit(DMA1_Channel2);  
	//外设地址  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);  
	//内存地址  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&RS485_tx_buf[0]);  
	//dma传输方向单向  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
	//设置DMA在传输时缓冲区的长度  
	DMA_InitStructure.DMA_BufferSize = UART3_BUF_LEN;  
	//设置DMA的外设递增模式，一个外设  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	//设置DMA的内存递增模式  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	//外设数据字长  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	//内存数据字长  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
	//设置DMA的传输模式  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	//设置DMA的优先级别  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
	//设置DMA的2个memory中的变量互相访问  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel2,&DMA_InitStructure);  
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);  
	
	//DMA发送中断设置  
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_2_INT_PREEM;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_2_INT_SUB;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);    
	//使能通道2  
	//DMA_Cmd(DMA1_Channel2, ENABLE);  
	
	//串口收DMA配置    
    //启动DMA时钟  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
    //DMA1通道3配置  
    DMA_DeInit(DMA1_Channel3);  
    //外设地址  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);  
    //内存地址  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart3_rec_buf;  
    //dma传输方向单向  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
    //设置DMA在传输时缓冲区的长度  
    DMA_InitStructure.DMA_BufferSize = UART3_BUF_LEN;  
    //设置DMA的外设递增模式，一个外设  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    //设置DMA的内存递增模式  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //外设数据字长  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //内存数据字长  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
    //设置DMA的传输模式  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    //设置DMA的优先级别  
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    //设置DMA的2个memory中的变量互相访问  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);  
    //使能通道5  
    DMA_Cmd(DMA1_Channel3, ENABLE);  
    //采用DMA方式接收  
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);  
}

void uart3_init (void)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟
  
	//USART3_TX   GPIOB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB10
   
	//USART3_RX	  GPIOB11初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB11

	//Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART3_RX_INT_PREEM;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART3_RX_INT_SUB;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = UART3_BOUND_RATE;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_DeInit (USART3);
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
	//USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE); //开启串口空闲中断 
	USART_Cmd(USART3, ENABLE);                    //使能串口3 
	
	uart3_dma_config ();
}

void stop_uart3_receive (void)
{
	DMA_Cmd(DMA1_Channel3,DISABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, DISABLE); //开启串口空闲中断 
}
void start_uart3_receive (void)
{        
	//设置传输数据长度  
	delay_ms(5); 
	memset (uart3_rec_buf, 0, UART3_BUF_LEN);
	DMA_Cmd(DMA1_Channel3,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel3, UART3_BUF_LEN);  
	DMA_Cmd(DMA1_Channel3,ENABLE);
	USART_ITConfig(USART3,USART_IT_IDLE, ENABLE); //开启串口空闲中断 
	HMI_RS485_TX_EN = 0;//切换为接收模式
}

#if EN_USART3_RX   //如果使能了接收
unsigned char uart3_rec_buf[UART3_BUF_LEN];
uint16_t uart3_rec_count = 0;
void USART3_IRQHandler(void)                                 
{     
    uint32_t temp = 0;  
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif    
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET){  
      //USART_ClearFlag(USART3,USART_IT_IDLE);  
      temp = USART3->SR;  
      temp = USART3->DR; //清USART_IT_IDLE标志  
			temp = temp;
			OSQPost(modbusRTU_msg, (void *) 0xA5);//发送消息	 
//			TIM_SetCounter(TIM7,0);//当接收到一个新的字节，将定时器7复位为0，重新计时（相当于喂狗）
//			TIM_Cmd(TIM7,ENABLE);//开始计时 
    }   
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif 
} 
//void USART3_IRQHandler(void)                	//串口3中断服务程序
//{
//	__IO u8 Res;
//	//Res = USART3->SR;
//#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//	OSIntEnter();    
//#endif
//	
//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//	#if (MODBUS_RTU_USE_UART == 3)
//		TIM_SetCounter(TIM7,0);//当接收到一个新的字节，将定时器7复位为0，重新计时（相当于喂狗）
//		TIM_Cmd(TIM7,ENABLE);//开始计时
//	#endif
//		uart3_rec_buf[uart3_rec_count++] = USART_ReceiveData(USART3);	//读取接收到的数据
//	}else{
//		Res = USART_ReceiveData(USART3);	//读取接收到的数据
//	}
//	 
//#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//	OSIntExit();  											 
//#endif
//} 
#endif











