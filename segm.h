#ifndef SEGM_H
#define SEGM_H

#include "pins.h"

#define DIGITS		4

#define BIT_A		(1<<0)
#define BIT_B		(1<<1)
#define BIT_C		(1<<2)
#define BIT_D		(1<<3)
#define BIT_E		(1<<4)
#define BIT_F		(1<<5)
#define BIT_G		(1<<6)
#define BIT_P		(1<<7)

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

/* Display modes */
enum {
	MODE_STANDBY,
	MODE_FM_RADIO,
	MODE_VOLUME,
	MODE_TIME,
	MODE_TIME_EDIT
};

#define DISPLAY_TIME_VOLUME		2000

#define ENC_0			0b00
#define ENC_A			0b01
#define ENC_B			0b10
#define ENC_AB			0b11

void segmInit(void);
void segmBrightness(uint8_t value);
void segmNum(int16_t number, uint8_t dot);
void segmTimeHM(void);
void segmFmFreq(uint16_t freq);

int8_t getEncoder(void);

void setDisplayTime(uint16_t value);
uint16_t getDisplayTime(void);

#endif // SEGM_H
