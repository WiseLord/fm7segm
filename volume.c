#include "volume.h"

#include "avr/interrupt.h"
#include "avr/eeprom.h"
#include "eeprom.h"

static volatile int8_t vol;

static const int8_t volPwms[] = {
	 0, 11, 13, 15, 17, 19, 21,
	23, 24, 25, 26, 27, 28, 29,
	30, 31, 32, 33, 34, 35, 36
};

void volumeInit(void)
{
	DDR(VOLUME) |= VOLUME_LINE;
	TCCR0 |= (0<<CS02) | (0<<CS01) | (1<<CS00);		/* Set timer prescaller to 1 for PWM */

	setVolume(eeprom_read_byte(eepromVolume));

	muteVolume();

	return;
}

ISR (TIMER0_OVF_vect)
{
	static uint8_t volPwm;

	volPwm++;
	if (volPwm > volPwms[VOL_MAX])
		volPwm = 0;

	if (volPwm == 0)
		PORT(VOLUME) |= VOLUME_LINE;
	if (volPwm > volPwms[vol])
		PORT(VOLUME) &= ~VOLUME_LINE;

	return;
}

void setVolume(int8_t value)
{
	if (value > VOL_MAX)
		value = VOL_MAX;
	if (value < VOL_MIN)
		value = VOL_MIN;

	vol = value;

	if (tunerGetType() == TUNER_RDA5807 ||
	    tunerGetType() == TUNER_RDA5802)
		rda580xSetVolume(value);

	return;
}

int8_t getVolume(void)
{
	return vol;
}

void muteVolume(void)
{
	tunerSetMute(1);

	if (tunerGetType() == TUNER_RDA5807 ||
	    tunerGetType() == TUNER_RDA5802) {
		PORT(VOLUME) |= VOLUME_LINE;					/* Pull amplifier input to ground */
	} else {
		TIMSK &= ~(1<<TOIE0);							/* Disable timer overflow interrupt */
		PORT(VOLUME) &= ~VOLUME_LINE;
	}

	return;
}

void unmuteVolume(void)
{
	tunerSetMute(0);

	if (tunerGetType() == TUNER_RDA5807 ||
	    tunerGetType() == TUNER_RDA5802) {
		PORT(VOLUME) &= ~VOLUME_LINE;					/* Release amplifier input */
	} else {
		TIMSK |= (1<<TOIE0);							/* Enable timer overflow interrupt */
	}

	return;
}

void volumeSaveParams(void)
{
	eeprom_update_byte(eepromVolume, vol);

	return;
}
