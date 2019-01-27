#include "main.h"



s_fill_bottle_module fill_bottle_module[2];
//
void fill_wait_piece_fall (s_fill_bottle_module * p_fill_bottle_module)
{
	p_fill_bottle_module->fill_bottle_op_delay = FILL_WAIT_PIECE_FALL_TIME;
	p_fill_bottle_module->big_gate_delay = p_fill_bottle_module->fill_bottle_op_delay;
}
//
void fill_insert_to_rej_q (s_fill_bottle_module * p_fill_bottle_module)
{
	if (GET_BIT(p_fill_bottle_module->input_buf, OUTPUT_BUF_REJECT) == 0){
		p_fill_bottle_module->reject_q_index %= REJECT_Q_16;
		p_fill_bottle_module->reject_q[p_fill_bottle_module->reject_q_index] = FILL_REJECT_START_DELAY;
		p_fill_bottle_module->reject_q_index++;
		p_fill_bottle_module->total_reject++;;
	}else{
		p_fill_bottle_module->total_good++;
	}
	p_fill_bottle_module->total_count_sum++;
}
//
void fill_open_big_gate_door (s_fill_bottle_module * p_fill_bottle_module)
{
	p_fill_bottle_module->fill_bottle_op_delay = FILL_OPEN_BIG_GATE_DOOR_DELAY;
	p_fill_bottle_module->big_gate_delay = p_fill_bottle_module->fill_bottle_op_delay;
	SET_OUTPUT_ON(&p_fill_bottle_module->output_buf, OUTPUT_BUF_BIG_GATE_DOOR);
}
void fill_close_big_gate_door (s_fill_bottle_module * p_fill_bottle_module)
{
	p_fill_bottle_module->fill_bottle_op_delay = FILL_CLOSE_BIG_GATE_DOOR_DELAY;
	p_fill_bottle_module->big_gate_delay = p_fill_bottle_module->fill_bottle_op_delay;
//	inc_Modbus_test_value ();
	SET_OUTPUT_OFF(&p_fill_bottle_module->output_buf, OUTPUT_BUF_BIG_GATE_DOOR);
}
//
void fill_open_shift_bottle (s_fill_bottle_module * p_fill_bottle_module)
{
	p_fill_bottle_module->fill_bottle_op_delay = FILL_OPEN_SHIFT_BOTTLE_DELAY;
	p_fill_bottle_module->shift_bottle_delay = p_fill_bottle_module->fill_bottle_op_delay;
	SET_OUTPUT_ON(&p_fill_bottle_module->output_buf, OUTPUT_BUF_BOTTLE_SHIFT);
}
void fill_close_shift_bottle (s_fill_bottle_module * p_fill_bottle_module)
{
	p_fill_bottle_module->fill_bottle_op_delay = FILL_CLOSE_SHIFT_BOTTLE_DELAY;
	p_fill_bottle_module->shift_bottle_delay = p_fill_bottle_module->fill_bottle_op_delay;
	SET_OUTPUT_OFF(&p_fill_bottle_module->output_buf, OUTPUT_BUF_BOTTLE_SHIFT);
}
//
void fill_up_nozzle (s_fill_bottle_module * p_fill_bottle_module)
{
	counter_env.screw_is_run = 0;
	virtual_input[6] = counter_env.screw_is_run;
	p_fill_bottle_module->fill_bottle_op_delay = FILL_UP_NOZZLE_DELAY;
	p_fill_bottle_module->nozzle_delay = p_fill_bottle_module->fill_bottle_op_delay;
	SET_OUTPUT_ON(&p_fill_bottle_module->output_buf, OUTPUT_BUF_NOZZLE);
}
//
void fill_down_nozzle (s_fill_bottle_module * p_fill_bottle_module)
{
	p_fill_bottle_module->fill_bottle_op_delay = FILL_DOWN_NOZZLE_DELAY;
	p_fill_bottle_module->nozzle_delay = p_fill_bottle_module->fill_bottle_op_delay;
	SET_OUTPUT_OFF(&p_fill_bottle_module->output_buf, OUTPUT_BUF_NOZZLE);
}
//
void fill_set_fill_bottle_complete (s_fill_bottle_module * p_fill_bottle_module)
{
	p_fill_bottle_module->fill_bottle_op_delay = FILL_SET_FILL_BOTTLE_COMPLETE_DELAY;
	p_fill_bottle_module->fill_complete_delay = p_fill_bottle_module->fill_bottle_op_delay;
	SET_OUTPUT_ON(&p_fill_bottle_module->output_buf, OUTPUT_BUF_COMPLETE);
}
void fill_reset_fill_bottle_complete (s_fill_bottle_module * p_fill_bottle_module)
{
	p_fill_bottle_module->fill_bottle_op_delay = FILL_RESET_FILL_BOTTLE_COMPLETE_DELAY;
	p_fill_bottle_module->fill_complete_delay = p_fill_bottle_module->fill_bottle_op_delay;
	SET_OUTPUT_OFF(&p_fill_bottle_module->output_buf, OUTPUT_BUF_COMPLETE);
}
//
void fill_start_reject (s_fill_bottle_module * p_fill_bottle_module)
{
	p_fill_bottle_module->reject_keep_delay = FILL_REJECT_KEEP_DELAY;
	SET_OUTPUT_ON(&p_fill_bottle_module->output_buf, OUTPUT_BUF_REJECT);
}
//
void fill_stop_reject (s_fill_bottle_module * p_fill_bottle_module)
{
	SET_OUTPUT_OFF(&p_fill_bottle_module->output_buf, OUTPUT_BUF_REJECT);
}
//
void fill_ready_to_shift_bottle (s_fill_bottle_module * p_fill_bottle_module)
{
	fill_insert_to_rej_q (p_fill_bottle_module);
}
//
void fill_bottle_finished (s_fill_bottle_module * p_fill_bottle_module)
{
	if (1){//(GET_BIT (p_fill_bottle_module->input_buf, SCREW_IS_RUN) == 1){
		p_fill_bottle_module->fill_bottle_state = STANDBY;
	}
}
//
void fill_nozzle_vib_process (s_fill_bottle_module * p_fill_bottle_module)
{
	if (p_fill_bottle_module->nozzle_vib_status == MY_ENABLE){
		if (p_fill_bottle_module->nozzle_vib_up_time > 0){
			p_fill_bottle_module->nozzle_vib_up_time--;
			if (p_fill_bottle_module->nozzle_vib_up_time == 0){
				p_fill_bottle_module->nozzle_vib_down_time = p_fill_bottle_module->set_para.set_nozzle_vib_down_time;
				SET_OUTPUT_ON(&p_fill_bottle_module->output_buf, OUTPUT_BUF_NOZZLE);
			}
		}else if (p_fill_bottle_module->nozzle_vib_down_time > 0){
			p_fill_bottle_module->nozzle_vib_down_time--;
			if (p_fill_bottle_module->nozzle_vib_down_time == 0){
				p_fill_bottle_module->nozzle_vib_up_time = p_fill_bottle_module->set_para.set_nozzle_vib_up_time;
				SET_OUTPUT_OFF(&p_fill_bottle_module->output_buf, OUTPUT_BUF_NOZZLE);
			}
		}
	}
}
void fill_check_reject_q_out_delay (s_fill_bottle_module * p_fill_bottle_module)
{
	uint16_t i;
	for (i = 0; i < REJECT_Q_16; i++){
		if (p_fill_bottle_module->reject_q[i] > 0){
			p_fill_bottle_module->reject_q[i]--;
			if (p_fill_bottle_module->reject_q[i] == 0){
				fill_start_reject(p_fill_bottle_module);
			}
		}
	}
}
void fill_check_reject_keep_delay (s_fill_bottle_module * p_fill_bottle_module)
{
	if (p_fill_bottle_module->reject_keep_delay > 0){
		p_fill_bottle_module->reject_keep_delay--;
		if (p_fill_bottle_module->reject_keep_delay == 0){
			fill_stop_reject(p_fill_bottle_module);
		}
	}
}
//
void fill_common_op (s_fill_bottle_module * p_fill_bottle_module)
{
	if (p_fill_bottle_module->big_gate_delay > 0){
		p_fill_bottle_module->big_gate_delay--;
	}
	if (p_fill_bottle_module->nozzle_delay > 0){
		p_fill_bottle_module->nozzle_delay--;
	}
	if (p_fill_bottle_module->shift_bottle_delay > 0){
		p_fill_bottle_module->shift_bottle_delay--;
	}
	if ((get_sys_run_time() % ((COUNTER_SYSTEM_ONE_SECOND_CTR+1) * 20)) == 0){
		p_fill_bottle_module->speed = (p_fill_bottle_module->total_count_sum - p_fill_bottle_module->total_good_tmp) * 3;
		p_fill_bottle_module->total_good_tmp = p_fill_bottle_module->total_count_sum;
	}
	fill_check_reject_q_out_delay (p_fill_bottle_module);
	fill_check_reject_keep_delay (p_fill_bottle_module);
}
//
void clear_op_delay (s_fill_bottle_module * p_fill_bottle_module)
{
	p_fill_bottle_module->fill_bottle_op_delay = 0;
}
//
void fill_bottle_sys_unuse_big_gate_process (s_fill_bottle_module * p_fill_bottle_module, uint16_t input_map)//不使用总料门装瓶步骤
{		
	////////////////////////////////////////////////////////////////////////////
	p_fill_bottle_module->input_buf = input_map;
	if (GET_BIT(input_map, OUTPUT_BUF_VIB) == 0){//振动器
		SET_OUTPUT_ON(&p_fill_bottle_module->output_buf, OUTPUT_BUF_VIB);
	}else{
		SET_OUTPUT_OFF(&p_fill_bottle_module->output_buf, OUTPUT_BUF_VIB);
	}
	////////////////////////////////////////////////////////////////////////////
	if (p_fill_bottle_module->first_run == FIRST_RUN){
		p_fill_bottle_module->first_run = !FIRST_RUN;
		fill_bottle_module_reset ();
		return;
	}
	fill_common_op (p_fill_bottle_module);
	if (p_fill_bottle_module->fill_bottle_op_delay > 0){
		p_fill_bottle_module->fill_bottle_op_delay--;
		if (p_fill_bottle_module->fill_bottle_op_delay > 0){
			return;
		}
	}
	switch (p_fill_bottle_module->fill_bottle_state)
	{
		case STANDBY:
			fill_open_big_gate_door (p_fill_bottle_module);
			clear_op_delay(p_fill_bottle_module);//这里不需要延时
			if (GET_BIT (p_fill_bottle_module->input_buf, OUTPUT_BUF_COMPLETE) == 0){
				fill_set_fill_bottle_complete (p_fill_bottle_module);
				p_fill_bottle_module->fill_bottle_state = WAIT_PIECE_FALL;
			}else{
				break;
			}
		case WAIT_PIECE_FALL://装料延时操作
			fill_wait_piece_fall (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = READY_TO_SHIFT_BOTTLE;
			break;
		case READY_TO_SHIFT_BOTTLE://准备换下一个瓶子
			fill_ready_to_shift_bottle (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = UP_NOZZLE;
			//break;
		case UP_NOZZLE:
			fill_up_nozzle (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = OPEN_SHIFT_BOTTLE;
			break;
		case OPEN_SHIFT_BOTTLE:
			fill_open_shift_bottle (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = CLOSE_SHIFT_BOTTLE;
			break;
		case CLOSE_SHIFT_BOTTLE:
			fill_close_shift_bottle (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = DOWN_NOZZLE;
			break;
		case DOWN_NOZZLE:
			fill_down_nozzle (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = FILL_BOTTLE_FINISHED;
			break;
		case FILL_BOTTLE_FINISHED:
			fill_reset_fill_bottle_complete (p_fill_bottle_module);
			fill_bottle_finished (p_fill_bottle_module);
			break;
		default:break;
	}
}
//
void fill_bottle_sys_use_big_gate_process (s_fill_bottle_module * p_fill_bottle_module, const uint16_t input_map)//不使用总料门装瓶步骤
{
	fill_nozzle_vib_process (p_fill_bottle_module);
	////////////////////////////////////////////////////////////////////////////
	p_fill_bottle_module->input_buf = input_map;
	if (GET_BIT(input_map, OUTPUT_BUF_VIB) == 0){//振动器
		SET_OUTPUT_ON(&p_fill_bottle_module->output_buf, OUTPUT_BUF_VIB);
	}else{
		SET_OUTPUT_OFF(&p_fill_bottle_module->output_buf, OUTPUT_BUF_VIB);
	}
	////////////////////////////////////////////////////////////////////////////
	if (p_fill_bottle_module->first_run == FIRST_RUN){
		p_fill_bottle_module->first_run = !FIRST_RUN;
		fill_bottle_module_reset ();
		return;
	}
	fill_common_op (p_fill_bottle_module);
	if (p_fill_bottle_module->fill_bottle_op_delay > 0){
		p_fill_bottle_module->fill_bottle_op_delay--;
		if (p_fill_bottle_module->fill_bottle_op_delay > 0){
			return;
		}
	}
	switch (p_fill_bottle_module->fill_bottle_state)
	{
		case STANDBY:
			if (GET_BIT (p_fill_bottle_module->output_buf, OUTPUT_BUF_BIG_GATE_DOOR) == 0){
				inc_Modbus_test_value (100);
			}
			fill_close_big_gate_door (p_fill_bottle_module);
			clear_op_delay(p_fill_bottle_module);//这里不需要延时
			if (GET_BIT (input_map, OUTPUT_BUF_COMPLETE) == 0){//数粒完成
				fill_set_fill_bottle_complete (p_fill_bottle_module);
				p_fill_bottle_module->fill_bottle_state = OPEN_BIG_GATE;
			}else{
				break;
			}
		case OPEN_BIG_GATE:
			fill_open_big_gate_door (p_fill_bottle_module);
			p_fill_bottle_module->nozzle_vib_status = p_fill_bottle_module->set_para.set_enable_nozzle_vib;//料嘴开始振动
			p_fill_bottle_module->nozzle_vib_up_time = p_fill_bottle_module->set_para.set_nozzle_vib_up_time;
			p_fill_bottle_module->nozzle_vib_down_time = 0;
			p_fill_bottle_module->fill_bottle_state = CLOSE_BIG_GATE;
			break;
		case CLOSE_BIG_GATE:
			fill_close_big_gate_door (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = READY_TO_SHIFT_BOTTLE;
			break;
		case READY_TO_SHIFT_BOTTLE://准备换下一个瓶子
			if (GET_BIT (p_fill_bottle_module->output_buf, OUTPUT_BUF_BIG_GATE_DOOR) == 0){
				inc_Modbus_test_value (1000);
			}
			fill_close_big_gate_door (p_fill_bottle_module);
			clear_op_delay(p_fill_bottle_module);//这里不需要延时
			fill_ready_to_shift_bottle (p_fill_bottle_module);
			fill_reset_fill_bottle_complete (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = WAIT_PIECE_FALL;
			break;
		case WAIT_PIECE_FALL://装料延时操作
			fill_wait_piece_fall (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = UP_NOZZLE;
			break;
		case UP_NOZZLE:
			fill_up_nozzle (p_fill_bottle_module);
			p_fill_bottle_module->nozzle_vib_status = MY_DISABLE;//料嘴停止振动
			p_fill_bottle_module->nozzle_vib_up_time = 0;
			p_fill_bottle_module->nozzle_vib_down_time = 0;
			p_fill_bottle_module->fill_bottle_state = OPEN_SHIFT_BOTTLE;
			break;
		case OPEN_SHIFT_BOTTLE:
			fill_open_shift_bottle (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = CLOSE_SHIFT_BOTTLE;
			break;
		case CLOSE_SHIFT_BOTTLE:
			fill_close_shift_bottle (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = DOWN_NOZZLE;
			break;
		case DOWN_NOZZLE:
			fill_down_nozzle (p_fill_bottle_module);
			p_fill_bottle_module->fill_bottle_state = FILL_BOTTLE_FINISHED;
			break;
		case FILL_BOTTLE_FINISHED:
			fill_bottle_finished (p_fill_bottle_module);
			break;
		default:break;
	}
}
//
void fill_bottle_module_reset (void)
{
	uint16_t i;
	for (i = 0; i < 2; i++){
		fill_reset_fill_bottle_complete (&fill_bottle_module[i]);
		fill_down_nozzle (&fill_bottle_module[i]);
		fill_stop_reject (&fill_bottle_module[i]);
		fill_close_shift_bottle (&fill_bottle_module[i]);
		if (fill_bottle_module[i].set_para.set_enable_big_gate == MY_ENABLE){
			fill_close_big_gate_door (&fill_bottle_module[i]);
		}else{
			fill_open_big_gate_door (&fill_bottle_module[i]);
		}
		fill_bottle_module[i].fill_bottle_op_delay = 0;
		fill_bottle_module[i].input_buf = 0xFFFF;
		fill_bottle_module[i].output_buf = 0xFFFF;
		fill_bottle_module[i].nozzle_vib_status = MY_DISABLE;
		fill_bottle_module[i].reject_keep_delay = 0;
		fill_bottle_module[i].big_gate_delay = 0;
		fill_bottle_module[i].nozzle_delay = 0;
		fill_bottle_module[i].shift_bottle_delay = 0;
	}
}
void fill_bottle_system_module_poll (s_fill_bottle_module * p_fill_bottle_module, const uint16_t input_map)
{

	if (p_fill_bottle_module->set_para.set_enable_big_gate == MY_ENABLE){
		fill_bottle_sys_use_big_gate_process (p_fill_bottle_module, input_map);//使用总料门
	}else{
		fill_bottle_sys_unuse_big_gate_process (p_fill_bottle_module, input_map);//不使用使用总料门
	}
}


void fill_output_map_func ()
{
	fill_bottle_module[0].input_buf_map = fill_bottle_module[0].input_buf;
	fill_bottle_module[0].output_buf_map = fill_bottle_module[0].output_buf;
//	LED1 = GET_BIT (fill_bottle_module[0].output_buf, OUTPUT_BUF_COMPLETE);
//	LED3 = GET_BIT (fill_bottle_module[0].output_buf, OUTPUT_BUF_NOZZLE);
	MAP_FILL_SIG_TO_EXT_OUTPUT (0, 16, OUTPUT_BUF_VIB);
	MAP_FILL_SIG_TO_EXT_OUTPUT (0, 17, OUTPUT_BUF_BIG_GATE_DOOR);
	MAP_FILL_SIG_TO_EXT_OUTPUT (0, 18, OUTPUT_BUF_NOZZLE);
	MAP_FILL_SIG_TO_EXT_OUTPUT (0, 19, OUTPUT_BUF_BOTTLE_SHIFT);
	MAP_FILL_SIG_TO_EXT_OUTPUT (0, 20, OUTPUT_BUF_REJECT);
	MAP_FILL_SIG_TO_EXT_OUTPUT (0, 31, OUTPUT_BUF_COMPLETE);
}










