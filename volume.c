#include "volume.h"

#include "avr/eeprom.h"
#include "rda5807.h"

static volatile int8_t vol;

void setVolume(int8_t value)
{
	if (value > VOL_MAX)
		value = VOL_MAX;
	if (value < VOL_MIN)
		value = VOL_MIN;

	vol = value;

	rda5807SetVolume(value);

	return;
}

int8_t getVolume(void)
{
	return vol;
}

void volumeLoadParams(void)
{
	vol = eeprom_read_byte(eepromVolume);
	rda5807SetVolume(vol);


	return;
}

void volumeSaveParams(void)
{
	eeprom_update_byte(eepromVolume, vol);

	return;
}
