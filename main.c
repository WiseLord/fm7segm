#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "segm.h"
#include "i2c.h"
#include "ds1307.h"
#include "tea5767.h"

#include "pins.h"

void hwInit(void)
{
	segmInit();							/* Indicator */
	I2CInit();							/* I2C bus */
	ds1307Init();						/* RTC */

	sei();

	return;
}

int main(void)
{
	hwInit();

	tea5767LoadParams();

	tea5767SetFreq(9950);

	while (1) {
//		segmTimeHM();
		segmFmFreq(tea5767GetFreq());
		_delay_ms(50);
	}

	return 0;
}

