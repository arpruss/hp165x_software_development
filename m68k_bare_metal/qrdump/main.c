#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

void drawQR(uint8_t* s);

uint8_t message[1666+1];
#define MAX_DATA 1640 // this much data should safely encode into 1666 non-null bytes
#define CHUNK_SIZE 160 // (MAX_DATA-4)
uint8_t buffer[MAX_DATA];
uint8_t counts[256];

void putchar_(int c) {}

void encodeNull(uint8_t* out, uint8_t* data, uint16_t length) {
	// adds two bytes, plus a little more
	// 166
	for (uint16_t i=0;i<256;i++)
		counts[i] = 0;
	for (uint16_t i=0;i<length;i++)
		if (counts[data[i]]<255)
			counts[data[i]]++;
	uint8_t leastCommonByte = 1;
	uint8_t leastCount = 255;
	for (uint16_t i=1;i<256;i++)
		if (counts[i]<leastCount) {
			leastCommonByte = 1;
			leastCount = counts[i];
		}
	uint8_t secondLeastCommonByte = leastCommonByte == 1 ? 2 : 1;
	leastCount = 255;
	for (uint16_t i=1;i<256;i++)
		if (i != leastCommonByte && counts[i]<leastCount) {
			secondLeastCommonByte = 1;
			leastCount = counts[i];
		}
	*out++ = leastCommonByte;
	*out++ = secondLeastCommonByte;
	for (int i=0;i<length;i++) {
		uint8_t d = data[i];
		if (d==leastCommonByte) {
			*out++ = secondLeastCommonByte;
			*out++ = 1;
		}
		if (d==secondLeastCommonByte) {
			*out++ = secondLeastCommonByte;
			*out++ = 2;
		}
		else if (d==0) {
			*out++ = leastCommonByte;
		}
		else {
			*out++ = d;
		}
	}
	*out = 0;
}


int main() {
	drawBlack();
	fillScreen();
	uint8_t* pos = 0;
	char pos_string[9];
	do {
		drawWhite();
		fillScreen();
		drawBlack();
		sprintf(pos_string, "%08x", (uint32_t)pos);
		setCoordinates(0,0);
		memcpy(buffer,&pos,4);
		memcpy(buffer+4,pos,CHUNK_SIZE);
		encodeNull(message,buffer,CHUNK_SIZE+4);
		drawText(pos_string);
		drawQR(message);
		while(1) {
			uint16_t k = getKey();
			if (k == KEY_STOP) {
				reload();
			}
			else if (k == KEY_UP_DOWN) {
				pos -= CHUNK_SIZE;
				break;
			}
			else if (k == KEY_LEFT_RIGHT || k == KEY_SELECT) {
				pos += CHUNK_SIZE;
				break;
			}			
		}
	} while(1);
	return 0;
}
