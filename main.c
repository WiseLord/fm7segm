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
static uint8_t dsOnBus = 0;

static void segmBr(void)
{
	segmNum(brWork, 0, CH_E);

	return;
}

/* Handle leaving standby mode */
static void powerOn(void)
{
	_delay_ms(50);
	tunerPowerOn();

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
	tunerPowerOff();

	eeprom_update_byte(eepromDispMode, defDispMode);
	eeprom_update_byte(eepromBrWork, brWork);

	setBrightness(brStby);

	return;
}

void hwInit(void)
{
	_delay_ms(100);

	sei();
	ds18x20SearchDevices();

	segmInit();							/* Indicator */
	I2CInit();							/* I2C bus */
	ds1307Init();						/* RTC */
	tunerInit();
	volumeInit();

	tunerInit();

	dsOnBus = ds18x20Process();			/* Try to find temperature sensor */

	return;
}

int main(void)
{
	hwInit();

	uint8_t dispMode = MODE_STANDBY;
	uint8_t editFM = 0;

	volumeLoadParams();
	defDispMode = eeprom_read_byte(eepromDispMode);
	brWork = eeprom_read_byte(eepromBrWork);
	brStby = eeprom_read_byte(eepromBrStby);
	if (brWork <= BR_MIN)
		brWork = BR_MIN;
	if (brWork >= BR_MAX)
		brWork = BR_MAX;

	int8_t encCnt = 0;
	uint8_t cmd = CMD_EMPTY;
	int8_t *time;

	while (1) {
		encCnt = getEncoder();
		cmd = getBtnCmd();
		time = readTime();
		if (dsOnBus)
			ds18x20Process();

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
					if (dsOnBus) {
						dispMode = MODE_TEMP;
						setDisplayTime(DISPLAY_TIME_TEMP);
					} else {
						defDispMode = MODE_FM_FREQ;
						dispMode = MODE_FM_FREQ;
						setDisplayTime(DISPLAY_TIME_FM_FREQ);
					}
				} else {
					dispMode = MODE_TIME;
					if (!dsOnBus)
						defDispMode = MODE_TIME;
					setDisplayTime(DISPLAY_TIME_TIME);
				}
			}
			break;
		case CMD_BTN_3:
			if (editFM) {
				tunerChangeFreq(10);
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
				tunerChangeFreq(-10);
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
				dispMode = MODE_FMTUNE_FREQ;
				setDisplayTime(DISPLAY_TIME_FMTUNE_FREQ);
				editFM = 1;
			}
			break;
		case CMD_BTN_4_LONG:
			tunerStoreStation();
			if (editFM) {
				dispMode = MODE_FMTUNE_CHAN;
				setDisplayTime(DISPLAY_TIME_FMTUNE_CHAN);
			} else {
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
			if (dsOnBus && (time[SEC] % 15 >= 13)) { /* Every 20 sec for 2 sec*/
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
			if (dsOnBus && (time[SEC] % 15 >= 13) && !getDisplayTime()) {
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

