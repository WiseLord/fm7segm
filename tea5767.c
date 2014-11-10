#include "tea5767.h"

#include <avr/eeprom.h>
#include "i2c.h"

static uint8_t bufFM[5];
static uint16_t freqFM;

static void tea5767WriteI2C(uint8_t *buf)
{
	uint8_t i;

	I2CStart(TEA5767_ADDR);
	for (i = 0; i < 5; i++) {
		I2CWriteByte(buf[i]);
	}
	I2CStop();

	return;
}

void tea5767SetFreq(uint16_t freq)
{
	uint16_t div;

	if (freq > FM_FREQ_MAX)
		freq = FM_FREQ_MIN;
	if (freq < FM_FREQ_MIN)
		freq = FM_FREQ_MAX;

	freqFM = freq;

	div = ((uint32_t)freq * 10000 + 225000) / 8192;

	bufFM[0] = (div >> 8) & 0x3F;
	bufFM[1] = div & 0xff;
	bufFM[2] = TEA5767_HLSI | TEA5767_MS;
	bufFM[3] = TEA5767_HCC | TEA5767_SNC | TEA5767_SMUTE| TEA5767_XTAL;
	bufFM[4] = 0;

	tea5767WriteI2C(bufFM);

	return;
}

uint16_t tea5767GetFreq()
{
	return freqFM;
}

void tea5767IncFreq(void)
{
	tea5767SetFreq(freqFM + FM_STEP);

	return;
}

void tea5767DecFreq(void)
{
	tea5767SetFreq(freqFM - FM_STEP);

	return;
}

/* Find station number (1..64) in EEPROM */
uint8_t tea5767StNum(void)
{
	uint8_t i;

	uint16_t freq = tea5767GetFreq();

	for (i = 0; i < FM_COUNT; i++)
		if (eeprom_read_word(eepromStations + i) == freq)
			return i + 1;

	return 0;
}

/* Find nearest next/prev stored station */
void tea5767ScanStoredFreq(uint8_t direction)
{
	uint8_t i;
	uint16_t freqCell;
	uint16_t freqFound = freqFM;

	for (i = 0; i < FM_COUNT; i++) {
		freqCell = eeprom_read_word(eepromStations + i);
		if (freqCell != 0xFFFF) {
			if (direction) {
				if (freqCell > freqFM) {
					freqFound = freqCell;
					break;
				}
			} else {
				if (freqCell < freqFM) {
					freqFound = freqCell;
				} else {
					break;
				}
			}
		}
	}

	tea5767SetFreq(freqFound);

	return;
}

/* Load station by number */
void tea5767LoadStation(uint8_t num)
{
	uint16_t freqCell = eeprom_read_word(eepromStations + num);

	if (freqCell != 0xFFFF)
		tea5767SetFreq(freqCell);

	return;
}

/* Save/delete station from eeprom */
void tea5767StoreStation(void)
{
	uint8_t i, j;
	uint16_t freqCell;
	uint16_t freq;

	freq = freqFM;

	for (i = 0; i < FM_COUNT; i++) {
		freqCell = eeprom_read_word(eepromStations + i);
		if (freqCell < freq)
			continue;
		if (freqCell == freq) {
			for (j = i; j < FM_COUNT; j++) {
				if (j == FM_COUNT - 1)
					freqCell = 0xFFFF;
				else
					freqCell = eeprom_read_word(eepromStations + j + 1);
				eeprom_update_word(eepromStations + j, freqCell);
			}
			break;
		} else {
			for (j = i; j < FM_COUNT; j++) {
				freqCell = eeprom_read_word(eepromStations + j);
				eeprom_update_word(eepromStations + j, freq);
				freq = freqCell;
			}
			break;
		}
	}

	return;
}

void tea5767LoadParams(void)
{
	freqFM = eeprom_read_word(eepromFMFreq);
	tea5767SetFreq(freqFM);

	return;
}

void tea5767SaveParams(void)
{
	eeprom_update_word(eepromFMFreq, freqFM);

	return;
}
