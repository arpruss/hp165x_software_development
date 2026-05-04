#include <string.h>
#include <hp165x.h>

#define MAX_PICK_FILES 144

static uint16_t pickFileList[MAX_PICK_FILES];
static uint16_t numPickFiles;

#define WRITE_OVERLAY_GRAY       0b110000000001
#define WRITE_OVERLAY_FOREGROUND 0b101000000001
#define WRITE_OVERLAY_BACKGROUND 0b100000000001
#define WRITE_OVERLAY_ERASE 	 0b111000000001

#define WIN_X1 10
#define WIN_Y1 3
static uint16_t WIN_X2;
static uint16_t WIN_Y2;

static uint16_t savedX;
static uint16_t savedY;
static uint16_t savedFore;
static uint16_t savedBack;
static uint16_t pickType;

static char* pickFileNamer(unsigned short i) {
	ROMDirEntry_t* dp = getROMDirEntry(pickFileList[i]);
    if (dp == NULL)
        return "";
	static char name[MAX_FILENAME_LENGTH+1];
    unpadFilename(name, dp->name);
    return name;
}

static int compareFiles(const void* p1, const void* p2) {
    char* n1 = (char*)getROMDirEntry(*(uint16_t*)p1)->name;
    char* n2 = (char*)getROMDirEntry(*(uint16_t*)p2)->name;
    return strncasecmp(n1, n2, MAX_FILENAME_LENGTH);
}

static unsigned short pickFileLoader(void) {
    if (refreshDir()<0)
        return 0;
    
	ROMDirEntry_t* dp;
	int i = 0;
	numPickFiles = 0;
	while (NULL != (dp=getROMDirEntry(i)) && numPickFiles < MAX_PICK_FILES) {
        if ((pickType == 0 && dp->type != 0) || dp->type == pickType) {
            pickFileList[numPickFiles++] = i;
        }
		i++;
	}
    
    if (numPickFiles > 0) {
        qsort(pickFileList, numPickFiles, sizeof(uint16_t), compareFiles);
    }
    
	return numPickFiles;
}

static void hp_set_window(void) {
    WIN_X2 = getTextColumns() - WIN_X1;
    WIN_Y2 = getTextRows() - WIN_Y1;
	savedFore = getTextForeground();
	savedBack = getTextBackground();
	savedX = getTextX();
	savedY = getTextY();
	uint16_t h = getFontHeight();
	*SCREEN_MEMORY_CONTROL = WRITE_OVERLAY_GRAY;
	frameRectangle(WIN_X1*FONT_WIDTH,WIN_Y1*h,WIN_X2*FONT_WIDTH,WIN_Y2*h,8);
	*SCREEN_MEMORY_CONTROL = WRITE_OVERLAY_BACKGROUND;
	fillRectangle(WIN_X1*FONT_WIDTH,WIN_Y1*h,WIN_X2*FONT_WIDTH,WIN_Y2*h);
	setTextWindow(WIN_X1,WIN_Y1,WIN_X2,WIN_Y2);
	setTextColors(WRITE_OVERLAY_FOREGROUND,WRITE_OVERLAY_BACKGROUND);
	setTextXY(0,0);
}

static void hp_clear_window() {
	uint16_t h = getFontHeight();
	*SCREEN_MEMORY_CONTROL = WRITE_OVERLAY_ERASE;
	fillRectangle(WIN_X1*FONT_WIDTH-8,WIN_Y1*h-8,WIN_X2*FONT_WIDTH+8,WIN_Y2*h+8);
	setTextWindow(0,0,0,0);
	setTextColors(savedFore,savedBack);
	setTextXY(savedX,savedY);
}

short quickPickFile(uint16_t type, char* name) {
    pickType = type;
    hp_set_window();
    short i = hpChooser(1, 1, WIN_X2-WIN_X1-2, WIN_Y2-WIN_Y1-2, 2, 10, pickFileLoader, pickFileNamer, CHOOSER_DISK_BASED);
    hp_clear_window();
    if (i < 0)
        return 0;
    strcpy(name, pickFileNamer(i));
    return 1;
}
