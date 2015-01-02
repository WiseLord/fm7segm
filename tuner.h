#ifndef TUNER_H
#define TUNER_H

#include <inttypes.h>

/* Tuner type selection */
#if !defined(TEA5767) && !defined(RDA5807)
#define TEA5767
#endif

#if defined(TEA5767)
#include "tuner/tea5767.h"
#elif defined(RDA5807)
#include "tuner/rda5807.h"
#endif

#define FM_COUNT			64

#define SEARCH_DOWN			0
#define SEARCH_UP			1

#define FM_MONO				1
#define FM_STEREO			0

#define FM_STEP				10

#define FM_FREQ_MIN			8750
#define FM_FREQ_MAX			10800

#define eepromFMFreq		((uint16_t*)0x04)
#define eepromStations		((uint16_t*)0x10)

void tunerInit();

void tunerSetFreq(uint16_t freq);
uint16_t tunerGetFreq();

void tunerIncFreq(uint8_t mult);
void tunerDecFreq(uint8_t mult);

void tunerReadStatus();
uint16_t tunerGetFreq();
void tunerSwitchMono();
uint8_t tunerStereo();
uint8_t tunerLevel();

void tunerChangeFreq(int8_t mult);

uint8_t stationNum(void);
void scanStoredFreq(uint8_t direction);
void loadStation(uint8_t num);
void storeStation(void);

void loadTunerParams(void);
void saveTunerParams(void);

#endif /* TUNER_H */
