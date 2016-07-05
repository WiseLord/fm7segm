#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "segm.h"
#include "i2c.h"
#include "ds1307.h"
#include "tuner/tuner.h"
#include "volume.h"
#include "ds18x20.h"
#include "eeprom.h"

static uint8_t defDispMode = MODE_TIME;
static int8_t brWork, brStby;
static uint8_t dsCnt = 0;

static void segmBr(void)
{
	segmNum(brWork, 0, CH_E, 0);

	return;
}

/* Handle leaving standby mode */
static void powerOn(void)
{
	tunerPowerOn();
	unmuteVolume();
	setVolume(getVolume());
	setBrightness(brWork);
	tunerSetFreq(tunerGetFreq());

	return;
}

/* Handle entering standby mode */
static void powerOff(void)
{
	stopEditTime();
	setBrightness(brStby);

	muteVolume();
	volumeSaveParams();
	tunerPowerOff();

	eeprom_update_byte(eepromDispMode, defDispMode);
	eeprom_update_byte(eepromBrWork, brWork);

	eeprom_update_word(eepromFMFreq, tunerGetFreq());
	eeprom_update_byte(eepromFMMono, tunerGetMono());

	return;
}

void hwInit(void)
{
	uint8_t i;

	_delay_ms(100);

	sei();
	/* 5 attempts to find temperature sensors */
	for (i = 0; i < 5 && !dsCnt; i++) {
		_delay_ms(1);
		ds18x20SearchDevices();
		dsCnt = getDevCount();
	}

	segmInit();							/* Indicator */
	I2CInit();							/* I2C bus */
	ds1307Init();						/* RTC */
	tunerInit();
	volumeInit();

	defDispMode = eeprom_read_byte(eepromDispMode);
	brWork = eeprom_read_byte(eepromBrWork);
	brStby = eeprom_read_byte(eepromBrStby);
	powerOff();

	return;
}

int main(void)
{
	hwInit();

	uint8_t dispMode = MODE_STANDBY;
	uint8_t editFM = 0;
	int8_t encCnt = 0;
	uint8_t cmd = CMD_EMPTY;

	int8_t *time = readTime();
	setRtcTimer(RTC_POLL_TIME);

	while (1) {
		encCnt = getEncoder();
		cmd = getBtnCmd();

		/* Poll RTC for time */
		if (getRtcTimer() == 0) {
			readTime();
			setRtcTimer(RTC_POLL_TIME);
		}

		/* If temperature sensor has not still found, continue searching */
		if (!dsCnt) {
			ds18x20SearchDevices();
			dsCnt = getDevCount();
		} else {
			ds18x20Process();
		}

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
			} else if (dispMode == MODE_TEMP) {
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
			} else {
				if (dispMode == MODE_TIME) {
					if (dsCnt) {
						dispMode = MODE_TEMP;
						setDisplayTime(DISPLAY_TIME_TEMP);
					} else {
						defDispMode = MODE_FM_FREQ;
						dispMode = MODE_FM_FREQ;
						setDisplayTime(DISPLAY_TIME_FM_FREQ);
					}
				} else {
					dispMode = MODE_TIME;
					if (!dsCnt)
						defDispMode = MODE_TIME;
					setDisplayTime(DISPLAY_TIME_TIME);
				}
			}
			break;
		case CMD_BTN_3:
			if (editFM) {
				tunerChangeFreq(-10);
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
					tunerNextStation(SEARCH_DOWN);
					dispMode = MODE_FM_CHAN;
					setDisplayTime(DISPLAY_TIME_FM_CHAN);
					break;
				}
			}
			break;
		case CMD_BTN_4:
			if (editFM) {
				tunerChangeFreq(10);
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
					tunerNextStation(SEARCH_UP);
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
				switch (dispMode) {
				case MODE_TIME_EDIT_H:
					changeTimeH(-10);
					setDisplayTime(DISPLAY_TIME_EDITTIME);
					break;
				case MODE_TIME_EDIT_M:
					changeTimeM(-10);
					setDisplayTime(DISPLAY_TIME_EDITTIME);
					break;
				default:
					dispMode = MODE_FMTUNE_FREQ;
					setDisplayTime(DISPLAY_TIME_FMTUNE_FREQ);
					editFM = 1;
					break;
				}

			}
			break;
		case CMD_BTN_4_LONG:
			if (editFM) {
			tunerStoreStation();
				dispMode = MODE_FMTUNE_CHAN;
				setDisplayTime(DISPLAY_TIME_FMTUNE_CHAN);
			} else {
				switch (dispMode) {
				case MODE_TIME_EDIT_H:
					changeTimeH(10);
					setDisplayTime(DISPLAY_TIME_EDITTIME);
					break;
				case MODE_TIME_EDIT_M:
					changeTimeM(10);
					setDisplayTime(DISPLAY_TIME_EDITTIME);
					break;
				default:
					tunerSwitchMono();
					dispMode = MODE_FM_CHAN;
					setDisplayTime(DISPLAY_TIME_FM_CHAN);
					break;
				}
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
				tunerChangeFreq(encCnt);
				dispMode = MODE_FMTUNE_FREQ;
				setDisplayTime(DISPLAY_TIME_FMTUNE_FREQ);
				break;
			case MODE_BRIGHTNESS:
				brWork += encCnt;
				if (brWork > BR_MAX)
					brWork = BR_MAX;
				if (brWork < BR_MIN)
					brWork = BR_MIN;
				setBrightness(brWork);
				setDisplayTime(DISPLAY_TIME_BRIGHTNESS);
				break;
			default:
				dispMode = MODE_VOLUME;
				setVolume(getVolume() + encCnt);
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
			if (dsCnt && (time[SEC] % 15 <= 2)) { /* Every 20 sec for 2 sec*/
				segmTemp();
			} else {
				segmTimeHM(time);
			}
			break;
		case MODE_TEMP:
			segmTemp();
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
			if (dsCnt && (time[SEC] % 15 <= 2) && !getDisplayTime()) {
				segmTemp();
			} else {
				segmTimeHM(time);
			}
			break;
		case MODE_TIME_EDIT_H:
			segmTimeEditH(time);
			break;
		case MODE_TIME_EDIT_M:
			segmTimeEditM(time);
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

