#ifndef __USART_H__
#define __USART_H__

#define UART1_BOUND_RATE 115200
#define EN_USART1_RX 1


#define UART2_BOUND_RATE 9600
#define EN_USART2_RX 1

#define UART3_BOUND_RATE 115200 //19200
#define UART3_BUF_LEN 64
#define EN_USART3_RX 1

void uart1_send_data (int ch);
void uart1_init (void);
void uart1_puts(const char *s);
void stop_uart1_receive (void);
void start_uart1_receive (void);
void start_uart1_dma (u32 _memBaseAddr, uint16_t _size);

void uart2_send_data (int ch);
void uart2_init (void);
void uart2_puts(const char *s);
void stop_uart2_receive (void);
void start_uart2_receive (void);


void uart3_init (void);
void stop_uart3_receive (void);
void start_uart3_receive (void);
void start_uart3_dma (u32 _memBaseAddr, uint16_t _size);


#define my_putc uart1_send_data
#define my_puts uart1_puts

extern unsigned char uart3_rec_buf[UART3_BUF_LEN];
extern uint16_t uart3_rec_count;

//extern uint16_t RS485_FrameEndFlag;


#endif
