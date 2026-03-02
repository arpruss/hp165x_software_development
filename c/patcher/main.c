// start SYSTEM, then start this via the IO/selftest
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
	
	simple_serial_init(BAUD_19200);
	while (1) {
		char s[] = "Ready... Press a key to launch...\n";
		simple_serial_write(s, sizeof(s)-1);
		if (getKey(0))
			break;
		delayTicks(30);
	}

//	loadAndRunForPatch("SYSTEM_", (void*)PATCH_LOCATION, (void*)ORIGINAL_START, (void*)ORIGINAL_SIZE);
	loadAndRunForPatch("osystem", (void*)PATCH_LOCATION, (void*)ORIGINAL_START, (void*)ORIGINAL_SIZE);
	char s[] = "Failed\n";
	simple_serial_write(s, sizeof(s)-1);
	reload();
}
