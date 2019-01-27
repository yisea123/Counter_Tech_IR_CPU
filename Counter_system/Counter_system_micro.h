#ifndef _COUNTER_SYSTEM_MICRO_H_
#define _COUNTER_SYSTEM_MICRO_H_

//Rej flag
#define REJ_TOO_MORE 								0x0001 //多数
#define REJ_TOO_LONG 								0x0002 //超长
#define REJ_START 									0x0004 //开机剔除
#define REJ_TOO_BIG 								0x0008 //面积太大
#define REJ_TOO_SMALL 							0x0010 //面积太小
#define REJ_TOO_CLOSE 							0x0020 //关门间隔太小
#define REJ_DOOR_SWITCH_TOO_FAST 		0x0040 //开关间隔太小

#define OUTPUT_BUF_VIB 						0x0001
#define OUTPUT_BUF_BIG_GATE_DOOR 	0x0002
#define OUTPUT_BUF_NOZZLE					0x0004
#define OUTPUT_BUF_BOTTLE_SHIFT 	0x0008
#define OUTPUT_BUF_REJECT					0x0010
#define OUTPUT_BUF_COMPLETE				0x8000

#define DOOR0  0x0001
#define DOOR1  0x0002
#define DOOR2  0x0004
#define DOOR3  0x0008
#define DOOR4  0x0010
#define DOOR5  0x0020
#define DOOR6  0x0040
#define DOOR7  0x0080
#define DOOR8  0x0100
#define DOOR9  0x0200
#define DOOR10 0x0400
#define DOOR11 0x0800
#define DOOR12 0x1000
#define DOOR13 0x2000
#define DOOR14 0x4000
#define DOOR15 0x8000

#define MY_RAND_MAX 10
#define MY_ENABLE 	0
#define MY_DISABLE 1
#define FIRST_RUN 0

#define DETECT_ON 0
#define DETECT_OFF 1

#define COUTER_MODULE_NUM 2
#define CHANEL_NUM 16
#define REJECT_Q_16 16

#define READY_SIGNAL_CTR 5



#define SCREW_IS_RUN 0x02

#define SET_OUTPUT_ON(P_VALUE,MASK) reset_32io_bit(P_VALUE, MASK)
#define SET_OUTPUT_OFF(P_VALUE,MASK) set_32io_bit(P_VALUE, MASK)
#define OUTPUT_ALL_ON  (0x00000000)
#define OUTPUT_ALL_OFF (0xFFFF)
#define ALL_DOOR_MASK  (0xFFFF)

#define WAIT_PIECE_FALL_TIME (p_counter_module->set_para.set_fill_bottle_time)
#define OPEN_BIG_GATE_DOOR_DELAY (p_counter_module->set_para.set_gate_open_time)
#define CLOSE_BIG_GATE_DOOR_DELAY (p_counter_module->set_para.set_gate_close_time)
#define READY_TO_SHIFT_BOTTLE_DELAY 0
#define OPEN_SHIFT_BOTTLE_DELAY (p_counter_module->set_para.set_shift_bottle_time)
#define CLOSE_SHIFT_BOTTLE_DELAY 10
#define UP_NOZZLE_DELAY 10
#define DOWN_NOZZLE_DELAY 10
#define REJECT_START_DELAY  (p_counter_module->set_para.set_reject_delay)
#define REJECT_KEEP_DELAY  (p_counter_module->set_para.set_reject_keep)


#define MAX_DISPLAY (1000 - 1)
#define COUNTER_SYSTEM_ONE_SECOND_CTR (((1000/TIM2_INT_CYCLE) * 1000)-1)

#define GET_BIT(V,N) (((V & N) == 0) ? 0 : 1)

#endif


