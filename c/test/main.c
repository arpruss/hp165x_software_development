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

_WRAP_0(_eb02,0xeb02);



void fileTest(void) {
	printf("starting test\n");
	static char testBuffer[6543];
	for (uint16_t i = 0 ; i < sizeof(testBuffer) ; i++)
		testBuffer[i] = (char)i;
	int fd = open("TESTFILE", O_WRONLY|O_CREAT);
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
	fd = open("TESTFILE", O_RDONLY);
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
	if (unlink("TESTFILE") < 0) {
		printf("Error unlinking");
		return;
	} 
	else {
		printf("Success unlinking");
	}
	if (unlink("TESTFILE") < 0) {
		printf("can't unlink a second time (GOOD)");
	} 
	else{
		printf("can unlink second time (BAD)");
	}
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

void pack(void) {
#if 0
	char buffer[256];
	char oldBuffer[256];
	printf("%d\n", readBlocks(2,1,oldBuffer));
	memset(buffer,'x',256);
	printf("%d\n", writeBlocks(2,1,buffer));
	_ec0a();
	memset(buffer,'y',256);
	printf("%d\n", readBlocks(2,1,buffer));
	printf("%.12s\n", buffer);
	//printf("%d\n", writeBlocks(2,1,oldBuffer));
#endif
	diskInfo();
	printf("Packing\n");
	int x = lifPack(1);
	printf("Packed %d\nPress a key\n",x );
	getKey(1);
	diskInfo();
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

extern uint32_t _original_stack_pointer;

void diskInfo(void) {
	uint32_t totalBlocks,freeBlocks,space;
	if (diskSpace(&totalBlocks, &freeBlocks, &space) < 0) {
		putText("Cannot read disk space\n");
	}
	else {
		printf("Total: %u; free: %u; max space: %u\n", (unsigned)totalBlocks, (unsigned)freeBlocks, (unsigned)space);
	}
	printf("BTW, the original stack was %lx\n", _original_stack_pointer);
}

//void acquire(int param_1);

void scope(void) {
//	acquire(1);
}

void goodbye(void) {
	printf("Goodbye!");
	waitSeconds(1);
	reload();
}

void line(void) {
	uint16_t x=0;
	uint16_t y=0;
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
	for (uint16_t i = 0 ; i < 50 ; i++) {
		uint16_t x2 = rand()%screenWidth;
		uint16_t y2 = rand()%screenHeight;
		drawLine(x,y,x2,y2);
		x = x2;
		y = y2;
	} 
}

main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	
	atexit(goodbye);
	
	initScreen(400, WRITE_BLACK);
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
	patchVBL();

	setTextColors(WRITE_WHITE,WRITE_BLACK);
	putText("0 - scrolling\n");
	putText("1 - text\n");
	putText("2 - lut\n");
	putText("3 - pack disk\n");
	putText("4 - setjmp/longjmp\n");
	putText("5 - rows\n");
	putText("6 - stack test\n");
//	putText("7 - scope\n");
	putText("8 - file test\n");
	putText("9 - info\n");
	putText("A - line\n");
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
		case KEY_3: pack(); break;
		case KEY_4: testJmp(); break;
		case KEY_5: rows(); break;
		case KEY_6: stack(); break;
//		case KEY_7: scope(); break;
		case KEY_8: fileTest(); break;
		case KEY_9: diskInfo(); break;
		case KEY_A: line(); break;
		default:
			reload();
	}
	getKey(1);
}
 