#include "segm.h"

#include "avr/interrupt.h"

#include "ds1307.h"
#include "tea5767.h"
#include "volume.h"

static volatile uint8_t ind[DIGITS];
static const uint8_t num[] = {CH_0, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9};

static volatile uint8_t brightness = 1;

static volatile int8_t encCnt;
static volatile uint8_t cmdBuf;
static volatile uint8_t encPrev = ENC_0;
static volatile uint8_t btnPrev = BTN_STATE_0;

static volatile uint16_t displayTime;
static volatile uint16_t blink;

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
	TCCR2 |= (0<<CS22) | (1<<CS21) | (0<<CS20);		/* Set timer prescaller to 8 */
	TCNT2 = 0;

	encCnt = 0;
	cmdBuf = CMD_EMPTY;

	return;
}

ISR (TIMER2_OVF_vect)								/* 8000000 / 8 / 256 = 3906 polls/sec */
{
	static uint8_t pos = 0;

	uint8_t dig = 0;

	dig = ind[pos];

	static uint8_t brPwm;
	brPwm++;
	if (brPwm > BR_MAX)
		brPwm = 0;

	/* Switch off segments */
	PORT(SEG_A) &= ~SEG_A_LINE;
	PORT(SEG_B) &= ~SEG_B_LINE;
	PORT(SEG_C) &= ~SEG_C_LINE;
	PORT(SEG_D) &= ~SEG_D_LINE;
	PORT(SEG_E) &= ~SEG_E_LINE;
	PORT(SEG_F) &= ~SEG_F_LINE;
	PORT(SEG_G) &= ~SEG_G_LINE;
	PORT(SEG_P) &= ~SEG_P_LINE;

	if (brPwm > 0 && brPwm <= brightness) {
		/* Change current digit */
		switch (pos) {
		case 3:
			PORT(DIG_2) &= ~DIG_2_LINE;
			PORT(DIG_3) |= DIG_3_LINE;
			pos = 0;
			break;
		case 2:
			PORT(DIG_1) &= ~DIG_1_LINE;
			PORT(DIG_2) |= DIG_2_LINE;
			pos = 3;
			break;
		case 1:
			PORT(DIG_0) &= ~DIG_0_LINE;
			PORT(DIG_1) |= DIG_1_LINE;
			pos = 2;
			break;
		default:
			PORT(DIG_3) &= ~DIG_3_LINE;
			PORT(DIG_0) |= DIG_0_LINE;
			pos = 1;
			break;
		}

		/* Set data on segments */
		if (dig & BIT_A)
			PORT(SEG_A) |= SEG_A_LINE;
		if (dig & BIT_B)
			PORT(SEG_B) |= SEG_B_LINE;
		if (dig & BIT_C)
			PORT(SEG_C) |= SEG_C_LINE;
		if (dig & BIT_D)
			PORT(SEG_D) |= SEG_D_LINE;
		if (dig & BIT_E)
			PORT(SEG_E) |= SEG_E_LINE;
		if (dig & BIT_F)
			PORT(SEG_F) |= SEG_F_LINE;
		if (dig & BIT_G)
			PORT(SEG_G) |= SEG_G_LINE;
		if (dig & BIT_P)
			PORT(SEG_P) |= SEG_P_LINE;
	}

	/* Handling buttons and encoder events */

	static int16_t btnCnt = 0;

	uint8_t encNow = ENC_0;
	uint8_t btnNow = BTN_STATE_0;

	if (~PIN(ENCODER_A) & ENCODER_A_LINE)
		encNow |= ENC_A;
	if (~PIN(ENCODER_B) & ENCODER_B_LINE)
		encNow |= ENC_B;

	if (~PIN(BUTTON_1) & BUTTON_1_LINE)
		btnNow |= BTN_1;
	if (~PIN(BUTTON_2) & BUTTON_2_LINE)
		btnNow |= BTN_2;
	if (~PIN(BUTTON_3) & BUTTON_3_LINE)
		btnNow |= BTN_3;
	if (~PIN(BUTTON_4) & BUTTON_4_LINE)
		btnNow |= BTN_4;


	/* If encoder event has happened, inc/dec encoder counter */
	switch (encNow) {
	case ENC_AB:
		if (encPrev == ENC_B)
			encCnt++;
		if (encPrev == ENC_A)
			encCnt--;
		break;
	case ENC_0:
		if (encPrev == ENC_A)
			encCnt++;
		if (encPrev == ENC_B)
			encCnt--;
		break;
	}
	encPrev = encNow; /* Save current encoder state */

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
		blink = 2000;

	return;
}

void segmNum(int16_t number, uint8_t dotPos, uint8_t label)
{
	uint8_t neg = 0;
	uint8_t i;

	uint8_t buf[DIGITS];

	for (i = 0; i < DIGITS; i++)
		buf[i] = CH_EMPTY;

	if (number < 0) {
		neg = 1;
		number = -number;
	}

	for (i = 0; i < DIGITS; i++) {
		if (number == 0 && i > dotPos)
			break;
		buf[i] = num[number % 10];
		if (i == (dotPos % DIGITS) && i)
			buf[i] |= BIT_P;
		number /= 10;
	}

	if (neg)
		buf[i] = BIT_G;

	ind[0] = buf[0];
	ind[1] = buf[1];
	ind[2] = buf[2];
	ind[3] = buf[3] | label;

	return;
}

void segmTimeHM(void)
{
	int8_t *time;
	uint8_t timeDot;

	time = readTime();

	timeDot = 8;
	if (time[SEC] % 2)
		timeDot = 6;

	segmNum(100 * time[HOUR] + time[MIN], timeDot, CH_EMPTY);

	return;
}

void segmTimeEditH(void)
{
	int8_t *time;
	uint8_t timeDot;

	time = readTime();

	timeDot = 8;
	if (time[SEC] % 2)
		timeDot = 6;

	segmNum(100 * time[HOUR] + time[MIN], timeDot, CH_EMPTY);
	if (blink < 400) {
		ind[2] = CH_EMPTY;
		ind[3] = CH_EMPTY;
	}

	return;
}

void segmTimeEditM(void)
{
	int8_t *time;
	uint8_t timeDot;

	time = readTime();

	timeDot = 8;
	if (time[SEC] % 2)
		timeDot = 6;


	segmNum(100 * time[HOUR] + time[MIN], timeDot, CH_EMPTY);
	if (blink < 400) {
		ind[0] = CH_EMPTY;
		ind[1] = CH_EMPTY;
	}

	return;
}

void segmFmFreq(void)
{
	segmNum(tea5767GetFreq()/10, 1, CH_EMPTY);

	return;
}

void segmFmEditFreq(void)
{
	if (blink > 400)
		segmNum(tea5767GetFreq()/10, 1, CH_EMPTY);
	else {
		ind[0] = CH_EMPTY;
		ind[1] = CH_EMPTY;
		ind[2] = CH_EMPTY;
		ind[3] = CH_EMPTY;
	}

	return;
}

void segmFmNum(void)
{
	uint8_t num = tea5767StNum();

	if (num) {
		segmNum(num, 0, CH_C);
	} else {
		ind[0] = BIT_G;
		ind[1] = BIT_G;
		ind[2] = CH_EMPTY;
		ind[3] = CH_C;
	}
	return;
}

void segmVol(void)
{
	segmNum(getVolume(), 0, CH_G);

	return;
}

int8_t getEncoder(void)
{
	int8_t ret = encCnt;
	encCnt = 0;

	return ret;
}

uint8_t getBtnCmd(void)
{
	uint8_t ret = cmdBuf;
	cmdBuf = CMD_EMPTY;
	return ret;
}

void setDisplayTime(uint16_t value)
{
	displayTime = value << 2;
}

uint16_t getDisplayTime(void)
{
	return displayTime >> 2;
}

void setBrightness(uint8_t value)
{
	if (value > BR_MAX)
		value = BR_MAX;

	brightness = value;

	return;
}
