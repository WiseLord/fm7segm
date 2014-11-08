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

void segmInit(void);
void segmBrightness(uint8_t value);
void segmNum(int16_t number, uint8_t dot);
void segmTimeHM(void);
void segmFreq(uint16_t freq);

#endif // SEGM_H
