#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "segm.h"
#include "i2c.h"
#include "ds1307.h"
#include "tuner.h"

#include "pins.h"

void hwInit(void)
{
	segmInit();							/* Indicator */
	I2CInit();							/* I2C bus */
	ds1307Init();						/* RTC */

	tunerInit();

	sei();

	return;
}

int main(void)
{
	hwInit();

	loadTunerParams();

	tunerSetFreq(9950);

	while (1) {
//		segmTimeHM();
		segmFreq(tunerGetFreq());
		_delay_ms(10);
	}

	return 0;
}

