#ifndef __MODBUSRTU_H
#define __MODBUSRTU_H

//#define MODBUS_RTU_USE_UART 3 //1
#define MODBUS_RTU_ADDR 0x02
#define MODBUS_RTU_MIN_LEN 8

#define MODBUS_VIRTUAL_INPUT_NUM 80
#define MODBUS_INPUT_NUM 80
#define MODBUS_OUTPUT_NUM 20 + MODBUS_VIRTUAL_INPUT_NUM
#define MODBUS_REG_NUM (512+512+512)


#define MODBUS_SAVE_DATA_NUM 70
#define MODBUS_SAVE_DATA_SIZE (MODBUS_SAVE_DATA_NUM)
#define MODBUS_SAVE_DATA_START 30

#define MODBUS_SAVE_DATA_NUM_EX 32
#define MODBUS_SAVE_DATA_SIZE_EX (MODBUS_SAVE_DATA_NUM_EX * 2)
#define MODBUS_SAVE_DATA_START_EX 512

#define HMI_RS485_TX_EN_RCC_PORT RCC_APB2Periph_GPIOA
#define HMI_RS485_TX_EN_PIN 0
#define HMI_RS485_TX_EN_PORT GPIOA
#define HMI_RS485_TX_EN_PIN_MAP GPIO_Pin_0
#define HMI_RS485_TX_EN PAout(HMI_RS485_TX_EN_PIN)


extern uint16_t Modbus_HoldReg_CPU_Usage;



u16 CRC16(u8 *data_buf, u16 len);
void modbus_analyze(u8 * _data_buf, uint16_t chanel, uint16_t len);
void inc_Modbus_test_value (uint16_t value);
void set_Modbus_test_value (uint16_t value);
void modbus_init(void);
int16_t save_para (int16_t flag);
int16_t load_para (void);
void check_data (void);
extern vu32 virtual_input[MODBUS_VIRTUAL_INPUT_NUM];

extern u8 RS485_tx_buf[256];//·¢ËÍ»º³åÇø


#endif

 
