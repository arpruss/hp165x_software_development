#include <hp165x.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/runtime.h"

static char inputBuffer[16] = "";
static int16_t bufferPosition = -1;
// Receive single character, blocking until one is available.
int mp_hal_stdin_rx_chr(void) {
	do {
		if (bufferPosition >= 0 && inputBuffer[bufferPosition]) {
			return (uint8_t)inputBuffer[bufferPosition++];
		}

		updateTextCursor(1);
		char c = getch();
		showTextCursor(0);
		
		switch(c) {
			case KEYBOARD_LEFT:
				strcpy(inputBuffer, "[D");
				bufferPosition = 0;
				return 27;
			case KEYBOARD_RIGHT:
				strcpy(inputBuffer, "[C");
				bufferPosition = 0;
				return 27;
			case KEYBOARD_UP:
				strcpy(inputBuffer, "[A");
				bufferPosition = 0;
				return 27;
			case KEYBOARD_DOWN:
				strcpy(inputBuffer, "[B");
				bufferPosition = 0;
				return 27;
			default:
				if (0 < c && c < 128)
					return c;
				else {
					sprintf(inputBuffer,"<%x>",(uint8_t)c);
					bufferPosition = 0;
				}
		}
	} while(1);
}

extern uint16_t _ticksPerSecond;
mp_uint_t mp_hal_ticks_ms(void) {
	return getVBLCounter() * 1000 / _ticksPerSecond;
}

mp_uint_t mp_hal_ticks_us(void) {
	return getVBLCounter() * 1000000uL / _ticksPerSecond;
}

mp_uint_t mp_hal_ticks_cpu(void) {
	return getVBLCounter();
}

void mp_hal_delay_ms(mp_uint_t ms) {
	uint32_t ticks = ms * _ticksPerSecond / 1000;
	uint32_t t0 = getVBLCounter();
	while (getVBLCounter()-t0 < ticks) {
        mp_handle_pending(true); 
    }
}

void mp_hal_delay_us(mp_uint_t us) {
	uint32_t ticks = us * _ticksPerSecond / 1000000;
	uint32_t t0 = getVBLCounter();
	while (getVBLCounter()-t0 < ticks) {
        mp_handle_pending(true); 
    }
}

// Send the string of given length.
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
	while(len) {
		if (*str == '\b') {
			int16_t x = getTextX();
			if (x > 0) 
				setTextX(x-1);
		}
		else if (*str == 13) {
			setTextX(0);
		}
		else if (*str == 27) {
#if 1
			if (len<=2)
				return;
			if (str[1] != '[')
				return;
			len -= 2;
			str += 2;
			int16_t x;
			switch(*str) {
				case '\n':
					x = getTextX();
					putChar('\n');
					setTextX(x);
					break;
				case 'K':
					x = getTextX();
					setScrollMode(0);
					for (int16_t i=getTextColumns(); i>x; i--)
						putChar(' ');
					setScrollMode(1);
					setTextX(x);
					break;
				case 'D':
					x = getTextX();
					if (x > 0)
						setTextX(x);
					break;
				default: 
					if (isdigit(*str)) {
						uint16_t value = 0;			
						do {
							value = value * 10 + *str - '0';
							str++;
							len--;
						} while (len && isdigit(*str));
						switch(*str++) {
							case 'D': 
								x = getTextX();
								if (x < value)
									x = 0;
								else
									x -= value;
								setTextX(x);
								break;
						}
						continue;
					}
					break;
			}
#endif			
		}
		else {
			putChar(*str);
		}
		str++;
		len--;
	}
}

