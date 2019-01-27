#ifndef __DRIVE_OUTPUT_H
#define __DRIVE_OUTPUT_H




//#define EXT_IN0  PDin(10)
//#define EXT_IN1  PDin(11)
//#define EXT_IN2  PDin(12)
//#define EXT_IN3  PDin(13)
//#define EXT_IN4  PDin(14)
//#define EXT_IN5  PDin(15)
//#define EXT_IN6  PDin(0)
//#define EXT_IN7  PDin(1)
//#define EXT_IN8  PDin(2)
//#define EXT_IN9  PDin(3)
//#define EXT_IN10 PDin(4)
//#define EXT_IN11 PDin(5)
//#define EXT_IN12 PDin(6)
//#define EXT_IN13 PDin(7)
//#define EXT_IN14 PEin(0)
//#define EXT_IN15 PEin(1)

//#define EXT_IN16 PEin(2)
//#define EXT_IN17 PEin(3)
//#define EXT_IN18 PEin(4)
//#define EXT_IN19 PEin(5)
//#define EXT_IN20 PEin(6)
//#define EXT_IN21 PEin(7)
//#define EXT_IN22 PEin(8)
//#define EXT_IN23 PEin(9)
//#define EXT_IN24 PEin(10)
//#define EXT_IN25 PEin(11)
//#define EXT_IN26 PEin(12)
//#define EXT_IN27 PEin(13)
//#define EXT_IN28 PEin(14)
//#define EXT_IN29 PEin(15)
//#define EXT_IN30 PDin(8)
//#define EXT_IN31 PDin(9)

#define EXT_IN31 PDin(10)
#define EXT_IN30 PDin(11)
#define EXT_IN29 PDin(12)
#define EXT_IN28 PDin(13)
#define EXT_IN27 PDin(14)
#define EXT_IN26 PDin(15)
#define EXT_IN25 PDin(0)
#define EXT_IN24 PDin(1)
#define EXT_IN23 PDin(2)
#define EXT_IN22 PDin(3)
#define EXT_IN21 PDin(4)
#define EXT_IN20 PDin(5)
#define EXT_IN19 PDin(6)
#define EXT_IN18 PDin(7)
#define EXT_IN17 PEin(0)
#define EXT_IN16 PEin(1)

#define EXT_IN15 PEin(2)
#define EXT_IN14 PEin(3)
#define EXT_IN13 PEin(4)
#define EXT_IN12 PEin(5)
#define EXT_IN11 PEin(6)
#define EXT_IN10 PEin(7)
#define EXT_IN9  PEin(8)
#define EXT_IN8  PEin(9)
#define EXT_IN7  PEin(10)
#define EXT_IN6  PEin(11)
#define EXT_IN5  PEin(12)
#define EXT_IN4  PEin(13)
#define EXT_IN3  PEin(14)
#define EXT_IN2  PEin(15)
#define EXT_IN1  PDin(8)
#define EXT_IN0  PDin(9)


#define OUTPUT0 		GPIO_Pin_0
#define OUTPUT1 		GPIO_Pin_1
#define OUTPUT2 		GPIO_Pin_2
#define OUTPUT3 		GPIO_Pin_3
#define OUTPUT4 		GPIO_Pin_4
#define OUTPUT5 		GPIO_Pin_5
#define OUTPUT6 		GPIO_Pin_6
#define OUTPUT7 		GPIO_Pin_7
#define OUTPUT8 		GPIO_Pin_8
#define OUTPUT9 		GPIO_Pin_9
#define OUTPUT10 		GPIO_Pin_10
#define OUTPUT11 		GPIO_Pin_11
#define OUTPUT12		GPIO_Pin_12
#define OUTPUT13		GPIO_Pin_13
#define OUTPUT14		GPIO_Pin_14
#define OUTPUT15		GPIO_Pin_15

#define OUTPUT_PIN_MAP 				OUTPUT0 | OUTPUT1 | OUTPUT2 | OUTPUT3 | \
									OUTPUT4 | OUTPUT5 | OUTPUT6 | OUTPUT7 | \
									OUTPUT8 | OUTPUT9 | OUTPUT10 | OUTPUT11 | \
									OUTPUT12 | OUTPUT13 | OUTPUT14 | OUTPUT15

#define EXT_OUT0  PEout(0)
#define EXT_OUT1  PEout(1)
#define EXT_OUT2  PEout(2)
#define EXT_OUT3  PEout(3)
#define EXT_OUT4  PEout(4)
#define EXT_OUT5  PEout(5)
#define EXT_OUT6  PEout(6)
#define EXT_OUT7  PEout(7)
#define EXT_OUT8  PEout(8)
#define EXT_OUT9  PEout(9)
#define EXT_OUT10 PEout(10)
#define EXT_OUT11 PEout(11)




#define VIB_START (0)
#define VIB_STOP (1)

#define IS_SYS_RUNNING 0


void drive_io_init (void);


extern int null_temp_value;

#endif

 
