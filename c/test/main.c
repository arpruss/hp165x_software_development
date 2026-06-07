#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>
#include <hpposix.h>
#include <hpsetjmp.h>

extern uint32_t _original_stack_pointer;

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

void printBinary(uint8_t x) {
	for (int8_t i=7; i>=0; i--)
		putChar(x&(1<<i) ? '1' : '0');
}

void miscControl(void) {
	uint8_t setMode = 1;
	uint16_t control = *(volatile uint16_t*)(0x980716);
	uint32_t n = 1;
	while(1) {
		setTextXY(0,0);
		putText(setMode ? "set mode  \n" : "clear mode\n");
		putText("MISC_CONTROL: ");
		printBinary(control);
		putText("\nHARDWARE_STATUS: ");
		printBinary(*HARDWARE_STATUS);
		uint16_t k = getKey(0);
		if (k) {
			k = parseKey(k);
			if ('0' <= k && k <='7') {
				if (setMode)
					control |= (1<<(k-'0'));
				else
					control &= ~(1<<(k-'0'));
				*MISC_CONTROL = control;
				
			}
			if ('R' == k)
				setMode = !setMode;
			if (KEYBOARD_BREAK == k)
				return;
		}
		printf("\n%u\n%u", (unsigned)getVBLCounter(),n++);
	}
}

static inline void wait100Micros(uint32_t n) {
    asm volatile(
       "0: tst.l %[n]\n"
       "   beq.s 2f\n"
       "   move.l #(8640582/16/10000),%%d0\n"
       "   moveq.l #1,%%d1\n"
       "1:\n"
	   "   sub.l %%d1,%%d0\n"
	   "   bne.s 1b\n"
       "   subq #1,%[n]\n"
       "   bra.s 0b\n" 
       "2:\n"
       : : [n] "d" (n) : "d0", "d1");
}


void beeper(void) {
    for (uint16_t i=0; i<70; i++) {
        *BEEPER=0;
        waitMillis(2);
    }
}

void fileTest(void) {
	printf("starting test\n");
	static char testBuffer[6543];
	for (uint16_t i = 0 ; i < sizeof(testBuffer) ; i++)
		testBuffer[i] = (char)i;
	int fd = open("TESTFILE", O_WRONLY|O_CREAT);
	printf("Opened %d\n", fd);
	if (fd < 0) {
		printf("Error opening file\n");
		return;
	}
	if (sizeof(testBuffer) != write(fd, testBuffer, sizeof(testBuffer))) {
		printf("Error writing file\n");
		close(fd);
		return;
	}
	if (close(fd)<0) {
		printf("Error closing file\n");
		return;
	}
	memset(testBuffer, 0, sizeof(testBuffer));
	fd = open("TESTFILE", O_RDONLY);
	if (fd < 0) {
		printf("Error opening file\n");
		return;
	}
	printf("Opened %d\n", fd);
	off_t z = lseek(fd, 0, SEEK_END);
	printf("size %Ld\n", z);
	lseek(fd, 0, SEEK_SET);
	printf("reading part 1\n");
	if (1234 != read(fd, testBuffer, 1234)) {
		printf("Error reading file\n");
		close(fd);
		return;
	}
	printf("Press a key to continue\n");
	getKey(1);
	printf("reading part 2\n");
	if (sizeof(testBuffer)-1234 != read(fd, testBuffer+1234, sizeof(testBuffer)-1234)) {
		printf("Error reading file\n");
		close(fd);
		return;
	}
	if (close(fd)<0) {
		printf("Error reclosing file\n");
		return;
	}
	for (uint16_t i = 0 ; i < sizeof(testBuffer) ; i++) {
		if ((char)testBuffer[i] != (char)i) {
			printf("Error at offset %u\n", i);
			return;
		}
	}
	printf("Verified file\n");
	if (unlink("TESTFILE") < 0) {
		printf("Error unlinking\n");
		return;
	} 
	else {
		printf("Success unlinking\n");
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

char* getItemName(unsigned short i) {
	static char n[10];
	sprintf(n, "item:%hd", i);
	return n;
}

unsigned short loader(void) {
	return 150;
}

void choose(void) {
	initKeyboard(1);
	setTextColors(WRITE_WHITE, WRITE_BLACK);
	short i = hpChooser(0, 0, 40, 15, 2, 8, loader, getItemName, CHOOSER_DISK_BASED);
	printf("chose %hd\n", i);
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
	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
	/*uint16_t x=0;
	uint16_t y=0;
	for (uint16_t i = 0 ; i < 50 ; i++) {
		uint16_t x2 = rand()%screenWidth;
		uint16_t y2 = rand()%screenHeight;
		drawLine(x,y,x2,y2);
		x = x2;
		y = y2;
	} */
	uint32_t t = getVBLCounter();
	for (uint16_t i = 0 ; i < 300; i++) {
		uint16_t y = rand() % screenHeight;
		uint16_t x1 = rand() % screenWidth;
		uint16_t x2 = rand() % screenWidth;
		drawLine(x1,y,x2,y);
		y--;
		if (x1<x2) 
			for (uint16_t x=x1;x<=x2;x++) drawPixel(x,y);
		else
			for (uint16_t x=x2;x<=x1;x++) drawPixel(x,y);
	}
	printf("Time: %lu\n", getVBLCounter()-t);
}

void inputEvents(void) {
	initKeyboard(1);
	while(1) {
		InputEvent_t e;
		if (getInputEvent(&e)) {
			if (e.type == INPUT_KEY) {
				printf("key: %x %x %x\n", e.data.key.character, e.data.key.nativeKey, e.data.key.modifiers);
				if (e.data.key.nativeKey == HP_KEY_STOP)
					return;
			}
			else if (e.type == INPUT_MOUSE) {
				printf("mouse: (%d,%d) %x\n", e.data.mouse.x, e.data.mouse.y, e.data.mouse.buttons);
			}
		}
	}
}

void mouse(void) {
	uint16_t x = 0;
	uint16_t y = 0;

	initKeyboard(1);
	setMouseCursor(mouseArrow, WRITE_SET_ATTR, WRITE_CLEAR_ATTR, 10);
	
	while(1) {
		InputEvent_t e;
		if (getInputEvent(&e)) {
			if (e.type == INPUT_KEY) {
				if (e.data.key.nativeKey == HP_KEY_STOP)
					return;
				else {
					setTextXY(x/getFontWidth(),y/getFontHeight());
					putChar(e.data.key.character);
				}
			}
			else if (e.type == INPUT_MOUSE) {
				if (e.data.mouse.buttons) {
					*SCREEN_MEMORY_CONTROL = WRITE_WHITE;
					drawPixel(e.data.mouse.x,e.data.mouse.y);
				}
			}
		}
	}
}

uint32_t superFastHashAligned32(const uint32_t* _data, uint16_t n);

void screenMemoryRand(void) {
    while(HP_KEY_STOP != getKey(0)) {
        *SCREEN_MEMORY_CONTROL = WRITE_BLACK;
        fillScreen();
        setTextXY(0,0);
        uint32_t n = getVBLCounter();
        while (n==getVBLCounter());
        *SCREEN_MEMORY_CONTROL = 0b1110;
        uint32_t x = superFastHashAligned32(SCREEN,64000/4);
        n = getVBLCounter();
        while (n==getVBLCounter());
        *SCREEN_MEMORY_CONTROL = 0b1101;
        uint32_t y = superFastHashAligned32(SCREEN,64000/4);
        n = getVBLCounter();
        while (n==getVBLCounter());
        *SCREEN_MEMORY_CONTROL = 0b0100;
        uint32_t z = superFastHashAligned32(SCREEN,64000/4);
        printf("%08lx %08lx %08lx\n", x, y, z);
        delayTicks(16);
    }
}

void delay1000(void) {
    printf("Go!"); //16:23.83 = 983.83 minus maybe .1 : 983.73, ticks 59079: 60.06 Hz ; 
    uint32_t t = getVBLCounter();
    for (int i=0;i<10;i++) {
        waitMillis(100000);
        putChar(i+'1');
    }
    printf("\nTicks: %ld\n", getVBLCounter()-t);
}

main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	
	atexit(goodbye);
	
	initScreen(0, WRITE_BLACK);

	*SCREEN_MEMORY_CONTROL = WRITE_WHITE;

	setTextColors(WRITE_WHITE,WRITE_BLACK);
	putText("0 - scrolling\n");
	putText("1 - text\n");
	putText("2 - lut\n");
	putText("3 - pack disk\n");
	putText("4 - setjmp/longjmp\n");
	putText("5 - rows\n");
	putText("6 - stack test\n");
	putText("7 - beeper\n");
	putText("8 - file test\n");
	putText("9 - info\n");
	putText("A - line\n");
	putText("B - choose\n");
	putText("C - inputEvents\n");
	putText("D - mouse\n");
	putText("E - misc control\n");
    putText("F - 1000 second = 16:40 wait\n");
	putText("IO - screen memory randomness\n");
	setTextXY(0,getTextRows()-1);
	putText("Please choose one");
	
	uint16_t k = getKey(1);
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	fillScreen();
	setTextXY(0,0);

	switch(k) {
		case HP_KEY_0: scrolling(); break;
		case HP_KEY_1: text(); break;
		case HP_KEY_2: lut(); break;
		case HP_KEY_3: pack(); break;
		case HP_KEY_4: testJmp(); break;
		case HP_KEY_5: rows(); break;
		case HP_KEY_6: stack(); break;
		case HP_KEY_7: beeper(); break;
		case HP_KEY_8: fileTest(); break;
		case HP_KEY_9: diskInfo(); break;
		case HP_KEY_A: line(); break;
		case HP_KEY_B: choose(); break;
		case HP_KEY_C: inputEvents(); break;
		case HP_KEY_D: mouse(); break;
		case HP_KEY_E: miscControl(); break;
        case HP_KEY_F: delay1000(); break;
		case HP_KEY_IO: screenMemoryRand(); break;
		default:
			reload();
	}
	getKey(1);
}
 