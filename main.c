#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "segm.h"

#include "pins.h"

void hwInit(void)
{
	segmInit();

	sei();
	return;
}

int main(void)
{
	hwInit();

	while (1) {
/*		PORT(DIG_3) |= DIG_3_LINE;
		PORT(SEG_A) |= SEG_A_LINE;  _delay_ms(100);
		PORT(SEG_A) &= ~SEG_A_LINE;
		PORT(SEG_B) |= SEG_B_LINE;  _delay_ms(100);
		PORT(SEG_B) &= ~SEG_B_LINE;
		PORT(SEG_C) |= SEG_C_LINE;  _delay_ms(100);
		PORT(SEG_C) &= ~SEG_C_LINE;
		PORT(SEG_D) |= SEG_D_LINE;  _delay_ms(100);
		PORT(SEG_D) &= ~SEG_D_LINE;
		PORT(SEG_E) |= SEG_E_LINE;  _delay_ms(100);
		PORT(SEG_E) &= ~SEG_E_LINE;
		PORT(SEG_F) |= SEG_F_LINE;  _delay_ms(100);
		PORT(SEG_F) &= ~SEG_F_LINE;
		PORT(SEG_G) |= SEG_G_LINE;  _delay_ms(100);
		PORT(SEG_G) &= ~SEG_G_LINE;
		PORT(SEG_P) |= SEG_P_LINE;  _delay_ms(100);
		PORT(SEG_P) &= ~SEG_P_LINE;
		PORT(DIG_3) &= ~DIG_3_LINE;

		PORT(DIG_2) |= DIG_2_LINE;
		PORT(SEG_A) |= SEG_A_LINE;  _delay_ms(100);
		PORT(SEG_A) &= ~SEG_A_LINE;
		PORT(SEG_B) |= SEG_B_LINE;  _delay_ms(100);
		PORT(SEG_B) &= ~SEG_B_LINE;
		PORT(SEG_C) |= SEG_C_LINE;  _delay_ms(100);
		PORT(SEG_C) &= ~SEG_C_LINE;
		PORT(SEG_D) |= SEG_D_LINE;  _delay_ms(100);
		PORT(SEG_D) &= ~SEG_D_LINE;
		PORT(SEG_E) |= SEG_E_LINE;  _delay_ms(100);
		PORT(SEG_E) &= ~SEG_E_LINE;
		PORT(SEG_F) |= SEG_F_LINE;  _delay_ms(100);
		PORT(SEG_F) &= ~SEG_F_LINE;
		PORT(SEG_G) |= SEG_G_LINE;  _delay_ms(100);
		PORT(SEG_G) &= ~SEG_G_LINE;
		PORT(SEG_P) |= SEG_P_LINE;  _delay_ms(100);
		PORT(SEG_P) &= ~SEG_P_LINE;
		PORT(DIG_2) &= ~DIG_2_LINE;

		PORT(DIG_1) |= DIG_1_LINE;
		PORT(SEG_A) |= SEG_A_LINE;  _delay_ms(100);
		PORT(SEG_A) &= ~SEG_A_LINE;
		PORT(SEG_B) |= SEG_B_LINE;  _delay_ms(100);
		PORT(SEG_B) &= ~SEG_B_LINE;
		PORT(SEG_C) |= SEG_C_LINE;  _delay_ms(100);
		PORT(SEG_C) &= ~SEG_C_LINE;
		PORT(SEG_D) |= SEG_D_LINE;  _delay_ms(100);
		PORT(SEG_D) &= ~SEG_D_LINE;
		PORT(SEG_E) |= SEG_E_LINE;  _delay_ms(100);
		PORT(SEG_E) &= ~SEG_E_LINE;
		PORT(SEG_F) |= SEG_F_LINE;  _delay_ms(100);
		PORT(SEG_F) &= ~SEG_F_LINE;
		PORT(SEG_G) |= SEG_G_LINE;  _delay_ms(100);
		PORT(SEG_G) &= ~SEG_G_LINE;
		PORT(SEG_P) |= SEG_P_LINE;  _delay_ms(100);
		PORT(SEG_P) &= ~SEG_P_LINE;
		PORT(DIG_1) &= ~DIG_1_LINE;

		PORT(DIG_0) |= DIG_0_LINE;
		PORT(SEG_A) |= SEG_A_LINE;  _delay_ms(100);
		PORT(SEG_A) &= ~SEG_A_LINE;
		PORT(SEG_B) |= SEG_B_LINE;  _delay_ms(100);
		PORT(SEG_B) &= ~SEG_B_LINE;
		PORT(SEG_C) |= SEG_C_LINE;  _delay_ms(100);
		PORT(SEG_C) &= ~SEG_C_LINE;
		PORT(SEG_D) |= SEG_D_LINE;  _delay_ms(100);
		PORT(SEG_D) &= ~SEG_D_LINE;
		PORT(SEG_E) |= SEG_E_LINE;  _delay_ms(100);
		PORT(SEG_E) &= ~SEG_E_LINE;
		PORT(SEG_F) |= SEG_F_LINE;  _delay_ms(100);
		PORT(SEG_F) &= ~SEG_F_LINE;
		PORT(SEG_G) |= SEG_G_LINE;  _delay_ms(100);
		PORT(SEG_G) &= ~SEG_G_LINE;
		PORT(SEG_P) |= SEG_P_LINE;  _delay_ms(100);
		PORT(SEG_P) &= ~SEG_P_LINE;
		PORT(DIG_0) &= ~DIG_0_LINE;
*/	}

	return 0;
}

