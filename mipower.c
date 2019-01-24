#include "mipower.h"

unionulong irdata; // Store received irdata
uint32_t ms50 = 0; // Timer 50ms countdown

uint8_t ledmode = 0;
#define LED1 1
#define LED2 2
#define LED3 3
#define LED4 4
void tryGetIr(){
	uint8_t i;
	if(IRPIN){return;}; // no signal
	
	EA = 0; // disable all interrupt 

	TR0=1;	// enter counting low level time

	while((!IRPIN) && TH0 < 0xFF); // 0xFEFF = 65279 means 65ms under 12MHz
	if(TH0 < 0x1E){TR0=0;EA = 1;return;}; // expect 9ms >=7.68ms
	TH0 = 0; TL0 = 0; //reset timer0
	while(IRPIN && TH0 < 0xFF); // counting high level timer
	if(TH0 < 0x0B){TR0=0;EA = 1;return;}; // expect 4.5ms >=2.8ms
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
	EA = 1;	// data received, enable any interrupt
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
	// code recieved do someting here.
	switch(irdata.int32){
		case BTNRED:
			ms50 = MINUTES(30L);
			POWER = 1;
			ledmode = LED1;
			break;
		case BTNGRE:
			ms50 = MINUTES(60L);
			POWER = 1;
			ledmode = LED2;
			break;
		case BTNYEL:
			ms50 = MINUTES(90L);
			POWER = 1;
			ledmode = LED3;
			break;
		case BTNBLU:
			ms50 = MINUTES(120L);
			POWER = 1;
			ledmode = LED4;
			break;
	}
	TR0 = 0;TH0 = TL0 = 0;
	return;

}


void isrPCA(void) __interrupt 6
{
	CH = CL = 0;	// restart 50ms timer
	CCF0 = 0;	// clear interrupt flag
	if(ms50 == 0){
		// didn't start 
		return;
	}
	ms50--;
	if(ms50 == 0){
		// time over 
		// do release relay power
		POWER = 0;
		LED = 1;
	}
}


void main(void){
	uint8_t softpwm = 0;
 	volatile uint8_t ledlight= 128;
	uint16_t softtimer=0;
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
	CCON = 0;
	CL = 0; CH = 0;
	CMOD = 0;
	CCAP0H = 0xC3; CCAP0L = 0x50; // 50ms
	CCAPM0 = 0x49; // 01001001 soft timer;
	CR = 1; // start
	IE |= 0x40;	// EPCA_LVD = 1;
	EA = 1;

	// Init Control IO
	POWER = 0; // default disconnect
	// P1.7 220V P1.6 USB5V
	P1M0 = 0xC0;	//推挽输出 P1.7 P1.6
	int8_t a=1;
	
	while(1){
		tryGetIr();
		if(0 == ledmode && (ms50>MINUTES(5L) || ms50 == 0))continue;
		softtimer++;
		if(softtimer == 0x1FF){
			ledlight += a;
			if(ledlight == 255) a=-1;
			if(ledlight == 0) {
				if(ledmode)ledmode--;
				a=1;
			}
			softtimer = 0;
		}

		softpwm++;	//0~255
		if(ledlight == 0){
			LED = 1;
			continue;	// dark led
		}
		if(ledlight == 255){
			LED = 0;
			continue;	// full pwm
		}
		// softpwm=[0-255] ledlight=[1-254]
		if(softpwm < ledlight){
			LED = 0;	// turn on
		}else{
			LED = 1;	// turn off
		}

	}
}
