#include "main.h"



vu16 process_rdy = PROCESS_RDY + PROCESS_RDY;

s_AD_buf AD_DMA_buf;
u16 After_filter[CHANEL_NUM]; //用来存放求平均值之后的结果


s_counter_env counter_env;
s_counter_info g_counter;
//vu16 Detect_Buf[DETECTG_BUF_GROUP][DETECTG_BUF_SIZE];
vu16 Detect_Buf[DETECTG_BUF_SIZE];
s_buf AD_buff;



void AD_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启GPIOB和GPIOF的外设时钟*/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE, ENABLE);
	//PA0/1/2/3 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
																GPIO_Pin_4| GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//PB0/1 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PC0/1/2/3/4/5 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);

//////////////////////////////////////////////////////////////////////////////////////////////
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;				 //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOG.15
	PDout(0) = 0;
	PDout(1) = 1;
	PDout(2) = 1;
	PDout(3) = 0;
}



void set_adc1_sample_time (void)
{
	#define SAMPLE_TIME sample_time//ADC_SampleTime_28Cycles5// ADC_SampleTime_7Cycles5 //ADC_SampleTime_71Cycles5 //ADC_SampleTime_239Cycles5
	uint8_t sample_time = ADC_SampleTime_55Cycles5;
		//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	switch (g_counter.set_adc_mode)
	{
//		case 0:
//			sample_time = ADC_SampleTime_1Cycles5;
//			break;
		case 1:
			sample_time = ADC_SampleTime_7Cycles5;
			break;
		case 2:
			sample_time = ADC_SampleTime_13Cycles5;
			break;
		case 3:
			sample_time = ADC_SampleTime_28Cycles5;
			break;
		case 4:
			sample_time = ADC_SampleTime_41Cycles5;
			break;
		case 5:
			sample_time = ADC_SampleTime_55Cycles5;
			break;
		case 6:
			sample_time = ADC_SampleTime_71Cycles5;
			break;
		case 7:
			sample_time = ADC_SampleTime_239Cycles5;
			break;
		default:break;
	}
	ADC_RegularChannelConfig(ADC1, CH0, 1, SAMPLE_TIME );
	ADC_RegularChannelConfig(ADC1, CH1, 2, SAMPLE_TIME );
	ADC_RegularChannelConfig(ADC1, CH2, 3, SAMPLE_TIME );
	ADC_RegularChannelConfig(ADC1, CH3, 4, SAMPLE_TIME );
	ADC_RegularChannelConfig(ADC1, CH4, 5, SAMPLE_TIME );
	ADC_RegularChannelConfig(ADC1, CH5, 6, SAMPLE_TIME );
	ADC_RegularChannelConfig(ADC1, CH6, 7, SAMPLE_TIME );
	ADC_RegularChannelConfig(ADC1, CH7, 8, SAMPLE_TIME );
	ADC_RegularChannelConfig(ADC1, CH8, 9, SAMPLE_TIME );
	ADC_RegularChannelConfig(ADC1, CH9, 10, SAMPLE_TIME );
	ADC_RegularChannelConfig(ADC1, CH10, 11, SAMPLE_TIME );
	ADC_RegularChannelConfig(ADC1, CH11, 12, SAMPLE_TIME );
}

void ADC1_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;



	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE ); //使能ADC1通道时钟，各个管脚时钟

	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //72M/6=12,ADC最大时间不能超过14M

	ADC_DeInit(ADC1); //将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //模数转换工作在扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //模数转换工作在连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //外部触发转换关闭
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = CHANEL_NUM; //顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure); //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

	set_adc1_sample_time ();

	// 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）
	ADC_DMACmd(ADC1, ENABLE);

	//开启模拟看门狗
//	ADC_AnalogWatchdogThresholdsConfig(ADC1,0xfff,0);
//	ADC_AnalogWatchdogCmd(ADC1,ADC_AnalogWatchdog_AllRegEnable);
//	ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);//允许模拟看门狗中断

	//使能ADC中断
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ADC1_2_INT_PREEM;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = ADC1_2_INT_SUB;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	ADC_Cmd(ADC1, ENABLE); //使能指定的ADC1

	ADC_ResetCalibration(ADC1); //复位指定的ADC1的校准寄存器

	while(ADC_GetResetCalibrationStatus(ADC1)); //获取ADC1复位校准寄存器的状态,设置状态则等待


	ADC_StartCalibration(ADC1); //开始指定ADC1的校准状态

	while(ADC_GetCalibrationStatus(ADC1)); //获取指定ADC1的校准程序,设置状态则等待
}



uint32_t send_IR_value (void)
{
	uint16_t i;
	for (i = 0; i < 12; i++){
		ad8804_write_ch (ad8804_env.da_addr[i], g_counter.view_IR_DA_value[i]);
	}
	return 0;
}
uint32_t calibrate_IR_0 (void)
{
	int i;
	uint16_t timeout = 0;
	uint16_t value_updated;
	
	for(i = 0; i < CHANEL_NUM; i++){
		value_updated = 1;
		while (value_updated){//每个通道单独调压
			value_updated = 0;
			delay_ms(10);
			if (After_filter[i] < STD_REF_VALUE_L){
				if (g_counter.view_IR_DA_value[i] < 160){
					g_counter.view_IR_DA_value[i]++;
					value_updated++;
				}
			}else if (After_filter[i] > STD_REF_VALUE_H){
				if (g_counter.view_IR_DA_value[i] > 0){
					g_counter.view_IR_DA_value[i]--;
					value_updated++;
				}
			}
			if (send_ad8804_ch_value (i, g_counter.view_IR_DA_value[i]) == 0){
				timeout++;
			}else{
				timeout = 0;
			}
			if (timeout >= 2){
				return timeout;
			}
		}
	}
	return 0;
}
uint32_t calibrate_IR_1 (void)
{
	int i;
	uint16_t value_updated;
	
	value_updated = 1;
	//delay_ms(2000);
	while (value_updated > 0){
		value_updated = 0;
		delay_ms(50);
		for(i = 0; i < CHANEL_NUM; i++){
			if (After_filter[i] < STD_REF_VALUE_L){
				if (g_counter.view_IR_DA_value[i] < 255){
					g_counter.view_IR_DA_value[i]++;
					value_updated++;
				}
			}else if (After_filter[i] > STD_REF_VALUE_H){
				if (g_counter.view_IR_DA_value[i] > 0){
					g_counter.view_IR_DA_value[i]--;
					value_updated++;
				}
			}
		}
		send_IR_value ();
	}
	return 0;
}
void calibrate_IR (void)
{
	calibrate_IR_1 ();
	//save_para (2);//强制保存
}

void re_calibration_detect (void)
{
	int i, j;
#if OS_CRITICAL_METHOD == 3u                           /* Allocate storage for CPU status register     */
    OS_CPU_SR  cpu_sr = 0u;
#endif
	for (i = 0; i < CHANEL_NUM; i++){
		//g_counter.view_IR_DA_value[i] = 0;
	}
	calibrate_IR ();
//	COUNT_COMPLETE = 1;
//	counter_reset ();
	OS_ENTER_CRITICAL();
//	COUNT_COMPLETE = 1;
//	VIBRATE_SWITCH = VIB_STOP;

	for (i = 0; i < CHANEL_NUM; i++){
		g_counter.ch[i].ad_max = 0;
		g_counter.ch[i].ad_min = 0xFFFF;
		g_counter.ch[i].std_v = 0;
		g_counter.ch[i].std_down_v = 0;
		g_counter.ch[i].ad_averaged_value = 0;
		for (j = 0; j < AD_FITTER_BUFF_SIZE; j++){
			g_counter.ch[i].ad_fitter_buff[j] = 0;
		}
	}
	process_rdy = 0;
	//AD_Sample_init ();
	OS_EXIT_CRITICAL();
}


U16 detect_chanel_index = 0xFFFF;//检测通道索引
U16 chanel_pos_index = 0;	//通道光敏二极管位置索引
U16 ADC_sync_signal = 0; //ADC转换处理同步信号
U16 ADC1_irq_cycle = 23;
U16 ADC1_process_time = 6;
//ADC中断服务函数
void ADC1_2_IRQHandler(void)
{
	//U16 temp;
//	unsigned long long tick_old;
//	tick_old = get_tim5_ticks();
//	refresh_adc1_cycle ();
///////////////////////////////////////////////////////////////////////////////
//	if (ADC_GetITStatus(ADC1, ADC_IT_AWD) != RESET){//电眼故障时进这里
//		detect_chanel_index = CHANEL_NUM - (DMA_GetCurrDataCounter (DMA1_Channel1) % CHANEL_NUM);
//		ADC_ClearFlag(ADC1, ADC_FLAG_AWD);
//		ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
//	}else
//	if (ADC_GetITStatus(ADC1, ADC_IT_EOC)){//这里不用判断ADC_IT_EOC是否置位，因为用了DMA后，转换完成硬件会清零ADC_IT_EOC位
		chanel_pos_index++; //采样处理下一个光敏二极管
		chanel_pos_index %= CHANEL_SENSOR_NUM;
		WRITE_SENSOR_ADDR(chanel_pos_index);//采样处理下一个光敏二极管
//		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);//这里不用清除ADCx的中断待处理位，因为用了DMA后，转换完成硬件会清零ADC_IT_EOC位
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//	}
////////////////////////////////////////////////////////////////////
//	ADC1_process_time = get_tim5_ticks () - tick_old + 2;
}


void AD1_DMA_Configuration(void)
{

	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能DMA传输
	DMA_DeInit(DMA1_Channel1); //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_DMA_buf; //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //内存作为数据传输的目的地
	DMA_InitStructure.DMA_BufferSize = SAMPLE_NUM * CHANEL_NUM * 2; //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// DMA_Mode_Normal; //工作在循环缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //根据DMA_InitStruct中指定的参数初始化DMA的通道

	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC | DMA_IT_HT, ENABLE); //传输结束中断

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_1_INT_PREEM;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA1_1_INT_SUB;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_Init(&NVIC_InitStructure);

}
//============================================
//函数名称:DMA1_Channel1_IRQHandler
//功能描述:DMA中断 对AD采集值平均滤波
//输入:无
//输出:无
//============================================

void Swap(uint32_t A[], uint16_t i, uint16_t j)
{
    uint32_t temp = A[i];
    A[i] = A[j];
    A[j] = temp;
//	A[i] = A[i] ^ A[j];                     //a = 3 ^ 5
//	A[j] = A[i] ^ A[j];					   //b = (3 ^ 5) ^ 5
//	A[i] = A[i] ^ A[j];	
}

void BubbleSort(uint32_t A[], uint16_t n)
{
	for (uint16_t j = 0; j < n - 1; j++){         // 每次最大元素就像气泡一样"浮"到数组的最后 
		for (uint16_t i = 0; i < n - 1 - j; i++){ // 依次比较相邻的两个元素,使较大的那个向后移
			if (A[i] > A[i + 1]){            // 如果条件改成A[i] >= A[i + 1],则变为不稳定的排序算法
				Swap(A, i, i + 1);
			}
		}
	}
}

uint32_t sort_temp[SAMPLE_NUM];

#define MIN_POW_V0 (30)
#define MIN_POW_V1 (20)
#define MIN_POW_V2 (15)
#define MIN_POW_V3 (12)
#define MIN_POW_V4 (10)
#define MIN_POW_V5 (5)
#define MIN_POW_V6 (4)
#define MIN_POW_V7 (3)
#define M_DIV (	MIN_POW_V0 + MIN_POW_V1 + \
								MIN_POW_V2 + MIN_POW_V3 + \
								MIN_POW_V4 + MIN_POW_V5 + \
								MIN_POW_V6 + MIN_POW_V7 \
							)
#define M_MULT (8)

uint16_t get_ad_fitter_value (uint32_t AD[])
{
#if 0
	BubbleSort (AD, SAMPLE_NUM);
	return (((AD[0] * (MIN_POW_V0 * M_MULT)) + (AD[1] * (MIN_POW_V1 * M_MULT)) + 
					(AD[2] * (MIN_POW_V2 * M_MULT)) + (AD[3] * (MIN_POW_V3 * M_MULT))+
					(AD[4] * (MIN_POW_V4 * M_MULT)) + (AD[5] * (MIN_POW_V5 * M_MULT)) +
					(AD[6] * (MIN_POW_V6 * M_MULT)) + (AD[7] * (MIN_POW_V7 * M_MULT))) / M_DIV);
#else	
	return (AD[0] + AD[1] + AD[2] + AD[3] + AD[4] + AD[5] + AD[6] + AD[7]);
#endif
}
///////////////////////////////////////////////////////////////////////////////////////
#define NEW_POW_V0 (3)
#define NEW_POW_V1 (4)
#define NEW_POW_V2 (5)
#define NEW_POW_V3 (10)
#define NEW_POW_V4 (12)
#define NEW_POW_V5 (15)
#define NEW_POW_V6 (20)
#define NEW_POW_V7 (30)
#define N_DIV (	NEW_POW_V0 + NEW_POW_V1 + \
								NEW_POW_V2 + NEW_POW_V3 + \
								NEW_POW_V4 + NEW_POW_V5 + \
								NEW_POW_V6 + NEW_POW_V7 \
							)
#define N_MULT (1)


uint16_t get_ad_averaged_value (uint16_t C, uint16_t ad_value)
{
#if 1
	
	#if (AD_FITTER_BUFF_SIZE != 8)
	#error "AD_FITTER_BUFF_SIZE must be == 8"
	#endif
	uint16_t ad = ad_value;
	uint16_t index;
	uint32_t AD[AD_FITTER_BUFF_SIZE];
	int i;
	g_counter.ch[C].ad_fitter_index %= AD_FITTER_BUFF_SIZE; 
	g_counter.ch[C].ad_fitter_buff[g_counter.ch[C].ad_fitter_index] = ad; 
	index = g_counter.ch[C].ad_fitter_index;
	for (i = 0; i < AD_FITTER_BUFF_SIZE; i++){
		index++;
		index %= AD_FITTER_BUFF_SIZE;
		AD[i] = g_counter.ch[C].ad_fitter_buff[index];
	}
	g_counter.ch[C].ad_fitter_index++; 
	return 	(((AD[0] * (NEW_POW_V0 * N_MULT)) + (AD[1] * (NEW_POW_V1 * N_MULT)) + 
						(AD[2] * (NEW_POW_V2 * N_MULT)) + (AD[3] * (NEW_POW_V3 * N_MULT))+
						(AD[4] * (NEW_POW_V4 * N_MULT)) + (AD[5] * (NEW_POW_V5 * N_MULT)) +
						(AD[6] * (NEW_POW_V6 * N_MULT)) + (AD[7] * (NEW_POW_V7 * N_MULT))) / N_DIV);
#else	
	uint16_t ad = ad_value;
	g_counter.ch[C].ad_fitter_index %= AD_FITTER_BUFF_SIZE; 
	g_counter.ch[C].ad_averaged_value += ad; 
	g_counter.ch[C].ad_averaged_value -= g_counter.ch[C].ad_fitter_buff[g_counter.ch[C].ad_fitter_index]; 
	g_counter.ch[C].ad_fitter_buff[g_counter.ch[C].ad_fitter_index] = ad; 
	ad = g_counter.ch[C].ad_averaged_value / AD_FITTER_BUFF_SIZE; 
	g_counter.ch[C].ad_fitter_index++; 
	return ad;
#endif
}
//backup//////////////////////////////////////////////////////\
		AD[C] = BUF[0][C] + BUF[1][C] + BUF[2][C] + BUF[3][C] + \
						BUF[4][C] + BUF[5][C] + BUF[6][C] + BUF[7][C]; \
		AD[C] /= S;  \
///////////////////////////////////////////////////////////////
#if (SAMPLE_NUM == 8)
#define AD_PRE_FITTER_1(AD,BUF,C,S) { \
		sort_temp[0] = BUF[0][C]; \
		sort_temp[1] = BUF[1][C]; \
		sort_temp[2] = BUF[2][C]; \
		sort_temp[3] = BUF[3][C]; \
		sort_temp[4] = BUF[4][C]; \
		sort_temp[5] = BUF[5][C]; \
		sort_temp[6] = BUF[6][C]; \
		sort_temp[7] = BUF[7][C]; \
		AD[C] = get_ad_fitter_value (sort_temp); \
		AD[C] = get_ad_averaged_value (C, AD[C]); \
	}
#endif
//

void set_std_offset (uint16 C)
{
//	g_counter.ch[C].std_down_offset = (g_counter.ch[C].ad_max - g_counter.ch[C].ad_min);
//	g_counter.ch[C].std_up_offset = (g_counter.ch[C].ad_max - g_counter.ch[C].ad_min - 1);
//	g_counter.ch[C].std_down_offset *= g_counter.set_std_numerator;
//	g_counter.ch[C].std_up_offset *= g_counter.set_std_numerator;
////	g_counter.ch[C].std_down_offset /= (g_counter.set_std_denumerator);
////	g_counter.ch[C].std_up_offset /= (g_counter.set_std_denumerator);	
//	g_counter.ch[C].std_down_offset /= 10;
//	g_counter.ch[C].std_up_offset /= 10;
	
	g_counter.ch[C].std_down_offset = STD_V_OFFSET;
	g_counter.ch[C].std_up_offset = STD_V_OFFSET;
	g_counter.set_wave_up_value = STD_V_OFFSET / 2;
}


//  \
	g_counter.ch[C].std_v = AD[C]; \

//============================================
#if (SAMPLE_NUM == 8)
#define AD_PRE_FITTER(AD,BUF,C,S) { \
		AD[C] = BUF[0][C] + BUF[1][C] + BUF[2][C] + BUF[3][C] + \
						BUF[4][C] + BUF[5][C] + BUF[6][C] + BUF[7][C]; \
		AD[C] -= my_env.ir_sub; \
		g_counter.ch[C].ad_fitter_index %= AD_FITTER_BUFF_SIZE; \
		if (process_rdy > START_DATA){ \
			g_counter.ch[C].ad_averaged_value += AD[C]; \
			g_counter.ch[C].ad_averaged_value -= g_counter.ch[C].ad_fitter_buff[g_counter.ch[C].ad_fitter_index]; \
			g_counter.ch[C].ad_fitter_buff[g_counter.ch[C].ad_fitter_index] = AD[C]; \
			AD[C] = g_counter.ch[C].ad_averaged_value / AD_FITTER_BUFF_SIZE; \
		}else if (process_rdy < START_DATA){ \
			g_counter.ch[C].ad_fitter_buff[g_counter.ch[C].ad_fitter_index] = AD[C]; \
		}else if (process_rdy == START_DATA){ \
			uint16_t i; \
			g_counter.ch[C].ad_fitter_buff[g_counter.ch[C].ad_fitter_index] = AD[C]; \
			g_counter.ch[C].ad_averaged_value = 0; \
			for (i = 0; i < AD_FITTER_BUFF_SIZE; i++){ \
				g_counter.ch[C].ad_averaged_value += g_counter.ch[C].ad_fitter_buff[i]; \
			} \
		} \
		g_counter.ch[C].ad_fitter_index++; \
	}

#endif
	
#define START_DATA 5
#if (SAMPLE_NUM == 8)
	#define GET_STD_AD_V(AD,BUF,C,S)  {\
		AD_PRE_FITTER (AD,BUF,C,S) \
		if (process_rdy > START_DATA){ \
			if (AD[C] > g_counter.ch[C].ad_max){ \
				g_counter.ch[C].ad_max = AD[C]; \
			} \
			if (AD[C] < g_counter.ch[C].ad_min){ \
				g_counter.ch[C].ad_min = AD[C]; \
			} \
			if (process_rdy != process_rdy_old) { \
				g_counter.ch[C].std_v += AD[C] / (PROCESS_RDY - (START_DATA ) - 1); \
			}\
		} \
		if ((process_rdy + 1) == PROCESS_RDY){ \
			set_std_offset (C); \
		} \
	}
#endif

#if (SAMPLE_NUM == 8)
	#define AD_FILTER(AD,BUF,C,S) {\
		AD_PRE_FITTER (AD,BUF,C,S) \
		if (AD[C] > g_counter.ch[C].ad_max){ \
			g_counter.ch[C].ad_max = AD[C]; \
		} \
		if (AD[C] < g_counter.ch[C].ad_min){ \
			g_counter.ch[C].ad_min = AD[C]; \
		} \
	}
#endif


int AD_Sample_init (void)
{
	AD_GPIO_Configuration ();
	chanel_pos_index = 0;
	WRITE_SENSOR_ADDR(chanel_pos_index);//采样处理下一个光敏二极管
	AD1_DMA_Configuration ();
	ADC1_Configuration ();
	ENABLE_SENSOR ();
	DMA_Cmd(DMA1_Channel1, ENABLE); //启动DMA通道
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	return 0;
}
//
void AD_Start_Sample (u32 _memBaseAddr)
{
	/* Write to DMA1 Channel4 CMAR */
	DMA1_Channel1->CMAR = _memBaseAddr;
	//ADC_sync_signal = 0;//AD 信号清零 采样同步
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
//
void adj_pos_ad_value (U16 (* AD_buf_p)[CHANEL_NUM])
{
	uint8_t i;
	for (i = 0; i < 12; i++){
//		AD_DMA_buf.AD_Value_2[0][i] = AD_DMA_buf.AD_Value_0[6][i];
//		AD_DMA_buf.AD_Value_2[1][i] = AD_DMA_buf.AD_Value_0[0][i];
//		AD_DMA_buf.AD_Value_2[2][i] = AD_DMA_buf.AD_Value_0[1][i];
//		AD_DMA_buf.AD_Value_2[3][i] = AD_DMA_buf.AD_Value_0[7][i];
//		AD_DMA_buf.AD_Value_2[4][i] = AD_DMA_buf.AD_Value_0[5][i];
//		AD_DMA_buf.AD_Value_2[5][i] = AD_DMA_buf.AD_Value_0[2][i];
//		AD_DMA_buf.AD_Value_2[6][i] = AD_DMA_buf.AD_Value_0[3][i];
//		AD_DMA_buf.AD_Value_2[7][i] = AD_DMA_buf.AD_Value_0[4][i];
		
		AD_DMA_buf.AD_Value_2[0][i] = AD_buf_p[4][i];
		AD_DMA_buf.AD_Value_2[1][i] = AD_buf_p[6][i];
		AD_DMA_buf.AD_Value_2[2][i] = AD_buf_p[7][i];
		AD_DMA_buf.AD_Value_2[3][i] = AD_buf_p[5][i];
		AD_DMA_buf.AD_Value_2[4][i] = AD_buf_p[3][i];
		AD_DMA_buf.AD_Value_2[5][i] = AD_buf_p[0][i];
		AD_DMA_buf.AD_Value_2[6][i] = AD_buf_p[1][i];
		AD_DMA_buf.AD_Value_2[7][i] = AD_buf_p[2][i];
	}
}


int save_detect_data (U16 _ch, U16 * _index, U16 _ad_value)
{
//	if (_ad_value == 0)
//	{
//		_ad_value = _ad_value;
//	}
	if (my_env.dma_state == 1){
		return 0;
	}
	if (_ch == g_counter.set_watch_ch){
		if ((*_index) < DETECTG_BUF_SIZE){
			Detect_Buf[*_index] = _ad_value;
			(*_index)++;
			return 0;
		}else{
			return 1;
		}
	}
	return 0;
}



int detect_ad_value_process(s_chanel_info * _ch, U16 _ad_value_, U16 _ch_id)
{
	int r_code = 0;
	int ad_change_v = 0;

	ad_change_v =  _ch->std_down_v - _ad_value_;
	if (ad_change_v < 0){
		ad_change_v = 0;
	}
	switch(_ch->process_step){
		case 0: {

			_ch->wave_down_flag = 0;
			_ch->wave_up_flag = 0;
			_ch->ad_value_min = AD_MAX_V;
			_ch->ad_value_min_temp = AD_MAX_V;

			_ch->cur_count = 0;
			_ch->counter_state = NORMAL_COUNT;
			_ch->sample_index = 0;

			_ch->piece_in_status = 1;
			_ch->process_step = 6;
			_ch->state = CH_IDLE;
			_ch->interval_ticks = get_sys_run_time ();
			_ch->max_interval.data_hl = 0;
			_ch->max_len.data_hl = 0;
			_ch->max_close_door_interval.data_hl = 0;
			_ch->max_area_sum.data_hl = 0;
			_ch->min_interval.data_hl = 0xFFFFFFFF;
			_ch->min_len.data_hl = 0xFFFFFFFF;
			_ch->min_close_door_interval.data_hl = 0xFFFFFFFF;
			_ch->min_area_sum.data_hl = 0xFFFFFFFF;
			_ch->area_sum.data_hl = 0;

			break;
		}
		case 6:{
			if (_ad_value_ < _ch->std_down_v){
				_ch->ch_idle_time = 0;
				r_code = save_detect_data (_ch_id, &_ch->sample_index, _ad_value_);
				_ch->state = CH_BUSY;
				_ch->wave_down_flag++;

				if (_ad_value_ < _ch->ad_value_min_temp){
					_ch->ad_value_min_temp = _ad_value_;
				}
			}else{
				_ch->ch_idle_time += dma_irq_cycle;
				if (_ch->ch_idle_time > 1000*1000){
					if (_ad_value_ < STD_REF_VALUE_L){
						if (g_counter.view_IR_DA_value[_ch_id] < 255){
							g_counter.view_IR_DA_value[_ch_id]++;
							ad8804_write_ch (ad8804_env.da_addr[_ch_id], g_counter.view_IR_DA_value[_ch_id]);
						}
					}
					_ch->ch_idle_time = 0;
					_ch->std_v = _ad_value_;
					_ch->std_up_v = _ch->std_v - STD_V_OFFSET;
					_ch->std_down_v = _ch->std_v - STD_V_OFFSET;
				}
				_ch->wave_down_flag = 0;
				_ch->sample_index = 0;
				_ch->state = CH_IDLE;
				_ch->ad_value_min_temp = 0xFFFF;
				_ch->ad_value_min = 0xFFFF;
			}

			if (_ch->wave_down_flag > WAVE_DOWN){//检测到有药粒
				_ch->piece_in_status = 0;
				_ch->wave_down_flag = 0;
				_ch->ad_value_min_temp = _ad_value_;
				_ch->process_step = 16;
				////////////////////////////////////////////////////////////////////////
			}
			break;
		}
		case 16:{

			r_code = save_detect_data (_ch_id, &_ch->sample_index, _ad_value_);

			if (_ad_value_ < _ch->ad_value_min_temp){
				_ch->ad_value_min_temp = _ad_value_;
				_ch->wave_up_flag = 0;
			}else if (_ad_value_ > _ch->ad_value_min_temp + WAVE_UP_V){
				_ch->wave_up_flag++;
			}else{
				_ch->wave_up_flag = 0;
			}

			if (_ch->wave_up_flag > WAVE_UP){//经过了波谷后
				_ch->ad_value_min = _ch->ad_value_min_temp;
				_ch->ad_value_min_temp = AD_MAX_V;
				_ch->wave_up_flag = 0;
				_ch->process_step = 31;
			}

			break;
		}
		case 31:{/*判断是否跟参考值相同或相近,确定最大采样值,波形恢复到参考值*/

			r_code = save_detect_data (_ch_id, &_ch->sample_index, _ad_value_);

			if (_ad_value_ >= _ch->std_up_v){
				_ch->wave_up_flag++;
			}else{
				_ch->wave_up_flag = 0;
			}

			if (_ch->wave_up_flag > WAVE_UP){
				
				_ch->process_step = 6;
				_ch->sample_size = _ch->sample_index;
				_ch->state = CH_DATA_RDY;
				_ch->sample_index = 0;
				_ch->wave_up_flag = 0;
				_ch->piece_in_status = 1;
				_ch->cur_count++;
			}

			break;
		}
	}
	return r_code;
}



u16 counter_process_time = 0;
u16 dma_irq_cycle = 0;
uint16_t tim5_dma_cur_cnt = 0;
uint16_t tim5_dma_pre_cnt = 0;
void DMA1_Channel1_IRQHandler(void)
{
	static int process_rdy_old = 0;
	int r_code = 0, i;
	//tick_old = get_tim5_ticks();
	//refresh_dma1_cycle (); //先统计DMA中断周期
	tim5_dma_cur_cnt = get_tim5_ticks();
	dma_irq_cycle = tim5_dma_cur_cnt - tim5_dma_pre_cnt;
	tim5_dma_pre_cnt = tim5_dma_cur_cnt;
	if ((dma_irq_cycle > 2100) && (process_rdy == PROCESS_RDY)){
		g_counter.system_status = ADC_TIME_ERROR;
	}
/////////////////////////////////////////////////////////////////////////////////
//	u8 count;
	if(DMA_GetITStatus(DMA1_IT_HT1)){
		g_counter.AD_buf_p = AD_DMA_buf.AD_Value_0;
	}else if(DMA_GetITStatus(DMA1_IT_TC1)){
		g_counter.AD_buf_p = AD_DMA_buf.AD_Value_1;
	}else if (DMA_GetITStatus(DMA1_IT_TE1)){
		g_counter.AD_buf_p = 0;
	}
	adj_pos_ad_value (g_counter.AD_buf_p);
	DMA_ClearITPendingBit(DMA1_IT_GL1); //清除全部中断标志
	if (g_counter.AD_buf_p != 0){
		if (process_rdy < PROCESS_RDY){
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 0, SAMPLE_NUM);
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 1, SAMPLE_NUM);
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 2, SAMPLE_NUM);
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 3, SAMPLE_NUM);
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 4, SAMPLE_NUM);
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 5, SAMPLE_NUM);
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 6, SAMPLE_NUM);
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 7, SAMPLE_NUM);
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 8, SAMPLE_NUM);
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 9, SAMPLE_NUM);
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 10, SAMPLE_NUM);
			GET_STD_AD_V (After_filter, g_counter.AD_buf_p, 11, SAMPLE_NUM);

			if ((process_rdy + 1) == PROCESS_RDY){
				for (i = 0; i < CHANEL_NUM; i++){
					if (g_counter.ch[i].std_v > STD_V_OFFSET) {
						g_counter.ch[i].std_up_v = g_counter.ch[i].std_v - STD_V_OFFSET;
					}else{
						g_counter.ch[i].std_up_v = g_counter.ch[i].std_v;
					}
					if (g_counter.ch[i].std_v > STD_V_OFFSET) {
						g_counter.ch[i].std_down_v = g_counter.ch[i].std_v - STD_V_OFFSET;
					}else{
						g_counter.ch[i].std_down_v = g_counter.ch[i].std_v;
					}
				}
				process_rdy = PROCESS_RDY;
//				COUNTER_FINISH_OP ();
			}else{
				process_rdy_old = process_rdy;
			}
		}else{
		//////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////// process begin ///////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////
			AD_FILTER (After_filter, g_counter.AD_buf_p, 0, SAMPLE_NUM);
			AD_FILTER (After_filter, g_counter.AD_buf_p, 1, SAMPLE_NUM);
			AD_FILTER (After_filter, g_counter.AD_buf_p, 2, SAMPLE_NUM);
			AD_FILTER (After_filter, g_counter.AD_buf_p, 3, SAMPLE_NUM);
			AD_FILTER (After_filter, g_counter.AD_buf_p, 4, SAMPLE_NUM);
			AD_FILTER (After_filter, g_counter.AD_buf_p, 5, SAMPLE_NUM);
			AD_FILTER (After_filter, g_counter.AD_buf_p, 6, SAMPLE_NUM);
			AD_FILTER (After_filter, g_counter.AD_buf_p, 7, SAMPLE_NUM);
			AD_FILTER (After_filter, g_counter.AD_buf_p, 8, SAMPLE_NUM);
			AD_FILTER (After_filter, g_counter.AD_buf_p, 9, SAMPLE_NUM);
			AD_FILTER (After_filter, g_counter.AD_buf_p, 10, SAMPLE_NUM);
			AD_FILTER (After_filter, g_counter.AD_buf_p, 11, SAMPLE_NUM);

			//After_filter[0] = g_counter.sim_ad_value;
			r_code += detect_ad_value_process (&g_counter.ch[0], After_filter[0], 0);
			r_code += detect_ad_value_process (&g_counter.ch[1], After_filter[1], 1);
			r_code += detect_ad_value_process (&g_counter.ch[2], After_filter[2], 2);
			r_code += detect_ad_value_process (&g_counter.ch[3], After_filter[3], 3);
			r_code += detect_ad_value_process (&g_counter.ch[4], After_filter[4], 4);
			r_code += detect_ad_value_process (&g_counter.ch[5], After_filter[5], 5);
			r_code += detect_ad_value_process (&g_counter.ch[6], After_filter[6], 6);
			r_code += detect_ad_value_process (&g_counter.ch[7], After_filter[7], 7);
			r_code += detect_ad_value_process (&g_counter.ch[8], After_filter[8], 8);
			r_code += detect_ad_value_process (&g_counter.ch[9], After_filter[9], 9);
			r_code += detect_ad_value_process (&g_counter.ch[10], After_filter[10], 10);
			r_code += detect_ad_value_process (&g_counter.ch[11], After_filter[11], 11);
						
		#ifdef USE_AS_COUNTER 
			COUNT_PIECES ();
		#else
			OUTPUT_ALL_PIECE_SIGNAL();
		#endif

			if (my_env.print == 1){
				if (r_code != 0){
				}else if (g_counter.ch[g_counter.set_watch_ch].state == CH_DATA_RDY){
					if (my_env.dma_state == 0){
						my_env.dma_state = 1;
						g_counter.buf_addr = (u32)&Detect_Buf[0];
						OSQPost(debug_msg, (void *) 0x55);//发送消息
					}
				}
			}else if ((AD_buff.buffer_en == 1)){
				AD_buff.buffer[AD_buff.buffer_index] = After_filter[g_counter.set_watch_ch];
				AD_buff.buffer_index++;
				if (AD_buff.buffer_index >= AD_BUFF_SIZE){
					AD_buff.buffer_index = 0;
					AD_buff.buffer_en = 0;
					OSQPost(debug_msg, (void *) 0x55);//发送消息
				}
			}

		//////////////////////////////// process end /////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////
		}
	}
//////////////////////////////////////////////////////////////////////
	counter_process_time = get_tim5_ticks () - tim5_dma_cur_cnt + 2;
}
//

void detector_reset (void)
{
#if OS_CRITICAL_METHOD == 3u                           /* Allocate storage for CPU status register     */
    OS_CPU_SR  cpu_sr = 0u;
#endif
	OS_ENTER_CRITICAL();
	g_counter.count.data.normal_count = 0;
	g_counter.count.data.pre_count = 0;
	g_counter.total_reject = 0;
	g_counter.total_good = 0;
	g_counter.rej_flag = 0;
	g_counter.rej_flag_buf.data_hl = 0;
	g_counter.system_status = RUNNING_OK;
	CHANEL_INIT(0);
	CHANEL_INIT(1);
	CHANEL_INIT(2);
	CHANEL_INIT(3);
	CHANEL_INIT(4);
	CHANEL_INIT(5);
	CHANEL_INIT(6);
	CHANEL_INIT(7);
	CHANEL_INIT(8);
	CHANEL_INIT(9);
	CHANEL_INIT(10);
	CHANEL_INIT(11);
	g_counter.max_len.data_hl = 0;
	g_counter.max_close_door_interval.data_hl = 0;
	g_counter.max_area_sum.data_hl = 0;
	g_counter.min_interval.data_hl = 0xFFFFFFFF;
	g_counter.min_len.data_hl = 0xFFFFFFFF;
	g_counter.min_close_door_interval.data_hl = 0xFFFFFFFF;
	g_counter.min_area_sum.data_hl = 0xFFFFFFFF;
	g_counter.counter_step = 0;
	g_counter.complete_count = 0;//数粒完成信号
	g_counter.complete_res = 0;//数粒完成信号
	OS_EXIT_CRITICAL();
}
//
void counter_data_clear (void)
{
#if OS_CRITICAL_METHOD == 3u                           /* Allocate storage for CPU status register     */
    OS_CPU_SR  cpu_sr = 0u;
#endif
	OS_ENTER_CRITICAL();
	CHANEL_DATA_CLEAR(0);
	CHANEL_DATA_CLEAR(1);
	CHANEL_DATA_CLEAR(2);
	CHANEL_DATA_CLEAR(3);
	CHANEL_DATA_CLEAR(4);
	CHANEL_DATA_CLEAR(5);
	CHANEL_DATA_CLEAR(6);
	CHANEL_DATA_CLEAR(7);
	CHANEL_DATA_CLEAR(8);
	CHANEL_DATA_CLEAR(9);
	CHANEL_DATA_CLEAR(10);
	CHANEL_DATA_CLEAR(11);
	g_counter.max_len.data_hl = 0;
	g_counter.max_close_door_interval.data_hl = 0;
	g_counter.max_area_sum.data_hl = 0;
	g_counter.min_interval.data_hl = 0xFFFFFFFF;
	g_counter.min_len.data_hl = 0xFFFFFFFF;
	g_counter.min_close_door_interval.data_hl = 0xFFFFFFFF;
	g_counter.min_area_sum.data_hl = 0xFFFFFFFF;
	OS_EXIT_CRITICAL();
}
//




