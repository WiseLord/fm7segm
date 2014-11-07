#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "segm.h"

#include "pins.h"

void hwInit(void)
{
	segmInit();

	sei();
	return;
}

int main(void)
{
	hwInit();

	uint8_t i;

	while (1) {
		for (i = BR_MIN; i <= BR_MAX; i++) {
			setBrightness(i);
			_delay_ms(500);
		}
	}

	return 0;
}

