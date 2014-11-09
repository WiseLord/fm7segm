#include "segm.h"

#include "avr/interrupt.h"

#include "ds1307.h"

static volatile uint8_t ind[DIGITS];
static const uint8_t num[] = {CH_0, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_7, CH_8, CH_9};

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

	DDR(ENCODER_A) |= ENCODER_A_LINE;
	DDR(ENCODER_B) |= ENCODER_B_LINE;

	TIMSK |= (1<<TOIE2);							/* Enable timer overflow interrupt */
	TCCR2 |= (0<<CS22) | (1<<CS21) | (1<<CS20);		/* Set timer prescaller to 64 */
	TCNT2 = 0;

	return;
}

ISR (TIMER2_OVF_vect)
{
	TCNT2 = 131;									/* 8000000 / 64 / (256-131) = 1000 */
	static uint8_t pos = 0;

	uint8_t dig = 0;

	/* Switch off all digits */
	PORT(DIG_0) &= ~DIG_0_LINE;
	PORT(DIG_1) &= ~DIG_1_LINE;
	PORT(DIG_2) &= ~DIG_2_LINE;
	PORT(DIG_3) &= ~DIG_3_LINE;

	dig = ind[pos];

	/* Set data on segments */
	if (dig & BIT_A)
		PORT(SEG_A) |= SEG_A_LINE;
	else
		PORT(SEG_A) &= ~SEG_A_LINE;
	if (dig & BIT_B)
		PORT(SEG_B) |= SEG_B_LINE;
	else
		PORT(SEG_B) &= ~SEG_B_LINE;
	if (dig & BIT_C)
		PORT(SEG_C) |= SEG_C_LINE;
	else
		PORT(SEG_C) &= ~SEG_C_LINE;
	if (dig & BIT_D)
		PORT(SEG_D) |= SEG_D_LINE;
	else
		PORT(SEG_D) &= ~SEG_D_LINE;
	if (dig & BIT_E)
		PORT(SEG_E) |= SEG_E_LINE;
	else
		PORT(SEG_E) &= ~SEG_E_LINE;
	if (dig & BIT_F)
		PORT(SEG_F) |= SEG_F_LINE;
	else
		PORT(SEG_F) &= ~SEG_F_LINE;
	if (dig & BIT_G)
		PORT(SEG_G) |= SEG_G_LINE;
	else
		PORT(SEG_G) &= ~SEG_G_LINE;
	if (dig & BIT_P)
		PORT(SEG_P) |= SEG_P_LINE;
	else
		PORT(SEG_P) &= ~SEG_P_LINE;

	/* Switch on current digit */
	switch (pos) {
	case 3:
		PORT(DIG_3) |= DIG_3_LINE;
		pos = 0;
		break;
	case 2:
		PORT(DIG_2) |= DIG_2_LINE;
		pos = 3;
		break;
	case 1:
		PORT(DIG_1) |= DIG_1_LINE;
		pos = 2;
		break;
	default:
		PORT(DIG_0) |= DIG_0_LINE;
		pos = 1;
		break;
	}

	return;
}

void segmNum(int16_t number, uint8_t dotPos)
{
	uint8_t neg = 0;
	uint8_t i;

	for (i = 0; i < DIGITS; i++)
		ind[i] = 0;

	if (number < 0) {
		neg = 1;
		number = -number;
	}

	for (i = 0; i < DIGITS; i++) {
		if (number == 0 && i > dotPos)
			break;
		ind[i] = num[number % 10];
		if (i == (dotPos % DIGITS) && i)
			ind[i] |= BIT_P;
		number /= 10;
	}

	if (neg)
		ind[i] = BIT_G;

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

	segmNum(100 * time[HOUR] + time[MIN], timeDot);

	return;
}

void segmFmFreq(uint16_t freq)
{
	segmNum(freq/10, 1);

	return;
}
