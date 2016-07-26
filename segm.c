#include "segm.h"

#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "rtc.h"
#include "tuner/tuner.h"
#include "volume.h"
#include "ds18x20.h"
#include "eeprom.h"

static volatile uint8_t ind[DIGITS];
static const uint8_t num[] = {CH_0, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9};

static uint8_t pos = 0;
static uint8_t zeroHour = 0;

static int8_t encRes = 0;

static volatile int8_t encCnt;
static volatile uint8_t cmdBuf;
static volatile uint8_t encPrev = ENC_0;
static volatile uint8_t btnPrev = BTN_STATE_0;

static volatile uint16_t displayTime = 0;
static volatile uint16_t blink = 0;

static volatile uint16_t tempTimer = 0;
static volatile uint16_t rtcTimer = 0;
static volatile uint16_t fmStatusTimer = 0;

void segmInit(void)
{
	DDR(SEG_A) |= SEG_A_LINE;
	DDR(SEG_B) |= SEG_B_LINE;
	DDR(SEG_C) |= SEG_C_LINE;
	DDR(SEG_D) |= SEG_D_LINE;
	DDR(SEG_E) |= SEG_E_LINE;
	DDR(SEG_F) |= SEG_F_LINE;
	DDR(SEG_G) |= SEG_G_LINE;
	DDR(SEG_P) |= SEG_P_LINE;

	DDR(DIG_0) |= DIG_0_LINE;
	DDR(DIG_1) |= DIG_1_LINE;
	DDR(DIG_2) |= DIG_2_LINE;
	DDR(DIG_3) |= DIG_3_LINE;

	/* Setup buttons and encoder as inputs with pull-up resistors */
	DDR(BUTTON_1) &= ~BUTTON_1_LINE;
	DDR(BUTTON_2) &= ~BUTTON_2_LINE;
	DDR(BUTTON_3) &= ~BUTTON_3_LINE;
	DDR(BUTTON_4) &= ~BUTTON_4_LINE;

	DDR(ENCODER_A) &= ~ENCODER_A_LINE;
	DDR(ENCODER_B) &= ~ENCODER_B_LINE;

	PORT(BUTTON_1) |= BUTTON_1_LINE;
	PORT(BUTTON_2) |= BUTTON_2_LINE;
	PORT(BUTTON_3) |= BUTTON_3_LINE;
	PORT(BUTTON_4) |= BUTTON_4_LINE;

	PORT(ENCODER_A) |= ENCODER_A_LINE;
	PORT(ENCODER_B) |= ENCODER_B_LINE;

	TIMSK |= (1<<TOIE2);							/* Enable timer overflow interrupt */
	TIMSK |= (1<<OCIE2);							/* Enable timer compare interrupt */
#ifdef _NIXIE
	TCCR2 |= (1<<CS22) | (0<<CS21) | (0<<CS20);		/* Set timer prescaller to 64 */
#else
	TCCR2 |= (0<<CS22) | (1<<CS21) | (0<<CS20);		/* Set timer prescaller to 8 */
#endif
	TCNT2 = 0;

	encCnt = 0;
	cmdBuf = CMD_EMPTY;

	encRes = eeprom_read_byte((uint8_t*)EEPROM_ENC_RES);
	zeroHour = eeprom_read_byte((uint8_t*)EEPROM_HOURZERO);

	return;
}

ISR (TIMER2_COMP_vect)
{
	/* Switch off digits */
#if defined(INV_DIG)
	PORT(DIG_0) |= DIG_0_LINE;
	PORT(DIG_1) |= DIG_1_LINE;
	PORT(DIG_2) |= DIG_2_LINE;
	PORT(DIG_3) |= DIG_3_LINE;
#else
	PORT(DIG_0) &= ~DIG_0_LINE;
	PORT(DIG_1) &= ~DIG_1_LINE;
	PORT(DIG_2) &= ~DIG_2_LINE;
	PORT(DIG_3) &= ~DIG_3_LINE;
#endif
	/* Switch off segments */
#if defined(_CC)
	PORT(SEG_A) &= ~SEG_A_LINE;
	PORT(SEG_B) &= ~SEG_B_LINE;
	PORT(SEG_C) &= ~SEG_C_LINE;
	PORT(SEG_D) &= ~SEG_D_LINE;
#ifndef _NIXIE
	PORT(SEG_E) &= ~SEG_E_LINE;
	PORT(SEG_F) &= ~SEG_F_LINE;
	PORT(SEG_G) &= ~SEG_G_LINE;
	PORT(SEG_P) &= ~SEG_P_LINE;
#endif
#else
	PORT(SEG_A) |= SEG_A_LINE;
	PORT(SEG_B) |= SEG_B_LINE;
	PORT(SEG_C) |= SEG_C_LINE;
	PORT(SEG_D) |= SEG_D_LINE;
	PORT(SEG_E) |= SEG_E_LINE;
	PORT(SEG_F) |= SEG_F_LINE;
	PORT(SEG_G) |= SEG_G_LINE;
	PORT(SEG_P) |= SEG_P_LINE;
#endif
}

ISR (TIMER2_OVF_vect)								/* 8000000 / 8 / 256 = 3906 polls/sec */
{
	uint8_t dig = ind[pos];

	/* Set data on segments */
#if defined(_CC)
	if (dig & BIT_A)
		PORT(SEG_A) |= SEG_A_LINE;
	if (dig & BIT_B)
		PORT(SEG_B) |= SEG_B_LINE;
	if (dig & BIT_C)
		PORT(SEG_C) |= SEG_C_LINE;
	if (dig & BIT_D)
		PORT(SEG_D) |= SEG_D_LINE;
#ifndef _NIXIE
	if (dig & BIT_E)
		PORT(SEG_E) |= SEG_E_LINE;
	if (dig & BIT_F)
		PORT(SEG_F) |= SEG_F_LINE;
	if (dig & BIT_G)
		PORT(SEG_G) |= SEG_G_LINE;
	if (dig & BIT_P)
		PORT(SEG_P) |= SEG_P_LINE;
#endif
#else
	if (dig & BIT_A)
		PORT(SEG_A) &= ~SEG_A_LINE;
	if (dig & BIT_B)
		PORT(SEG_B) &= ~SEG_B_LINE;
	if (dig & BIT_C)
		PORT(SEG_C) &= ~SEG_C_LINE;
	if (dig & BIT_D)
		PORT(SEG_D) &= ~SEG_D_LINE;
	if (dig & BIT_E)
		PORT(SEG_E) &= ~SEG_E_LINE;
	if (dig & BIT_F)
		PORT(SEG_F) &= ~SEG_F_LINE;
	if (dig & BIT_G)
		PORT(SEG_G) &= ~SEG_G_LINE;
	if (dig & BIT_P)
		PORT(SEG_P) &= ~SEG_P_LINE;
#endif

	/* Change current digit */
#if defined(INV_DIG)
	switch (pos) {
	case 3:
		PORT(DIG_3) &= ~DIG_3_LINE;
		break;
	case 2:
		PORT(DIG_2) &= ~DIG_2_LINE;
		break;
	case 1:
		PORT(DIG_1) &= ~DIG_1_LINE;
		break;
	default:
		PORT(DIG_0) &= ~DIG_0_LINE;
		break;
	}
#else
	switch (pos) {
	case 3:
		PORT(DIG_3) |= DIG_3_LINE;
		break;
	case 2:
		PORT(DIG_2) |= DIG_2_LINE;
		break;
	case 1:
		PORT(DIG_1) |= DIG_1_LINE;
		break;
	default:
		PORT(DIG_0) |= DIG_0_LINE;
		break;
	}
#endif

	/* Go to next digit */
	if (++pos >= DIGITS)
		pos = 0;

	/* Handling buttons and encoder events */

	static int16_t btnCnt = 0;

	uint8_t encNow = ENC_0;
	uint8_t btnNow = BTN_STATE_0;

	if (encRes) {
		if (~PIN(ENCODER_A) & ENCODER_A_LINE)
			encNow |= ENC_A;
		if (~PIN(ENCODER_B) & ENCODER_B_LINE)
			encNow |= ENC_B;
	}

	if (~PIN(BUTTON_1) & BUTTON_1_LINE)
		btnNow |= BTN_1;
	if (~PIN(BUTTON_2) & BUTTON_2_LINE)
		btnNow |= BTN_2;
	if (~PIN(BUTTON_3) & BUTTON_3_LINE)
		btnNow |= BTN_3;
	if (~PIN(BUTTON_4) & BUTTON_4_LINE)
		btnNow |= BTN_4;
	if (!encRes) {
		if (~PIN(ENCODER_A) & ENCODER_A_LINE)
			btnNow |= ENC_A;
		if (~PIN(ENCODER_B) & ENCODER_B_LINE)
			btnNow |= ENC_B;
	}

	/* If encoder event has happened, inc/dec encoder counter */
	if (encRes) {
		if ((encPrev == ENC_0 && encNow == ENC_A) ||
				(encPrev == ENC_A && encNow == ENC_AB) ||
				(encPrev == ENC_AB && encNow == ENC_B) ||
				(encPrev == ENC_B && encNow == ENC_0))
			encCnt--;
		if ((encPrev == ENC_0 && encNow == ENC_B) ||
				(encPrev == ENC_B && encNow == ENC_AB) ||
				(encPrev == ENC_AB && encNow == ENC_A) ||
				(encPrev == ENC_A && encNow == ENC_0))
			encCnt++;
		encPrev = encNow; /* Save current encoder state */
	}

	/* If button event has happened, place it to command buffer */
	if (btnNow) {
		if (btnNow == btnPrev) {
			btnCnt++;
			if (btnCnt == LONG_PRESS) {
				switch (btnPrev) {
				case BTN_1:
					cmdBuf = CMD_BTN_1_LONG;
					break;
				case BTN_2:
					cmdBuf = CMD_BTN_2_LONG;
					break;
				case BTN_3:
					cmdBuf = CMD_BTN_3_LONG;
					break;
				case BTN_4:
					cmdBuf = CMD_BTN_4_LONG;
					break;
				}
				if (!encRes) {
					switch (btnPrev) {
					case (BTN_1 | ENC_A):
						cmdBuf = CMD_BTN_4_LONG;
						break;
					case (BTN_2 | ENC_B):
						cmdBuf = CMD_BTN_3_LONG;
						break;
					}
				}
			} else if (!encRes) {
				if (btnCnt == LONG_PRESS + AUTOREPEAT) {
					switch (btnPrev) {
					case ENC_A:
						encCnt++;
						break;
					case ENC_B:
						encCnt--;
						break;
					}
					btnCnt = LONG_PRESS + 1;
				}
			}
		} else {
			btnPrev = btnNow;
		}
	} else {
		if ((btnCnt > SHORT_PRESS) && (btnCnt < LONG_PRESS)) {
			switch (btnPrev) {
			case BTN_1:
				cmdBuf = CMD_BTN_1;
				break;
			case BTN_2:
				cmdBuf = CMD_BTN_2;
				break;
			case BTN_3:
				cmdBuf = CMD_BTN_3;
				break;
			case BTN_4:
				cmdBuf = CMD_BTN_4;
				break;
			}
			if (!encRes) {
				switch (btnPrev) {
				case ENC_A:
					cmdBuf = CMD_BTN_4;
					break;
				case ENC_B:
					cmdBuf = CMD_BTN_3;
					break;
				}
			}
		}
		btnCnt = 0;
	}

	/* Timer of current display mode */
	if (displayTime > 0)
		displayTime--;

	/* Blink timer */
	if (blink > 0)
		blink--;
	else
		blink = BLINK_PERIOD;

	if (tempTimer > 0)
		tempTimer--;

	if (rtcTimer > 0)
		rtcTimer--;

	if (fmStatusTimer > 0)
		fmStatusTimer--;

	return;
}

void segmNum(int16_t number, uint8_t dot, uint8_t label, uint8_t stInd)
{
#ifdef _NIXIE
	PORT(SEG_G) &= ~SEG_G_LINE;
	if (dot == 1)
		PORT(SEG_P) |= SEG_P_LINE;
	else
		PORT(SEG_P) &= ~SEG_P_LINE;
	if (stInd)
		PORT(SEG_F) |= SEG_F_LINE;
	else
		PORT(SEG_F) &= ~SEG_F_LINE;
#endif

#ifndef _NIXIE
	uint8_t neg = 0;
#endif
	uint8_t i;

	uint8_t buf[DIGITS];

	for (i = 0; i < DIGITS; i++)
		buf[i] = CH_EMPTY;
#ifndef _NIXIE
	if (stInd)
		buf[0] = BIT_P;
#endif

	if (number < 0) {
#ifndef _NIXIE
		neg = 1;
#endif
		number = -number;
	}

	for (i = 0; i < DIGITS; i++) {
		if (number == 0 && i > dot)
			break;
		buf[i] = num[number % 10];
#ifndef _NIXIE
		if (i == (dot % DIGITS) && i)
			buf[i] |= BIT_P;
#endif
		number /= 10;
	}

#ifndef _NIXIE
	if (neg)
		buf[i] |= BIT_G;
#endif
	if (buf[3] == CH_EMPTY)
		buf[3] = label;

	ind[0] = buf[0];
	ind[1] = buf[1];
	ind[2] = buf[2];
	ind[3] = buf[3];

	return;
}

void segmTime(void)
{
	// Second dot
	uint8_t secDot = 0;

	// Data for left and right halfs of display
	uint8_t tmL = rtc.hour;
	uint8_t tmR = rtc.min;

	// Indicator data;
	uint8_t d[4];

	if (rtcTimer > RTC_POLL_TIME * SCAN_FACTOR / 4)
		secDot = 1;

	// Select time parameters to show on display
	switch (rtc.etm) {
	case RTC_SEC:
		tmL = rtc.min;
		tmR = rtc.sec;
		break;
	case RTC_DATE:
		tmL = rtc.date;
		break;
	case RTC_MONTH:
		tmR = rtc.month;
		break;
	case RTC_YEAR:
		tmL = 20;
		tmR = rtc.year;
		break;
	default:
		break;
	}

	// Limit time values
	if (tmL > 99)
		tmL = 0;
	if (tmR > 99)
		tmR = 0;

	// Prepare indicators data
	d[0] = num[tmR % 10];
	d[1] = num[tmR / 10];
	d[2] = num[tmL % 10];
	d[3] = num[tmL / 10];

	// Hide month
	if (rtc.etm == RTC_DATE) {
		d[0] = CH_EMPTY;
		d[1] = CH_EMPTY;
	}

	// Hide date
	if (rtc.etm == RTC_MONTH) {
		d[2] = CH_EMPTY;
		d[3] = CH_EMPTY;
	}

	// hide blinking parameter
	if (blink < BLINK_TIME) {
		switch (rtc.etm) {
		case RTC_SEC:
		case RTC_MIN:
		case RTC_MONTH:
		case RTC_YEAR:
			d[0] = CH_EMPTY;
			d[1] = CH_EMPTY;
			break;
		}
		switch (rtc.etm) {
		case RTC_HOUR:
		case RTC_DATE:
		case RTC_YEAR:
			d[2] = CH_EMPTY;
			d[3] = CH_EMPTY;
			break;
		default:
			break;
		}
	}

	// Add seconds dot
	switch (rtc.etm) {
	case RTC_DATE:
	case RTC_MONTH:
	case RTC_YEAR:
		break;
	default:
#ifdef _NIXIE
		PORT(SEG_G) |= SEG_G_LINE;
#else
		d[2] |= BIT_P;
#endif
		break;
	}

	// Hide seconds dot and hour zero
	switch (rtc.etm) {
	case RTC_SEC:
	case RTC_DATE:
	case RTC_MONTH:
	case RTC_YEAR:
		break;
	default:
		if (zeroHour) {
			if (rtc.hour < 10)
				d[3] = CH_EMPTY;
		}
		if (secDot) {
#ifdef _NIXIE
			PORT(SEG_G) &= ~SEG_G_LINE;
#else
			d[2] &= ~BIT_P;
#endif
		}
		break;
	}

	// Show result
	uint8_t i;
	for (i = 0; i < DIGITS; i++)
		ind[i] = d[i];

	return;
}

void segmFmFreq(void)
{
	uint16_t freq = tuner.freq;

	if (getFmStatusTimer() == 0) {
		tunerReadStatus();
		setFmStatusTimer(FM_STATUS_TIME);
	}

	if (freq >= 10000) {
		segmNum(freq/10, 1, CH_EMPTY, tunerStereo());
	} else {
		if (freq >= FM_BAND_DIV_FREQ && eeprom_read_byte((uint8_t*)EEPROM_FM_STEP2) >= 10)
			segmNum(freq/10, 1, CH_EMPTY, tunerStereo());
		else
			segmNum(freq, 2, CH_EMPTY, tunerStereo());
	}

	return;
}

void segmFmEditFreq(void)
{
	uint16_t freq= tuner.freq;

	if (blink > BLINK_TIME) {
		if ((freq >= FM_BAND_DIV_FREQ && eeprom_read_byte((uint8_t*)EEPROM_FM_STEP2) >= 10) ||
				(freq < FM_BAND_DIV_FREQ && eeprom_read_byte((uint8_t*)EEPROM_FM_STEP1) >= 10))
			segmNum(freq/10, 1, CH_EMPTY, 0);
		else
			segmNum(freq, 2, CH_EMPTY, 0);
	} else {
		ind[0] = CH_EMPTY;
		ind[1] = CH_EMPTY;
		ind[2] = CH_EMPTY;
		ind[3] = CH_EMPTY;
	}

	return;
}

void segmFmNum(void)
{
	uint8_t num = tunerStationNum();

	if (num) {
		segmNum(num, 0, CH_C, 0);
	} else {
#ifdef _NIXIE
		ind[0] = CH_0;
		ind[1] = CH_EMPTY;
#else
		ind[0] = BIT_G;
		ind[1] = BIT_G;
#endif
		ind[2] = CH_EMPTY;
		ind[3] = CH_C;
	}
	return;
}

void segmVol(void)
{
	segmNum(getVolume(), 0, CH_G, 0);

	return;
}

void segmTemp(void)
{
	segmNum(ds18x20GetTemp(0), 1, CH_EMPTY, 0);

	return;
}

void segmTimeOrTemp()
{
	if (getDevCount() && (rtc.sec % 15 < 2)) {
		segmTemp();
	} else {
		segmTime();
	}

	return;
}


int8_t getEncoder(void)
{
	int8_t ret = 0;

	if (encRes) {
		if (encRes > 0) {
			while (encCnt >= encRes) {
				ret++;
				encCnt -= encRes;
			}
			while (encCnt <= -encRes) {
				ret--;
				encCnt += encRes;
			}
		} else {
			while (encCnt <= encRes) {
				ret++;
				encCnt -= encRes;
			}
			while (encCnt >= -encRes) {
				ret--;
				encCnt += encRes;
			}
		}
	} else {
		ret = encCnt;
		encCnt = 0;
	}

	return ret;
}

uint8_t getBtnCmd(void)
{
	uint8_t ret = cmdBuf;
	cmdBuf = CMD_EMPTY;
	return ret;
}

uint16_t getDisplayTime(void)
{
	return displayTime / SCAN_FACTOR * 2;
}

void setDisplayTime(uint16_t value)
{
	displayTime = value / 2 * SCAN_FACTOR;
}

void setBrightness(uint8_t value)
{
	if (value > BR_MAX)
		value = BR_MAX;

	OCR2 = value * 21;

	return;
}

uint16_t getTempTimer(void)
{
	return tempTimer * 2 / SCAN_FACTOR;
}

void setTempTimer(uint16_t val)
{
	tempTimer = val * SCAN_FACTOR / 2;
}

uint16_t getRtcTimer()
{
	return rtcTimer * 2 / SCAN_FACTOR;
}

void setRtcTimer(uint16_t val)
{
	rtcTimer = val * SCAN_FACTOR / 2;
}

uint16_t getFmStatusTimer()
{
	return fmStatusTimer * 2 / SCAN_FACTOR;
}

void setFmStatusTimer(uint16_t val)
{
	fmStatusTimer = val * SCAN_FACTOR / 2;
}
