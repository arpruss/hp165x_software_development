#include <string.h>
#include "hp165x.h"


static uint16_t startX;
static uint16_t startY;
static uint16_t cols;
static uint16_t rows;
static char* buffer;
static uint16_t cursor;
static uint16_t length;

static void setXYFromOffset(uint16_t offset) {
	uint16_t p = startX + offset;
	uint16_t row = startY + p / cols;
	uint16_t col = p % cols;
	if (row >= rows) {
		uint16_t delta = row - rows + 1;
		scrollText(delta);
		row = rows-1;
		startY -= delta;
	}
	setTextXY(col,row);
}

static void drawFrom(uint16_t offset) {
	setXYFromOffset(offset);
	startY -= putText(buffer+offset);
}

static void clearCursor(void) {
	setXYFromOffset(cursor);
	setScrollMode(0);
	if (cursor<length)
		putChar(buffer[cursor]);
	else
		putChar(' ');
	setScrollMode(1);
}

static void drawCursor(void) {
	setXYFromOffset(cursor);
	setScrollMode(0);
	setTextReverse(1);
	if (cursor<length) 
		putChar(buffer[cursor]);
	else
		putChar(' ');
	setTextReverse(0);
	setScrollMode(1);
}

int getText(char* _buffer, uint16_t maxSize) {	
	if (maxSize == 0)
		return -1;
	else if (maxSize == 1) {
		*_buffer = 0;
		return 0;
	}
	
	setTextReverse(0);
	setScrollMode(1);
	startX = getTextX();
	startY = getTextY();
	rows = getTextRows();
	cols = getTextColumns();
	buffer = _buffer;
	cursor = length = strlen(_buffer);

	char* p = buffer;
	while(*p) {
		if (*p == '\n' || *p == '\r')
			*p = ' ';
		p++;
	}

	drawFrom(0);
	drawCursor();
	
	while(1) {
		char c = getch();
		switch(c) {
			case '\n':
			case '\r':
				clearCursor();
				return length;
			case KEYBOARD_BREAK:
				clearCursor();
				return -1;
			case KEYBOARD_LEFT:
				if (cursor > 0) {
					clearCursor();
					cursor--;
					drawCursor();
				}
				break;
			case KEYBOARD_RIGHT:
				if (cursor < length) {
					clearCursor();
					cursor++;
					drawCursor();
				}
				break;
			case '\b':
			case '\x7F':
				if (cursor > 0) {
					clearCursor();
					if (cursor < length) 
						memmove(buffer+cursor-1, buffer+cursor, length+1-cursor);
					else
						buffer[cursor-1] = 0;
					cursor--;
					length--;
					drawFrom(cursor);
					putChar(' ');
					drawCursor();
				}
				break;
			default:
				if (length + 1 < maxSize - 1) {
					clearCursor();
					if (cursor < length)
						memmove(buffer+cursor+1, buffer+cursor, length+1-cursor);
					else
						buffer[cursor+1] = 0;
					buffer[cursor] = c;
					cursor++;
					length++;
					drawFrom(cursor-1);
					drawCursor();
				}
				break;
		}
	}
}

/*
int main() {
	setTextColors(WRITE_WHITE, WRITE_BLACK);
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	fillScreen();
	initKeyboard(1);
	
	while (1) {
		char buffer[200];
		putText("Input: ");
		*buffer=0;
		int16_t s = getText(buffer,200);
		printf("\nGot %d\n", s);
		if (s<0) 
			break;
		putChar('>');
		putText(buffer);
		putChar('\n');
	}
	
	reload();
	return 0;
}
*/