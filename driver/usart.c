#include "main.h"

//uint16_t RS485_FrameEndFlag;

void uart1_send_data (int ch)
{
    //USART1->SR;  //USART_GetFlagStatus(USART1, USART_FLAG_TC) �����һ���ַ�����ʧ�ܵ�����
	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	if(ch == '\n'){
		USART_SendData(USART1, '\r');
		my_env.roll_count++;
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
    //һ��һ�������ַ�
    USART_SendData(USART1, (unsigned char) ch);
    //�ȴ��������
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
	
	//---------------------���ڹ�������---------------------   
	//����DMAʱ��  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
	//DMA1ͨ��4����  
	DMA_DeInit(DMA1_Channel4);  
	//�����ַ  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  
	//�ڴ��ַ  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&uart1_dma_buf[0]);  
	//dma���䷽����  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
	//����DMA�ڴ���ʱ�������ĳ���  
	DMA_InitStructure.DMA_BufferSize = UART_DMA_BUF_SIZE;  
	//����DMA���������ģʽ��һ������  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	//����DMA���ڴ����ģʽ  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	//���������ֳ�  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	//�ڴ������ֳ�  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
	//����DMA�Ĵ���ģʽ  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	//����DMA�����ȼ���  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
	//����DMA��2��memory�еı����������  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel4,&DMA_InitStructure);  
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);     
	
	//DMA�����ж�����  
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_4_INT_PREEM;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_4_INT_SUB;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);    
	//ʹ��ͨ��4  
	//DMA_Cmd(DMA1_Channel4, ENABLE);  
	
	//������DMA����    
    //����DMAʱ��  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
    //DMA1ͨ��5����  
    DMA_DeInit(DMA1_Channel5);  
    //�����ַ  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  
    //�ڴ��ַ  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)cmd_analyze.rec_buf;  
    //dma���䷽����  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
    //����DMA�ڴ���ʱ�������ĳ���  
    DMA_InitStructure.DMA_BufferSize = CMD_BUF_LEN;  
    //����DMA���������ģʽ��һ������  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    //����DMA���ڴ����ģʽ  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //���������ֳ�  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //�ڴ������ֳ�  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
    //����DMA�Ĵ���ģʽ  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    //����DMA�����ȼ���  
    DMA_InitStructure.DMA_Priority = DMA_Mode_Circular;  
    //����DMA��2��memory�еı����������  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
    DMA_Init(DMA1_Channel5,&DMA_InitStructure);  
    //ʹ��ͨ��5  
    DMA_Cmd(DMA1_Channel5,ENABLE);  
    //����DMA��ʽ����  
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);  
}
//����1 DMA��ʽ�����ж�  

void DMA1_Channel4_IRQHandler(void)  
{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif	
	if(DMA_GetITStatus(DMA1_IT_TC4)){
		//�����־λ  
		DMA_ClearFlag(DMA1_FLAG_TC4);//���ͨ��4������ɱ�־
		DMA_ClearITPendingBit(DMA1_IT_GL4); //���ȫ���жϱ�־ 
		USART_DMACmd(USART1,USART_DMAReq_Tx, DISABLE); //DISABLE����1��DMA����  
		DMA_Cmd(DMA1_Channel4, DISABLE);  

		if (my_env.tty == TTY_MODBUS){
			OSQPost(modbusRTU_msg, (void *) 0xaa);//������Ϣ
		}else{
			OSQPost(debug_msg, (void *) 0xaa);//������Ϣ
		}
	}
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
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
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //ʹ�ܴ���1��DMA����   
}

void start_uart3_dma (u32 _memBaseAddr, uint16_t _size)
{
	/*--------------------------- DMAy Channelx CMAR Configuration ----------------*/
	/* Write to DMA1 Channel2 CMAR */
	DMA1_Channel2->CMAR = _memBaseAddr;
	DMA_SetCurrDataCounter (DMA1_Channel2, _size);
	DMA_Cmd(DMA1_Channel2, ENABLE);  
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE); //ʹ�ܴ���1��DMA����   
}
void uart1_init (void)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART1_RX_INT_PREEM;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART1_RX_INT_SUB;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = UART1_BOUND_RATE;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_DeInit (USART1);
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж� 
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE); //�������ڿ����ж� 
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
	
	uart1_dma_config ();

}


void stop_uart1_receive (void)
{
	DMA_Cmd(DMA1_Channel5,DISABLE);
	USART_ITConfig(USART1,USART_IT_IDLE, DISABLE); //�������ڿ����ж� 
}
void start_uart1_receive (void)
{        
	//���ô������ݳ���  
	memset (cmd_analyze.rec_buf, 0, CMD_BUF_LEN);
	DMA_Cmd(DMA1_Channel5,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel5, CMD_BUF_LEN);  
	DMA_Cmd(DMA1_Channel5,ENABLE);
	USART_ITConfig(USART1,USART_IT_IDLE, ENABLE); //�������ڿ����ж� 
}


#if EN_USART1_RX   //���ʹ���˽���
//����1�����ж�     
void USART1_IRQHandler(void)                                 
{     
    uint32_t temp = 0;  
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET){  
			USART_ClearFlag(USART1,USART_IT_IDLE);  
			temp = USART1->SR;  
			temp = USART1->DR; //��USART_IT_IDLE��־  
			temp = temp;
			//DMA_Cmd(DMA1_Channel5,DISABLE);
			OSQPost(cmd_msg, (void *) 0x55);//������Ϣ	 
    }   
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif 
} 
//void USART1_IRQHandler(void)                	//����1�жϷ������
//{
//	__IO u8 Res;
//#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//	OSIntEnter();    
//#endif
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//	{
//		Res = USART_ReceiveData(USART1);	//��ȡ���յ�������	
//		fill_rec_buf((char)Res);
//	}else{
//		Res = USART_ReceiveData(USART1);	//��ȡ���յ�������
//	}
//	Res = USART1->SR;
//	 
//#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//	OSIntExit();  											 
//#endif
//} 
#endif





//




void uart2_dma_config (void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//---------------------���ڹ�������---------------------   
	
	//������DMA����    
	//����DMAʱ��  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
	//DMA1ͨ��5����  
	DMA_DeInit(DMA1_Channel6);  
	//�����ַ  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);  
	//�ڴ��ַ  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)cmd_analyze.emitter_cmd;  
	//dma���䷽����  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
	//����DMA�ڴ���ʱ�������ĳ���  
	DMA_InitStructure.DMA_BufferSize = CMD_BUF_LEN;  
	//����DMA���������ģʽ��һ������  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	//����DMA���ڴ����ģʽ  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	//���������ֳ�  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	//�ڴ������ֳ�  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
	//����DMA�Ĵ���ģʽ  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	//����DMA�����ȼ���  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
	//����DMA��2��memory�еı����������  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel6,&DMA_InitStructure); 
		
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE); //��������ж�

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_6_INT_PREEM;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_6_INT_SUB;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	//ʹ��ͨ��5  
	DMA_Cmd(DMA1_Channel6,ENABLE);  
	//����DMA��ʽ����  
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);  
}
//����2 DMA��ʽ�����ж�  
void stop_uart2_receive (void)
{
	DMA_Cmd(DMA1_Channel6,DISABLE);
	USART_ITConfig(USART2,USART_IT_IDLE, DISABLE); //�������ڿ����ж� 
}
void start_uart2_receive (void)
{        
	//���ô������ݳ���  
	memset (cmd_analyze.emitter_cmd, 0, CMD_BUF_LEN);
	DMA_Cmd(DMA1_Channel6,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel6, CMD_BUF_LEN);  
	DMA_Cmd(DMA1_Channel6,ENABLE);
	USART_ITConfig(USART2,USART_IT_IDLE, ENABLE); //�������ڿ����ж� 
}
//����2����
void uart2_init (void)
{
	//GPIO�˿�����
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOD5
   
	//USART2_RX	  GPIOD6��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PD6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOD6 

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART2_RX_INT_PREEM;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART2_RX_INT_SUB;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = UART2_BOUND_RATE;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_DeInit (USART2);
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж� 
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE); //�������ڿ����ж� 
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2
	
	uart2_dma_config ();

}

//����2
#if EN_USART2_RX   //���ʹ���˽���
//����2�����ж�     
void USART2_IRQHandler(void)                                 
{     
    uint32_t temp = 0;  
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif    
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  
    {  
			temp = USART2->SR;  
			temp = USART2->DR; //��USART_IT_IDLE��־  
			temp = temp;
			OSQPost(cmd_msg, (void *) 0xaa);//������Ϣ	 
    }   
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif 
} 
//} 
#endif
//

//����1 DMA��ʽ�������ж�
void DMA1_Channel6_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(DMA_GetITStatus(DMA1_IT_TC6)){
		//�����־λ  
		DMA_ClearFlag(DMA1_FLAG_TC6);//���ͨ��4������ɱ�־
		DMA_ClearITPendingBit(DMA1_IT_GL6); //���ȫ���жϱ�־ 
		OSQPost(cmd_msg, (void *) 0xab);//������Ϣ
	}
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}
//


void uart2_send_data (int ch)
{
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
    //һ��һ�������ַ�
    USART_SendData(USART2, (unsigned char) ch);
    //�ȴ��������
    while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
}
void uart2_puts(const char *s)
{
	while (*s) {
		uart2_send_data(*s++);
	}
}


//����3 DMA��ʽ�����ж�  
void DMA1_Channel2_IRQHandler(void)  
{  
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(DMA_GetITStatus(DMA1_IT_TC2)){
		//�����־λ  
		DMA_ClearFlag(DMA1_FLAG_TC2);//���ͨ��4������ɱ�־
		DMA_ClearITPendingBit(DMA1_IT_GL2); //���ȫ���жϱ�־ 
		USART_DMACmd(USART3,USART_DMAReq_Tx, DISABLE); //DISABLE����1��DMA����  
		DMA_Cmd(DMA1_Channel2, DISABLE);  
		OSQPost(modbusRTU_msg, (void *) 0xaa);//������Ϣ
	}
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
void uart3_dma_config (void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//---------------------���ڹ�������---------------------   
	//����DMAʱ��  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
	//DMA1ͨ��2����  
	DMA_DeInit(DMA1_Channel2);  
	//�����ַ  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);  
	//�ڴ��ַ  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&RS485_tx_buf[0]);  
	//dma���䷽����  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
	//����DMA�ڴ���ʱ�������ĳ���  
	DMA_InitStructure.DMA_BufferSize = UART3_BUF_LEN;  
	//����DMA���������ģʽ��һ������  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	//����DMA���ڴ����ģʽ  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	//���������ֳ�  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
	//�ڴ������ֳ�  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
	//����DMA�Ĵ���ģʽ  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
	//����DMA�����ȼ���  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
	//����DMA��2��memory�еı����������  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA1_Channel2,&DMA_InitStructure);  
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);  
	
	//DMA�����ж�����  
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_2_INT_PREEM;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_2_INT_SUB;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);    
	//ʹ��ͨ��2  
	//DMA_Cmd(DMA1_Channel2, ENABLE);  
	
	//������DMA����    
    //����DMAʱ��  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
    //DMA1ͨ��3����  
    DMA_DeInit(DMA1_Channel3);  
    //�����ַ  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);  
    //�ڴ��ַ  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart3_rec_buf;  
    //dma���䷽����  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
    //����DMA�ڴ���ʱ�������ĳ���  
    DMA_InitStructure.DMA_BufferSize = UART3_BUF_LEN;  
    //����DMA���������ģʽ��һ������  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    //����DMA���ڴ����ģʽ  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //���������ֳ�  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //�ڴ������ֳ�  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
    //����DMA�Ĵ���ģʽ  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    //����DMA�����ȼ���  
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    //����DMA��2��memory�еı����������  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);  
    //ʹ��ͨ��5  
    DMA_Cmd(DMA1_Channel3, ENABLE);  
    //����DMA��ʽ����  
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);  
}

void uart3_init (void)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3ʱ��
  
	//USART3_TX   GPIOB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB10
   
	//USART3_RX	  GPIOB11��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB11

	//Usart3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART3_RX_INT_PREEM;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART3_RX_INT_SUB;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = UART3_BOUND_RATE;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_DeInit (USART3);
	USART_Init(USART3, &USART_InitStructure); //��ʼ������3
	//USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE); //�������ڿ����ж� 
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3 
	
	uart3_dma_config ();
}

void stop_uart3_receive (void)
{
	DMA_Cmd(DMA1_Channel3,DISABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, DISABLE); //�������ڿ����ж� 
}
void start_uart3_receive (void)
{        
	//���ô������ݳ���  
	delay_ms(5); 
	memset (uart3_rec_buf, 0, UART3_BUF_LEN);
	DMA_Cmd(DMA1_Channel3,DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel3, UART3_BUF_LEN);  
	DMA_Cmd(DMA1_Channel3,ENABLE);
	USART_ITConfig(USART3,USART_IT_IDLE, ENABLE); //�������ڿ����ж� 
	HMI_RS485_TX_EN = 0;//�л�Ϊ����ģʽ
}

#if EN_USART3_RX   //���ʹ���˽���
unsigned char uart3_rec_buf[UART3_BUF_LEN];
uint16_t uart3_rec_count = 0;
void USART3_IRQHandler(void)                                 
{     
    uint32_t temp = 0;  
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif    
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET){  
      //USART_ClearFlag(USART3,USART_IT_IDLE);  
      temp = USART3->SR;  
      temp = USART3->DR; //��USART_IT_IDLE��־  
			temp = temp;
			OSQPost(modbusRTU_msg, (void *) 0xA5);//������Ϣ	 
//			TIM_SetCounter(TIM7,0);//�����յ�һ���µ��ֽڣ�����ʱ��7��λΪ0�����¼�ʱ���൱��ι����
//			TIM_Cmd(TIM7,ENABLE);//��ʼ��ʱ 
    }   
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif 
} 
//void USART3_IRQHandler(void)                	//����3�жϷ������
//{
//	__IO u8 Res;
//	//Res = USART3->SR;
//#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//	OSIntEnter();    
//#endif
//	
//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//	{
//	#if (MODBUS_RTU_USE_UART == 3)
//		TIM_SetCounter(TIM7,0);//�����յ�һ���µ��ֽڣ�����ʱ��7��λΪ0�����¼�ʱ���൱��ι����
//		TIM_Cmd(TIM7,ENABLE);//��ʼ��ʱ
//	#endif
//		uart3_rec_buf[uart3_rec_count++] = USART_ReceiveData(USART3);	//��ȡ���յ�������
//	}else{
//		Res = USART_ReceiveData(USART3);	//��ȡ���յ�������
//	}
//	 
//#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//	OSIntExit();  											 
//#endif
//} 
#endif











