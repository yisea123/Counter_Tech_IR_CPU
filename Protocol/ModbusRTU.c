#include "main.h"


/*
* 函数名 :CRC16
* 描述 : 计算CRC16
* 输入 : ptr---数据,len---长度
* 输出 : 校验值
*/ 
uint16_t CRC16(uint8_t *ptr, uint16_t len) 
{
	uint8_t i;
	uint16_t crc = 0xFFFF; 
	if (len == 0) {
		len = 1;
	} 
	while (len--) {
		crc ^= *ptr;
		for (i = 0; i<8; i++) {
			if (crc & 1) {
				crc >>= 1; crc ^= 0xA001;
			}else{
				crc >>= 1;
			}
		} ptr++;
	} 
	return((crc >> 8) |(crc << 8)); 
}



///////////////////////////////////////////////////////////

u8 RS485_tx_buf[256];//发送缓冲区
static uint16_t modbus_ch = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Modbus寄存器和单片机寄存器的映射关系
static vu32 *Modbus_InputIO[MODBUS_INPUT_NUM];//输入开关量寄存器指针(这里使用的是位带操作)
vu32 *Modbus_OutputIO[MODBUS_OUTPUT_NUM];//输出开关量寄存器指针(这里使用的是位带操作)
u16 *Modbus_HoldReg[MODBUS_REG_NUM];//保持寄存器

vu32 virtual_input[MODBUS_VIRTUAL_INPUT_NUM];

#define MAP_MODBUS_INPUT(D,PORT,S) Modbus_InputIO[D]=(vu32*)&P##PORT##in(S)
#define MAP_MODBUS_OUTPUT(D,PORT,S) Modbus_OutputIO[D]=(vu32*)&P##PORT##out(S)
#define MAP_MODBUS_HOLDREG(D,V) {Modbus_HoldReg[D] = &V;}

uint16_t Modbus_HoldReg_CPU_Usage = 0;
uint16_t Modbus_HoldReg_NULL = 999;
uint16_t Modbus_test_value = 0;
uint16_t Modbus_com_error = 0;
uint16_t data_change_flag = 0;

void inc_Modbus_test_value (uint16_t value)
{
	if (value > 0){
		Modbus_test_value += value;
	}else{
		Modbus_test_value++;
	}
}
void set_Modbus_test_value (uint16_t value)
{
	Modbus_test_value = value;
}

void Modbus_RegMap(void)
{
	int i;
	//初始化保持寄存器指针指向///////////////////////////////////
	for (i = 0; i < MODBUS_REG_NUM; i++){
		Modbus_HoldReg[i] = &Modbus_HoldReg_NULL;
	}	
	for (i = 0; i < MODBUS_VIRTUAL_INPUT_NUM; i++){
		virtual_input[i] = 1;
		Modbus_OutputIO[20 + i] = &virtual_input[i];
	}	
	MAP_MODBUS_HOLDREG(12, dma_irq_cycle);
	MAP_MODBUS_HOLDREG(13, counter_process_time);
	MAP_MODBUS_HOLDREG(14, tim2_irq_process_time);
	MAP_MODBUS_HOLDREG(15, ADC1_irq_cycle);
	MAP_MODBUS_HOLDREG(16, ADC1_process_time);
	MAP_MODBUS_HOLDREG(17, g_counter.ch[0].old_std_index);

	MAP_MODBUS_HOLDREG(20, chanel_pos_index);
	MAP_MODBUS_HOLDREG(21, Modbus_com_error);
	MAP_MODBUS_HOLDREG(22, my_env.ir_sub);
	MAP_MODBUS_HOLDREG(23, g_counter.count.data.pre_count);
	MAP_MODBUS_HOLDREG(24, g_counter.counter_step);
	MAP_MODBUS_HOLDREG(25, g_counter.total_reject);
	MAP_MODBUS_HOLDREG(26, my_env.print);
	MAP_MODBUS_HOLDREG(27, g_counter.rej_flag);
	MAP_MODBUS_HOLDREG(28, g_counter.total_good);
	MAP_MODBUS_HOLDREG(29, Modbus_HoldReg_CPU_Usage);
	////////////////////////////////////////////////////////////
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 0, 	g_counter.set_count_new);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 1, 	g_counter.set_pre_count_new);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 2, 	g_counter.set_door_close_delay);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 3,  g_counter.set_adc_mode);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 4, 	g_counter.set_max_interval.data.h);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 5, 	g_counter.set_max_interval.data.l);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 6, 	g_counter.set_door_close_interval.data.h);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 7, 	g_counter.set_door_close_interval.data.l);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 8, 	g_counter.set_max_len.data.h);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 9, 	g_counter.set_max_len.data.l);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 10, g_counter.set_min_len.data.h);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 11, g_counter.set_min_len.data.l);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 12, g_counter.set_std_numerator);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 13, g_counter.set_std_denumerator);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 14, g_counter.set_wave_down_flag);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 15, g_counter.set_wave_up_flag);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 16, g_counter.set_wave_up_value);
	MAP_MODBUS_HOLDREG(MODBUS_SAVE_DATA_START + 17, g_counter.set_watch_ch);

	for (i = 0; i < 12; i++){
		MAP_MODBUS_HOLDREG(64 + i, g_counter.ch[i].ad_min);
	}
	for (i = 0; i < 12; i++){
		MAP_MODBUS_HOLDREG(76 + i, g_counter.ch[i].ad_max);
	}
	for (i = 0; i < 12; i++){
		MAP_MODBUS_HOLDREG(88 + i, g_counter.ch[i].std_v);
	}
	for (i = 0; i < 12; i++){
		MAP_MODBUS_HOLDREG(100 + i, After_filter[i]);
	}
	for (i = 0; i < 12; i++){
		MAP_MODBUS_HOLDREG(112 + i, g_counter.ch[i].cur_count);
	}
	for (i = 0; i < 12; i++){
		MAP_MODBUS_HOLDREG(128 + i, g_counter.ch[i].std_down_v);
	}
	for (i = 0; i < 12; i++){
		MAP_MODBUS_HOLDREG(140 + i, g_counter.ch[i].process_step);
	}
	MAP_MODBUS_HOLDREG(499, g_counter.total_count_sum.data.h);
	MAP_MODBUS_HOLDREG(500, g_counter.total_count_sum.data.l);
	for (i = 0; i < 12; i++){
		MAP_MODBUS_HOLDREG(524 + i, g_counter.view_IR_DA_value[i]);
	}
	MAP_MODBUS_HOLDREG(536, g_counter.std_ref_value_old);
	MAP_MODBUS_HOLDREG(537, g_counter.std_ref_value);
	MAP_MODBUS_HOLDREG(538, g_counter.std_ref_value_offset);
	////////////////////////////////////////////////////////////
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1024 + i, AD_DMA_buf.AD_Value_2[i][0]);
	}
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1032 + i, AD_DMA_buf.AD_Value_2[i][1]);
	}
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1040 + i, AD_DMA_buf.AD_Value_2[i][2]);
	}
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1048 + i, AD_DMA_buf.AD_Value_2[i][3]);
	}
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1056 + i, AD_DMA_buf.AD_Value_2[i][4]);
	}
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1064 + i, AD_DMA_buf.AD_Value_2[i][5]);
	}
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1072 + i, AD_DMA_buf.AD_Value_2[i][6]);
	}
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1080 + i, AD_DMA_buf.AD_Value_2[i][7]);
	}
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1088 + i, AD_DMA_buf.AD_Value_2[i][8]);
	}
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1096 + i, AD_DMA_buf.AD_Value_2[i][9]);
	}
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1104 + i, AD_DMA_buf.AD_Value_2[i][10]);
	}
	for (i = 0; i < 8; i++){
		MAP_MODBUS_HOLDREG(1112 + i, AD_DMA_buf.AD_Value_2[i][11]);
	}
	save_para (0); //save_para(1) 保存参数save_para(0) 读取参数
	Modbus_HoldReg_NULL = 999;
}
//




void get_reg (s_reg_file *reg_file)
{
	uint16_t i;
//	W25QXX_Read ((U8*)reg_file, FLASH_REG_INFO_ADDR, sizeof(s_reg_file));
	STMFLASH_Read (FLASH_REG_INFO_ADDR, (uint16_t *)reg_file, sizeof(s_reg_file) / 2);
	my_println ();
	my_println ("-----------------------------------------------------------------");
	my_print("read reg info: ");
	for (i = 0; i < REG_SIZE; i++){
		my_print("%02x", reg_file->reg_info[i]);
	}
	my_println ();
	my_println ("-----------------------------------------------------------------");
}
int16_t check_reg_info (int16_t flag)
{	
	int16_t i;
	char id[32];
	char hash[16];
//	char reg_temp[REG_SIZE];
	s_reg_file *p_reg_file = NULL;
	
	p_reg_file = malloc (sizeof (s_reg_file));
	if (p_reg_file == NULL){
		my_println ("malloc (sizeof (s_reg_file)) failed! save para abort!");
		return -1;
	}
	memset (id, 0, sizeof(id));
	GetLockCode (id);
	MD5Digest(id, hash);
//	sprintf((char *)reg_temp, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
//		hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7],
//		hash[8], hash[9], hash[10], hash[11], hash[12], hash[13], hash[14], hash[15]
//	);
	get_reg (p_reg_file);
	for (i = 0; i < REG_SIZE; i++){
		if (p_reg_file->reg_info[i] != hash[i]){
			flag = -1;
			break;
		}
	}

	if (p_reg_file != NULL){
		free (p_reg_file);
	}
	return flag;
}
//
void read_para_from_flash (s_para_data_file * p_spi_flash_data)
{
//		W25QXX_Read ((U8*), FLASH_DATA_ADDR, sizeof(s_para_data_file));
	STMFLASH_Read (FLASH_DATA_ADDR, (uint16_t *)p_spi_flash_data, sizeof(s_para_data_file) / 2);
}
//
void save_para_to_flash (s_para_data_file *p_spi_flash_data)
{
//		W25QXX_Write ((U8*), FLASH_DATA_ADDR, sizeof(s_para_data_file));
	STMFLASH_Write (FLASH_DATA_ADDR, (uint16_t *)p_spi_flash_data, sizeof(s_para_data_file) / 2);
}
//
int16_t save_para (int16_t flag)
{
	int16_t i;
	s_para_data_file *p_spi_flash_data = NULL;

	if (flag == 1){
		if (data_change_flag == 0)
			return 0;
		else
			data_change_flag = 0;
	}
	p_spi_flash_data = malloc (sizeof (s_para_data_file));
	if (p_spi_flash_data == NULL){
		my_println ("malloc (sizeof (s_para_data_file)) failed! save para abort!");
		return -1;
	}
//check reg info*****************************************************************************
	flag = check_reg_info (flag);
//check reg info*****************************************************************************
	read_para_from_flash (p_spi_flash_data);
	if (flag > 0){//保存参数
		for (i = 0; i < MODBUS_SAVE_DATA_NUM; i++){
			p_spi_flash_data->SAVE_DATA[i] = *Modbus_HoldReg[MODBUS_SAVE_DATA_START + i];
		}
		for (i = 0; i < MODBUS_SAVE_DATA_NUM_EX; i++){
			p_spi_flash_data->SAVE_DATA_EX[i] = *Modbus_HoldReg[MODBUS_SAVE_DATA_START_EX + i];
		}
		save_para_to_flash (p_spi_flash_data);
	}else if(flag == 0){//读取参数
		check_para_flag (p_spi_flash_data);
		for (i = 0; i < MODBUS_SAVE_DATA_NUM; i++){
			*Modbus_HoldReg[MODBUS_SAVE_DATA_START + i] = p_spi_flash_data->SAVE_DATA[i];
		}
		for (i = 0; i < MODBUS_SAVE_DATA_NUM_EX; i++){
			*Modbus_HoldReg[MODBUS_SAVE_DATA_START_EX + i] = p_spi_flash_data->SAVE_DATA_EX[i];
		}
	}
	if (flag == -1){
		my_env.is_registered = NOT_REGISTERED;
		my_println("-_-");//未注册
	}else{
		my_env.is_registered = REGISTERED;
		my_println("^_^");//已注册
		cmd ();
	}
	if (p_spi_flash_data != NULL){
		free (p_spi_flash_data);
	}
	return 0;
}

void check_para_flag (s_para_data_file *p_spi_flash_data)
{
	#define PARA_MAGIC_NUM 0xA5A2
	int16_t i;
	if (p_spi_flash_data->para_flag != PARA_MAGIC_NUM){
		p_spi_flash_data->para_flag = PARA_MAGIC_NUM;
		////////////////////////////////////////////////////////////////
		STD_V_OFFSET = 50;
		STD_REF_VALUE = 28000;
		STD_REF_OFFSET = 200;
		WAVE_DOWN = 8;
		AD_MODE = 1;
		////////////////////////////////////////////////////////////////
		for (i = 0; i < MODBUS_SAVE_DATA_NUM; i++){
			p_spi_flash_data->SAVE_DATA[i] = *Modbus_HoldReg[MODBUS_SAVE_DATA_START + i];
		}
		for (i = 0; i < MODBUS_SAVE_DATA_NUM_EX; i++){
			p_spi_flash_data->SAVE_DATA_EX[i] = *Modbus_HoldReg[MODBUS_SAVE_DATA_START_EX + i];
		}
		save_para_to_flash (p_spi_flash_data);
	}
//	DATA_RANGE_CHECK ();
}

void check_data ()
{
}


void modbus_init(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	/*开启GPIOB和GPIOF的外设时钟*/
	RCC_APB2PeriphClockCmd( HMI_RS485_TX_EN_RCC_PORT, ENABLE);
	/*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	/*设置引脚速率为50MHz */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/*选择要控制的GPIOB引脚*/
	GPIO_InitStructure.GPIO_Pin = HMI_RS485_TX_EN_PIN_MAP;
	/*调用库函数，初始化GPIOG*/
	GPIO_Init(HMI_RS485_TX_EN_PORT, &GPIO_InitStructure);
	/* 关闭所有led灯	*/
	GPIO_SetBits(HMI_RS485_TX_EN_PORT, HMI_RS485_TX_EN_PIN_MAP);

	HMI_RS485_TX_EN=0;//开启接收模式

	Modbus_RegMap ();
	check_data ();
}
void Modbus_01_Handle(u8 * _data_buf);
void Modbus_02_Handle(u8 * _data_buf);
void Modbus_03_Handle(u8 * _data_buf);
void Modbus_05_Handle(u8 * _data_buf);
void Modbus_06_Handle(u8 * _data_buf);
void Modbus_15_Handle(u8 * _data_buf);
void Modbus_16_Handle(u8 * _data_buf);

//////////////////////////////////////////////////////////////////////////////
//发送n个字节数据
//buff:发送区首地址
//len：发送的字节数
void RS485_SendData(u8 *buff, u8 len)
{
	if (RS485_tx_buf[1] & 0x80){
		Modbus_com_error++;
	}
	if (modbus_ch == 1){
		start_uart1_dma ((uint32_t)RS485_tx_buf, len);
	}else if (modbus_ch == 3){
		HMI_RS485_TX_EN = 1;//切换为发送模式
		start_uart3_dma ((uint32_t)RS485_tx_buf, len);
	}
	modbus_ch = 0;
}

void modbus_analyze(u8 * _data_buf, uint16_t chanel, uint16_t len)
{
	u16 crc;
	u16 startRegAddr;
	if((_data_buf[1]==01)||(_data_buf[1]==02)||(_data_buf[1]==03)||(_data_buf[1]==05)||(_data_buf[1]==06)||(_data_buf[1]==15)||(_data_buf[1]==16))//功能码正确
	{
		modbus_ch = chanel;
		startRegAddr=(((u16)_data_buf[2])<<8)|_data_buf[3];//获取寄存器起始地址
		if(startRegAddr < MODBUS_REG_NUM+1){//寄存器地址在范围内
//		#if (MODBUS_RTU_USE_UART == 1)
//			crc = cmd_analyze.rec_buf[uart1_rec_count - 2] << 8 | cmd_analyze.rec_buf[uart1_rec_count - 1];
//			if (crc == CRC16 (cmd_analyze.rec_buf, uart1_rec_count - 2)){//CRC校验正确
//		#elif (MODBUS_RTU_USE_UART == 3)
//			crc = uart3_rec_buf[uart3_rec_count - 2] << 8 | uart3_rec_buf[uart3_rec_count - 1];
//			if (crc == CRC16 (uart3_rec_buf, uart3_rec_count - 2)){//CRC校验正确
//		#endif
			crc = _data_buf[len - 2] << 8 | _data_buf[len - 1];
			if (crc == CRC16 (_data_buf, len - 2)){//CRC校验正确
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				switch(_data_buf[1])//根据不同的功能码进行处理
				{
					case 1:{//读输出开关量
						Modbus_01_Handle(_data_buf);
						break;
					}
					case 2:{//读输入开关量
						Modbus_02_Handle(_data_buf);
						break;
					}
					case 03:{//读多个寄存器
						Modbus_03_Handle(_data_buf);
						break;
					}
					case 5:{//写单个输出开关量
						Modbus_05_Handle(_data_buf);
						OSQPost(io_msg, (void *) 0xaa);//发送消息
						break;
					}
					case 06: {//写单个寄存器
						Modbus_06_Handle(_data_buf);
						break;
					}
					case 15:{//写多个输出开关量
						Modbus_15_Handle(_data_buf);
						OSQPost(io_msg, (void *) 0xaa);//发送消息
						break;
					}
					case 16:{ //写多个寄存器
						Modbus_16_Handle(_data_buf);
						break;
					}
				}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
			else//CRC校验错误
			{
				RS485_tx_buf[0]=_data_buf[0];
				RS485_tx_buf[1]=_data_buf[1]| 0x80;
				RS485_tx_buf[2]=0x04; //异常码
				RS485_SendData(RS485_tx_buf,3);
			}
		}
		else//寄存器地址超出范围
		{
			RS485_tx_buf[0]=_data_buf[0];
			RS485_tx_buf[1]=_data_buf[1]|0x80;
			RS485_tx_buf[2]=0x02; //异常码
			RS485_SendData(RS485_tx_buf,3);
		}
	}
	else//功能码错误
	{
		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1]|0x80;
		RS485_tx_buf[2]=0x01; //异常码
		RS485_SendData(RS485_tx_buf,3);
	}
}


//Modbus功能码01处理程序
//读输出开关量
void Modbus_01_Handle(u8 * _data_buf)
{
	u16 ByteNum;
	u16 i;
	u16 calCRC;
	u16 RegNum;
	u16 startRegAddr;

	startRegAddr=(((u16)_data_buf[2])<<8)|_data_buf[3];//获取寄存器起始地址
	RegNum= (((u16)_data_buf[4])<<8)|_data_buf[5];//获取寄存器数量
	if((startRegAddr+RegNum - 1)<MODBUS_OUTPUT_NUM)//寄存器地址+数量在范围内
	{
		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1];
		ByteNum=RegNum/8;//字节数
		if(RegNum%8) ByteNum+=1;//如果位数还有余数，则字节数+1
		RS485_tx_buf[2]=ByteNum;//返回要读取的字节数
		for(i=0;i<RegNum;i++)
		{
			if(i%8==0) RS485_tx_buf[3+i/8]=0x00; //新字节开始
			RS485_tx_buf[3+i/8] >>= 1;//低位先发送
			RS485_tx_buf[3+i/8] |= ((*Modbus_OutputIO[startRegAddr+i])<<7)&0x80;
			if(i==RegNum-1)//发送到最后一个位了
			{
				if(RegNum%8) RS485_tx_buf[3+i/8] >>= 8-(RegNum%8);//如果最后一个字节还有余数，则剩余MSB填充0
			}
		}
		calCRC=CRC16(RS485_tx_buf,ByteNum+3);
		RS485_tx_buf[ByteNum+3]=(calCRC>>8)&0xFF;
		RS485_tx_buf[ByteNum+4]=calCRC&0xFF;
		RS485_SendData(RS485_tx_buf,ByteNum+5);
	}
	else//寄存器地址+数量超出范围
	{
		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1]|0x80;
		RS485_tx_buf[2]=0x02; //异常码
		RS485_SendData(RS485_tx_buf,3);
	}
}


//Modbus功能码02处理程序
//读输入开关量
void Modbus_02_Handle(u8 * _data_buf)
{
	u16 ByteNum;
	u16 i;
	u16 calCRC;
	u16 RegNum;
	u16 startRegAddr;

	startRegAddr=(((u16)_data_buf[2])<<8)|_data_buf[3];//获取寄存器起始地址
	RegNum= (((u16)_data_buf[4])<<8)|_data_buf[5];//获取寄存器数量
	if((startRegAddr+RegNum - 1)<MODBUS_INPUT_NUM)//寄存器地址+数量在范围内
	{
		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1];
		ByteNum=RegNum/8;//字节数
		if(RegNum%8) ByteNum+=1;//如果位数还有余数，则字节数+1
		RS485_tx_buf[2]=ByteNum;//返回要读取的字节数
		for(i=0;i<RegNum;i++)
		{
			if(i%8==0) RS485_tx_buf[3+i/8]=0x00;
			RS485_tx_buf[3+i/8]>>=1;//低位先发送
			RS485_tx_buf[3+i/8]|=((*Modbus_InputIO[startRegAddr+i])<<7)&0x80;
			if(i==RegNum-1)//发送到最后一个位了
			{
				if(RegNum%8) RS485_tx_buf[3+i/8]>>=8-(RegNum%8);//如果最后一个字节还有余数，则剩余MSB填充0
			}
		}
		calCRC=CRC16(RS485_tx_buf,ByteNum+3);
		RS485_tx_buf[ByteNum+3]=(calCRC>>8)&0xFF;
		RS485_tx_buf[ByteNum+4]=calCRC&0xFF;
		RS485_SendData(RS485_tx_buf,ByteNum+5);
	}
	else//寄存器地址+数量超出范围
	{
		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1]|0x80;
		RS485_tx_buf[2]=0x02; //异常码
		RS485_SendData(RS485_tx_buf,3);
	}
}


//Modbus功能码03处理程序
//读保持寄存器
void Modbus_03_Handle(u8 * _data_buf)
{
	u8 i;
	u16 calCRC;
	u16 RegNum;
	u16 startRegAddr;

	startRegAddr=(((u16)_data_buf[2])<<8)|_data_buf[3];//获取寄存器起始地址
	RegNum= (((u16)_data_buf[4])<<8)|_data_buf[5];//获取寄存器数量
	if((startRegAddr+RegNum - 1)<MODBUS_REG_NUM)//寄存器地址+数量在范围内
	{
		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1];
		RS485_tx_buf[2]=RegNum * 2;
		for(i=0;i<RegNum;i++)
		{
			RS485_tx_buf[3+i*2] = (*Modbus_HoldReg[startRegAddr+i] >> 8) & 0xFF; //先发送高字节
			RS485_tx_buf[4+i*2] = *Modbus_HoldReg[startRegAddr+i] & 0xFF;//后发送低字节
		}
		calCRC=CRC16(RS485_tx_buf,RegNum*2+3);

        RS485_tx_buf[RegNum*2+3]=(calCRC>>8)&0xFF;
        RS485_tx_buf[RegNum*2+4]=calCRC&0xFF;
		RS485_SendData(RS485_tx_buf,RegNum*2+5);
	}
	else//寄存器地址+数量超出范围
	{
		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1]|0x80;
		RS485_tx_buf[2]=0x02; //异常码
		RS485_SendData(RS485_tx_buf,3);
	}
}



//Modbus功能码05处理程序
//写单个输出开关量
void Modbus_05_Handle(u8 * _data_buf)
{
	u16 calCRC;
	u16 startRegAddr;

	startRegAddr=(((u16)_data_buf[2])<<8)|_data_buf[3];//获取寄存器起始地址
	if(startRegAddr < MODBUS_OUTPUT_NUM)//寄存器地址在范围内
	{
		if((_data_buf[4]==0xFF)||(_data_buf[5]==0xFF)) *Modbus_OutputIO[startRegAddr]=0x01;
		else *Modbus_OutputIO[startRegAddr]=0x00;

		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1];
		RS485_tx_buf[2]=_data_buf[2];
		RS485_tx_buf[3]=_data_buf[3];
		RS485_tx_buf[4]=_data_buf[4];
		RS485_tx_buf[5]=_data_buf[5];

		calCRC=CRC16(RS485_tx_buf,6);
		RS485_tx_buf[6]=(calCRC>>8)&0xFF;
		RS485_tx_buf[7]=calCRC&0xFF;
		RS485_SendData(RS485_tx_buf,8);
	}
	else//寄存器地址超出范围
	{
		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1]|0x80;
		RS485_tx_buf[2]=0x02; //异常码
		RS485_SendData(RS485_tx_buf,3);
	}
}


//Modbus功能码06处理程序
//写单个保持寄存器
void Modbus_06_Handle(u8 * _data_buf)
{
	u16 calCRC;
	u16 startRegAddr;

	startRegAddr=(((u16)_data_buf[2])<<8)|_data_buf[3];//获取寄存器起始地址
	if(startRegAddr < MODBUS_REG_NUM){//寄存器地址在范围内
	
		*Modbus_HoldReg[startRegAddr] = ((u16)_data_buf[4]) << 8;//高字节在前
		*Modbus_HoldReg[startRegAddr] |= _data_buf[5];//低字节在后

		check_data ();
		if (((startRegAddr < MODBUS_SAVE_DATA_START + MODBUS_SAVE_DATA_NUM + 1) && (startRegAddr >= MODBUS_SAVE_DATA_START)) ||
			  ((startRegAddr < MODBUS_SAVE_DATA_START_EX + MODBUS_SAVE_DATA_NUM_EX + 1) && (startRegAddr >= MODBUS_SAVE_DATA_START_EX))
			 ){
			data_change_flag = 1;
			save_para (1); //save_para(1) 保存参数save_para(0) 读取参数
//			inc_Modbus_test_value ();
		}

		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1];
		RS485_tx_buf[2]=_data_buf[2];
		RS485_tx_buf[3]=_data_buf[3];
		RS485_tx_buf[4]=_data_buf[4];
		RS485_tx_buf[5]=_data_buf[5];

		calCRC=CRC16(RS485_tx_buf,6);
		RS485_tx_buf[6]=(calCRC>>8)&0xFF;
		RS485_tx_buf[7]=calCRC&0xFF;
		RS485_SendData(RS485_tx_buf,8);
	}
	else//寄存器地址超出范围
	{
		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1]|0x80;
		RS485_tx_buf[2]=0x02; //异常码
		RS485_SendData(RS485_tx_buf,3);
	}
}
//Modbus功能码15处理程序
//写多个输出开关量
void Modbus_15_Handle(u8 * _data_buf)
{
	u16 i;
	u16 calCRC;
	u16 RegNum;
	u16 startRegAddr;

	startRegAddr=(((u16)_data_buf[2])<<8)|_data_buf[3];//获取寄存器起始地址
	RegNum=(((u16)_data_buf[4])<<8)|_data_buf[5];//获取寄存器数量
	if((startRegAddr+RegNum - 1)<MODBUS_OUTPUT_NUM)//寄存器地址+数量在范围内
	{
		for(i=0;i<RegNum;i++)
		{
		        if(_data_buf[7+i/8]&0x01) *Modbus_OutputIO[startRegAddr+i]=0x01;
		        else *Modbus_OutputIO[startRegAddr+i]=0x00;
		        _data_buf[7+i/8]>>=1;//从低位开始
		}

		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1];
		RS485_tx_buf[2]=_data_buf[2];
		RS485_tx_buf[3]=_data_buf[3];
		RS485_tx_buf[4]=_data_buf[4];
		RS485_tx_buf[5]=_data_buf[5];
		calCRC=CRC16(RS485_tx_buf,6);
		RS485_tx_buf[6]=(calCRC>>8)&0xFF;
		RS485_tx_buf[7]=calCRC&0xFF;
		RS485_SendData(RS485_tx_buf,8);
	}
	else//寄存器地址+数量超出范围
	{
		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1]|0x80;
		RS485_tx_buf[2]=0x02; //异常码
		RS485_SendData(RS485_tx_buf,3);
	}
}
//Modbus功能码16处理程序
//写多个保持寄存器
void Modbus_16_Handle(u8 * _data_buf)
{
	u8 i;
	u16 calCRC;
	u16 RegNum;
	u16 startRegAddr;

	startRegAddr=(((u16)_data_buf[2])<<8)|_data_buf[3];//获取寄存器起始地址
	RegNum= (((u16)_data_buf[4])<<8)|_data_buf[5];//获取寄存器数量
	if((startRegAddr+RegNum - 1)<MODBUS_REG_NUM)//寄存器地址+数量在范围内
	{
		for(i=0;i<RegNum;i++){
			*Modbus_HoldReg[startRegAddr+i]=((u16)_data_buf[7+i*2])<<8; //高字节在前
			*Modbus_HoldReg[startRegAddr+i]|=_data_buf[8+i*2]; //低字节在后
			if ((startRegAddr+i < MODBUS_SAVE_DATA_START + MODBUS_SAVE_DATA_NUM + 1) && (startRegAddr+i >= MODBUS_SAVE_DATA_START)){
				data_change_flag = 1;
			}
		}
		check_data ();
		if (data_change_flag == 1){
			save_para (1); //save_para(1) 保存参数save_para(0) 读取参数
		}

		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1];
		RS485_tx_buf[2]=_data_buf[2];
		RS485_tx_buf[3]=_data_buf[3];
		RS485_tx_buf[4]=_data_buf[4];
		RS485_tx_buf[5]=_data_buf[5];

		calCRC=CRC16(RS485_tx_buf,6);
		RS485_tx_buf[6]=(calCRC>>8)&0xFF;
		RS485_tx_buf[7]=calCRC&0xFF;
		RS485_SendData(RS485_tx_buf,8);

		data_change_flag = 1;
	}
	else//寄存器地址+数量超出范围
	{
		RS485_tx_buf[0]=_data_buf[0];
		RS485_tx_buf[1]=_data_buf[1]|0x80;
		RS485_tx_buf[2]=0x02; //异常码
		RS485_SendData(RS485_tx_buf,3);
	}
}

void HardFault_Handler (void)
{
	while (1);
}










