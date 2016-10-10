#ifndef __RS4_H_
#define __RS4_H_

#include <inttypes.h>
#include <avr/io.h>

#define	MASTER1_ID	(0x10)
#define MASTER2_ID	(0x11)

#define SLAVE1_ID 	(0x20)
#define SLAVE2_ID 	(0x21)
#define SLAVE3_ID 	(0x22)
#define EVERYBODY 	(0x40)

#define UART1_DDR   DDRD
#define UART1_PORT  PORTD
#define UART1_TX    PD3
#define UART1_RX    PD2

#define RS_RE_DDR   DDRC
#define RS_RE_PORT  PORTC
#define RS_RE_PIN   PC7

#define RS_DE_DDR   DDRE
#define RS_DE_PORT  PORTE
#define RS_DE_PIN   PE2

#define RS_START    (0x55)
#define RS_STOP     (0xDD)
#define RS_TOKEN   (0x77)





struct RSframe  {

        uint8_t start;
        uint8_t id;
        uint8_t cmd;
        uint8_t data0;
        uint8_t data1;
        uint8_t stop;
};

#define RS_FRAMESIZE sizeof(struct RSframe)

extern void RS_init(uint8_t ID);
extern void RS_sendframe(struct RSframe* frame);
extern void RS_sendframedirect(uint8_t ID, uint8_t CMD,uint8_t DATA0, uint8_t DATA1);
extern uint8_t RS_readframe(struct RSframe* frame);

extern void RS_sendtoken(uint8_t ID);
extern void RS_chektoken(struct RSframe* frame);

extern void RS_drivebus();
extern void RS_releasebus();

#endif
