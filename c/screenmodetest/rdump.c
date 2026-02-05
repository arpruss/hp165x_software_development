#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

void line(uint16_t x, uint16_t y, uint16_t y2, uint16_t color) {
	volatile uint16_t* pos = SCREEN + y * (SCREEN_WIDTH/4) + x*2;
	for (int i=0; i<y2-y;i++) {
		*pos=color;
		pos[1]=color;
		pos+=SCREEN_WIDTH/4;
	}
}

void test(uint16_t mode, uint16_t x, uint16_t y) {
	uint16_t h = SCREEN_HEIGHT/4;
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
	line(x,y,y+h/2,0xF);
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	line(x,y+h/2,y+h-1,0xF);
	*SCREEN_MEMORY_CONTROL = mode;
	line(x,y+h/4,y+3*h/4,0xF);
}

int main(void) {
	drawWhite();
	fillScreen();
	*SCREEN = 0b0101;
	
	setTextXY(0,2);
	
	for (uint32_t mode = 0 ; mode < 256 ; ) {
		uint32_t m = mode;
		for (int area = 0 ; area < 4 ; area++) {
			for (int x = 0 ; x < 64 ; x++) {
				test((mode&0xF)|(mode&0xF0)<<4,x,area*(SCREEN_HEIGHT/4));
				setTextXY(x,area*getTextRows()/4);
				mode++;
			}
		}
		setTextXY(0,getTextRows()-1); printf("%x", m);
		mode = m;
		for (int area = 0 ; area < 4 ; area++) {
			for (int x = 0 ; x < 64 ; x++) {
				setTextXY(x,(area*getTextRows()+2)/4);
				printf("%x", mode&0xF);
				mode++;
			}
		}
		
		while(1) {
			uint16_t k = getKey();
			
			if (k==KEY_SELECT)
				goto NEXT;
			if (k==KEY_STOP)
				reload();
		}
		
		NEXT:
	}
	
	printf("DONE");
		
	while(1) {
		uint16_t k = getKey(1);
		
		if (k==KEY_STOP)
			reload();
	}
}
