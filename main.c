#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "segm.h"
#include "i2c.h"
#include "rtc.h"
#include "tuner/tuner.h"
#include "volume.h"
#include "ds18x20.h"
#include "eeprom.h"

static uint8_t defDispMode = MODE_TIME;
static int8_t brWork, brStby;

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
	tunerSetFreq();

	return;
}

/* Handle entering standby mode */
static void powerOff(void)
{
	rtc.etm = RTC_NOEDIT;
	setBrightness(brStby);

	muteVolume();
	volumeSaveParams();
	tunerPowerOff();

	eeprom_update_byte((uint8_t*)EEPROM_DISPLAY, defDispMode);
	eeprom_update_byte((uint8_t*)EEPROM_BR_WORK, brWork);

	eeprom_update_word((uint16_t*)EEPROM_FM_FREQ, tuner.freq);
	eeprom_update_byte((uint8_t*)EEPROM_FM_MONO, tuner.mono);

	return;
}

void hwInit(void)
{
	_delay_ms(100);

	/* Attempt to find temperature sensors and make first measurement */
	ds18x20SearchDevices();
	if (getDevCount())
		ds18x20Process();

	sei();

	segmInit();							/* Indicator */
	I2CInit();							/* I2C bus */
	rtc.etm = RTC_NOEDIT;
	tunerInit();
	volumeInit();

	defDispMode = eeprom_read_byte((uint8_t*)EEPROM_DISPLAY);
	brWork = eeprom_read_byte((uint8_t*)EEPROM_BR_WORK);
	brStby = eeprom_read_byte((uint8_t*)EEPROM_BR_STBY);
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

	setRtcTimer(RTC_POLL_TIME);

	while (1) {
		encCnt = getEncoder();
		cmd = getBtnCmd();

		/* Poll RTC for time */
		if (getRtcTimer() == 0) {
			rtcReadTime();
			setRtcTimer(RTC_POLL_TIME);
		}

		if (getDevCount()) {
			// Get all temperatures
			if (getTempTimer() == 0)
				ds18x20GetAllTemps();
			// Run measurement 4 times per minute
			if (rtc.sec % 15 == 14)
				ds18x20Process();
		} else {
			// Continue searching devices
			ds18x20SearchDevices();
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
			if (dispMode == MODE_TIME_EDIT) {
				if (rtc.etm == RTC_YEAR) {
					rtc.etm = RTC_NOEDIT;
					dispMode = MODE_TIME;
					setDisplayTime(DISPLAY_TIME_TIME);
					break;
				}
				rtcNextEditParam();
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
					if (getDevCount()) {
						ds18x20Process();
						dispMode = MODE_TEMP;
						setDisplayTime(DISPLAY_TIME_TEMP);
					} else {
						defDispMode = MODE_FM_FREQ;
						dispMode = MODE_FM_FREQ;
						setDisplayTime(DISPLAY_TIME_FM_FREQ);
					}
				} else {
					dispMode = MODE_TIME;
					if (!getDevCount())
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
				case MODE_TIME_EDIT:
					rtcChangeTime(-1);
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
				case MODE_TIME_EDIT:
					rtcChangeTime(+1);
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
			case MODE_TIME_EDIT:
				dispMode = defDispMode;
				rtc.etm = RTC_NOEDIT;
				break;
			default:
				dispMode = MODE_TIME_EDIT;
				rtc.etm = RTC_HOUR;
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
				case MODE_TIME_EDIT:
					rtcChangeTime(-10);
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
				case MODE_TIME_EDIT:
					rtcChangeTime(+10);
					setDisplayTime(DISPLAY_TIME_EDITTIME);
					break;
				default:
					tunerSetMono(!tuner.mono);
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
			case MODE_TIME_EDIT:
				rtcChangeTime(encCnt);
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
					rtc.etm = RTC_NOEDIT;
				}
				break;
			}
		}

		/* Show things */
		switch (dispMode) {
		case MODE_STANDBY:
		case MODE_TIME:
			segmTimeOrTemp();
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
		case MODE_TIME_EDIT:
			segmTime();
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

