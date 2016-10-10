//RS_chektoken

#define F_CPU (80000000UL)
#include <avr/io.h>
#include <avr/interrupt.h>
#include "rs4.h"
#include "fifo.h"
#include <string.h>

_FIFOu8 RS_RXfifo;
uint8_t rs_rxfifo[FIFOSize];
uint8_t RS_ID = 0;
uint8_t RS_token = 0;
uint8_t data = 0;

void RS_init(uint8_t ID){

  FIFO_Init(&RS_RXfifo, &rs_rxfifo[0]);

  UART1_DDR |= (1<<UART1_TX);
  UART1_DDR &= (1<<UART1_RX);

  RS_RE_DDR |= (1<<RS_RE_PIN);
  RS_DE_DDR |= (1<<RS_DE_PIN);
  RS_releasebus();

  UBRR1H = 0;
  UBRR1L = 8;

  UCSR1A = 0;
  UCSR1B = ((1<<RXEN)|(1<<TXEN)|(1<<RXCIE1));
  UCSR1C = ((1<<USBS1)|(1<<UCSZ10)|(1<<UCSZ11));

  RS_ID = ID;
  RS_token = 0;
  sei();


}
ISR(USART1_RX_vect){

    data = UDR1;
	//PORTB = (data<<4);
    FIFO_Pushu8(&RS_RXfifo, data);


}
ISR(USART1_TX_vect){
	RS_releasebus();
	UCSR1B &= ~(1<<TXCIE1);
}
void RS_drivebus(/* arguments */) {
  RS_RE_PORT |= (1<<RS_RE_PIN);
  RS_DE_PORT |= (1<<RS_DE_PIN);

}

void RS_releasebus(/* arguments */) {
  RS_DE_PORT  &= ~(1<<RS_RE_PIN);
  RS_DE_PORT  &= ~(1<<RS_DE_PIN);

  /* code */
}
void RS_sendframedirect(uint8_t ID, uint8_t CMD, uint8_t DATA0, uint8_t DATA1){
	struct RSframe frame;
	frame.id = ID;
	frame.cmd = CMD;
	frame.data0 = DATA0;
	frame.data1 = DATA1;

	RS_sendframe(&frame);
}
void RS_sendframe(struct RSframe* frame){
	uint8_t* data = (uint8_t*) frame;
	uint8_t i;
	frame->start = RS_START;
	frame->stop = RS_STOP;
	while(!RS_token);
	RS_drivebus();
	for(i = 0; i< RS_FRAMESIZE; i++){
		UDR1 = data[i];
		while(!(UCSR1A & (1<<UDRE1)));

	}
	UCSR1B |= (1<<TXCIE1);
}

void RS_sendtoken(uint8_t ID){
	struct RSframe frame;
	frame.id = ID;
	frame.cmd = RS_TOKEN;
	frame.data0 = RS_ID;		//sajat id kuldese
	RS_sendframe(&frame);
	RS_token = 0;

}
//0. ha sikertelen
//1.ha skeeres
uint8_t RS_readframe(struct RSframe* frame){
	uint8_t data[RS_FRAMESIZE], i=0, ret=0;
	struct RSframe* tmpframe;

	if(RS_RXfifo.Number >= RS_FRAMESIZE){
		while(data[0] !=RS_START && (RS_RXfifo.Number >= (RS_FRAMESIZE-1))){
			FIFO_Popu8(&RS_RXfifo, &data[0]);
		}
		if (data[0] != RS_START)
		{return ret;}
		i=1;
		while(i <RS_FRAMESIZE) {
			FIFO_Popu8(&RS_RXfifo, &data[i]);
			i++;
		}
		if ( i== RS_FRAMESIZE ){
			tmpframe = (struct RSframe*) &data[0];
			memcpy(frame, tmpframe, RS_FRAMESIZE);
			ret = 1;
		}
	}
	return ret;
}
void RS_chektoken(struct RSframe* frame){
	if((frame->start == RS_START)&&(frame->stop ==RS_STOP)&&(frame->id == RS_ID)&&(frame->cmd == RS_TOKEN)) {
		RS_token=1;
	}

}
