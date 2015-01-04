#include "ds18x20.h"

#include "segm.h"

#include <util/delay.h>
#include <avr/interrupt.h>

static volatile int16_t tempData = 0;

static uint8_t ds18x20IsOnBus(void)
{
	uint8_t ret;

	DDR(TEMP_SENSOR) |= TEMP_SENSOR_LINE;				/* Pin as output (0) */
	_delay_us(480);										/* Reset */
	DDR(TEMP_SENSOR) &= ~TEMP_SENSOR_LINE;				/* Pin as input (1) */
	_delay_us(60);										/* Wait for response */

	ret = !(PIN(TEMP_SENSOR) & TEMP_SENSOR_LINE);

	_delay_us(420);

	return ret;
}

static void ds18x20SendByte(uint8_t byte)
{
	uint8_t i;

	for (i = 0; i < 8; i++) {
		DDR(TEMP_SENSOR) |= TEMP_SENSOR_LINE;			/* Pin as output (0) */
		if (byte & (1<<i)) {							/* If bit == 1*/
			_delay_us(1);
			DDR(TEMP_SENSOR) &= ~TEMP_SENSOR_LINE;		/* Pin as input (1) */
			_delay_us(60);
		} else {
			_delay_us(60);
			DDR(TEMP_SENSOR) &= ~TEMP_SENSOR_LINE;		/* Pin as input (1) */
			_delay_us(1);
		}

	}

	return;
}

static int16_t ds18x20GetRawTemp(void)
{
	uint8_t i;
	int16_t ret = 0;

	for (i = 0; i < 16; i++) {

		DDR(TEMP_SENSOR) |= TEMP_SENSOR_LINE;			/* pin as output */
		_delay_us(2);									/* Strob */
		DDR(TEMP_SENSOR) &= ~TEMP_SENSOR_LINE;			/* pin as input */
		_delay_us(2);

		if (PIN(TEMP_SENSOR) & TEMP_SENSOR_LINE)
			ret |= (1<<i);

		_delay_us(60);
	}

	return ret;
}

uint8_t ds18x20Init(void)
{
	PORT(TEMP_SENSOR) &= ~TEMP_SENSOR_LINE;				/* Set active 0 */

	/* Convert temperature */
	if (ds18x20IsOnBus()) {
		ds18x20SendByte(DS18X20_CMD_SKIP_ROM);
		ds18x20SendByte(DS18X20_CMD_CONVERT);
		setTempTimer(1000);

		return 1;
	}

	return 0;
}

void ds18x20Process(void)
{
	if (getTempTimer() == 0) {

		if (ds18x20IsOnBus()) {
			ds18x20SendByte(DS18X20_CMD_SKIP_ROM);
			ds18x20SendByte(DS18X20_CMD_READ_SCRATCH);
			cli();
			tempData = ds18x20GetRawTemp() * 5 / 8;
			sei();
			/* Convert temperature */
			if (ds18x20IsOnBus()) {
				ds18x20SendByte(DS18X20_CMD_SKIP_ROM);
				ds18x20SendByte(DS18X20_CMD_CONVERT);
			}

			setTempTimer(15000);
		}
	}

	return;
}

int16_t ds18x20GetTemp(void)
{
	return tempData;
}
