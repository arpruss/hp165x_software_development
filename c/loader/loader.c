#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

#define MAX_FILES 64

#define DRAW_BACKGROUND WRITE_BLACK
#define DRAW_FOREGROUND WRITE_WHITE

char _names[MAX_FILES][MAX_FILENAME_LENGTH+1];
char* names[MAX_FILES];
int numNames = 0;

char SYSTEM[] = "SYSTEM_   ";
char PVTEST[] = "PVTEST_   ";

void bubbleSort() {
	uint16_t swapped;
	for (uint16_t i=0; i<numNames-1; i++) {
		swapped = 0;
		for (uint16_t j=0; j < numNames - i - 1 ; j++) {
			if (!strcmp(names[j+1],SYSTEM) || (strcmp(names[j],names[j+1])>0 && strcmp(names[j],SYSTEM) ) ) {
				char* z = names[j];
				names[j] = names[j+1];
				names[j+1] = z;
				swapped = 1;
			}
		}
		if (!swapped)
			break;
	}
}

void getFiles(void) {
	DirEntry_t entry;

	refreshDir();
	
	int in = 0;
	numNames = 0;
	
	int i = 0;
	int pvtest = -1;
	int pvtestBlocks = 0;
	int system = -1;
	int systemBlocks = 0;
	
	while(numNames < MAX_FILES && -1 != (i=getDirEntry(in, &entry))) {
		if ((uint16_t)i == (uint16_t)0xC001) {
			_names[numNames][MAX_FILENAME_LENGTH] = 0;
			memcpy(_names[numNames], entry.name, MAX_FILENAME_LENGTH);
			names[numNames] = _names[numNames];
			if (!strcmp(names[numNames],SYSTEM)) {
				system = numNames;
				systemBlocks = entry.numBlocks;
			}
			else if (!strcmp(names[numNames],PVTEST)) {
				pvtest = numNames;
				pvtestBlocks = entry.numBlocks;
			}
			numNames++;
		}
		in++;
	}
	
	if (pvtest >= 0 && system >=0 && pvtestBlocks == systemBlocks) {
		if (pvtest != numNames - 1) {
			memmove(names+pvtest, names+pvtest+1, (numNames - 1 - pvtest) * sizeof(char*));
		}
		numNames--;
	}
	
	bubbleSort(names);
}

void scan(void) {
	while(1) {
		if ( (HARDWARE_STATUS_NO_DISC & *HARDWARE_STATUS ) ) {
			setTextXY(0,0);
			putText("No disc in drive...");
			while ( (HARDWARE_STATUS_NO_DISC & *HARDWARE_STATUS ) ) {
				uint16_t k = getKey(); 
				if (k != 0) {
					if (k == KEY_STOP)
						reload();
				}
			}
			setTextXY(0,0);
			putText("                   ");
		}
		setTextXY(0,0);
		putText("Scanning...");
		getFiles();
		if (numNames == 0) {
			setTextXY(0,1);
			putText("No files found...");
		}
		else {
			setTextXY(0,1);
			putText("                 ");
			return;
		}
	} while(numNames == 0);
}

void drawEntry(int16_t pos,char inv) {
	setTextReverse(inv);
	setTextXY((pos/16)*16,2+(pos%16));
	if (pos < 16)
		printf(" [%X] %s ", pos, names[pos]);
	else {
		putText("   ");
		putText(names[pos]);
		putText(" ");
	}
	setTextReverse(0);
}

void menu(void) {
	int selected = 0;
	*SCREEN_MEMORY_CONTROL = DRAW_BACKGROUND;
	fillScreen();
	setTextXY(0,0);
	putText("Choose program to execute:");
	setTextXY(0,getTextRows()-1);
	putText("STOP: reboot, RUN: refresh");
	for (int i=0; i<numNames; i++) {
		drawEntry(i, i==selected);
	}
	while (1) {
		uint16_t k = getKey(); 
		if (k == KEY_STOP)
			reload();
		else if (k == KEY_RUN)
			return;
		if (HARDWARE_STATUS_NO_DISC & *HARDWARE_STATUS )
			return;
		int c = parseKey(k);
		if ('0' <= c && c <= '9') {
			c -= '0';
			drawEntry(selected, 0);
			if (c < numNames) {
				drawEntry(c, 1);
				loadAndRun(names[c]);
			}
			continue;
		}
		else if ('A' <= c && c <= 'F') {
			c += 10 - 'A';
			drawEntry(selected, 0);
			if (c < numNames) {
				drawEntry(c, 1);
				loadAndRun(names[c]);
			}
			continue;
		}
		else if (KEY_SELECT == k) {
			loadAndRun(names[selected]);
		}
		else if (KEY_TURN_CW == k) {
			drawEntry(selected, 0);
			selected = (selected + 1) % numNames;
			drawEntry(selected, 1);
		}
		else if (KEY_TURN_CCW == k) {
			drawEntry(selected, 0);
			selected = (selected + numNames - 1) % numNames;
			drawEntry(selected, 1);
		}
	}
}


int main(void) {
	*LAST_KEY = 0;
	
	setTextReverse(0);
	setTextColors(DRAW_FOREGROUND, DRAW_BACKGROUND);
	
	while(1) {
		*SCREEN_MEMORY_CONTROL = DRAW_BACKGROUND;
		fillScreen();
		
		scan();
		menu();
	}
}
