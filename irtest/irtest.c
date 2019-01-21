#include "irtest.h"
__idata uint8_t buffer[200]; // 红外电平宽度测量，一个宽度占用两字节
					// 低电平开始，长高电平结束
	
void main(void){
	uint8_t buffer_head = 0; // the next buffer index to be filled
	TMOD = 21; // 16bit reload mode timer0; 8bit autoreload timer1
	TH0 = 0; TL0 = 0; // init t0

	// INit UART Serial Port
	
	SCON = 0x40;
	//TH1 = 0xFF; //28800
	TH1 = 243; //2400
	TR1 = 1;
	
	while(1){
	SBUF=0x56;
	while(!TI);
	TI=0;
	}
	

	
	while(IRPIN);	// a long time high,waiting signal
	TR0=1;	// counting low level time
	while(1){
		while((!IRPIN) && TH0 < 0xFF); // 0xFEFF = 65279 means 65ms under 12MHz
		//buffer_append(); // signal change to high level
	buffer[buffer_head] = TH0;
	buffer_head++;
	buffer[buffer_head] = TL0;
	buffer_head++;
		TH0 = 0; TL0 = 0; //reset timer0
		while(IRPIN && TH0 < 0xFF); // counting high level timer
		//buffer_append(); // signal change to low level
	buffer[buffer_head] = TH0;
	buffer_head++;
	buffer[buffer_head] = TL0;
	buffer_head++;
		// fill 4 bytes in buffer per routin
		if(buffer_head == 200 || TH0==0xFF) { // check if buffer full, or button release event 
			// send to uart and reset buffer
			buffer_head = 0;
			for(;buffer_head < 200; buffer_head++){
				SBUF = buffer[buffer_head];
				while(!TI);
				TI=0;
			}
			buffer_head = 0;
		}
		TH0 = 0; TL0 = 0;
	}





}
