#include "main.h"

/* Private variables ---------------------------------------------------------*/
const uint16_t Sine12bit[32] = {
                      4056,3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 
                      909, 599, 344, 155, 38, 0, 38, 155, 344, 599, 909,1263,  
                      1647, 2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, };

void dac_init (void)
{
  
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //使能PORTA通道时钟
   	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //使能DAC通道时钟 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // 端口配置
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //模拟输入
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4)	;//PA.4 输出高
					
	DAC_InitType.DAC_Trigger=DAC_Trigger_None;	//不使用触发功能 TEN1=0
	//DAC_InitType.DAC_Trigger=DAC_Trigger_T6_TRGO;	//使用T6_TRGO触发功能
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//不使用波形发生
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//屏蔽、幅值设置
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	//DAC1输出缓存关闭 BOFF1=1
    DAC_Init(DAC_Channel_1,&DAC_InitType);	 //初始化DAC通道1
    DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值

	DMA_DeInit(DMA2_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine12bit;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 32;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA2_Channel3, &DMA_InitStructure);
	
	DMA_ITConfig(DMA2_Channel3, DMA_IT_TC,ENABLE); //传输结束中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_3_INT_PREEM;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA2_3_INT_SUB;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;
	NVIC_Init(&NVIC_InitStructure);   

	DMA_Cmd(DMA2_Channel3, ENABLE); //使能 DMA2 Channel3
	DAC_DMACmd(DAC_Channel_1, ENABLE);  //使能 DAC1 DMA
	DAC_Cmd(DAC_Channel_1, ENABLE);  //使能 DAC1
//	
	dac1_set_vol (4095);
	DAC_InitType.DAC_Trigger=DAC_Trigger_T6_TRGO;	//使用T6_TRGO触发功能
    DAC_Init(DAC_Channel_1, &DAC_InitType);	 //初始化DAC通道1

}
void DMA2_Channel3_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif	
	if(DMA_GetITStatus(DMA2_IT_TC3)){
		DMA_ClearITPendingBit(DMA2_IT_GL3); //清除全部中断标志
		TIM_Cmd(TIM6, DISABLE);  //停止定时器TIM6
	} 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}


//设置通道1输出电压
//vol:0~3300,代表0~3.3V
void dac1_set_vol(u16 vol)
{
//	float temp=vol;
//	temp/=1000;
//	temp=temp*4096/3.3;
	DAC_SetChannel1Data(DAC_Align_12b_R, vol);//12位右对齐数据格式设置DAC值
}



