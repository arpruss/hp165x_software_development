#include <stdlib.h>
#include <string.h>
#include "hp165x.h"

static uint8_t click = 1;
static uint16_t repeatDelay = 20;
static uint16_t repeatRate = 8;

void setKeyClick(uint8_t _click) {
	click = _click;
}

void setKeyRepeat(uint16_t delay, uint16_t rate) {
	if (delay == 0 || rate == 0) {
		repeatDelay = 0;
		repeatRate = 0;
	}
	else {
		repeatDelay = delay;
		repeatRate = rate;
	}
}

static const struct {
	uint16_t key;
	char	 ascii;
} keyToASCII[] = {
	{ KEY_0, '0' },
	{ KEY_1, '1' },
	{ KEY_2, '2' },
	{ KEY_3, '3' },
	{ KEY_4, '4' },
	{ KEY_5, '5' },
	{ KEY_6, '6' },
	{ KEY_7, '7' },
	{ KEY_8, '8' },
	{ KEY_9, '9' },
	{ KEY_A, 'A' },
	{ KEY_B, 'B' },
	{ KEY_C, 'C' },
	{ KEY_D, 'D' },
	{ KEY_E, 'E' },
	{ KEY_F, 'F' },
	{ KEY_SELECT, '\n' },
	{ KEY_CLEAR, '\b' },
	{ KEY_DONT_CARE, ' ' },
	{ KEY_RUN, 'R' },
	{ KEY_TRACE, 'T' },
	{ KEY_IO, 'I' },
	{ KEY_CHS, '-' },
	{ KEY_DECIMAL, '.' },
	{ KEY_STOP, KEYBOARD_BREAK }, // ctrl-c
	{ KEY_TURN_CW, KEYBOARD_RIGHT },
	{ KEY_TURN_CCW, KEYBOARD_LEFT },
};

static uint16_t lastKey = 0;
static uint16_t lastKeyTime = 0;
static uint8_t initialized = 0;

uint16_t getKey(char wait) {
	if (! initialized) {
		*LAST_KEY = 0;
		initialized = 1;
	}
	
	do {	
		uint16_t k = *LAST_KEY;
		
		if (k == 0xFFFF) {
			lastKey = 0;
			if (*KEY_HOLD_TIME < 10)
				continue;
			*KEY_HOLD_TIME = 0;
			uint32_t spinnerState = *(volatile uint32_t*)0x98070C;
			if (click) {
				*BEEPER = BEEPER_ON;
				*BEEPER = BEEPER_OFF;
			}
			if (spinnerState & 0x80000000)
				return KEY_TURN_CCW;
			else
				return KEY_TURN_CW;
		}
		else if (k == 0) {
			if (lastKey == 0)
				continue;
			
			k = *CURRENT_KEY;
			
			if (k != lastKey || repeatRate == 0) {
				lastKey = 0;
				lastKeyTime = 0;
			}
			else {
				uint16_t t = *LAST_KEY_DURATION;
				
				if (lastKeyTime == 0) {
					if (t >= repeatDelay) {
						lastKeyTime = t;
						return lastKey;
					}
				}
				else {
					if (t >= lastKeyTime + repeatRate) {
						lastKeyTime = t;
						return lastKey;
					}
				}
			}			
		}
		else {
			*LAST_KEY = 0;
			if (click) {
				*BEEPER = BEEPER_ON;
				*BEEPER = BEEPER_OFF;
			}
			lastKey = k;
			return k;
		}
	} while(wait);
	return 0;
}

uint16_t peekKey(void) {
	if (! initialized) {
		*LAST_KEY = 0;
		initialized = 1;
	}
	
	uint16_t k = *LAST_KEY;
	
	if (k == 0xFFFF) {
		lastKey = 0;
		if (*KEY_HOLD_TIME < 10)
			return 0;
		uint32_t spinnerState = *(volatile uint32_t*)0x98070C;
		if (spinnerState & 0x80000000)
			return KEY_TURN_CCW;
		else
			return KEY_TURN_CW;
	}
	else if (k == 0) {
		if (lastKey == 0)
			return 0;
		
		k = *CURRENT_KEY;
		
		if (k != lastKey || repeatRate == 0) {
			return 0;
		}
		else {
			uint16_t t = *LAST_KEY_DURATION;
			
			if (lastKeyTime == 0) {
				if (t >= repeatDelay) {
					return lastKey;
				}
			}
			else {
				if (t >= lastKeyTime + repeatRate) {
					return lastKey;
				}
			}
			return 0;
		}			
	}
	else {
		return k;
	}
}

char parseKey(uint16_t k) {
	for (unsigned i=0; i<sizeof(keyToASCII)/sizeof(*keyToASCII); i++) {
		if (k == keyToASCII[i].key)
			return keyToASCII[i].ascii;
	}
	return 0;
}

