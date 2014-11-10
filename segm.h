#ifndef SEGM_H
#define SEGM_H

#include "pins.h"

#define BR_MIN		0
#define BR_MAX		8
#define BR_STBY		1
#define BR_WORK		8

#define DIGITS		4

#define BIT_A		(1<<0)
#define BIT_B		(1<<1)
#define BIT_C		(1<<2)
#define BIT_D		(1<<3)
#define BIT_E		(1<<4)
#define BIT_F		(1<<5)
#define BIT_G		(1<<6)
#define BIT_P		(1<<7)

#define CH_EMPTY	0
#define CH_0		(BIT_A | BIT_B | BIT_C | BIT_D | BIT_E | BIT_F)
#define CH_1		(BIT_B | BIT_C)
#define CH_2		(BIT_A | BIT_B | BIT_D | BIT_E | BIT_G)
#define CH_3		(BIT_A | BIT_B | BIT_C | BIT_D | BIT_G)
#define CH_4		(BIT_B | BIT_C | BIT_F | BIT_G)
#define CH_5		(BIT_A | BIT_C | BIT_D | BIT_F | BIT_G)
#define CH_6		(BIT_A | BIT_C | BIT_D | BIT_E | BIT_F | BIT_G)
#define CH_7		(BIT_A | BIT_B | BIT_C)
#define CH_8		(BIT_A | BIT_B | BIT_C | BIT_D | BIT_E | BIT_F | BIT_G)
#define CH_9		(BIT_A | BIT_B | BIT_C | BIT_D | BIT_F | BIT_G)

#define CH_G		(BIT_A | BIT_E | BIT_F)
#define CH_C		(BIT_A | BIT_D | BIT_E | BIT_F)

/* Display modes */
enum {
	MODE_FM_FREQ,
	MODE_TIME,
	MODE_FM_CHAN,
	MODE_FMTUNE_FREQ,
	MODE_FMTUNE_CHAN,
	MODE_VOLUME,
	MODE_TIME_EDIT_H,
	MODE_TIME_EDIT_M,
	MODE_STANDBY
};

#define DISPLAY_TIME_VOLUME			2000
#define DISPLAY_TIME_FM_FREQ		3000
#define DISPLAY_TIME_FM_CHAN		1000
#define DISPLAY_TIME_FMTUNE_FREQ	10000
#define DISPLAY_TIME_FMTUNE_CHAN	1000
#define DISPLAY_TIME_TIME			2000

#define eepromDispMode		((uint16_t*)0x02)

#define ENC_0			0b00
#define ENC_A			0b01
#define ENC_B			0b10
#define ENC_AB			0b11

#define BTN_STATE_0		0b0000
#define BTN_1			0b0001
#define BTN_2			0b0010
#define BTN_3			0b0100
#define BTN_4			0b1000

enum {
	CMD_BTN_1,
	CMD_BTN_2,
	CMD_BTN_3,
	CMD_BTN_4,
	CMD_BTN_1_LONG,
	CMD_BTN_2_LONG,
	CMD_BTN_3_LONG,
	CMD_BTN_4_LONG,

	CMD_EMPTY = 0xEF
};

// Handling long press actions */
#define SHORT_PRESS		400
#define LONG_PRESS		2400

void segmInit(void);
void segmBrightness(uint8_t value);
void segmNum(int16_t number, uint8_t dot, uint8_t label);

void segmTimeHM(void);

void segmFmFreq(void);
void segmFmEditFreq(void);
void segmFmNum(void);

void segmVol(void);

int8_t getEncoder(void);
uint8_t getBtnCmd(void);

void setDisplayTime(uint16_t value);
uint16_t getDisplayTime(void);

void setBrightness(uint8_t value);

#endif // SEGM_H
