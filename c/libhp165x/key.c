#include <stdlib.h>
#include <string.h>
#include "hp165x.h"

static uint8_t click = 1;
static uint16_t repeatDelay = 20;
static uint16_t repeatRate = 8;
static uint8_t dialHorizontal = 1;

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

struct key_translate {
    uint16_t key;
    uint8_t  character;
};

static const struct key_translate serialKeyToASCII[] = {
    { KEYBOARD_KP_DOT, '.' },
    { KEYBOARD_KP_ASTERISK, '*' },
    { KEYBOARD_KP_PLUS, '+' },
    { KEYBOARD_KP_SLASH, '/' },
    { KEYBOARD_KP_ENTER, '\n' },
    { KEYBOARD_KP_MINUS, '-' },
    { KEYBOARD_KP_0, '0' },
    { KEYBOARD_KP_1, '1' },
    { KEYBOARD_KP_2, '2' },
    { KEYBOARD_KP_3, '3' },
    { KEYBOARD_KP_4, '4' },
    { KEYBOARD_KP_5, '5' },
    { KEYBOARD_KP_6, '6' },
    { KEYBOARD_KP_7, '7' },
    { KEYBOARD_KP_8, '8' },
    { KEYBOARD_KP_9, '9' },
    { 0, 0 } };

static const struct key_translate hpKeyToASCII[] = {
	{ HP_KEY_0, '0' },
	{ HP_KEY_1, '1' },
	{ HP_KEY_2, '2' },
	{ HP_KEY_3, '3' },
	{ HP_KEY_4, '4' },
	{ HP_KEY_5, '5' },
	{ HP_KEY_6, '6' },
	{ HP_KEY_7, '7' },
	{ HP_KEY_8, '8' },
	{ HP_KEY_9, '9' },
	{ HP_KEY_A, 'A' },
	{ HP_KEY_B, 'B' },
	{ HP_KEY_C, 'C' },
	{ HP_KEY_D, 'D' },
	{ HP_KEY_E, 'E' },
	{ HP_KEY_F, 'F' },
	{ HP_KEY_SELECT, '\n' },
	{ HP_KEY_CLEAR, '\b' },
	{ HP_KEY_DONT_CARE, ' ' },
	{ HP_KEY_RUN, 'R' },
	{ HP_KEY_TRACE, 'T' },
	{ HP_KEY_IO, 'I' },
	{ HP_KEY_CHS, '-' },
	{ HP_KEY_DECIMAL, '.' },
	{ HP_KEY_STOP, KEYBOARD_BREAK }, // ctrl-c
    { 0, 0 }
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
				return HP_KEY_TURN_CCW;
			else
				return HP_KEY_TURN_CW;
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
			return HP_KEY_TURN_CCW;
		else
			return HP_KEY_TURN_CW;
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

static uint8_t translateKey(const struct key_translate* dict, uint16_t k) {
    unsigned short i = 0;
    while (dict[i].key) {
        if (dict[i].key == k)
            return dict[i].character;
        i++;
    }
    return 0;
}

char parseKey(uint16_t k) {
	if (k == HP_KEY_TURN_CCW) {
		return dialHorizontal ? KEYBOARD_LEFT : KEYBOARD_UP;
	}
	else if (k == HP_KEY_TURN_CW) {
		return dialHorizontal ? KEYBOARD_RIGHT : KEYBOARD_DOWN;
	}
    return translateKey(hpKeyToASCII, k);
}

char parseSerialKey(uint8_t k) {
    char c = translateKey(serialKeyToASCII, k);
    if (c == 0)
        return k;
    else
        return c;
}

void setDialHorizontal(uint8_t h) {
	dialHorizontal = h;
}