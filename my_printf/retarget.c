#include "main.h"




//#pragma import(__use_no_semihosting_swi)

#define ITM_PRINTF 0

#define ITM_Port8(n) (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n) (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n) (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA 0x01000000

struct __FILE { int handle; /* Add whatever you need here */ };
    FILE __stdout;
    FILE __stdin;
    
   
int fputc(int ch, FILE *f)
{ 
#if ITM_PRINTF
	ITM_SendChar(ch);
#endif
	uart1_send_data (ch);
	return (ch);
}

volatile int32_t ITM_RxBuffer = ITM_RXBUFFER_EMPTY;
int fgetc(FILE *f)
{
	char ch = '\0';
#ifdef ITM_PRINTF
	while (ITM_CheckChar() != 1) __NOP();
	ch = ITM_ReceiveChar();
	ITM_SendChar(ch);
	if (ch == '\r')
		ITM_SendChar ('\n');
#endif
	return (ch);
}

int ferror(FILE *f)
{
    /* Your implementation of ferror */
    return EOF;
}

void _ttywrch(int c)
{
    fputc(c, &__stdout);
}

int __backspace()
{
    return 0;
}
void _sys_exit(int return_code)
{
label:
    goto label; /* endless loop */
}

