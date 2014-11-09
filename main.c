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

	uint8_t dispMode = MODE_TIME;

	tea5767LoadParams();
	tea5767SetFreq(9950);

	DDR(ENC_GND) |= ENC_GND_LINE;
	PORT(ENC_GND) &= ~ENC_GND_LINE;

//	int8_t vol = 8;
//	setVolume(vol);
	volumeLoadParams();

	int8_t encCnt = 0;

	while (1) {
		encCnt = getEncoder();

		/* Handle encoder */
		if (encCnt) {
			switch (dispMode) {
			case MODE_STANDBY:
				break;
			case MODE_TIME:
			case MODE_FM_RADIO:
				dispMode = MODE_VOLUME;
			default:
				changeVolume(encCnt);
				setDisplayTime(DISPLAY_TIME_VOLUME);
				break;
			}
		}

		/* Exit to default mode and save params to EEPROM*/
		if (getDisplayTime() == 0) {
			switch (dispMode) {
			case MODE_STANDBY:
				break;
			default:
				dispMode = MODE_TIME;
				volumeSaveParams();
				break;
			}
		}

		/* Show things */
		switch (dispMode) {
		case MODE_STANDBY:
			segmTimeHM();
			break;
		case MODE_FM_RADIO:
			segmFmFreq(tea5767GetFreq());
			break;
		case MODE_TIME:
		case MODE_TIME_EDIT:
			segmTimeHM();
			break;
		default:
			segmNum(getVolume(), 0);
			break;
		}

		_delay_ms(100);
	}

	return 0;
}

