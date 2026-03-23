#include "chooser.h"
#include <string.h>

static uint16_t foreground;
static uint16_t background;
static short width;
static short height; 
static short columns;
static uint16_t numItems;
static short currentItem;
static short topItem;
static short topLeftX;
static short topLeftY;
static short spacing;
static short maxWidth;
static ChooserItemNamer_t namer;

static void moveCursorToCurrent() {
	short x = (currentItem - topItem) % columns;
	short y = (currentItem - topItem) / columns;
	setTextXY(topLeftX + x * (maxWidth + spacing), topLeftY + y);
}

static void drawName(const char* name) {
	short n = strlen(name);
	if (n > maxWidth) {
		putTextN(name, maxWidth);
		return;
	}
	putText(name);	
	n = maxWidth - n;
	while(n--)
		putChar(' ');
}

static void drawItems(short startY, short endY) {
	for (short i = topItem ; i < numItems && i - topItem < columns * height ; i++) {
		short y = (i - topItem) / columns;
		if (startY <= y && y < endY) {
			short x = (i - topItem) % columns;
			setTextXY(topLeftX + x * (maxWidth + spacing), topLeftY + y);
			drawName(namer(i));
		}
	}
}

static void move(short delta) {
	if (numItems == 0)
		return;
	short oldX = (currentItem - topItem) % columns;
	short oldY = (currentItem - topItem) / columns;
	setTextXY(topLeftX + oldX * (maxWidth + spacing), topLeftY + oldY);
	setTextReverse(0);
	drawName(namer(currentItem));
	currentItem += delta;
	short newX = (currentItem - topItem) % columns;
	short newY = (currentItem - topItem) / columns;
	short lastY = (numItems - 1 - topItem) / columns;
	short firstY = (- topItem) / columns;
	if (delta > 0 && ( newY == height || ( newY == height - 1 && newY < lastY ) ) ) {
		scrollUp(getFontHeight(), textToPixelX(topLeftX), textToPixelY(topLeftY), 
			textToPixelX(topLeftX+width), textToPixelY(topLeftY+height), 
			background, getScrollBitplanes());
		topItem += columns;
		newY--;
		drawItems(height-1, height);
	}
	else if (delta < 0 && ( newY == -1 || ( newY == 0 && newY > firstY ) ) ) {
		scrollDown(getFontHeight(), textToPixelX(topLeftX), textToPixelY(topLeftY), 
			textToPixelX(topLeftX+width), textToPixelY(topLeftY+height), 
			background, getScrollBitplanes());
		topItem -= columns;
		newY++;
		drawItems(0,1);
	}
	setTextXY(topLeftX + newX * (maxWidth + spacing), topLeftY + newY);
	setTextReverse(1);
	drawName(namer(currentItem));
	setTextReverse(0);
}

// refresher is called when 
int hpChooser(uint16_t _topLeftX, uint16_t _topLeftY, 
		uint16_t _width, uint16_t _height,
		uint16_t _spacing, uint16_t _maxWidth, 
		uint8_t diskBased,
		ChooserItemLoader_t loader, ChooserItemNamer_t _namer) {
			
	namer = _namer;
	width = _width;
	height = _height;
	topLeftX = _topLeftX;
	topLeftY = _topLeftY;
	spacing = _spacing;
	maxWidth = _maxWidth;
	foreground = getTextForeground();
	background = getTextBackground();
	
	*SCREEN_MEMORY_CONTROL = background;
	
	columns = (width + spacing) / (maxWidth + spacing);
	
	
	while(1) {
		*SCREEN_MEMORY_CONTROL = background;
		fillRectangle(textToPixelX(topLeftX), textToPixelY(topLeftY), 
			textToPixelX(topLeftX+width), textToPixelY(topLeftY+height));
		if (diskBased) {
			if ( (HARDWARE_STATUS_NO_DISK & *HARDWARE_STATUS ) ) {
				setTextXY(topLeftX,topLeftY);
				putText("No disc in drive...");
			}
			while ( (HARDWARE_STATUS_NO_DISK & *HARDWARE_STATUS ) ) {
				if (kbhit()) {
					short k = getch();
					if (k == 27 || k == KEYBOARD_BREAK)
						return -1;
				}
			}
			setTextXY(topLeftX,topLeftY);
			putText("Scanning files...  ");
			refreshDir();
			numItems = loader();
			setTextXY(topLeftX,topLeftY);
			putText("                 ");
		}
		else {
			numItems = loader();
		}
		
		topItem = currentItem = 0;
		*SCREEN_MEMORY_CONTROL = background;
		fillRectangle(textToPixelX(topLeftX), textToPixelY(topLeftY), 
			textToPixelX(topLeftX+width), textToPixelY(topLeftY+height));

		drawItems(0, height);

		if (numItems > 0) {
			moveCursorToCurrent();
			setTextReverse(1);
			drawName(namer(currentItem));
			setTextReverse(0);
		}
		
		while (1) {
			if (diskBased && ( 
					( HARDWARE_STATUS_NO_DISK & *HARDWARE_STATUS ) || 
					0 == ( HARDWARE_STATUS_OLD_DISK & *HARDWARE_STATUS ) ) ) {
				break;
			}
			if (kbhit()) {
				switch(getch()) {
					case '\n':
					case '\r':
						*SCREEN_MEMORY_CONTROL = background;
						fillRectangle(textToPixelX(topLeftX), textToPixelY(topLeftY), 
							textToPixelX(topLeftX+width), textToPixelY(topLeftY+height));
						setTextXY(textToPixelX(topLeftX), textToPixelY(topLeftY));
						return numItems == 0 ? -1 : currentItem;
					case KEYBOARD_BREAK:
					case 27:
						*SCREEN_MEMORY_CONTROL = background;
						fillRectangle(textToPixelX(topLeftX), textToPixelY(topLeftY), 
							textToPixelX(topLeftX+width), textToPixelY(topLeftY+height));
						setTextXY(textToPixelX(topLeftX), textToPixelY(topLeftY));
						return -1;
					case KEYBOARD_RIGHT:
						if (currentItem + 1 < numItems)
							move(1);
						else
							move(0);
						break;
					case KEYBOARD_LEFT:
						if (currentItem > 0)
							move(-1);
						else
							move(0);
						break;
					case KEYBOARD_UP:
						if (currentItem >= columns)
							move(-columns);
						else
							move(-currentItem);
						break;
					case KEYBOARD_DOWN:
						if (currentItem + columns < numItems)
							move(columns);
						else
							move(numItems - 1 - currentItem);
						break;
				}
			}
		}
	}
}