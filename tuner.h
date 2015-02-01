#ifndef TUNER_H
#define TUNER_H

#include <inttypes.h>
/* Tuner type selection */

#if !defined(RDA5807) && !defined(TEA5767) && !defined(TUX032)
#define RDA5807
#endif

#if defined(RDA5807)
#include "tuner/rda5807.h"
#elif defined(TEA5767)
#include "tuner/tea5767.h"
#elif defined(TUX032)
#include "tuner/tux032.h"
#endif

#define FM_COUNT			64

#define SEARCH_DOWN			0
#define SEARCH_UP			1

#define FM_MONO				1
#define FM_STEREO			0

#if defined(RDA5807)
#define FM_FREQ_MIN			RDA5807_FREQ_MIN
#define FM_FREQ_MAX			RDA5807_FREQ_MAX
#elif defined(TEA5767)
#define FM_FREQ_MIN			TEA5767_FREQ_MIN
#define FM_FREQ_MAX			TEA5767_FREQ_MAX
#elif defined(TUX032)
#define FM_FREQ_MIN			TUX032_FREQ_MIN
#define FM_FREQ_MAX			TUX032_FREQ_MAX
#endif

#define FM_STEP				10

#define eepromFMFreq		((uint16_t*)0x08)
#define eepromStations		((uint16_t*)0x10)
#define eepromFMMono		((void*)0x0C)
#define eepromFMStep		((void*)0x0D)
#define eepromFMCtrl		((void*)0x0E)

void tunerInit(void);

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
