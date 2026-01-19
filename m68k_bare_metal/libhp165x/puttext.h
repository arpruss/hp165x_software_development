#ifndef _PUTTEXT_H

#define _PUTTEXT_H

#include <stdint.h>

#define TEXT_ROWS (SCREEN_HEIGHT/14)
#define TEXT_COLUMNS (SCREEN_WIDTH/8)

void setTextBlackOnWhite(char value);
uint16_t getTextX(void);
uint16_t getTextY(void);
void setTextXY(uint16_t x, uint16_t y);
void setTextX(uint16_t x);
void setTextY(uint16_t x);
void putText(char* s);
void putchar_(int c);

#endif