#ifndef SEGM_H
#define SEGM_H

#include "pins.h"

/* _CC: Use indicators with common cathode by default */
#if !defined(_CC) && !defined(_CA)
#define _CC
#endif

#if (defined (_CC) && !defined(_TR)) || (!defined(_CC) && defined(_TR))
#define INV_DIG
#endif

#define BR_MIN		1
#define BR_MAX		12

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
#define CH_E		(BIT_A | BIT_B | BIT_C | BIT_D | BIT_G)

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
	MODE_TEMP,
	MODE_BRIGHTNESS,
	MODE_STANDBY
};

#define DISPLAY_TIME_VOLUME			2000
#define DISPLAY_TIME_FM_FREQ		3000
#define DISPLAY_TIME_FM_CHAN		1000
#define DISPLAY_TIME_FMTUNE_FREQ	10000
#define DISPLAY_TIME_FMTUNE_CHAN	1000
#define DISPLAY_TIME_TIME			2000
#define DISPLAY_TIME_TEMP			2000
#define DISPLAY_TIME_EDITTIME		10000
#define DISPLAY_TIME_BRIGHTNESS		2000

#define eepromDispMode		((uint8_t*)0x02)
#define eepromBrWork		((uint8_t*)0x04)
#define eepromBrStby		((uint8_t*)0x05)
#define eepromEncoder		((uint8_t*)0x06)

#define ENC_0			0b000000
#define ENC_A			0b000001
#define ENC_B			0b000010
#define ENC_AB			0b000011

#define BTN_STATE_0		0b000000
#define BTN_1			0b000100
#define BTN_2			0b001000
#define BTN_3			0b010000
#define BTN_4			0b100000

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
#define AUTOREPEAT		800

void segmInit(void);
void segmBrightness(uint8_t value);
void segmNum(int16_t number, uint8_t dot, uint8_t label);

void segmTimeHM(int8_t *time);
void segmTimeEditH(int8_t *time);
void segmTimeEditM(int8_t *time);

void segmFmFreq(void);
void segmFmEditFreq(void);
void segmFmNum(void);

void segmVol(void);
void segmTemp(void);

int8_t getEncoder(void);
uint8_t getBtnCmd(void);

void setDisplayTime(uint16_t value);
uint16_t getDisplayTime(void);

void setBrightness(uint8_t value);

uint16_t getTempTimer(void);
void setTempTimer(uint16_t val);

#endif // SEGM_H
