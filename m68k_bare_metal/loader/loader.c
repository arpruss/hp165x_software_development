#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <hp165x.h>

int loadAndRun(char* f);

int main(void) {
	putText("Loading\n");
	loadAndRun("DUMPER    ");
	putText("Exited");
	getKeyWait();
	reload();
	loadAndRun("dumper");
}