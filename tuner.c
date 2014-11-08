#include "tuner.h"

#include <avr/eeprom.h>
#include "eeprom.h"

uint8_t bufFM[5];

static uint16_t freqFM;
static uint8_t monoFM;
static uint8_t stepFM;

void tunerInit(void)
{
	tea5767Init();

	return;
}

void tunerSetFreq(uint16_t freq)
{
	if (freq > FM_FREQ_MAX)
		freq = FM_FREQ_MIN;
	if (freq < FM_FREQ_MIN)
		freq = FM_FREQ_MAX;

	tea5767SetFreq(freq, monoFM);

	freqFM = freq;

	return;
}

uint16_t tunerGetFreq()
{
	return freqFM;
}

void tunerIncFreq(void)
{
	tunerSetFreq(tunerGetFreq() + stepFM);

	return;
}

void tunerDecFreq(void)
{
	tunerSetFreq(tunerGetFreq() - stepFM);

	return;
}

/* Find station number (1..64) in EEPROM */
uint8_t stationNum(uint16_t freq)
{
	uint8_t i;

	for (i = 0; i < FM_COUNT; i++)
		if (eeprom_read_word(eepromStations + i) == freq)
			return i + 1;

	return 0;
}

/* Find nearest next/prev stored station */
void scanStoredFreq(uint8_t direction)
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

	tunerSetFreq(freqFound);

	return;
}

/* Load station by number */
void loadStation(uint8_t num)
{
	uint16_t freqCell = eeprom_read_word(eepromStations + num);

	if (freqCell != 0xFFFF)
		tunerSetFreq(freqCell);

	return;
}

/* Save/delete station from eeprom */
void storeStation(void)
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

void loadTunerParams(void)
{
	freqFM = eeprom_read_word(eepromFMFreq);
	monoFM = eeprom_read_byte(eepromFMMono);
	stepFM = eeprom_read_byte(eepromFMStep);

	tunerSetFreq(freqFM);

	return;
}

void saveTunerParams(void)
{
	eeprom_update_word(eepromFMFreq, freqFM);
	eeprom_update_byte(eepromFMMono, monoFM);

	return;
}
