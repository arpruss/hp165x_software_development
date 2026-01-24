#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

/*
 a data,attr -> x function can be described as 
 a sequence of 4 bits, corresponding to the data/attr
 inputs 00 -> bit0
		01 -> bit1
		10 -> bit2
		11 -> bit3
 */
 
#define FLEN 10
 
char functions[][FLEN+1]={
	"zero      ",       // 0000
	"nor       ",       // 0001 (00->1)
	"attr&~data", 		// 0010 (01->1)
	"~data     ",       // 0011 (10 and 11->0)
	"data&~attr", 		// 0100 (10->1)
	"~attr     ",       // 0101 (00->1, 10->1)
	"xor       ",       // 0110 (01 and 10->1)
	"nand      ",       // 0111 (11->0)
	"and       ",       // 1000 (11->1)
	"same      ",       // 1001 (00,11->1)
	"attr      ",       // 1010 (00,01->1)
	"~data|attr", 		// 1011 (10->0)
	"data      ",       // 1100 (10,11->1)
	"data|~attr", 		// 1101 (01->0)
	"or        ",       // 1110 (00->0)
	"one       "        // 1111
};

// X0X readfunc   datawrite attrwrite\n
#define LINE_LENGTH (3+3*FLEN+4)
char buffer[256 * LINE_LENGTH + 1];
 
uint8_t getBit(char type) { // type=='d' or 'a'
	if (type == 'd') 
		*SCREEN_MEMORY_CONTROL = 0xF0E;
	else
		*SCREEN_MEMORY_CONTROL = 0x007;
	return 0x1 & *SCREEN;
}

void setDataAttr(uint8_t data, uint8_t attr) {
	*SCREEN_MEMORY_CONTROL = SCREEN_CLEAR_DATA_CLEAR_ATTR;
	SCREEN[0] = 1;
	*SCREEN_MEMORY_CONTROL = SCREEN_SET_DATA_ONLY;
	SCREEN[0] = data;
	*SCREEN_MEMORY_CONTROL = SCREEN_SET_ATTR;
	SCREEN[0] = attr;
}

uint8_t analyzeRead(uint16_t mode) {
	uint8_t signature = 0;
	for (uint8_t data=0;data<2;data++)
		for(uint8_t attr=0;attr<2;attr++) {
			setDataAttr(data,attr);
			*SCREEN_MEMORY_CONTROL = mode;
			uint8_t x = 0x1&*SCREEN;
			signature = (signature>>1) | (x<<3);
		}
	return signature;
}

uint8_t analyzeWrite(char type, uint16_t mode) {
	uint8_t signature = 0;
	for (uint8_t data=0;data<2;data++)
		for(uint8_t attr=0;attr<2;attr++) {
			setDataAttr(data,attr);
			*SCREEN_MEMORY_CONTROL = mode;
			*SCREEN = 1;
			uint8_t x = getBit(type);
			signature = (signature>>1) | (x<<3);
		}
	return signature;
}

int main(void) {
	setKeyWait(1);
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	fillScreen();
	setTextColors(WRITE_WHITE,WRITE_BLACK);
	for (uint16_t i=0;i<256;i++) {
		uint16_t mode = (i&0xF)|(i&0xF0)<<4;
		uint8_t r = analyzeRead(mode);
		uint8_t d = analyzeWrite('d',mode);
		uint8_t a = analyzeWrite('a',mode);
		sprintf(buffer + i*LINE_LENGTH, "%03x %s %s %s\n", (unsigned int)mode, functions[r], functions[d], functions[a]);
//		sprintf(buffer + i*LINE_LENGTH, "%03x %x %x %x\n", (unsigned int)mode, (unsigned int)r,(unsigned int)d,(unsigned int)a);
		setTextXY(0,i%getTextRows());
//		putText(buffer + i*LINE_LENGTH);
		*SCREEN_MEMORY_CONTROL = mode;
		printf("%04x", *SCREEN_MEMORY_CONTROL);
		if (i%getTextRows() == getTextRows()-1) {
			if (getKey()==KEY_STOP)
				reload();
		}
	}
	getKey();
	setTextXY(TEXT_COLUMNS-6,0); putText("SAVING");
	int f = openFile("MODES", 1, WRITE_FILE);
	writeFile(f, buffer, sizeof(buffer)-1);
	closeFile(f);
	setTextXY(TEXT_COLUMNS-6,0); putText("DONE  ");
	getKey();
	reload();
}
