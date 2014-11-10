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
static int8_t brWork;

static void segmBr(void)
{
	segmNum(brWork, 0, CH_E);

	return;
}

/* Handle leaving standby mode */
static void powerOn(void)
{
	_delay_ms(50);
	tea5767LoadParams();

	unmuteVolume();

	setBrightness(brWork);

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
	eeprom_update_word(eepromBrWork, brWork);

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
	brWork = eeprom_read_word(eepromBrWork);
	if (brWork <= BR_MIN)
		brWork = BR_STBY;
	if (brWork >= BR_MAX)
		brWork = BR_MAX;

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
			if (dispMode == MODE_TIME_EDIT_H) {
				dispMode = MODE_TIME_EDIT_M;
				setDisplayTime(DISPLAY_TIME_EDITTIME);
			} else if (dispMode == MODE_TIME_EDIT_M) {
				dispMode = MODE_TIME_EDIT_H;
				setDisplayTime(DISPLAY_TIME_EDITTIME);
			} else {
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
			}
			break;
		case CMD_BTN_3:
			if (editFM) {
				tea5767DecFreq(10);
				dispMode = MODE_FMTUNE_FREQ;
				setDisplayTime(DISPLAY_TIME_FMTUNE_FREQ);
			} else {
				switch (dispMode) {
				case MODE_TIME_EDIT_H:
					changeTimeH(-1);
					setDisplayTime(DISPLAY_TIME_EDITTIME);
					break;
				case MODE_TIME_EDIT_M:
					changeTimeM(-1);
					setDisplayTime(DISPLAY_TIME_EDITTIME);
					break;
				default:
					tea5767ScanStoredFreq(SEARCH_DOWN);
					dispMode = MODE_FM_CHAN;
					setDisplayTime(DISPLAY_TIME_FM_CHAN);
					break;
				}
			}
			break;
		case CMD_BTN_4:
			if (editFM) {
				tea5767IncFreq(10);
				dispMode = MODE_FMTUNE_FREQ;
				setDisplayTime(DISPLAY_TIME_FMTUNE_FREQ);
			} else {
				switch (dispMode) {
				case MODE_TIME_EDIT_H:
					changeTimeH(+1);
					setDisplayTime(DISPLAY_TIME_EDITTIME);
					break;
				case MODE_TIME_EDIT_M:
					changeTimeM(+1);
					setDisplayTime(DISPLAY_TIME_EDITTIME);
					break;
				default:
					tea5767ScanStoredFreq(SEARCH_UP);
					dispMode = MODE_FM_CHAN;
					setDisplayTime(DISPLAY_TIME_FM_CHAN);
					break;
				}
			}
			break;
		case CMD_BTN_1_LONG:
			editFM = 0;
			dispMode = MODE_BRIGHTNESS;
			setDisplayTime(DISPLAY_TIME_BRIGHTNESS);
			break;
		case CMD_BTN_2_LONG:
			editFM = 0;
			switch (dispMode) {
			case MODE_TIME_EDIT_H:
			case MODE_TIME_EDIT_M:
				dispMode = defDispMode;
				break;
			default:
				dispMode = MODE_TIME_EDIT_H;
				setDisplayTime(DISPLAY_TIME_EDITTIME);
				break;
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
			case MODE_TIME_EDIT_H:
				changeTimeH(encCnt);
				setDisplayTime(DISPLAY_TIME_EDITTIME);
				break;
			case MODE_TIME_EDIT_M:
				changeTimeM(encCnt);
				setDisplayTime(DISPLAY_TIME_EDITTIME);
				break;
			case MODE_FMTUNE_FREQ:
				if (encCnt > 0)
					tea5767IncFreq(1);
				else
					tea5767DecFreq(1);
				dispMode = MODE_FMTUNE_FREQ;
				setDisplayTime(DISPLAY_TIME_FMTUNE_FREQ);
				break;
			case MODE_BRIGHTNESS:
				brWork += encCnt;
				if (brWork > BR_MAX)
					brWork = BR_MAX;
				if (brWork < BR_STBY)
					brWork = BR_STBY;
				setBrightness(brWork);
				setDisplayTime(DISPLAY_TIME_BRIGHTNESS);
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
			segmTimeHM();
			break;
		case MODE_TIME_EDIT_H:
			segmTimeEditH();
			break;
		case MODE_TIME_EDIT_M:
			segmTimeEditM();
			break;
		case MODE_BRIGHTNESS:
			segmBr();
			break;
		default:
			segmVol();
			break;
		}
	}

	return 0;
}

