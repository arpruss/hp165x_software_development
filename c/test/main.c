#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>
#include <hpposix.h>
#include <hpsetjmp.h>

uint32_t bottom; // 764,923

/*
  9842bc.w: -1 to prepare, 0 when ready
  9842cc.l: address to put data
  9842c0.w: 0
  9842f4.w: 0
  9842e4.w: length wanted?
*/

hpjmp_buf jb;

void fileTest(void) {
	printf("starting test\n");
	static char testBuffer[6543];
	for (uint16_t i = 0 ; i < sizeof(testBuffer) ; i++)
		testBuffer[i] = (char)i;
	int fd = open("testfile:1234", O_WRONLY|O_CREAT);
	printf("Opened %d\n", fd);
	if (fd < 0) {
		printf("Error opening file");
		return;
	}
	if (sizeof(testBuffer) != write(fd, testBuffer, sizeof(testBuffer))) {
		printf("Error writing file");
		close(fd);
		return;
	}
	if (close(fd)<0) {
		printf("Error closing file");
		return;
	}
	memset(testBuffer, 0, sizeof(testBuffer));
	fd = open("testfile:1234", O_RDONLY);
	printf("Opened %d\n", fd);
	if (fd < 0) {
		printf("reError opening file");
		return;
	}
	if (sizeof(testBuffer) != read(fd, testBuffer, sizeof(testBuffer))) {
		printf("Error reading file");
		close(fd);
		return;
	}
	if (close(fd)<0) {
		printf("Error reclosing file");
		return;
	}
	for (uint16_t i = 0 ; i < sizeof(testBuffer) ; i++) {
		if ((char)testBuffer[i] != (char)i) {
			printf("Error at offset %u\n", i);
			return;
		}
	}
/*	if (unlink("testfile") < 0) {
		printf("Error unlinking");
		return;
	} */
	fd = open("testfile:1235", O_WRONLY);
	if (fd < 0) {
		printf("Error opening testfile:4321\n");
		return;
	}
	else {
		printf("Opened %d\n", fd);
	}
	if (4 != write(fd, "4321", 4)) {
		printf("Error writing 4 bytes\n");
	}
	else {
		printf("Wrote\n");
	}
	if (close(fd) < 0) {
		printf("Error closing");
	}
	else {
		printf("closed\n");
	}
	printf("Success!");
}

void jumpBack(void) {
	hplongjmp(jb, 3);
}

void testJmp(void) {
	int z = hpsetjmp(jb);
	printf("\nstatus: %d\n",z);
	if (z==0)
		jumpBack();	
}

extern uint32_t _bitplanes;

void scrolling(void) { /* 760 */
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
	for (uint16_t i=0;i<4;i++) {
		uint16_t x = 64+i * 64 + i;
		for (uint16_t j=0;j<64;j++) 
			fillRectangle(x-j-1,j,x+j,j+1);
	}
	setTextXY(0,getTextRows()-1);
	printf("This is the bottom line (%u) of the screen. [Press a key.]", getTextRows()-1);
	for (uint16_t i=0;i<getTextRows();i++) {
		setTextXY(0,i); printf("%d",i);
	}
	getKey(1);
	uint32_t counter = getVBLCounter();
	for (short i=0; i<20;i++) {
		//uint32_t t = getVBLCounter();
		//while (t == getVBLCounter());
		scrollUp(14,0,0,screenWidth,screenHeight,WRITE_BLACK,0xF);
	}
	printf("\nTime: %u\n", (unsigned)(getVBLCounter()-counter));
	getKey(1);
}

void text(void) {
	uint32_t t = getVBLCounter();
	for (int i=0;i<500;i++) {
		setTextXY(0,0);
		putText("This is a long line of text for testing...");
	}
	printf("\nTime: %u", (unsigned)(getVBLCounter()-t));
}

uint8_t
testLut[16] = 
{
	0,2,1,1,0,
	0,2,2,2,0,
	0,0,2,2,0,0 
};

void lut(void) {
	printf("Default LUT\n");
	getKey(1);
	printf("new lut!");
	setScreenLookupTable(testLut);
	getKey(1);
}

void misc(void) {
	printf("384");
	getKey(1);
	setScreenHeight(400);
	printf("400");
	getKey(1);
	(*(volatile uint8_t*)0x202001) = 0;
	waitSeconds(2);
//	setScreenHeight(392);
	(*(volatile uint8_t*)0x202001) = 1<<6;
	printf("392");
}

void stack(void) {
	memset((char*)0x00a708b6,0x11,0x00A7FFFF-0x00a708b6);
	memset((char*)0x00a60000,0x11,0xFFFF);
	printf("filled 0x00a708b6-0xA7FFFE with 0x11\n");
	_refreshDir();
	printf("refreshed disk\n");
	printf("press a key to reboot\n");
	getKey(1);
	_restore_original_int_handlers();
	asm volatile(
		"move.l #0xa6FFFE,%sp\n"
		"jsr 0xece2"); // 
}

void rows(void) {
	for (int i=0;i<30;i++)
		printf("\n%d",i);
}

void scroll(void) {
	showTextCursor(1);
	getKey(1); // see
	setTextCursorXY(0,0);
	getKey(1); // see
	*SCREEN_MEMORY_CONTROL = WRITE_SET_ATTR;
	fillScreen();
	getKey(1);
	*SCREEN_MEMORY_CONTROL = WRITE_CLEAR_ATTR;
	fillScreen();
	showTextCursor(0);
}

main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	
	initScreen(400, WRITE_BLACK);
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
	patchVBL();

	setTextColors(WRITE_WHITE,WRITE_BLACK);
	putText("0 - scrolling\n");
	putText("1 - text\n");
	putText("2 - lut\n");
	putText("3 - misc\n");
	putText("4 - setjmp/longjmp\n");
	putText("5 - rows\n");
	putText("6 - stack test\n");
	putText("7 - cursor\n");
	putText("8 - file test\n");
	setTextXY(0,getTextRows()-1);
	putText("Please choose one");
	
	uint16_t k = getKey(1);
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	fillScreen();
	setTextXY(0,0);

	switch(k) {
		case KEY_0: scrolling(); break;
		case KEY_1: text(); break;
		case KEY_2: lut(); break;
		case KEY_3: misc(); break;
		case KEY_4: testJmp(); break;
		case KEY_5: rows(); break;
		case KEY_6: stack(); break;
		case KEY_7: scroll(); break;
		case KEY_8: fileTest(); break;
		
		default:
			reload();
	}
	getKey(1);
	resetMC6845();
	reload();
}
 