#include <stdio.h>
#include <stdint.h>

uint8_t displayDWordToByte(uint32_t dw) {
	//return (dw>>16)&0x0F | ((dw)&0x0F)<<4;
	return (dw>>12)&0xF0 | ((dw)&0x0F);
}

void convertROMFont(uint8_t* out, const uint32_t* romFont) {
	for (int i=0; i<256*4; i++) {

/*
    *puVar3 = uVar2;
    uVar1 = uVar2 << 0x1c | uVar2 >> 4;
    puVar3[0x4a] = uVar1;
    uVar1 = uVar1 >> 4;
    uVar2 = (uVar2 >> 4) << 0x1c | uVar1;
    puVar3[0x94] = uVar2;
    puVar3[0xde] = uVar1 << 0x1c | uVar2 >> 4; */
		uint32_t x = *romFont++;
//		printf("%x ",x);
//		x = 0xDEADBEEF;
		x = (x>>24) | (x>>8)&0xFF00 | (x<<8)&0xFF0000 | (x<<24)&0xFF000000;
//		printf("%x \n",x);
		
		*out++ = displayDWordToByte(x);
		x = x << 28 | x >> 4;
		*out++ = displayDWordToByte(x);
		x = x << 28 | x >> 4;
		*out++ = displayDWordToByte(x);
		x = x << 28 | x >> 4;
		*out++ = displayDWordToByte(x);
	}
}

/* TODO: adjust window */
static void _setFontSystem(uint32_t defaultLocation, uint32_t testLocation, uint32_t testValue) {
	uint8_t* location = 0;
	if (*(uint32_t*)testLocation == testValue) {
		location = (uint8_t*)defaultLocation;
		setFont(location, 16, 16);
	}
	else {
		location = romFind(testValue);
		if (location == 0) {
			setFont(font8x14, 14, 16); 
			return;
		}
		location -= (testLocation-defaultLocation);
		setFont(location, 16, 16);
	}
}

void setFontSystem(uint8_t bold) {
	// search for fonts in ROM in case the user has a different ROM version
	// from mine; default to VGA if not found
	if (bold) {
		_setFontSystem(0xa674,0xaa84,0x63106C80); // start of B
	}
	else {
		_setFontSystem(0x9e74,0xa288,0xF884C448); // start of B
	}
}

main() {
	uint32_t data[256*4];
	FILE* f = fopen("../../../floppy/hxc/pvtest/fullrom.bin", "rb");
	if (f==NULL)
		printf("oops");
	fseek(f, 0xa674, SEEK_SET);
	int n = fread(data,256,16,f);
	uint8_t out[256*16];
	convertROMFont(out, data);
	for (int i = 0 ; i < 256*16 ; i++) {
		for (int j=0x80; j; j>>=1) 
			putchar(out[i] & j ? 'X' : '.');
		putchar('\n');
	}
}