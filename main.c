#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "segm.h"
#include "i2c.h"
#include "ds1307.h"
#include "tea5767.h"
#include "volume.h"

#include "pins.h"

/* Handle leaving standby mode */
static void powerOn(void)
{
	_delay_ms(50);
	tea5767LoadParams();

	unmuteVolume();

	return;
}

/* Handle entering standby mode */
static void powerOff(void)
{
	muteVolume();

	stopEditTime();

	volumeSaveParams();
	tea5767SaveParams();

	return;
}

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

	uint8_t dispMode = MODE_STANDBY;

	tea5767LoadParams();
	tea5767SetFreq(9950);

	volumeLoadParams();

	int8_t encCnt = 0;
	uint8_t cmd = CMD_EMPTY;

	while (1) {
		encCnt = getEncoder();
		cmd = getBtnCmd();

		/* Don't handle commands in standby mode except STBY */
		if (dispMode == MODE_STANDBY) {
			if (cmd != CMD_BTN_1)
				cmd = CMD_EMPTY;
		}

		/* Handle command */
		switch (cmd) {
		case CMD_BTN_1:
			switch (dispMode) {
			case MODE_STANDBY:
				powerOn();
				dispMode = MODE_FM_RADIO;
				break;
			default:
				powerOff();
				dispMode = MODE_STANDBY;
				break;
			}
			break;
			dispMode = MODE_FM_RADIO;
			setDisplayTime(DISPLAY_TIME_FM_RADIO);
			break;
		case CMD_BTN_2:
			dispMode = MODE_TIME;
			setDisplayTime(DISPLAY_TIME_TIME);
			break;
		case CMD_BTN_3:
			tea5767ScanStoredFreq(SEARCH_DOWN);
			dispMode = MODE_FM_RADIO;
			setDisplayTime(DISPLAY_TIME_FM_RADIO);
			break;
		case CMD_BTN_4:
			tea5767ScanStoredFreq(SEARCH_UP);
			dispMode = MODE_FM_RADIO;
			setDisplayTime(DISPLAY_TIME_FM_RADIO);
			break;
		default:
			break;
		}

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
				dispMode = MODE_FM_RADIO;
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
	}

	return 0;
}

