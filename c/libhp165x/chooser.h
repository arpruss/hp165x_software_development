#ifndef _CHOOSER_H
#define _CHOOSER_H

#include <hp165x.h>

typedef char* (*ChooserItemNamer_t)(uint16_t item);
typedef uint16_t (*ChooserItemLoader_t)(void);

#define CHOOSER_DISK_BASED             1
#define CHOOSER_DEFAULT_MOUSE_CURSOR   2

int hpChooser(uint16_t _topLeftX, uint16_t _topLeftY, 
		uint16_t _width, uint16_t _height,
		uint16_t _spacing, uint16_t _maxWidth, 
		ChooserItemLoader_t loader, ChooserItemNamer_t _namer, uint32_t flags);
short quickPickFile(uint16_t type, char* name);
#endif