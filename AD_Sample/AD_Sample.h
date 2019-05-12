#ifndef __AD_SAMPLE__
#define __AD_SAMPLE__

#define SAMPLE_NUM 8 //每通道采样次数
#define CHANEL_NUM 12 //为12个通道
#define CHANEL_SENSOR_NUM 8

#define START_DATA 5
#define PROCESS_RDY 15
#define ADJ_PERIOD 200 //基准值调整周期


//#define WAVE_DOWN 	g_counter.set_wave_down_flag //16
//#define WAVE_UP 	g_counter.set_wave_up_flag //16
//#define WAVE_UP_V 	g_counter.set_wave_up_value //10
#define WAVE_DOWN 	g_counter.set_wave_down_flag //
#define WAVE_UP 	g_counter.set_wave_down_flag //
#define WAVE_UP_V 	g_counter.set_wave_up_value //
#define AD_MODE g_counter.set_adc_mode


#define STD_UP_LIMIT_VALUE 30000
#define STD_REF_OFFSET (g_counter.std_ref_value_offset)
#define STD_REF_VALUE (g_counter.std_ref_value)
#define STD_REF_VALUE_L (STD_REF_VALUE - STD_REF_OFFSET)
#define STD_REF_VALUE_H (STD_REF_VALUE + STD_REF_OFFSET)
#define STD_DOWN_V_OFFSET g_counter.ch[i].std_down_offset
#define STD_UP_V_OFFSET g_counter.ch[i].std_up_offset
#define STD_V_OFFSET g_counter.set_std_numerator

#define AD_MAX_V 4096 * 8

#define CH0 	ADC_Channel_9
#define CH1 	ADC_Channel_8
#define CH2 	ADC_Channel_15
#define CH3 	ADC_Channel_14
#define CH4 	ADC_Channel_7
#define CH5 	ADC_Channel_6
#define CH6 	ADC_Channel_5
#define CH7 	ADC_Channel_4
#define CH8 	ADC_Channel_3
#define CH9 	ADC_Channel_2
#define CH10 	ADC_Channel_1
#define CH11 	ADC_Channel_0
//#define CH12 	ADC_Channel_12
//#define CH13 	ADC_Channel_13


#define AD_BUFF_SIZE 1024 * 2
#define AD_FITTER_BUFF_SIZE 8

//#define SAMPLE_BUF_SIZE 512 //每通道采50次
#define SENSOR_ADDR_OFFSET 0
#define SET_SENSOR_ADDR_PORT GPIOD
#define SET_SENSOR_EN_PORT GPIOD
#define SET_SENSOR_EN_PIN GPIO_Pin_3
#define WRITE_SENSOR_ADDR(ADDR) GPIO_Write(SET_SENSOR_ADDR_PORT, ((GPIO_ReadOutputData(SET_SENSOR_ADDR_PORT) & (~(7 << SENSOR_ADDR_OFFSET))) | (ADDR<<SENSOR_ADDR_OFFSET)))
#define RESET_SENSOR_EN() GPIO_ResetBits(SET_SENSOR_EN_PORT, SET_SENSOR_EN_PIN)
#define SET_SENSOR_EN() GPIO_SetBits(SET_SENSOR_EN_PORT, SET_SENSOR_EN_PIN)




#define OUTPUT_PIECE_SIGNAL(CH) { \
	EXT_OUT##CH = g_counter.ch[CH].piece_in_status; \
}
#define OUTPUT_ALL_PIECE_SIGNAL() { \
	OUTPUT_PIECE_SIGNAL(0); \
	OUTPUT_PIECE_SIGNAL(1); \
	OUTPUT_PIECE_SIGNAL(2); \
	OUTPUT_PIECE_SIGNAL(3); \
	OUTPUT_PIECE_SIGNAL(4); \
	OUTPUT_PIECE_SIGNAL(5); \
	OUTPUT_PIECE_SIGNAL(6); \
	OUTPUT_PIECE_SIGNAL(7); \
	OUTPUT_PIECE_SIGNAL(8); \
	OUTPUT_PIECE_SIGNAL(9); \
	OUTPUT_PIECE_SIGNAL(10); \
	OUTPUT_PIECE_SIGNAL(11); \
}

#define CHANEL_INIT(CH) { \
	g_counter.ch[CH].max_interval.data_hl = 0; \
	g_counter.ch[CH].max_len.data_hl = 0; \
	g_counter.ch[CH].max_close_door_interval.data_hl = 0; \
	g_counter.ch[CH].max_area_sum.data_hl = 0; \
	g_counter.ch[CH].max_door_switch_interval.data_hl = 0; \
	g_counter.ch[CH].min_door_switch_interval.data_hl = 0xFFFFFFFF; \
	g_counter.ch[CH].min_interval.data_hl = 0xFFFFFFFF; \
	g_counter.ch[CH].min_len.data_hl = 0xFFFFFFFF; \
	g_counter.ch[CH].min_close_door_interval.data_hl = 0xFFFFFFFF; \
	g_counter.ch[CH].min_area_sum.data_hl = 0xFFFFFFFF; \
	g_counter.ch[CH].interval.data_hl = 0; \
	g_counter.ch[CH].close_door_interval.data_hl = 0; \
	g_counter.ch[CH].len.data_hl = 0; \
	g_counter.ch[CH].area_sum.data_hl = 0; \
	g_counter.ch[CH].cur_count = 0; \
	g_counter.total_count_sum.data_hl = 0; \
}
#define CHANEL_DATA_CLEAR(CH) { \
	g_counter.ch[CH].ad_max = 0; \
	g_counter.ch[CH].max_interval.data_hl = 0; \
	g_counter.ch[CH].max_len.data_hl = 0; \
	g_counter.ch[CH].max_close_door_interval.data_hl = 0; \
	g_counter.ch[CH].max_area_sum.data_hl = 0; \
	g_counter.ch[CH].max_door_switch_interval.data_hl = 0; \
	g_counter.ch[CH].min_door_switch_interval.data_hl = 0xFFFFFFFF; \
	g_counter.ch[CH].min_interval.data_hl = 0xFFFFFFFF; \
	g_counter.ch[CH].min_len.data_hl = 0xFFFFFFFF; \
	g_counter.ch[CH].min_close_door_interval.data_hl = 0xFFFFFFFF; \
	g_counter.ch[CH].min_area_sum.data_hl = 0xFFFFFFFF; \
	g_counter.ch[CH].ad_min = 0xFFFF; \
	g_counter.ch[CH].cur_count = 0; \
}

#define ENABLE_SENSOR() RESET_SENSOR_EN()
#define DISABLE_SENSOR() SET_SENSOR_EN()


#define DETECTG_BUF_SIZE 1024 * 4
#define DETECTG_BUF_GROUP 2
#define AD_DMA_BUF_GROUP 2

enum CHANEL_STATE
{
	CH_IDLE = 0,
	CH_BUSY,
	CH_DATA_RDY
};

enum COUNTER_STATE
{
	COUNTER_IDLE = 0,
	NORMAL_COUNT,
	SEPARATE_PRE_COUNT,
	PRE_COUNT
};

typedef union{
	uint32_t data_hl;
	struct{
		uint16_t l;
		uint16_t h;
	}data;
}s_32;
typedef union{
	U32 data_hl;
	struct{
		U16 normal_count;
		U16 pre_count;
	}data;
}s_ctr;


typedef union{
	U32 data_hl;
	struct{
		U16 current_bottle;
		U16 next_bottle;
	}data;
}s_rej;

typedef struct{
	//s_chanel_pos pos[CHANEL_SENSOR_NUM];
	U16 process_step;
	U16 wave_down_flag;
	U16 wave_up_flag;
	U16 ad_value_min;
	U16 ad_value_min_temp;
	U16 cur_count;
	U16 counter_state;
	U16 std_down_v;
	U16 std_up_v;
	U16 std_v;
	uint16_t old_std_index;
	uint32_t old_std_v;
	
	U16 std_down_offset;
	U16 std_up_offset;
	U16 ad_min;
	U16 ad_max;
	U16 sample_index;
	U16 sample_size;
	U16 state;
	U16 piece_in;
	U16 piece_in_new;
	U16 piece_in_old;
	U32 piece_in_time;
	U16 piece_in_status;
	U16 set_ch_door_close_delay;
	U16 ch_door_close_delay;
	U16 ad_fitter_index;
	S32 ad_averaged_value;
	uint32_t ad_fitter_buff[AD_FITTER_BUFF_SIZE];
	uint32_t area_sum_buf;//截面积
	uint32_t ch_idle_time;
	uint32_t ch_busy_time;
	s_32 area_sum;//截面积
	s_32 min_area_sum;
	s_32 max_area_sum;
	s_32 len;//长度
	s_32 min_len;
	s_32 max_len;
	s_32 interval;
	s_32 close_door_interval;
	s_32 min_close_door_interval;
	s_32 max_close_door_interval;
	s_32 min_interval;
	s_32 max_interval;
	s_32 door_switch_interval;
	s_32 min_door_switch_interval;
	s_32 max_door_switch_interval;
	uint32_t door_open_ticks;
	uint32_t interval_ticks;
	uint32_t length_start_ticks;
}s_chanel_info;
typedef struct{
	u16 buffer[AD_BUFF_SIZE];
	u16 buffer_index;
	u16 buffer_en;
	u16 cir;
}s_buf;

typedef struct {
	u16 AD_Value_0[SAMPLE_NUM][CHANEL_NUM]; //用来存放ADC转换结果，也是DMA的目标地址
	u16 AD_Value_1[SAMPLE_NUM][CHANEL_NUM]; //用来存放ADC转换结果，也是DMA的目标地址
	u16 AD_Value_2[SAMPLE_NUM][CHANEL_NUM]; //用来存放ADC转换结果，也是DMA的目标地址
}s_AD_buf;

	
typedef struct{
	uint16_t auto_scan;
	uint16_t clear_monitor_data;
	uint16_t mannual_scan_time;
	uint16_t scan_process_time;
	uint16_t min_scan_process_time;
	uint16_t max_scan_process_time;
	uint16_t scan_period;
	uint16_t min_scan_period;
	uint16_t max_scan_period;
	uint16_t sim_too_long_flag;
	uint16_t sim_too_close_flag;
	uint16_t screw_is_run;
	uint16_t running_status;
}s_counter_env;

typedef struct{
	s_chanel_info ch[CHANEL_NUM];
	U16 set_count;
	U16 set_pre_count;
	U16 set_count_new;
	U16 set_pre_count_new;
	U16 normal_count;
	U16 counter_state;
	U16 set_door_close_delay;
	U16 set_adc_mode;
	U16 pre_count;
	U16 total_reject;
	U16 total_good;
	U16 dma_buf_index;
	U16 AD_use_buf_index;
	U16 set_watch_ch;
	U32 dma_buf_addr;
	U32 buf_addr;
	U16 (* AD_buf_p)[CHANEL_NUM];
	U16 counter_step;
	
	U16 set_std_denumerator;
	U16 set_std_numerator;
	U16 set_wave_down_flag;
	U16 set_wave_up_flag;
	U16 set_wave_up_value;
	U16 set_door_switch_interval;
	U16 set_door_close_ex_delay;
	U16 set_vib_restart_delay;
	U16 set_pre_count_threshold;
	U16 std_ref_value_old;
	U16 std_ref_value;
	U16 std_ref_value_offset;
	U16 counter_fin_signal_delay;
	U16 last_piece_chanel_id;//最后一粒所在的通道号
	U16 rej_flag;
	U16 rej_flag_clear_delay;
	s_ctr count;
	s_rej rej_flag_buf;
	s_32 total_count_sum;
	s_32 area_sum;//截面积
	s_32 min_area_sum;
	s_32 max_area_sum;
	s_32 set_min_len; 
	s_32 set_max_len;
	s_32 set_door_close_interval;
	s_32 set_max_interval;
	s_32 set_min_area_sum;
	s_32 set_max_area_sum;
	s_32 min_len;
	s_32 max_len;
	s_32 min_interval;
	s_32 min_close_door_interval;
	s_32 max_close_door_interval;
	U32 complete_count;
	U32 complete_res;
	U16 sim_ad_value;
	U16 sim_flag;
	U16 set_door_n_close_delay[CHANEL_NUM];
	U16 running_status;
	U16 system_status;
	U16 view_IR_DA_value[CHANEL_NUM];
}s_counter_info;

void AD_filter(U16 _detect_chanel_index, U16 _chanel_pos_index);
int AD_Sample_init (void);
void calibrate_IR (void);
uint32_t send_IR_value (void);
void re_calibration_detect (void);
void send_re_calibration_msg (void);
void detect_check_func (void);
u16 GetVolt(u16 advalue);
extern u16 After_filter[CHANEL_NUM]; //用来存放求平均值之后的结果
//extern vu16 Detect_Buf[DETECTG_BUF_GROUP][DETECTG_BUF_SIZE];
extern vu16 Detect_Buf[DETECTG_BUF_SIZE];

extern U16 ADC1_irq_cycle;
extern U16 ADC1_process_time;

extern u16 dma_irq_cycle;
extern u16 counter_process_time;

extern U16 detect_chanel_index;//检测通道索引
extern U16 chanel_pos_index;	//通道光敏二极管位置索引

extern s_buf AD_buff;
extern s_AD_buf AD_DMA_buf;


extern s_counter_env counter_env;
extern s_counter_info g_counter;
extern vu16 process_rdy;
extern u16 wave_down_flag;
extern u16 wave_up_flag;
extern u16 wave_up_value;
void counter_data_clear (void);
void AD_Start_Sample (u32 _memBaseAddr);
void detector_reset (void);
void re_calibration_detect (void);

#endif

