#include "main.h"

//开始任务任务堆栈 
OS_STK START_TASK_STK[START_STK_SIZE]; 
//任务函数 
void start_task(void *pdata); 



//LED1 任务 
//设置任务优先级 
#define LED1_TASK_PRIO 9 
//设置任务堆栈大小 
#define LED1_STK_SIZE 128 
//任务堆栈 
OS_STK LED1_TASK_STK[LED1_STK_SIZE]; 
//任务函数 
void led1_task(void *pdata); 
 

//cmd 任务 
#define CMD_MSG_SIZE 1
//设置任务优先级 
OS_EVENT * cmd_msg;			//串口消息队列
void * cmdMsgGrp[CMD_MSG_SIZE];			//消息队列存储地址,最大支持1个消息
#define CMD_TASK_PRIO 7 
//设置任务堆栈大小 
#define CMD_STK_SIZE 128 
//任务堆栈 
OS_STK CMD_TASK_STK[CMD_STK_SIZE]; 
//任务函数 
void cmd_task(void *pdata); 

//ad8804 msg
#define AD8804_MSG_SIZE 1
OS_EVENT * ad8804_msg;			//串口消息队列
void * ad8804MsgGrp[AD8804_MSG_SIZE];			//消息队列存储地址,最大支持1个消息
 

//DEBUG 任务 
#define DEBUG_MSG_SIZE 1
OS_EVENT * debug_msg;			//串口消息队列
void * debugMsgGrp[DEBUG_MSG_SIZE];			//消息队列存储地址,最大支持1个消息

#define DEBUG_TASK_PRIO 6 
//设置任务堆栈大小 
#define DEBUG_STK_SIZE 128 
//任务堆栈 
//如果任务中使用printf 来打印浮点数据的话一定要8 字节对齐 
__align(8) OS_STK DEBUG_TASK_STK[DEBUG_STK_SIZE]; 
//任务函数 
void debug_task(void *pdata); 
 

//modbusRTU 任务 
#define MODBUSRTU_MSG_SIZE 3
OS_EVENT * modbusRTU_msg;			//串口消息队列
void * modbusRTUMsgGrp[MODBUSRTU_MSG_SIZE];			//消息队列存储地址,最大支持3个消息
//设置任务优先级 
#define MODBUSRTU_TASK_PRIO 5 
//设置任务堆栈大小 
#define MODBUSRTU_STK_SIZE 128 
//任务堆栈 
OS_STK MODBUSRTU_TASK_STK[MODBUSRTU_STK_SIZE]; 
//任务函数 
void modbusRTU_msg_task(void *pdata); 



//IO处理 任务 
#define IO_MSG_SIZE 32
OS_EVENT * io_msg;			//串口消息队列
void * ioMsgGrp[IO_MSG_SIZE];			//消息队列存储地址,最大支持1个消息
//设置任务优先级 
#define IO_TASK_PRIO 4 
//设置任务堆栈大小 
#define IO_STK_SIZE 128 
//任务堆栈 
OS_STK IO_TASK_STK[IO_STK_SIZE]; 
//任务函数 
void io_task(void *pdata); 


void uart3_SendData(u8 *buff,u8 len)
{
	HMI_RS485_TX_EN=1;//切换为发送模式
	while(len--)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);//等待发送区为空
		USART_SendData(USART3,*(buff++));
	}
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);//等待发送完成
	HMI_RS485_TX_EN = 0;//切换为接收模式
}
int main (void)
{
//	int a,b,c;
//	IO_DATA input;
	INT8U err;
	SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal */
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart1_init ();
	uart2_init ();
	uart3_init ();
	
	my_println ();
	my_println ("*****************************************************************");
	my_println ("                      Compile Time Info                     ");     
	my_println ("                      %s %s                    ", __DATE__, __TIME__);     
	my_println ("*****************************************************************");
	my_println ("*                   Micrium uC/OS-II Start                      *");
	my_println ("*****************************************************************");
	my_println ("my_cmd_Start at 0x%08x", (unsigned int)MY_CMD_RO_SEC_START);
	my_println ("my_cmd_End at   0x%08x", (unsigned int)MY_CMD_RO_SEC_END);
	my_println ("my_cmd_Length   0x%02x", (unsigned int)MY_CMD_RO_SEC_LENGTH);
	//my_println ("__my_cmd_usbboot at 0x%08x", (int)&__my_cmd_usbboot);
	
	my_println ("*************************Timer Int Info**************************");
	my_println ("TIM2 int cycle is %10d us", TIM2_INT_CYCLE);
	my_println ("TIM3 int cycle is %10d us", TIM3_INT_CYCLE);
	my_println ("TIM4 int cycle is %10d us", TIM4_INT_CYCLE);
	my_println ("TIM5 int cycle is %10d us", TIM5_INT_CYCLE);
	my_println ("TIM6 int cycle is %10d us", TIM6_INT_CYCLE);
	my_println ("TIM7 int cycle is %10d us", TIM7_INT_CYCLE);
		 	
//	my_println ("**************************SPI Flash Info*************************");
//	W25QXX_Init();			//W25QXX初始化
//	if (W25QXX_ReadID()!=W25Q128){
//		my_println ("W25Q128 Check Failed! ");
//	}else{
//		my_println ("W25Q128 Ready! ");
//	}
	//my_println ("****************************FatFs Info***************************");
	my_println ("*****************************************************************");
	LOG ("System Start");
	//test_counter_module ();

	my_println ("*****************************************************************");
	cmd ();
	OSInit(); //UCOS 初始化 
	OSTaskCreate(start_task, (void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1], START_TASK_PRIO); //创建开始任务 
	OSTaskNameSet(START_TASK_PRIO, (INT8U *)(void *)"start_task", &err);
	OSStart(); //开始任务 
	
}



//开始任务 
void start_task(void *pdata) 
{ 
#if OS_CRITICAL_METHOD == 3u                           /* Allocate storage for CPU status register     */
    OS_CPU_SR  cpu_sr = 0u;
#endif
	pdata=pdata; 
	INT8U err;
	
	OSStatInit(); //开启统计任务 
 
	OS_ENTER_CRITICAL(); //进入临界区(关闭中断) 
	//创建cmd消息队列
	cmd_msg=OSQCreate(&cmdMsgGrp[0], CMD_MSG_SIZE);	//创建消息队列
	//创建io消息队列
	io_msg=OSQCreate(&ioMsgGrp[0], IO_MSG_SIZE);	//创建消息队列
	//创建debug消息队列
	debug_msg=OSQCreate(&debugMsgGrp[0], CMD_MSG_SIZE);	//创建消息队列
	//创建modbusRTU消息队列
	modbusRTU_msg=OSQCreate(&modbusRTUMsgGrp[0], MODBUSRTU_MSG_SIZE);	//创建消息队列
	//创建ad8804消息队列
	ad8804_msg=OSQCreate(&ad8804MsgGrp[0], AD8804_MSG_SIZE);	//创建消息队列
	
	//创建CMD 任务 
	OSTaskCreate(cmd_task,(void*)0,(OS_STK*)&CMD_TASK_STK[CMD_STK_SIZE-1],CMD_TASK_PRIO); 
	OSTaskNameSet(CMD_TASK_PRIO, (INT8U *)(void *)"cmd_task", &err);
	//创建LED1 任务 
	OSTaskCreate(led1_task,(void*)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO); 
	OSTaskNameSet(LED1_TASK_PRIO, (INT8U *)(void *)"led1_task", &err);
	//创建IO 处理任务 
	OSTaskCreate(io_task,(void*)0,(OS_STK*)&IO_TASK_STK[IO_STK_SIZE-1],IO_TASK_PRIO); 
	OSTaskNameSet(IO_TASK_PRIO, (INT8U *)(void *)"io_task", &err);
	//创建测试任务 
	OSTaskCreate(debug_task,(void*)0,(OS_STK*)&DEBUG_TASK_STK[DEBUG_STK_SIZE-1],DEBUG_TASK_PRIO);
	OSTaskNameSet(DEBUG_TASK_PRIO, (INT8U *)(void *)"debug_task", &err);
	//创建ModbusRTU任务 
	OSTaskCreate(modbusRTU_msg_task,(void*)0,(OS_STK*)&MODBUSRTU_TASK_STK[MODBUSRTU_STK_SIZE-1],MODBUSRTU_TASK_PRIO);
	OSTaskNameSet(MODBUSRTU_TASK_PRIO, (INT8U *)(void *)"modbusRTU_task", &err);
	
	TIM2_Int_Init (TIM2_ARR, TIM2_PSC); //1ms
	//TIM3_PWM_Init (TIM3_ARR, TIM3_PSC); //1ms
	//TIM4_PWM_Init (TIM4_ARR, TIM4_PSC); //1ms
	TIM5_PWM_Init (TIM5_ARR, TIM5_PSC); //65.536ms
	TIM6_PWM_Init (TIM6_ARR, TIM6_PSC); //250us
	TIM7_PWM_Init (TIM7_ARR, TIM7_PSC); //4ms
	
	led_init ();
	drive_io_init ();
	system_env_init ();
	ad8804_init ();
	modbus_init ();
	AD_Sample_init ();
	
	//OSTaskSuspend(START_TASK_PRIO);//挂起开始任务 
	OS_EXIT_CRITICAL(); //退出临界区(开中断) 
		
	if ( OSTaskDel(OS_PRIO_SELF) != OS_ERR_NONE){
		my_println ("delete handle_task failed");
		OSTaskSuspend(START_TASK_PRIO);//删除失败，挂起开始任务 
	}	
} 


void modbusRTU_msg_task(void *pdata)
{
	u8 err;	
	uint32_t modbus_req = 0;
	pdata=pdata; 
	while (1){
		modbus_req = (uint32_t)OSQPend(modbusRTU_msg, 0, &err);
		if (modbus_req == 0x55){//uart 1 有MODBUS RTU 请求
			if ((uart1_rec_count > (MODBUS_RTU_MIN_LEN - 1)) && cmd_analyze.rec_buf[0] == my_env.modbus_rtu_addr){
				stop_uart1_receive ();
				modbus_analyze (cmd_analyze.rec_buf, 1, uart1_rec_count);
				if ((uint32_t)OSQPend(modbusRTU_msg, 20, &err) == 0xaa){ //传输完毕
					if (my_env.tty == TTY_MODBUS){
						my_env.tty = TTY_IDLE;
					}
					start_uart1_receive ();
				}else{//发送超时
					start_uart1_receive ();
				}	
			}else{
				start_uart1_receive ();
			}
		}else if (modbus_req == 0xA5){//uart 3 有MODBUS RTU 请求
			uart3_rec_count = UART3_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Channel3); 
			if ((uart3_rec_count > (MODBUS_RTU_MIN_LEN - 1)) && (uart3_rec_buf[0] == my_env.modbus_rtu_addr)){
				stop_uart3_receive ();
				modbus_analyze (uart3_rec_buf, 3, uart3_rec_count);
				if ((uint32_t)OSQPend(modbusRTU_msg, 20, &err) == 0xaa){ //传输完毕
					start_uart3_receive ();
				}else{//发送超时
					start_uart3_receive ();
				}
			}else{
				start_uart3_receive ();
			}
		}
	}
} 

//cmd 任务 
void cmd_task(void *pdata) 
{ 
	u8 err;	
	uint32_t msg;
	uint8_t last_byte = 0;
	//vTaskCmdAnalyze_cmd_str ("print ad all");
	while (1){
		msg = (uint32_t)OSQPend(cmd_msg,0,&err);
		if (msg == 0x55){    
			uart1_rec_count = CMD_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Channel5); 
			if (cmd_analyze.rec_buf[uart1_rec_count - 1] == CTRL_C && my_env.tty == TTY_CONSOLE){
				my_env.sys_break = 1;
			}else{
				my_env.tty = TTY_CONSOLE;//默认console状态
				uint8_t i;
				for (i = 0; i < uart1_rec_count - 1; i++){
					if (!isprint (cmd_analyze.rec_buf[i])){
						my_env.tty = TTY_MODBUS;
						break;
					}
				}
				if (my_env.tty == TTY_CONSOLE){//console
					last_byte = cmd_analyze.rec_buf[uart1_rec_count - 1];
					if (last_byte == 0x0D){
						cmd_analyze.rec_buf[uart1_rec_count - 1] = 0;
						my_println();
						//vTaskCmdAnalyze ();
						vTaskCmdAnalyze_cmd_str (cmd_analyze.rec_buf);
					}else if (last_byte == '~'){
						process_FunKey ();
					}else if ((last_byte == '\b')){
						if (uart1_rec_count > 1){//退格键
							uart1_send_data('\b');
							uart1_send_data(' ');
							uart1_send_data (0x1b);
							uart1_send_data (0x5b);
							uart1_send_data (0x44);
						}else{
							start_uart1_receive ();
						}
					}else{
						if (isprint (cmd_analyze.rec_buf[uart1_rec_count - 1])){
							uart1_send_data(cmd_analyze.rec_buf[uart1_rec_count - 1]);	
						}else{
							uart1_send_data (0x79);
						}
					}
				}else{//Modbus
					my_env.tty = TTY_MODBUS;
					OSQPost(modbusRTU_msg, (void *) 0x55);//发送消息
					//TIM_SetCounter(TIM7,0);//当接收到一个新的字节，将定时器7复位为0，重新计时（相当于喂狗）
					//TIM_Cmd(TIM7,ENABLE);//开始计时
				}
			}
		}
	}
} 

//U8 test_dma[] = "Hello world!";

void io_task (void *pdata)
{
	u8 err;	
	uint32_t msg;
	OS_Q_DATA q_data;
	while (1)
	{	
		if (my_env.is_registered != REGISTERED){//如果没有注册则不能进行操作
			delay_ms (200);
			continue;
		}
		msg = (uint32_t)OSQPend(io_msg,0,&err);
		delay_ms (10);//滤波
		OSQQuery (io_msg, &q_data);
		if (q_data.OSNMsgs != 0){//只使用队列中最后一个提交的信息
			continue;
		}
		if (msg == 0x55){//外部IO信号
			virtual_input[0] = PCin(6);
			virtual_input[2] = PAin(8);
			if (IS_SYS_RUNNING){
				virtual_input[1] = 1;
				virtual_input[3] = 1;
				virtual_input[4] = 1;
			}else{
				virtual_input[1] = PCin(7);
				virtual_input[3] = PGin(11);
				virtual_input[4] = PGin(0);
			}
			virtual_input[5] = PGin(1);
			virtual_input[6] = PGin(2);
			virtual_input[7] = PGin(3);
		}else if (msg != 0xaa){
			continue;
		}
		if (virtual_input[1] == 0){//重新调整
			virtual_input[1] = 1;
			re_calibration_detect();
		}
		if (virtual_input[3] == 0){//复位
			virtual_input[3] = 1;
			run_command("reset", 0);
		}
		if (virtual_input[6] == 0){//清零
			virtual_input[6] = 1;
			counter_data_clear ();
		}
	}
}
//LED1 任务 
void led1_task(void *pdata) 
{ 
	uint16_t adj_ctr;
	uart3_SendData ("helloworldnick", 14);
	my_env.system_runing = 1;
	//vTaskCmdAnalyze_cmd_str ("print ad all");
	while(1) 
	{ 
		if (my_env.is_registered == REGISTERED){
			switch (g_counter.system_status)
			{
				case IR_ADJ:
					led_output (adj_ctr);
					adj_ctr++;
					delay_ms(500); 
					break;
				case RUNNING_OK:
					led_alarm (1, 20, 1000);
					break;
				case COUNTER_ERROR:
					led_alarm (2, 200, 2000);
					break;
				case STATUS_ERROR:
					led_alarm (3, 200, 2000);
					break;
				case ADC_TIME_ERROR:
					led_alarm (4, 200, 2000);
					break;
				case DETECTOR_ERROR:
					led_alarm (5, 200, 2000);
					break;
				default:break;
			}
		}else{
			//闪烁三次然后停2秒，提示未注册
			led_alarm (3, 200, 2000);
		}
		Modbus_HoldReg_CPU_Usage = OSCPUUsage;
	}
} 


//浮点测试任务 
void debug_task(void *pdata) 
{ 
	u8 err;	
//	uint8_t i;
	re_calibration_detect();
	while (1)
	{
		
//////		if ((uint32_t)OSQPend(ad8804_msg, 0, &err) == 0xaa){
//////			LED3 = !LED3;
//////			DA_V++;
//////			if (DA_V > 50){
//////				DA_V = 0;
//////			}
//////			for (i = 0; i < 12; i++){
//////				ad8804_write_ch (ad8804_env.da_addr[i], DA_V);
//////			}
//////		}
		if ((uint32_t)OSQPend(debug_msg, 0, &err) == 0x55){
			my_println ("start");
			if (my_env.print == 0){
				start_uart1_dma ((uint32_t)AD_buff.buffer, AD_BUFF_SIZE * 2);
			}else if (my_env.print == 1){
				start_uart1_dma (g_counter.buf_addr, g_counter.ch[g_counter.set_watch_ch].sample_size * 2);
			}
			//start_uart1_dma ((uint32_t)test_dma, sizeof (test_dma));
		}
		
		if ((uint32_t)OSQPend(debug_msg, 0, &err) == 0xaa){
			my_println ("\nend");
			my_env.dma_state = 0;
			memset (AD_buff.buffer, 0, AD_BUFF_SIZE);
			if (my_env.print == 0){
				AD_buff.buffer_en = AD_buff.cir;
			}else{
				AD_buff.buffer_en = 0;
			}
		}
	}
} 

