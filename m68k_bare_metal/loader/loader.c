#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

#define MAX_FILES 16

char names[MAX_FILES][MAX_FILENAME_LENGTH+1];
int numNames = 0;

enum { SELECT=0, WAIT } mode;

void getFiles(void) {
	DirEntry_t entry;
	
	int in = 0;
	numNames = 0;
	
	int i = 0;
	while(numNames < MAX_FILES && -1 != (i=getDirEntry(in, &entry))) {
		if ((uint16_t)i == (uint16_t)0xC001) {
			printf(entry.name);
			names[numNames][MAX_FILENAME_LENGTH] = 0;
			memcpy(names[numNames], entry.name, MAX_FILENAME_LENGTH);
			numNames++;
		}
		in++;
	}
}

void scan(void) {
	*LAST_KEY = 0;
	while(1) {
		if ( (HARDWARE_STATUS_NO_DISC & *HARDWARE_STATUS ) ) {
			setTextXY(0,0);
			putText("No disc in drive...");
			while ( (HARDWARE_STATUS_NO_DISC & *HARDWARE_STATUS ) ) {
				uint16_t k = *LAST_KEY;
				*LAST_KEY = 0;
				if (k) {
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

void menu(void) {
	drawBlack();
	fillScreen();
	setTextXY(0,0);
	putText("Choose program to execute:\n\n");
	for (int i=0; i<numNames; i++) {
		printf(" [%X] %s\n", i, names[i]);
	}
	while (1) {
		uint16_t k = *LAST_KEY;
		*LAST_KEY = 0;
		if (k == KEY_STOP)
			return;
		if (HARDWARE_STATUS_NO_DISC & *HARDWARE_STATUS )
			return;
		int c = parseKey(k);
		if ('0' <= c && c <= '9') {
			c -= '0';
			if (c < numNames)
				loadAndRun(names[c]);
			continue;
		}
		else if ('A' <= c && c <= 'F') {
			c += 10 - 'A';
			if (c < numNames)
				loadAndRun(names[c]);
			continue;
		}
	}
}


int main(void) {
	setTextBlackOnWhite(0);

	while(1) {
		drawBlack();
		fillScreen();
		
		scan();
		menu();
	}
}