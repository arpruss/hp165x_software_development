#include <hp165x.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qrcodegen.h"
#include "base64.h"

bool qrcodegen_encodeBinary(uint8_t dataAndTemp[], size_t dataLen, uint8_t qrcode[],
	enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, bool boostEcl);

void cls() {
  *SCREEN_MEMORY_CONTROL = WRITE_WHITE;
  for (volatile uint16_t* p=SCREEN;p<SCREEN+SCREEN_WIDTH*SCREEN_HEIGHT/4;p++)
	  *p = 0x0F;
}

static void printQr(const uint8_t qrcode[]) {
	int size = qrcodegen_getSize(qrcode);
	int pixelHeight = SCREEN_HEIGHT / size;
	int pixelWidth = ADJUST_WIDTH(pixelHeight);
	int height = pixelHeight * size;
	int width = pixelWidth * size;
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	for (uint16_t y = 0; y < size ; y++) {
		for (uint16_t x = 0; x < size ; x++) {
			if (qrcodegen_getModule(qrcode, x, y)) {
				uint16_t x0 = x * pixelWidth + (SCREEN_WIDTH-width)/2;
				uint16_t y0 = y * pixelHeight + (SCREEN_HEIGHT-height)/2;
				volatile uint16_t* pos = SCREEN + y0 * (SCREEN_WIDTH / 4) + x0/4;
				volatile uint16_t* pos2;
				uint16_t mask = 8>>(x0%4);
				uint16_t value = 0;

				for (uint16_t dx = 0 ; dx < pixelWidth ; dx++) {
					value |= mask;
					mask >>= 1;
					if (mask == 0) {
						pos2 = pos;
						for (uint16_t dy = 0 ; dy < pixelHeight ; dy++) {
							*pos2 = value;
							pos2 += SCREEN_WIDTH / 4;
						}
						pos++;
						mask = 8;
						value = 0;
					}
				}
				if (value != 0) {
					pos2 = pos;
					for (uint16_t dy = 0 ; dy < pixelHeight ; dy++) {
						*pos2 = value;
						pos2 += SCREEN_WIDTH / 4;
					}
				}
			}
		}
	}
}

void dump(uint32_t location, uint32_t size)
{
	uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
	uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
	char text[3000];

	sprintf(text,"%08x:%04x ",location,size);
	b64_encode((uint8_t*)location, size, (unsigned char*)text+14);

	bool ok = qrcodegen_encodeText(text, tempBuffer, qrcode,
		qrcodegen_Ecc_LOW, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_1, true);

	if (ok) {
		cls();
		setCoordinates(0,0);
		text[13] = 0;
		drawText(text);
		printQr(qrcode);
	}
}


int main()
{
	*LAST_KEY = 0;
#if 0
	drawBlack();
	fillScreen();
	waitSeconds(10);
#endif	
	drawWhite();
	fillScreen();
	
	for (uint32_t pos=0; pos<65536; pos += 512) {
		dump(pos,512);
		for (int i=0;i<10; i++) {
			waitSeconds(1);
			if (*LAST_KEY==KEY_STOP) 
				reload();
		}
	}
	while (KEY_STOP != getKey(1));
	reload();
    return 0;
}
