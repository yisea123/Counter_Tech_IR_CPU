#ifndef __FILL_BOTTLE_SYSTEM_
#define __FILL_BOTTLE_SYSTEM_

#include "Fill_bottle_system_micro.h"


#define MAP_FILL_SIG_TO_EXT_OUTPUT(C_CH, EX_CH, SIGNAL){ \
	EXT_OUT##EX_CH = GET_BIT (fill_bottle_module[C_CH].output_buf, SIGNAL); \
}

typedef struct{
	uint16_t set_fill_bottle_time;
	uint16_t set_shift_bottle_time;
	
	uint16_t set_reject_delay;
	uint16_t set_reject_keep;
	
	uint16_t set_enable_big_gate;
	uint16_t set_enable_nozzle_vib;
	uint16_t set_nozzle_vib_up_time;
	uint16_t set_nozzle_vib_down_time;
	uint16_t set_gate_open_time;
	uint16_t set_gate_close_time;
}s_fill_bottle_set_para;
	
typedef struct{
	uint16_t total_count_sum;
	uint16_t total_reject;
	uint16_t total_good;
	uint16_t total_good_tmp;
	uint16_t speed;
	
	uint16_t reject_q[REJECT_Q_16];
	uint16_t reject_q_index;
	uint16_t reject_keep_delay;
	uint16_t big_gate_delay;
	uint16_t nozzle_delay;
	uint16_t shift_bottle_delay;
	uint16_t fill_complete_delay;
	e_fill_bottle_status fill_bottle_state;//װƿ״̬��
	
	uint16_t nozzle_vib_status;
	uint16_t nozzle_vib_up_time;
	uint16_t nozzle_vib_down_time;
	
	
	uint16_t input_buf;
	uint16_t output_buf;
	uint16_t input_buf_map;
	uint16_t output_buf_map;
	
	s_fill_bottle_set_para set_para;
	uint16_t first_run;
	uint32_t fill_bottle_op_delay;
}s_fill_bottle_module;






extern s_fill_bottle_module fill_bottle_module[2];



void fill_bottle_system_module_poll (s_fill_bottle_module * p_fill_bottle_module, const uint16_t input_map);
void fill_output_map_func (void);
void fill_bottle_module_reset (void);





#endif


