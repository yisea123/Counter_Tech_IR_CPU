#include "main.h"





s_counter_module counter_module[2];
s_counter_env counter_env;
//s_counter_env counter_env;
void switch_to_normal_count (s_counter_module * p_counter_module);

void open_big_gate_door (s_counter_module * p_counter_module, uint32_t * op_delay);
void close_big_gate_door (s_counter_module * p_counter_module, uint32_t * op_delay);
void up_nozzle (s_counter_module * p_counter_module, uint32_t * op_delay);
void down_nozzle (s_counter_module * p_counter_module, uint32_t * op_delay);
void start_reject (s_counter_module * p_counter_module);
void stop_reject (s_counter_module * p_counter_module);
void open_shift_bottle (s_counter_module * p_counter_module, uint32_t * op_delay);
void close_shift_bottle (s_counter_module * p_counter_module, uint32_t * op_delay);
void start_vibrate (s_counter_module * p_counter_module);
void stop_vibrate (s_counter_module * p_counter_module);
void counter_output_map_func (void);
void counter_reset (void);
void counter_start (void);
void counter_stop (void);
void counter_data_clear(void);
void counter_init (void)
{
	uint32_t i, j;
	S8 *p;
	for (j = 0; j < 2; j++){
		p = (S8 *) &counter_module[j];
		for (i = 0; i < sizeof(s_counter_module); i++){
			*(p++) = 0;
		}
	}
	sys_run_time.data_hl = 0;
}
//


//

void update_piece_info(s_counter_module * p_counter_module, s_chanel_info * p_chanel) {
	if (p_counter_module->min_piece_interval > p_chanel->piece_interval){
		p_counter_module->min_piece_interval = p_chanel->piece_interval;
	}
	if (p_counter_module->max_length < p_chanel->length){
		p_counter_module->max_length = p_chanel->length;
	}
	if (p_counter_module->min_length > p_chanel->length){
		p_counter_module->min_length = p_chanel->length;
	}
}

void check_normal_count_length(s_counter_module * p_counter_module, s_chanel_info * p_chanel) {
	if (p_chanel->length > p_counter_module->set_para.set_max_length){ /*超过设定长度*/
		p_counter_module->normal_count_rej_flag |= REJ_TOO_LONG;
	}
}
void check_pre_count_length(s_counter_module * p_counter_module, s_chanel_info * p_chanel) {
	if (p_chanel->length > p_counter_module->set_para.set_max_length){ /*超过设定长度*/
		p_counter_module->pre_count_rej_flag |= REJ_TOO_LONG;
	}
}
//
void check_pre_count_close_door_interval(s_counter_module * p_counter_module, s_chanel_info * p_chanel) {
	p_chanel->close_door_interval = p_chanel->piece_interval;
	if (p_counter_module->min_close_door_interval > p_chanel->close_door_interval){
		p_counter_module->min_close_door_interval = p_chanel->close_door_interval;
	}
	/*判断是否小料门关闭时药粒间隔太小*/
	if (p_chanel->close_door_interval < p_counter_module->set_para.set_door_close_interval){
		p_counter_module->normal_count_rej_flag |= REJ_TOO_CLOSE;
		p_counter_module->pre_count_rej_flag |= REJ_TOO_CLOSE;
	}
}
//
void check_pre_count_door_switch_interval (s_counter_module * p_counter_module, s_chanel_info * p_chanel) {
	if (p_counter_module->min_door_switch_interval > p_chanel->door_switch_interval){
		p_counter_module->min_door_switch_interval = p_chanel->door_switch_interval;
	}
	/*判断是否小料门开关间隔太小*/
	if ((p_chanel->door_switch_interval + p_chanel->ch_door_close_delay) < (p_counter_module->set_para.set_door_switch_interval)){
		/*开关频率太快，需额外加一段延时等待料门完全打开*/
		p_chanel->ch_door_close_delay = (p_counter_module->set_para.set_door_switch_interval) - (p_chanel->door_switch_interval + p_chanel->ch_door_close_delay);
		p_counter_module->normal_count_rej_flag |= REJ_DOOR_SWITCH_TOO_FAST;
		p_counter_module->pre_count_rej_flag |= REJ_DOOR_SWITCH_TOO_FAST;
	}
}
//

void set_all_chanel_status (s_counter_module * p_counter_module, uint16_t status) {
	uint16_t i;
	for (i = 0; i < CHANEL_NUM; i++){
		p_counter_module->chanel[i].counter_state = status;
	}
}

void set_32io_bit (uint16_t *p_value, uint32_t mask)
{
	*p_value |= mask;
}
void reset_32io_bit (uint16_t *p_value, uint32_t mask)
{
	*p_value &= ~mask;
}
////////////////////////////////////////////////////////////////////////////////
void counter_module_pieces_process(s_counter_module * p_counter_module, uint16 ch)
{
	s_chanel_info *p_chanel;
	p_chanel = &p_counter_module->chanel[ch];
	p_chanel->input_status_new = ((p_counter_module->input_buf_low) & (0x1 << ch)) != 0? 1 : 0;
	//1 表示有效， 0表示无效
	///////////////////////////////////////////////////////////
	if ((p_chanel->input_status_new == DETECT_OFF) && (p_chanel->input_status_old == DETECT_OFF)){      //电眼没有药粒状态
		if (p_chanel->piece_interval < MAX_DISPLAY){
			p_chanel->piece_interval++;
		}else{
		}
	}else if ((p_chanel->input_status_new == DETECT_ON) && (p_chanel->input_status_old == DETECT_OFF)){//药粒刚进电眼状态
		p_chanel->length = 0;
	}else if ((p_chanel->input_status_new == DETECT_ON) && (p_chanel->input_status_old == DETECT_ON)){//药粒进入电眼状态
		if (p_chanel->length < MAX_DISPLAY){
			p_chanel->length++;
		}else{
		}
	}else if ((p_chanel->input_status_new == DETECT_OFF) && (p_chanel->input_status_old == DETECT_ON)){//药粒刚出电眼状态
		////////////////////////////////////////////////////////////////////////////////////
		if (p_chanel->length >= p_counter_module->set_para.set_min_length){//长度大于设定值最小长度才能计数
			/*药粒出了检测区，更新药粒相关信息****************************************************************/
			update_piece_info (p_counter_module, p_chanel);
			///////////////////////////////////////////////////////////////////////////////////////////
			//计数/////////////////////////////////////////////////////////////////////////////////////
			p_chanel->cur_count++;
			///////////////////////////////////////////////////////////////////////////////////////////
			switch (p_chanel->counter_state)
			{
				case NORMAL_COUNT://通道正常数粒状态
					if (p_counter_module->normal_count >= p_counter_module->set_para.set_count){//错误状态
						p_counter_module->system_status = COUNTER_ERROR;
					}
					check_normal_count_length (p_counter_module, p_chanel);//检查药粒长度
					p_counter_module->normal_count++;
					if (p_counter_module->normal_count == p_counter_module->set_para.set_count){//当前这一瓶的最后一粒
						set_all_chanel_status(p_counter_module, SEPARATE_PRE_COUNT);//通知其他通道下一颗要进行分药动作
						p_counter_module->last_piece_chanel_id = ch;//记录发出通知的通道ID
					}
					break;
				case SEPARATE_PRE_COUNT://每个通道的下一瓶的第一颗药，需要小料门分药动作
					p_chanel->ch_door_close_delay = p_counter_module->set_para.set_door_close_delay + 1;/*小料门关闭延时*/
					check_pre_count_length (p_counter_module, p_chanel);//检查药粒长度
					check_pre_count_close_door_interval (p_counter_module, p_chanel);//检查小料门关门间隔
					check_pre_count_door_switch_interval (p_counter_module, p_chanel);//检查小料门开关间隔
					p_chanel->cur_count = 1;//每个通道的下一瓶的第一颗药
					p_counter_module->pre_count++;
					p_chanel->counter_state = PRE_COUNT;
					break;
				case PRE_COUNT://通道预数粒状态
					check_pre_count_length (p_counter_module, p_chanel);
					p_counter_module->pre_count++;
					break;
				default://错误状态
					p_counter_module->system_status = STATUS_ERROR;
					break;
				}
		}else{//长度小于设定值最小长度不计数
		}
		////////////////////////////////////////////////////////////////////////////////
		p_chanel->piece_interval = 0;
		////////////////////////////////////////////////////////////////////////////////
		if (p_counter_module->pre_count > p_counter_module->set_para.set_count){//预数超过设定数
			p_counter_module->pre_count_rej_flag |= REJ_TOO_MORE;//更新剔除原因
		}
	}
	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	p_chanel->input_status_old = p_chanel->input_status_new;
	//////////////////////////////////////////////////////////////////////////////////
}
//
void set_reject_flag (s_counter_module * p_counter_module)
{
	p_counter_module->reject_flag = p_counter_module->normal_count_rej_flag;
	if (p_counter_module->reject_flag != 0){
		p_counter_module->last_reject_flag = p_counter_module->reject_flag;//保存最后一次提出原因
		SET_OUTPUT_ON(&p_counter_module->output_buf_high, OUTPUT_BUF_REJECT);
	}
}
//
void start_vibrate (s_counter_module * p_counter_module)
{
	SET_OUTPUT_ON(&p_counter_module->output_buf_high, OUTPUT_BUF_VIB);
}
//
void stop_vibrate (s_counter_module * p_counter_module)
{
	SET_OUTPUT_OFF(&p_counter_module->output_buf_high, OUTPUT_BUF_VIB);
}
//
void close_specify_door (s_counter_module * p_counter_module, uint16 door_id)
{
	if (door_id < CHANEL_NUM){
		p_counter_module->chanel[door_id].door_is_close = 1;
		SET_OUTPUT_ON (&p_counter_module->output_buf_low, (0x01 << door_id));
	}
}
void set_count_complete (s_counter_module * p_counter_module)
{
	SET_OUTPUT_ON(&p_counter_module->output_buf_high, OUTPUT_BUF_COMPLETE);
	p_counter_module->counter_module_state = FILL_BOTTLE;
}
void reset_count_complete (s_counter_module * p_counter_module)
{
	p_counter_module->counter_module_state = COUNTER_IDLE;
	SET_OUTPUT_OFF(&p_counter_module->output_buf_high, OUTPUT_BUF_COMPLETE);
	SET_OUTPUT_OFF(&p_counter_module->output_buf_high, OUTPUT_BUF_REJECT);
}
void open_all_door (s_counter_module * p_counter_module)
{
	uint16_t i;
	for (i = 0; i < CHANEL_NUM; i++){
		p_counter_module->chanel[i].ch_door_close_delay = 0;
		if (p_counter_module->chanel[i].door_is_close){
			p_counter_module->chanel[i].door_is_close = 0;
			p_counter_module->chanel[i].door_switch_interval = 0;
		}
	}
	SET_OUTPUT_OFF (&p_counter_module->output_buf_low, ALL_DOOR_MASK);
}
//
void switch_to_normal_count (s_counter_module * p_counter_module)
{
	p_counter_module->set_para.set_count = p_counter_module->set_para.set_count_new;
	p_counter_module->set_para.set_pre_count = p_counter_module->set_para.set_pre_count_new;
	if (p_counter_module->pre_count < p_counter_module->set_para.set_count){//如果预数粒还没数够
		set_all_chanel_status (p_counter_module, NORMAL_COUNT);
	}else{
		set_all_chanel_status (p_counter_module, SEPARATE_PRE_COUNT);
	}
	reset_count_complete (p_counter_module);
	start_vibrate(p_counter_module);
	p_counter_module->normal_count = p_counter_module->pre_count;//
	p_counter_module->pre_count = 0;
	p_counter_module->normal_count_rej_flag = p_counter_module->pre_count_rej_flag;//
	p_counter_module->pre_count_rej_flag = 0;
	open_all_door (p_counter_module);//打开所有小料门
}
//
void check_door_delay (s_counter_module * p_counter_module)
{
	uint16_t i;
	for (i = 0; i < CHANEL_NUM; i++){
		if (p_counter_module->chanel[i].ch_door_close_delay > 0){
			p_counter_module->chanel[i].ch_door_close_delay--;
			if (p_counter_module->chanel[i].ch_door_close_delay == 0){
				close_specify_door (p_counter_module, i);
			}
		}
	}
}
//
void common_op (s_counter_module * p_counter_module)
{
	uint16_t i;
	if (counter_env.running_status == 1){//停机状态
		stop_vibrate(p_counter_module);
	}else{
		start_vibrate(p_counter_module);
	}
	if (p_counter_module->pre_count >= p_counter_module->set_para.set_pre_count){//达到设定的预数
		stop_vibrate(p_counter_module);
	}		
	for (i = 0; i < CHANEL_NUM; i++){
		if (p_counter_module->chanel[i].door_switch_interval < MAX_DISPLAY){
			if (p_counter_module->chanel[i].door_is_close == 0){
				p_counter_module->chanel[i].door_switch_interval++;
			}
		}
	}
	set_reject_flag (p_counter_module);
}
//
void poll_fill_bottle_status (s_counter_module * p_counter_module)
{
	static uint16_t poll_status = 0;
	switch (poll_status)
	{
		case 0:
			if (p_counter_module->normal_count >= p_counter_module->set_para.set_count){//等待数量完成
				if (p_counter_module->start_rej < p_counter_module->set_para.set_start_rej){
					p_counter_module->start_rej++;
					p_counter_module->normal_count_rej_flag |= REJ_START;
				}
				set_count_complete (p_counter_module);
				poll_status++;
			}
			break;
		case 1:
			if (GET_BIT(p_counter_module->input_buf_high, OUTPUT_BUF_COMPLETE) == 0){//等待信号拉低
				poll_status++;
			}
			break;
		case 2:
			if (GET_BIT(p_counter_module->input_buf_high, OUTPUT_BUF_COMPLETE) == 1){//等待信号拉高
				switch_to_normal_count(p_counter_module);
				poll_status = 0;
			}
			break;
		default:break;
	}
}
void counter_module_poll_chanel (s_counter_module * p_counter_module, const uint16_t input_high, const uint16_t input_low)
{
	uint16_t i;
	////////////////////////////////////////////////////////////////////////////////
	p_counter_module->input_buf_high = input_high;		
	p_counter_module->input_buf_low = input_low;		
	////////////////////////////////////////////////////////////////////////////////
	if (p_counter_module->first_run == FIRST_RUN){
		p_counter_module->first_run = !FIRST_RUN;
		counter_reset ();
	}
	////////////////////////////////////////////////////////////////////////////////
	common_op (p_counter_module);
	poll_fill_bottle_status (p_counter_module);
	for (i = 0; i <CHANEL_NUM; i++){
		counter_module_pieces_process (p_counter_module, i);
	}	
	check_door_delay(p_counter_module);//检查小料门延时，看是否需要关闭			
	/////////////////////////////////////////////////////////////////////////////
}



uint16_t get_rand(uint16_t X, uint16_t Y)
{
	srand (get_tim5_ticks());
	//set_Modbus_test_value();
	return (rand()%(Y-X+1)+X);
}
void input_sim_ch (const uint16_t input_map, uint16_t ch)
{
	#define VIB_DELAY_STOP_X 8
	#define VIB_DELAY_STOP_Y 32
	#define VIB_DELAY_START 500
	#define LENGTH_X 		counter_module[0].set_para.set_sim_length_X
	#define LENGTH_Y 		counter_module[0].set_para.set_sim_length_Y
	#define INTERVAL_X 	counter_module[0].set_para.set_sim_interval_X
	#define INTERVAL_Y 	counter_module[0].set_para.set_sim_interval_Y
	static uint16_t vib_delay_stop = 0;
	static uint16_t vib_delay_start = 0;
	static uint16_t delay_op[CHANEL_NUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	static uint16_t status[CHANEL_NUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint16_t tmp;
	if (delay_op[ch] > 0){
		delay_op[ch]--;
		if (delay_op[ch] > 0){
			return;
		}
	}
	tmp = rand_tmp;
	switch (status[ch])
	{
		case 0:
			status[ch] = 1;
			delay_op[ch] = get_rand (INTERVAL_X, INTERVAL_Y);
			SET_OUTPUT_OFF (&tmp, (0x01 << ch));
			if (counter_env.sim_too_close_flag == 1){
				counter_env.sim_too_close_flag = 0;
				delay_op[ch] >>= 1;
			}
			break;
		case 1:
			if (vib_delay_start > 0){
				vib_delay_start--;
			}else{
				if (GET_BIT (input_map, OUTPUT_BUF_VIB) == 0){
					vib_delay_stop = get_rand (VIB_DELAY_STOP_X, VIB_DELAY_STOP_Y);;
					status[ch] = 0;
					delay_op[ch] = get_rand (LENGTH_X, LENGTH_Y);
					SET_OUTPUT_ON (&tmp, (0x01 << ch));
					if (counter_env.sim_too_long_flag == 1){
						counter_env.sim_too_long_flag = 0;
						delay_op[ch] *= 2;
					}
				}else{
					if (vib_delay_stop > 0){
						vib_delay_stop--;
						status[ch] = 0;
						delay_op[ch] = get_rand (LENGTH_X, LENGTH_Y);
						SET_OUTPUT_ON (&tmp, (0x01 << ch));
					}else{
						vib_delay_start = VIB_DELAY_START;
					}
				}
			}
			break;
		default:
			break;
	}
	rand_tmp = tmp;
}
void input_sim (const uint16_t input_map)
{
	uint16_t i;
	for (i = 0; i < CHANEL_NUM; i++){
		input_sim_ch(input_map, i);
	}
}
void input_output_test (void)
{
	#define MAP_INPUT_TO_OUTPUT(CH){ \
		EXT_OUT##CH = !EXT_IN##CH; \
	}
	MAP_INPUT_TO_OUTPUT(0);
	MAP_INPUT_TO_OUTPUT(1);
	MAP_INPUT_TO_OUTPUT(2);
	MAP_INPUT_TO_OUTPUT(3);
	MAP_INPUT_TO_OUTPUT(4);
	MAP_INPUT_TO_OUTPUT(5);
	MAP_INPUT_TO_OUTPUT(6);
	MAP_INPUT_TO_OUTPUT(7);
	MAP_INPUT_TO_OUTPUT(8);
	MAP_INPUT_TO_OUTPUT(9);
	MAP_INPUT_TO_OUTPUT(10);
	MAP_INPUT_TO_OUTPUT(11);
	MAP_INPUT_TO_OUTPUT(12);
	MAP_INPUT_TO_OUTPUT(13);
	MAP_INPUT_TO_OUTPUT(14);
	MAP_INPUT_TO_OUTPUT(15);
	MAP_INPUT_TO_OUTPUT(16);
	MAP_INPUT_TO_OUTPUT(17);
	MAP_INPUT_TO_OUTPUT(18);
	MAP_INPUT_TO_OUTPUT(19);
	MAP_INPUT_TO_OUTPUT(20);
	MAP_INPUT_TO_OUTPUT(21);
	MAP_INPUT_TO_OUTPUT(22);
	MAP_INPUT_TO_OUTPUT(23);
	MAP_INPUT_TO_OUTPUT(24);
	MAP_INPUT_TO_OUTPUT(25);
	MAP_INPUT_TO_OUTPUT(26);
	MAP_INPUT_TO_OUTPUT(27);
	MAP_INPUT_TO_OUTPUT(28);
	MAP_INPUT_TO_OUTPUT(29);
	MAP_INPUT_TO_OUTPUT(30);
	MAP_INPUT_TO_OUTPUT(31);
}
void counter_task_poll (void)
{
	static uint16_t tick_old;
	uint16_t period;
	uint16_t tick_new;
	uint16_t input_high;
	uint16_t input_low;
	
	////////////////////////////////////////////////////////////////////////////////
	period = get_tim5_ticks();
	if (period > tick_old){
		counter_env.scan_period = period - tick_old;
		if (counter_env.min_scan_period > counter_env.scan_period){
			counter_env.min_scan_period = counter_env.scan_period;
		}
		if (counter_env.max_scan_period < counter_env.scan_period){
			counter_env.max_scan_period = counter_env.scan_period;
		}
	}
	tick_old = period;
	////////////////////////////////////////////////////////////////////////////////
	if (counter_env.auto_scan == 1){
		if (counter_env.mannual_scan_time == 0){
			return;
		}else{
			if (counter_env.mannual_scan_time > 0){
				counter_env.mannual_scan_time--;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////
	sys_run_time.data_hl++;
	//input_output_test ();
	////////////////////////////////////////////////////////////////////////////////
	if (counter_env.clear_monitor_data == 1){
		counter_data_clear ();
	}
	if (counter_env.running_status == 0){
		counter_start ();
	}else{
		counter_stop ();
	}
	////////////////////////////////////////////////////////////////////////////////
	if (counter_env.auto_scan == 0){
		input_low = rand_tmp;//模拟数据
	}else{
		input_low = counter_module[0].input_buf_low;
	}
//	input_high = fill_bottle_module[0].output_buf;
	counter_module_poll_chanel (&counter_module[0], input_high, input_low);
	input_low = (counter_module[0].output_buf_high & (~SCREW_IS_RUN));//清零SCREW_IS_RUN位
	input_low |= counter_env.screw_is_run;
//	fill_bottle_system_module_poll (&fill_bottle_module[0], input_low);
	////////////////////////////////////////////////////////////////////////////////
	//counter_module_poll_chanel (&counter_module[1], input_map);	
	//fill_bottle_system_module_poll (&fill_bottle_module[1], counter_module[1].input_buf_high);
	//counter_module[1].input_buf_high = fill_bottle_module[1].output_buf;
	////////////////////////////////////////////////////////////////////////////////
	counter_output_map_func ();
//	fill_output_map_func ();
	////////////////////////////////////////////////////////////////////////////////
	if (counter_env.auto_scan == 0){
		input_sim (counter_module[0].output_buf_high);
	}
	////////////////////////////////////////////////////////////////////////////////
	tick_new = get_tim5_ticks();
	if (tick_new > tick_old){
		counter_env.scan_process_time = tick_new - tick_old + 2;
	}
	if (counter_env.min_scan_process_time > counter_env.scan_process_time){
		counter_env.min_scan_process_time = counter_env.scan_process_time;
	}
	if (counter_env.max_scan_process_time < counter_env.scan_process_time){
		counter_env.max_scan_process_time = counter_env.scan_process_time;
	}
	////////////////////////////////////////////////////////////////////////////////
}
void counter_output_map_func ()
{
	counter_module[0].input_buf_high_map = counter_module[0].input_buf_high;
	counter_module[0].input_buf_low_map = counter_module[0].input_buf_low;
	counter_module[0].output_buf_high_map = counter_module[0].output_buf_high;
	counter_module[0].output_buf_low_map = counter_module[0].output_buf_low;
//	LED1 = GET_BIT (counter_module[counter_id].output_buf_high, OUTPUT_BUF_BIG_GATE_DOOR);
//	LED3 = GET_BIT (counter_module[counter_id].output_buf_high, OUTPUT_BUF_NOZZLE);
	MAP_DOOR_TO_EXT_OUTPUT (0, 0, 0);
	MAP_DOOR_TO_EXT_OUTPUT (0, 1, 1);
	MAP_DOOR_TO_EXT_OUTPUT (0, 2, 2);
	MAP_DOOR_TO_EXT_OUTPUT (0, 3, 3);
	MAP_DOOR_TO_EXT_OUTPUT (0, 4, 4);
	MAP_DOOR_TO_EXT_OUTPUT (0, 5, 5);
	MAP_DOOR_TO_EXT_OUTPUT (0, 6, 6);
	MAP_DOOR_TO_EXT_OUTPUT (0, 7, 7);
	MAP_DOOR_TO_EXT_OUTPUT (0, 8, 8);
	MAP_DOOR_TO_EXT_OUTPUT (0, 9, 9);
	MAP_DOOR_TO_EXT_OUTPUT (0, 10, 10);
	MAP_DOOR_TO_EXT_OUTPUT (0, 11, 11);
	MAP_DOOR_TO_EXT_OUTPUT (0, 12, 12);
	MAP_DOOR_TO_EXT_OUTPUT (0, 13, 13);
	MAP_DOOR_TO_EXT_OUTPUT (0, 14, 14);
	MAP_DOOR_TO_EXT_OUTPUT (0, 15, 15);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 16, 0);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 17, 1);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 18, 2);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 19, 3);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 20, 4);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 21, 5);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 22, 6);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 23, 7);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 24, 8);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 25, 9);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 26, 10);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 27, 11);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 28, 12);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 29, 13);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 30, 14);
//	MAP_DOOR_TO_EXT_OUTPUT (0, 31, 15);
}
//
void counter_reset (void)
{
	uint16_t i, j;
	for (i = 0; i < COUTER_MODULE_NUM; i++){
		for (j = 0; j < CHANEL_NUM; j++){
			counter_module[i].chanel[j].input_status_new = 1;
			counter_module[i].chanel[j].input_status_old = 1;
		}
		switch_to_normal_count (&counter_module[i]);
		reset_count_complete (&counter_module[i]);

		counter_module[i].input_buf_high = 0xFFFF;
		counter_module[i].input_buf_low = 0xFFFF;
		counter_module[i].output_buf_high = 0xFFFF;
		counter_module[i].output_buf_low = 0xFFFF;
		counter_module[i].max_length = 0;
		counter_module[i].min_length = 999;
		counter_module[i].min_piece_interval = 999;
		counter_module[i].min_close_door_interval = 999;
		counter_module[i].min_door_switch_interval = 999;
		start_vibrate (&counter_module[i]);
	}
	counter_env.auto_scan = 0;
	counter_env.running_status = 1;
}
//
void counter_start (void)
{
	uint16_t i;
	for (i = 0; i < COUTER_MODULE_NUM; i++){
	}
}
//
void counter_stop (void)
{
	uint16_t i;
	for (i = 0; i < COUTER_MODULE_NUM; i++){
		counter_module[i].start_rej = 0;;
	}
}
//
void counter_data_clear (void)
{
	uint16_t i, j;
	counter_env.clear_monitor_data = 0;
	for (i = 0; i < COUTER_MODULE_NUM; i++){
		for (j = 0; j < CHANEL_NUM; j++){
			counter_module[i].max_length = 0;
			counter_module[i].min_length = 999;
			counter_module[i].min_piece_interval = 999;
			counter_module[i].min_close_door_interval = 999;
			counter_module[i].min_door_switch_interval = 999;
		}
	}
}
//
void test_counter_module (void)
{
	s_counter_module * p_counter_module;
	p_counter_module = &counter_module[0];
	
	p_counter_module->output_buf_high = OUTPUT_ALL_OFF;
	p_counter_module->output_buf_low = OUTPUT_ALL_OFF;
	start_vibrate (p_counter_module);
	stop_vibrate (p_counter_module);
}


















