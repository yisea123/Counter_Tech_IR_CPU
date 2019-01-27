#ifndef AD8804_H
#define AD8804_H



//#define DA1 5
//#define DA2 4
//#define DA3 3
//#define DA4 2
//#define DA5 1
//#define DA6 0
//#define DA7 11
//#define DA8 10
//#define DA9 9
//#define DA10 8
//#define DA11 7
//#define DA12 6


#define DA1 6
#define DA2 7
#define DA3 8
#define DA4 9
#define DA5 10
#define DA6 11
#define DA7 0
#define DA8 1
#define DA9 2
#define DA10 3
#define DA11 4
#define DA12 5


#define SHDL PBout(2)
#define SPI_CS PDout(7)
#define SPI_SCK PBout(3)
#define SPI_MOSI PBout(5)

#define SPI_DELAY 2


typedef struct {
	uint8_t da_addr[CHANEL_NUM];
	uint8_t da_value[CHANEL_NUM];
}s_ad8804_env;


extern s_ad8804_env ad8804_env;

extern uint8_t  DA_V;

int ad8804_write_ch (uint8_t ch, uint8_t value);
int ad8804_init (void);


#endif







