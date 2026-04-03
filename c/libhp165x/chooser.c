#include "chooser.h"
#include <string.h>

static uint8_t originalReverse;
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
static uint32_t flags;
static ChooserItemNamer_t namer;
static MouseCursorData_t mouseData;

static void moveCursorToCurrent(void) {
	short x = (currentItem - topItem) % columns;
	short y = (currentItem - topItem) / columns;
	setTextXY(topLeftX + x * (maxWidth + spacing), topLeftY + y);
}

static void clearWindow(void) {
	*SCREEN_MEMORY_CONTROL = background;
	fillRectangle(textToPixelX(topLeftX), textToPixelY(topLeftY), 
		textToPixelX(topLeftX+width), textToPixelY(topLeftY+height));
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

static void drawSelection(uint8_t active) {
	short x = (currentItem - topItem) % columns;
	short y = (currentItem - topItem) / columns;
	setTextXY(topLeftX + x * (maxWidth + spacing), topLeftY + y);
	setTextReverse(active);
	drawName(namer(currentItem));
}

static short getItemXY(short x, short y) {
	x = pixelToTextX(x) - topLeftX;
	y = pixelToTextY(y) - topLeftY;
	if (x < 0 || y < 0 || x >= width || y >= height)
		return -1;
	if (x % (maxWidth + spacing) >= maxWidth)
		return -1;
	short i = x / (maxWidth + spacing) + columns * y + topItem;
	if (i >= numItems)
		return -1;
	return i;
}

static void select(int16_t item) {
	drawSelection(0);
	currentItem = item;
	drawSelection(1);
}

static void move(short delta) {
	if (numItems == 0)
		return;
	drawSelection(0);
	currentItem += delta;
	//short newX = (currentItem - topItem) % columns;
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
	drawSelection(1);
}

static void reset(void) {
	if (! (flags & CHOOSER_DEFAULT_MOUSE_CURSOR) ) {
		uint8_t m = isMouseCursorVisible();
		restoreMouseCursor(&mouseData);
		clearWindow();
		if (m)
			drawMouseCursor();
	}
	else {
		clearWindow();
	}
	setTextXY(topLeftX,topLeftY);
	setTextReverse(originalReverse);
}

int hpChooser(uint16_t _topLeftX, uint16_t _topLeftY, 
		uint16_t _width, uint16_t _height,
		uint16_t _spacing, uint16_t _maxWidth, 
		ChooserItemLoader_t loader, ChooserItemNamer_t _namer, uint32_t _flags) {

	InputEvent_t event;

	flags = _flags;
	namer = _namer;
	width = _width;
	height = _height;
	topLeftX = _topLeftX;
	topLeftY = _topLeftY;
	spacing = _spacing;
	maxWidth = _maxWidth;
	foreground = getTextForeground();
	background = getTextBackground();
	originalReverse = getTextReverse();
	
	columns = (width + spacing) / (maxWidth + spacing);

	if (! (flags & CHOOSER_DEFAULT_MOUSE_CURSOR) ) {
		saveMouseCursor(&mouseData);
		setMouseCursor(mouseArrow, WRITE_SET_ATTR, WRITE_CLEAR_ATTR, 30);
	}
	
	while(1) {
		clearWindow();
		if (flags & CHOOSER_DISK_BASED) {
			if ( (HARDWARE_STATUS_NO_DISK & *HARDWARE_STATUS ) ) {
				setTextXY(topLeftX,topLeftY);
				putText("No disc in drive...");
			}
			while ( (HARDWARE_STATUS_NO_DISK & *HARDWARE_STATUS ) ) {
				if (getInputEvent(&event)) {
					if (event.type == INPUT_KEY && (event.data.key.character == 27 ||
						event.data.key.character == KEYBOARD_BREAK)) {
						reset();
						return -1;
					}
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
		clearWindow();

		drawItems(0, height);

		if (numItems > 0) {
			moveCursorToCurrent();
			setTextReverse(1);
			drawName(namer(currentItem));
			setTextReverse(0);
		}
		
		while (1) {
			InputEvent_t event;
			
			if ((flags & CHOOSER_DISK_BASED) && ( 
					( HARDWARE_STATUS_NO_DISK & *HARDWARE_STATUS ) || 
					0 == ( HARDWARE_STATUS_OLD_DISK & *HARDWARE_STATUS ) ) ) {
				break;
			}
			if (getInputEvent(&event)) {
				if (event.type == INPUT_MOUSE && numItems > 0) {
					int16_t item = getItemXY(event.data.mouse.x, event.data.mouse.y);
					if (0 <= item) {
						if (event.data.mouse.buttons & MOUSE_DOUBLE_CLICK) {
							select(item);
							reset();
							return currentItem;
						}
						else if (event.data.mouse.buttons & event.data.mouse.buttonDifference & MOUSE_BUTTON_LEFT) {
							select(item);
						}
					}
				}
				if (event.type != INPUT_KEY)
					continue;
				switch(event.data.key.character) {
					case '\n':
					case '\r':
						reset();
						return numItems == 0 ? -1 : currentItem;
					case KEYBOARD_BREAK:
					case 27:
						reset();
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