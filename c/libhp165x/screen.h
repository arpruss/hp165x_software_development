#ifndef _SCREEN_H
#define _SCREEN_H

#include <hp165x.h>

#define SCREEN_WRITE_VALUE(active,value) = ((0xF^(uint8_t)(active)) | (uint16_t)(0xF^(uint8_t)(value))<<8)
// reading won't be very useful in these modes
#define WRITE_BLACK   0xF00 // clears all other planes than data, draws black on data
#define WRITE_WHITE   0xE00 // clears all other planes than data, draws white on data
#define WRITE_GRAY    0xC00 // clears attr & overlay planes, then draws gray using data and overlay-data

/* useful for overlay windows */
#define WRITE_OVERLAY_GRAY       0b110000000001
#define WRITE_OVERLAY_BLACK      0b101000000001
#define WRITE_OVERLAY_WHITE      0b100000000001
#define WRITE_OVERLAY_ERASE 	 0b111000000001


// the following 4 modes allow reading
#define WRITE_SET_ATTR   0x007  // leaves data unchanged
#define WRITE_CLEAR_ATTR 0x807  // leaves data unchanged
#define WRITE_SET_DATA   0x00E  // leaves data unchanged
#define WRITE_CLEAR_DATA 0x10E  // leaves data unchanged
#define WRITE_NOP        0x00F

extern uint16_t screenHeight;

#define SCREEN ((volatile uint16_t*)0x600000)
#define SCREEN_HEIGHT screenHeight

#define ROM_SCREEN_WIDTH  592
#define ROM_SCREEN_HEIGHT 384

#ifndef DEFAULT_SCREEN_HEIGHT
#define DEFAULT_SCREEN_HEIGHT ROM_SCREEN_HEIGHT
#endif
#define MAX_SCREEN_HEIGHT 392
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH ROM_SCREEN_WIDTH
#endif
#define SCREEN_WIDTH_BYTES  (SCREEN_WIDTH/2)
#define SCREEN_WIDTH_WORDS  (SCREEN_WIDTH/4)
#define SCREEN_WIDTH_DWORDS (SCREEN_WIDTH/8)

void drawPixel(uint16_t x, uint16_t y);
void fillScreen(void);
void drawBlack(void);
void drawWhite(void);
uint16_t getScreenWidth(void);
uint16_t getScreenHeight(void);
void setScreenHeight(uint16_t height);

void setTextMode(uint32_t mode);
void drawText(const char* p);
void setCoordinates(int32_t x, int32_t y);
void fillRectangle(uint16_t topLeftX, uint16_t topLeftY, uint16_t bottomRightX, uint16_t bottomRightY);
void drawVerticalLine(uint16_t x, uint16_t y1, uint16_t y2);
void drawHorizontalLine(uint16_t x1, uint16_t y, uint16_t x2);
void frameRectangle(uint16_t topLeftX, uint16_t topLeftY, uint16_t bottomRightX, uint16_t bottomRightY, uint16_t thickness);

#endif