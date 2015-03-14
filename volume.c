#include "volume.h"

#include "avr/interrupt.h"
#include "avr/eeprom.h"
#include "eeprom.h"

static volatile int8_t vol;

#if !defined(RDA5807)
static const int8_t volPwms[] = {
	 0, 11, 13, 15, 17, 19, 21,
	23, 24, 25, 26, 27, 28, 29,
	30, 31, 32, 33, 34, 35, 36
};
#endif

void volumeInit(void)
{
	DDR(VOLUME) |= VOLUME_LINE;
	muteVolume();

	TCCR0 |= (0<<CS02) | (0<<CS01) | (1<<CS00);		/* Set timer prescaller to 1 for PWM */

	return;
}

#if !defined(RDA5807)
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
#endif

void setVolume(int8_t value)
{
	if (value > VOL_MAX)
		value = VOL_MAX;
	if (value < VOL_MIN)
		value = VOL_MIN;

	vol = value;

#if defined(RDA5807)
	rda5807SetVolume(value);
#endif

	return;
}

int8_t getVolume(void)
{
	return vol;
}

void muteVolume(void)
{
#if defined(RDA5807)
	rda5807MuteVolume();
	PORT(VOLUME) |= VOLUME_LINE;					/* Pull amplifier input to ground */
#else
	TIMSK &= ~(1<<TOIE0);							/* Disable timer overflow interrupt */
	PORT(VOLUME) &= ~VOLUME_LINE;
#endif

	return;
}

void unmuteVolume(void)
{
#if defined(RDA5807)
	PORT(VOLUME) &= ~VOLUME_LINE;					/* Release amplifier input */
	rda5807UnmuteVolume();
#else
	TIMSK |= (1<<TOIE0);							/* Enable timer overflow interrupt */
#endif

	return;
}

void volumeLoadParams(void)
{
	vol = eeprom_read_byte(eepromVolume);
#if defined(RDA5807)
	rda5807SetVolume(vol);
#endif

	return;
}

void volumeSaveParams(void)
{
	eeprom_update_byte(eepromVolume, vol);

	return;
}
