#include <hp165x.h>

typedef char* (*ChooserItemNamer_t)(uint16_t item);
typedef uint16_t (*ChooserItemLoader_t)(void);

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
	int x = (currentItem - topItem) % columns;
	int y = (currentItem - topItem) / columns;
	setTextXY(topLeftX + x * (columns + spacing), topLeftY + y);
}

static void drawName(const char* name) {
	int n = strlen(name);
	if (n > maxWidth) {
		putTextN(name, maxWidth);
		return;
	}
	putText(name);
	n -= maxWidth;
	while(n--)
		putChar(' ');
}

static void move(short delta) {
	if (numItems == 0)
		return;
	moveCursorToCurrent();
	setTextReverse(0);
	drawName(namer(currentItem));
	// do I need to scroll?
}

// refresher is called when 
int hpChooser(uint16_t _topLeftX, uint16_t _topLeftY, 
		uint16_t _width, uint16_t _height,
		uint16_t _spacing, uint16_t _maxWidth, uint8_t diskBased,
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
	
	columns = (width + horizontalSpacing) / (maxWidth + horizontalSpacing);
	
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
					if (getch() == KEY_ESC)
						return -1;
				}
			}
			setTextXY(topLeftX,topLeftY);
			putText("Scanning files...  ");
			numItems = loader();
			setTextXY(topLeftX,topLeftY);
			putText("                 ");
		}
		else {
			numItems = loader();
		}
		
		topItem = currentItem = 0;
		drawInitialItems();
		if (numItems > 0) {
			moveCursorToCurrent();
			setTextReverse(1);
			drawName(namer(currentItem));
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
						return numItems == 0 ? -1 : currentItem;
					case KEYBOARD_BREAK:
					case 27:
						*SCREEN_MEMORY_CONTROL = background;
						fillRectangle(textToPixelX(topLeftX), textToPixelY(topLeftY), 
							textToPixelX(topLeftX+width), textToPixelY(topLeftY+height));
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
	
	while(1) {
		columns = (bottomRightX - topLeftX) / maxWidth + 
		
	}
}