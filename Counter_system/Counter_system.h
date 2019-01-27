#ifndef __COUNTER_SYSTEM_
#define __COUNTER_SYSTEM_

#include "Counter_system_micro.h"






#define OPEN_DOOR(CH) { \
}

#define OPEN_ALL_DOOR() { \
	OPEN_DOOR(0); \
	OPEN_DOOR(1); \
	OPEN_DOOR(2); \
	OPEN_DOOR(3); \
	OPEN_DOOR(4); \
	OPEN_DOOR(5); \
	OPEN_DOOR(6); \
	OPEN_DOOR(7); \
	OPEN_DOOR(8); \
	OPEN_DOOR(9); \
	OPEN_DOOR(10); \
	OPEN_DOOR(11); \
}

#define UPDATE_CHANEL_STATUS(CH, STATUS) { \
	p_counter_module->ch[CH].cur_count = 0; \
	p_counter_module->last_piece_chanel_id = 0xFF; \
	p_counter_module->ch[CH].counter_state = STATUS; \
}

#define UPDATE_ALL_CHANEL_STATUS(STATUS) { \
	UPDATE_CHANEL_STATUS(0, STATUS); \
	UPDATE_CHANEL_STATUS(1, STATUS); \
	UPDATE_CHANEL_STATUS(2, STATUS); \
	UPDATE_CHANEL_STATUS(3, STATUS); \
	UPDATE_CHANEL_STATUS(4, STATUS); \
	UPDATE_CHANEL_STATUS(5, STATUS); \
	UPDATE_CHANEL_STATUS(6, STATUS); \
	UPDATE_CHANEL_STATUS(7, STATUS); \
	UPDATE_CHANEL_STATUS(8, STATUS); \
	UPDATE_CHANEL_STATUS(9, STATUS); \
	UPDATE_CHANEL_STATUS(10, STATUS); \
	UPDATE_CHANEL_STATUS(11, STATUS); \
	p_counter_module->rej_flag_buf.data_hl >>= 16; \
	p_counter_module->count.data_hl >>= 16; \
}


#define MAP_DOOR_TO_EXT_OUTPUT(C_CH, EX_CH, DOOR_CH){ \
	EXT_OUT##EX_CH = GET_BIT (counter_module[C_CH].output_buf_low, DOOR##DOOR_CH); \
}


#define CHANEL_INIT(CH) { \
	OPEN_DOOR(CH) \
}
#define CHANEL_DATA_CLEAR(CH) { \
}

#define SEND_COUNTER_FIN_SIGNAL(TIME) { \
}

#define COUNTER_FINISH_OP() { \
}


typedef enum CHANEL_STATUS
{
	CH_IDLE = 0,
	CH_BUSY,
	CH_DATA_RDY,
	NORMAL_COUNT,
	SEPARATE_PRE_COUNT,
	PRE_COUNT
}e_chanel_status;

typedef enum COUNTER_MODULE_STATUS
{
	COUNTER_IDLE = 0,
	SWITCH_TO_NORMAL_COUNT,
	FILL_BOTTLE,
}e_counter_module_status;
typedef enum FILL_BOTTLE_STATUS
{
	STANDBY = 0,
	WAIT_PIECE_FALL,
	OPEN_BIG_GATE,
	CLOSE_BIG_GATE,
	READY_TO_SHIFT_BOTTLE,
	OPEN_SHIFT_BOTTLE,
	CLOSE_SHIFT_BOTTLE,
	INSERT_TO_REJ_Q,
	UP_NOZZLE,
	DOWN_NOZZLE,
	FILL_BOTTLE_FINISHED,
}e_fill_bottle_status;

typedef enum SYSTEM_STATUS
{
	RUNNING_OK = 0,
	COUNTER_ERROR,
	STATUS_ERROR,
	ADC_TIME_ERROR,
	DETECTOR_ERROR
}e_system_status;


typedef union{
	uint32_t data_hl;
	struct{
		uint16_t l;
		uint16_t h;
	}data;
}s_32;


typedef union{
	uint32_t data_hl;
	struct{
		uint16_t normal_count;
		uint16_t pre_count;
	}data;
}s_ctr;


typedef union{
	uint32_t data_hl;
	struct{
		uint16_t current_bottle;
		uint16_t next_bottle;
	}data;
}s_rej;

typedef struct{
	//s_chanel_pos pos[CHANEL_SENSOR_NUM];
	uint16_t cur_count;
	uint16_t counter_state;
	uint16_t state;
	uint16_t input_status_new;
	uint16_t input_status_old;
	uint16_t set_ch_door_close_delay;
	uint16_t ch_door_close_delay;
	uint16_t close_door_interval;//小料门关门间隔
	uint16_t door_switch_interval;//小料门开关间隔
	uint16_t door_is_close;
	uint16_t length;//长度
	uint16_t piece_interval;//药粒间隔
	uint32_t input_status_time;
}s_chanel_info;

typedef struct{
	uint16_t set_count_new;
	uint16_t set_pre_count_new;
	uint16_t set_count;
	uint16_t set_pre_count;
	uint16_t set_min_length; 
	uint16_t set_max_length; 
	uint16_t set_min_interval;
	uint16_t set_door_close_delay;
	uint16_t set_fill_bottle_time;
	uint16_t set_shift_bottle_time;
	uint16_t set_start_rej;
	uint16_t set_reject_delay;
	uint16_t set_reject_keep;
	uint16_t set_door_close_interval;
	uint16_t set_door_switch_interval;
	uint16_t set_door_n_close_delay[CHANEL_NUM];
	uint16_t set_door_close_ex_delay;
	uint16_t set_enable_big_gate;
	uint16_t set_enable_nozzle_vib;
	uint16_t set_nozzle_vib_up_time;
	uint16_t set_nozzle_vib_down_time;
	uint16_t set_gate_open_time;
	uint16_t set_gate_close_time;
	
	uint16_t set_sim_length_X;
	uint16_t set_sim_length_Y;
	uint16_t set_sim_interval_X;
	uint16_t set_sim_interval_Y;
	
	uint16_t set_vib_restart_delay;
	uint16_t set_pre_count_threshold;
	
	uint16_t sim_scan_ctr;
}s_counter_set_para;
	
typedef struct{
	uint16_t normal_count;
	uint16_t pre_count;
	uint16_t normal_count_rej_flag;
	uint16_t pre_count_rej_flag;
	
	uint16_t last_piece_chanel_id;//最后一粒所在的通道号
	uint16_t reject_flag;
	uint16_t last_reject_flag;
	uint16_t start_rej;

	uint16_t min_length;
	uint16_t max_length;
	uint16_t min_piece_interval;
	uint16_t min_close_door_interval;
	uint16_t min_door_switch_interval;
	
	uint16_t input_buf_high;
	uint16_t input_buf_low;
	uint16_t output_buf_high;
	uint16_t output_buf_low;
	
	uint16_t input_buf_high_map;
	uint16_t input_buf_low_map;
	uint16_t output_buf_high_map;
	uint16_t output_buf_low_map;
	
	s_chanel_info chanel[CHANEL_NUM];
	s_counter_set_para set_para;
	e_counter_module_status counter_module_state;
	uint16_t first_run;
	uint16_t system_status;
}s_counter_module;

// 
	
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
#define COUNT_PIECES(MODULE_N) { \
}
#define CHECK_DOOR_CLOSE_FLAG(CH) if (p_counter_module->ch[CH].ch_door_close_delay > 0){ \
				p_counter_module->ch[CH].ch_door_close_delay--; \
				if ((p_counter_module->ch[CH].ch_door_close_delay == 0)){ \
					DOOR_##CH = 0; \
				} \
			}
#define CHECK_ALL_DOOR_CLOSE_FLAG() { \
		CHECK_DOOR_CLOSE_FLAG (0); \
		CHECK_DOOR_CLOSE_FLAG (1); \
		CHECK_DOOR_CLOSE_FLAG (2); \
		CHECK_DOOR_CLOSE_FLAG (3); \
		CHECK_DOOR_CLOSE_FLAG (4); \
		CHECK_DOOR_CLOSE_FLAG (5); \
		CHECK_DOOR_CLOSE_FLAG (6); \
		CHECK_DOOR_CLOSE_FLAG (7); \
		CHECK_DOOR_CLOSE_FLAG (8); \
		CHECK_DOOR_CLOSE_FLAG (9); \
		CHECK_DOOR_CLOSE_FLAG (10); \
		CHECK_DOOR_CLOSE_FLAG (11); \
}

//
#define CHECK_PIECE_IN_STATUS(CH) if (p_counter_module->ch[CH].input_status_new == 1){ \
	p_counter_module->ch[CH].input_status_time++; \
	if (p_counter_module->ch[CH].input_status_time > 10000){ \
		p_counter_module->system_status = DETECTOR_ERROR; \
	} \
}else{ \
	p_counter_module->ch[CH].input_status_time = 0; \
}
#define CHECK_ALL_PIECE_IN_STATUS() { \
		CHECK_PIECE_IN_STATUS (0); \
		CHECK_PIECE_IN_STATUS (1); \
		CHECK_PIECE_IN_STATUS (2); \
		CHECK_PIECE_IN_STATUS (3); \
		CHECK_PIECE_IN_STATUS (4); \
		CHECK_PIECE_IN_STATUS (5); \
		CHECK_PIECE_IN_STATUS (6); \
		CHECK_PIECE_IN_STATUS (7); \
		CHECK_PIECE_IN_STATUS (8); \
		CHECK_PIECE_IN_STATUS (9); \
		CHECK_PIECE_IN_STATUS (10); \
		CHECK_PIECE_IN_STATUS (11); \
}



extern s_32 sys_run_time;//1000us的精度
extern s_counter_module counter_module[2];
extern s_counter_env counter_env;
//void counter_process (void);
void counter_init (void);
void counter_task_poll (void);
void set_32io_bit (uint16_t *p_value, uint32_t mask);
void reset_32io_bit (uint16_t *p_value, uint32_t mask);

void test_counter_module (void);


#endif









