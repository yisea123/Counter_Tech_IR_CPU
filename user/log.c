#include "main.h"



S16 coin_log (char *fmt, ...)
{
//	FATFS fs;
//	FIL file;
//	FRESULT Res;	
	va_list ap;
	char string[64];
	char log_str[64];

	va_start(ap, fmt);
	vsprintf(string, fmt, ap);
		
	
	sprintf((char *)log_str, "LOG: %4d-%02d-%02d %02d:%02d:%02d %s\n", 
						calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec,
						string);
	
	my_print ("%s", log_str);	
	
	va_end(ap);
	return 0;
}

