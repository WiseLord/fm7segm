#include "segm.h"

#include "avr/interrupt.h"

static volatile uint8_t br = 0;						/* Brightness */

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

	TIMSK |= (1<<TOIE0);							/* Enable timer overflow interrupt */
	TCCR0 |= (0<<CS02) | (1<<CS01) | (0<<CS00);		/* Set timer prescaller to 8 */
	TCNT0 = 0;

	segmBrightness(BR_MAX);

	return;
}

ISR (TIMER0_OVF_vect)
{
	static uint8_t pos = 0;
	static uint8_t duty = 0;

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
	if (br > duty) {
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
	}

	duty++;
	if (duty >= BR_MAX) {
		duty = 0;
		pos++;
		if (pos >= DIGITS)
			pos = 0;
	}

	return;
}

void segmBrightness(uint8_t value)
{
	br = value;

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
		if (i == dotPos && i)
			ind[i] |= BIT_P;
		number /= 10;
	}

	if (neg)
		ind[i] = BIT_G;

	return;
}
