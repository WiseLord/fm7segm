#ifndef TEA5767_H
#define TEA5767_H

#include <inttypes.h>

#include "tea5767.h"

#define FM_COUNT				64

#define SEARCH_DOWN				0
#define SEARCH_UP				1

#define FM_STEP					10

#define FM_FREQ_MIN				8750
#define FM_FREQ_MAX				10800

#define TEA5767_ADDR			0b11000000

#define eepromFMFreq			((uint16_t*)0x04)
#define eepromStations			((uint16_t*)0x10)

/* Write mode TEA5767 register values */

/* 0 register */
#define TEA5767_MUTE			(1<<7)	/* Mute output */
#define TEA5767_SM				(1<<6)	/* Search mode */
/* PLL 13..8 bits */

/* 1 register */
/* PLL 7..0 bits */

/* 2 register */
#define TEA5767_SUD				(1<<7)	/* Search Up(1) / Down(0) */
#define TEA5767_SSL_LOW			(1<<5)	/* Search stop level = 5 */
#define TEA5767_SSL_MID			(2<<5)	/* Search stop level = 7 */
#define TEA5767_SSL_HIGH		(3<<5)	/* Search stop level = 10 */
#define TEA5767_HLSI			(1<<4)	/* High(1) / Low(0) Side Injection */
#define TEA5767_MS				(1<<3)	/* Mono to stereo */
#define TEA5767_MR				(1<<2)	/* Mute Right */
#define TEA5767_ML				(1<<1)	/* Mute Left */
#define TEA5767_SWP1			(1<<0)	/* Software programmable port 1 high */

/* 3 register */
#define TEA5767_SWP2			(1<<7)	/* Software programmable port 2 high */
#define TEA5767_STBY			(1<<6)	/* Standby */
#define TEA5767_BL				(1<<5)	/* Band limit: Japan(1) / Europe(0) */
#define TEA5767_XTAL			(1<<4)	/* Clock frequency 32768 (1) */
#define TEA5767_SMUTE			(1<<3)	/* Soft mute */
#define TEA5767_HCC				(1<<2)	/* High cut control */
#define TEA5767_SNC				(1<<1)	/* Stereo Noise Cancelling */
#define TEA5767_SI				(1<<0)	/* Search indicator */

/* 4 register */
#define TEA5767_PLLREF			(1<<7)	/* 6.5MHz reference frequency */
#define TEA5767_DTC				(1<<6)	/* De-emphasis 75us(1) / 50us(0) */
/* Not used 5..0 bits */

void tea5767SetFreq(uint16_t freq);
uint16_t tea5767GetFreq();

void tea5767IncFreq(uint8_t mult);
void tea5767DecFreq(uint8_t mult);

uint8_t tea5767StNum(void);
void tea5767ScanStoredFreq(uint8_t direction);
void tea5767LoadStation(uint8_t num);
void tea5767StoreStation(void);

void tea5767LoadParams(void);
void tea5767SaveParams(void);

#endif /* TEA5767_H */
