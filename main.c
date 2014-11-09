#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "segm.h"
#include "i2c.h"
#include "ds1307.h"
#include "tea5767.h"
#include "volume.h"

#include "pins.h"

void hwInit(void)
{
	segmInit();							/* Indicator */
	I2CInit();							/* I2C bus */
	ds1307Init();						/* RTC */
	volumeInit();

	sei();

	return;
}

int main(void)
{
	hwInit();

	tea5767LoadParams();

	tea5767SetFreq(9950);

	DDR(VOLUME) |= VOLUME_LINE;

	DDR(ENC_GND) |= ENC_GND_LINE;
	PORT(ENC_GND) &= ~ENC_GND_LINE;

	int8_t vol = 8;
	setVolume(vol);

	while (1) {
		segmNum(getVolume(), 0);
		_delay_ms(500);
//		changeVolume(1);
//		segmTimeHM();
//		segmFmFreq(tea5767GetFreq());
	}

	return 0;
}

