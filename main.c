//RS485 MAIN FÁJL
#define F_CPU (8000000UL)
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "fifo.h"
#include "rs4.h"

//GLOBAL
uint8_t data;
extern uint8_t RS_ID;
extern uint8_t RS_token;
extern _FIFOu8 RS_RXfifo;

//FÜGGVÉNYEK

void RS_cmdprocess(struct RSframe* frame);
void Init()
{
			RS_init(SLAVE1_ID);
			RS_token = 0;
			DDRB=0xF0;
			DDRD=0xF0;
			sei();
}

void LED_out(uint8_t s){

			PORTD = s & (0xF0);
			PORTB = (s<<4)&0xF0;
}
//MAIN
int main (void){

	struct RSframe frame;
	Init();

/*
*/

	while (1) {

		_delay_ms(5);



		if(RS_RXfifo.Number>= RS_FRAMESIZE) {
			//LED_out(0xFF);
				while (RS_readframe(&frame)){
					RS_cmdprocess(&frame);
				}
		}

	}

	return 0;
}
/*
commands:
	0x10	LED_out(0-255);
	0x11	LED_off(összeset);
	0x12 	LED_on(0-7);
	0x13	LED_off1(0-7);
	*/
void RS_cmdprocess(struct RSframe* frame){
	//LED_out(0xFF);
	if((frame->start != RS_START)||
		(frame->stop !=RS_STOP)||
		((frame->id != RS_ID)&&
		(frame->id != EVERYBODY)))
		{
		return;
	}
	switch (frame->cmd)
	{
		case 0x10: LED_out((frame->data0));break;
		case RS_TOKEN:
		{
		RS_token =1;
		RS_sendtoken(frame->data0);
		}

		default:break;
	}
	//LED_out(0x0);
}
