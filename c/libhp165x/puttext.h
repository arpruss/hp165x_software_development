#ifndef _PUTTEXT_H

#define _PUTTEXT_H

#include <stdint.h>

#define FONT_WIDTH 8
#define TEXT_COLUMNS (SCREEN_WIDTH/FONT_WIDTH)

void highlightText(uint16_t x, uint16_t y, uint16_t n, uint8_t highlightState);
void setTextColors(uint16_t f, uint16_t b);
void setTextReverse(char reverse);
uint16_t getTextX(void);
uint16_t getTextY(void);
void setTextXY(uint16_t x, uint16_t y);
void setTextX(uint16_t x);
void setTextY(uint16_t x);
void putText(char* s);
void putchar_(int c);
#define putChar putchar_
void setFont(uint8_t* data, uint16_t height, uint16_t lineHeight);
uint16_t getFontLineHeight(void);
uint16_t getFontWidth(void);
uint16_t getTextRows(void);
uint16_t getTextColumns(void);
//void setFontSystem(uint8_t bold);



#endif