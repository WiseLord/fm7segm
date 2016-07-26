#ifndef SEGM_H
#define SEGM_H

#include <inttypes.h>

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

#define NIX_EMPTY	0xF

#define NIX_G		9
#define NIX_C		5
#define NIX_E		3

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

// Indicator types
enum {
	IND_CC_TR = 0b000,
	IND_CC_NO = 0b001,
	IND_CA_TR = 0b010,
	IND_CA_NO = 0b011,
	IND_NIXIE = 0b100,

	IND_END,
};
#define IND_BIT_TRANS				0b001
#define IND_BIT_TYPE				0b010
#define IND_BIT_NIXIE				0b100

/* Display modes */
enum {
	MODE_FM_FREQ,
	MODE_TIME,
	MODE_FM_CHAN,
	MODE_FMTUNE_FREQ,
	MODE_FMTUNE_CHAN,
	MODE_VOLUME,
	MODE_TIME_EDIT,
	MODE_TEMP,
	MODE_BRIGHTNESS,
	MODE_STANDBY
};

/* Display timings in msec */
#define DISPLAY_TIME_VOLUME			2000
#define DISPLAY_TIME_FM_FREQ		3000
#define DISPLAY_TIME_FM_CHAN		1000
#define DISPLAY_TIME_FMTUNE_FREQ	10000
#define DISPLAY_TIME_FMTUNE_CHAN	1000
#define DISPLAY_TIME_TIME			2000
#define DISPLAY_TIME_TEMP			2000
#define DISPLAY_TIME_EDITTIME		15000
#define DISPLAY_TIME_BRIGHTNESS		2000

/* Other timers */
#define TEMP_MEASURE_TIME			750
#define RTC_POLL_TIME				1000
#define FM_STATUS_TIME				200

/* Encoder and buttons status bits */
#define ENC_0						0b000000
#define ENC_A						0b000001
#define ENC_B						0b000010
#define ENC_AB						0b000011

#define BTN_STATE_0					0b000000
#define BTN_1						0b000100
#define BTN_2						0b001000
#define BTN_3						0b010000
#define BTN_4						0b100000

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

#define SCAN_FACTOR_NIXIE			1
#define SCAN_FACTOR_7SEGM			8

#define SHORT_PRESS					(scan * 50)
#define LONG_PRESS					(scan * 200)
#define AUTOREPEAT					(scan * 150)
#define BLINK_PERIOD				(scan * 250)
#define BLINK_TIME					(scan * 100)

void segmInit(void);
void segmBrightness(uint8_t value);
void segmNum(int16_t number, uint8_t dot, uint8_t label, uint8_t stInd);

void segmTime(void);

void segmFmFreq(void);
void segmFmEditFreq(void);
void segmFmNum(void);

void segmVol(void);
void segmTemp(void);

void segmBr(uint8_t value);

void segmTimeOrTemp(void);

int8_t getEncoder(void);
uint8_t getBtnCmd(void);

uint16_t getDisplayTime(void);
void setDisplayTime(uint16_t value);

void setBrightness(uint8_t value);

uint16_t getTempTimer(void);
void setTempTimer(uint16_t val);

uint16_t getRtcTimer(void);
void setRtcTimer(uint16_t val);

uint16_t getFmStatusTimer(void);
void setFmStatusTimer(uint16_t val);

#endif // SEGM_H
