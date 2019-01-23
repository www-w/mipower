#include "config.h"
#include <stdint.h>

// #define DEBUG // 串口输出红外编码数据

#define LED P1_2

//红外编码数据
//引导：9ms_04.7ms_1
//逻辑0: 0.5ms_0.5ms- 逻辑1：0.5ms_1.7ms-
//8位厂商码+8位反码
//8位数据+8位反码
typedef union{
	uint32_t int32;
	uint8_t int8[4];
} unionulong;

#define BTNRED 0x1CE3926D
#define BTNGRE 0x1CE352AD
#define BTNYEL 0x1CE3D22D
#define BTNBLU 0x1CE332CD

unionulong irdata;



void tryGetIr(){
	uint8_t i;
	if(IRPIN){return;}; // no signal
	TR0=1;	// enter counting low level time

	while((!IRPIN) && TH0 < 0xFF); // 0xFEFF = 65279 means 65ms under 12MHz
	if(TH0 < 0x1E){TR0=0;return;}; // expect 9ms >=7.68ms
	TH0 = 0; TL0 = 0; //reset timer0
	while(IRPIN && TH0 < 0xFF); // counting high level timer
	if(TH0 < 0x0B){TR0=0;return;}; // expect 4.5ms >=2.8ms
	for(i=0;i<32;i++){ // receive 32bit data
		irdata.int32 <<= 1;
		TH0 = 0; TL0 = 0; // reset timer0
		while(!IRPIN && TH0 < 0xFF); // skip _
		TH0 = 0; TL0 = 0; // reset timer0
		while(IRPIN && TH0 < 0xFF); // counting -
		if(TH0>4){ // >1.024ms high
			irdata.int32 |= 1;
		}else{ // <=1.024ms low
			// 0
		}
	}
#ifdef DEBUG
	SBUF = irdata.int8[3];
	while(!TI);TI=0;
	SBUF = irdata.int8[2];
	while(!TI);TI=0;
	SBUF = irdata.int8[1];
	while(!TI);TI=0;
	SBUF = irdata.int8[0];
	while(!TI);TI=0;
#endif
	switch(irdata.int32){
		case BTNRED:
			LED = 0;
			break;
		case BTNGRE:
			LED = 1;
			break;
	}
	TR0 = 0;TH0 = TL0 = 0;
	return;

}

uint8_t ms50 = 0;

void isrPCA(void) __interrupt 6
{
	CH=CL=0;
	ms50++;
	if(ms50 == 20){

	}
}

void main(void){
	TMOD = 0x21; // 16bit reload mode timer0; 8bit autoreload timer1
	TH0 = 0; TL0 = 0; // init t0

	// INit UART Serial Port
	
	SCON = 0x40;
	TH1 = 243; //2400
	TL1 = TH1;
	TR1 = 1;
#ifdef DEBUG
	SBUF=0x56;
	while(!TI);
	TI=0;
#endif

	// Init PCA timer
	CCAPM0 = 0x49; // 01001001 soft timer;
	CCAP0H = 0xC3; CCAP0L = 0x50; // 50ms
	EA = 1;
	CR = 1; // start
	
	while(1){
		tryGetIr();
	}
}
