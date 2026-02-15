#ifndef _PUTTEXT_H

#define _PUTTEXT_H

#include <stdint.h>

#define FONT_WIDTH 8
#define MAX_TEXT_COLUMNS (SCREEN_WIDTH/FONT_WIDTH)

void highlightText(uint16_t n, uint8_t highlightState);
void setScrollBitplanes(uint8_t b);
void setTextColors(uint16_t f, uint16_t b);
void setTextReverse(char reverse);
uint16_t getTextX(void);
uint16_t getTextY(void);
void setTextXY(uint16_t x, uint16_t y);
void setTextX(uint16_t x);
void setTextY(uint16_t x);
uint16_t putTextN(const char* s,uint16_t count);
#define putText(s) putTextN((s),0xFFFF)
void putChar(char c);
void setFont(uint8_t* data, uint16_t height);
uint16_t getFontHeight(void);
uint16_t getFontWidth(void);
uint16_t getTextRows(void);
uint16_t getTextColumns(void);
void setFontSystem(uint8_t bold);
/* leftX and rightX must be divisible by 4 and their difference by 8 */
void scrollUp(uint16_t lines, uint16_t leftX, uint16_t topY, uint16_t rightX, uint16_t bottomY, uint16_t fillMode, uint8_t bitplanes);
void scrollDown(uint16_t lines, uint16_t leftX, uint16_t topY, uint16_t rightX, uint16_t bottomY, uint16_t fillMode, uint8_t bitplanes);
void getTextWindow(uint16_t* topLeftXP,uint16_t* topLeftYP,uint16_t* bottomRightXP,uint16_t* bottomRightYP);
void setTextWindow(uint16_t topLeftX,uint16_t topLeftY,int16_t bottomRightX,int16_t bottomRightY);
void scrollTextUp(uint16_t rows);
void scrollTextDown(uint16_t rows);
void setScrollMode(uint8_t active);
uint16_t getTextForeground(void);
uint16_t getTextBackground(void);
extern uint8_t font8x14[];

#endif