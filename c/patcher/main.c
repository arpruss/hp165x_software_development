#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>
#include "locations.h"

const uint8_t patch[] = {
#include "patch.c"
};

main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	
	memcpy((void*)PATCH_LOCATION, patch, sizeof(patch));
	
	initScreen(0, WRITE_BLACK);

	setTextColors(WRITE_WHITE,WRITE_BLACK);
	
	uint16_t k = getKey(1);
	*SCREEN_MEMORY_CONTROL = WRITE_BLACK;
	fillScreen();
	loadAndRunForPatch("osystem", (void*)PATCH_LOCATION, (void*)ORIGINAL_START, (void*)ORIGINAL_SIZE);
	reload();
}
