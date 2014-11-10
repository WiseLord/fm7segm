#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "segm.h"
#include "i2c.h"
#include "ds1307.h"
#include "tea5767.h"
#include "volume.h"

#include "pins.h"

static uint8_t defDispMode = MODE_TIME;

/* Handle leaving standby mode */
static void powerOn(void)
{
	_delay_ms(50);
	tea5767LoadParams();

	unmuteVolume();

	setBrightness(BR_WORK);

	return;
}

/* Handle entering standby mode */
static void powerOff(void)
{
	muteVolume();

	stopEditTime();

	volumeSaveParams();
	tea5767SaveParams();
	eeprom_update_word(eepromDispMode, defDispMode);

	setBrightness(BR_STBY);

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
	uint8_t editFM = 0;

	volumeLoadParams();
	defDispMode = eeprom_read_word(eepromDispMode);

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
			editFM = 0;
			switch (dispMode) {
			case MODE_STANDBY:
				powerOn();
				dispMode = MODE_FM_CHAN;
				setDisplayTime(DISPLAY_TIME_FM_CHAN);
				break;
			default:
				powerOff();
				dispMode = MODE_STANDBY;
				break;
			}
			break;
		case CMD_BTN_2:
			if (editFM) {
				editFM = 0;
				dispMode = defDispMode;
				break;
			}
			switch (defDispMode) {
			case MODE_TIME:
				defDispMode = MODE_FM_FREQ;
				dispMode = MODE_FM_FREQ;
				setDisplayTime(DISPLAY_TIME_FM_FREQ);
				break;
			default:
				defDispMode = MODE_TIME;
				dispMode = MODE_TIME;
				setDisplayTime(DISPLAY_TIME_TIME);
				break;
			}
			break;
		case CMD_BTN_3:
			if (editFM) {
				tea5767DecFreq();
				dispMode = MODE_FMTUNE_FREQ;
				setDisplayTime(DISPLAY_TIME_FMTUNE_FREQ);
			} else {
				tea5767ScanStoredFreq(SEARCH_DOWN);
				dispMode = MODE_FM_CHAN;
				setDisplayTime(DISPLAY_TIME_FM_CHAN);
			}
			break;
		case CMD_BTN_4:
			if (editFM) {
				tea5767IncFreq();
				dispMode = MODE_FMTUNE_FREQ;
				setDisplayTime(DISPLAY_TIME_FMTUNE_FREQ);
			} else {
				tea5767ScanStoredFreq(SEARCH_UP);
				dispMode = MODE_FM_CHAN;
				setDisplayTime(DISPLAY_TIME_FM_CHAN);
			}
			break;
		case CMD_BTN_3_LONG:
			if (editFM) {
				dispMode = MODE_FM_CHAN;
				setDisplayTime(DISPLAY_TIME_FM_CHAN);
				editFM = 0;
			} else {
				dispMode = MODE_FMTUNE_FREQ;
				setDisplayTime(DISPLAY_TIME_FMTUNE_FREQ);
				editFM = 1;
			}
			break;
		case CMD_BTN_4_LONG:
			if (editFM) {
				tea5767StoreStation();
				dispMode = MODE_FMTUNE_CHAN;
				setDisplayTime(DISPLAY_TIME_FMTUNE_CHAN);
			} else {
				tea5767StoreStation();
				dispMode = MODE_FM_CHAN;
				setDisplayTime(DISPLAY_TIME_FM_CHAN);
			}
			break;
		default:
			break;
		}

		/* Handle encoder */
		if (encCnt) {
			switch (dispMode) {
			case MODE_STANDBY:
				break;
			default:
				dispMode = MODE_VOLUME;
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
			case MODE_FM_CHAN:
				dispMode = MODE_FM_FREQ;
				setDisplayTime(DISPLAY_TIME_FM_FREQ);
				break;
			case MODE_FMTUNE_CHAN:
				dispMode = MODE_FMTUNE_FREQ;
				setDisplayTime(DISPLAY_TIME_FMTUNE_FREQ);
				break;
			case MODE_FMTUNE_FREQ:
				editFM = 0;
			default:
				if (editFM) {
					dispMode = MODE_FMTUNE_FREQ;
					setDisplayTime(DISPLAY_TIME_FMTUNE_FREQ);
				} else {
					dispMode = defDispMode;
				}
				break;
			}
		}

		/* Show things */
		switch (dispMode) {
		case MODE_STANDBY:
			segmTimeHM();
			break;
		case MODE_FM_CHAN:
		case MODE_FMTUNE_CHAN:
			segmFmNum();
			break;
		case MODE_FM_FREQ:
			segmFmFreq();
			break;
		case MODE_FMTUNE_FREQ:
			segmFmEditFreq();
			break;
		case MODE_TIME:
		case MODE_TIME_EDIT_H:
		case MODE_TIME_EDIT_M:
			segmTimeHM();
			break;
		default:
			segmVol();
			break;
		}
	}

	return 0;
}

