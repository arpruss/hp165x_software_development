#include <stdio.h>
#include <string.h>
#include "hp165x.h"

void _loadexec_relocatable_start(void);
void _loadexec_relocatable_end(void);

#define RELOCATABLE_SIZE ( (uint32_t)( (uint8_t*)&_loadexec_relocatable_end - (uint8_t*)&_loadexec_relocatable_start ) )

int loadAndRun(const char* filename) {
	uint8_t header[0x24];
	uint32_t codeAddress;
	uint32_t codeSize;
	uint8_t relocatableCode[RELOCATABLE_SIZE + 2048]; // safety margin for stack
	
	memcpy(relocatableCode, _loadexec_relocatable_start, RELOCATABLE_SIZE);
	
	int fd = openFile(filename, 0xC001, OPEN_READ);
	if (fd < 0)
		return fd;
	if (sizeof(header) != readFile(fd, header, sizeof(header)) || 4 != readFile(fd, &codeSize, 4) ||
		4 != readFile(fd, &codeAddress, 4) ) {
		closeFile(fd);
		return -1;
	}
	
	initialScreen();

	if (! strcmp(filename, "SYSTEM_") || ! strcmp(filename, "SYSTEM_   ")) {
		closeFile(fd);
		unpatchVBL();
		_reload();
		return -1;// should not happen
	}

	asm volatile(
			"  move.l %0,%%a0\n"
			"  jmp (%%a0)\n"
			"  .globl _loadexec_relocatable_start,_loadexec_relocatable_end\n"
			"_loadexec_relocatable_start:\n" : : "a" (relocatableCode));
	
	asm volatile(
		"  lea _fd(%%pc), %%a1\n"
		"  move.l %[fd], (%%a1)\n"  // save
		"  lea _codeSize(%%pc), %%a1\n"
		"  move.l %[codeSize], (%%a1)\n" //save
		"  lea _codeAddress(%%pc), %%a1\n"
		"  move.l %[codeAddress], (%%a1)\n" // sve

		"  move.l %[codeSize], -(%%sp)\n" 
		"  move.l %[codeAddress], -(%%sp)\n" 
		"  move.l %[fd], -(%%sp)\n"
		"  jsr 0xeb86\n"
		"  add #12,%%sp\n"
		
		"  cmp.l _codeSize(%%pc),%%d0\n"
		"  bne 4f\n"
		"  pea -1\n" // size
		"  move.l _dataAddress(%%pc),-(%%sp)\n" 
		"  move.l _fd(%%pc),-(%%sp)\n"
		"  jsr 0xeb86\n"
		"  add #12,%%sp\n"
		"  tst.l %%d0\n"
		"  bmi 4f\n"
		"  move.l _codeAddress(%%pc),%%a0\n"
		"  move.l #0x00A7FFFC,%%sp\n" // hope the code here doesn't get overwritten too quickly
		"  jsr (%%a0)\n" // should not return
		"4:\n"
		"  move.l #0x00A7FFFC,%%sp\n"
		"5:\n"
		"  jsr 0xece2\n" // reload
		"  bra 5b\n" // try again forever
		"_fd: dc.l 0\n"
		"_codeAddress: dc.l 0\n"
		"_codeSize: dc.l 0\n"
		"_dataAddress: dc.l 0\n"
		"_loadexec_relocatable_end:\n"
		:  : [fd] "d" (fd), [codeAddress] "a" (codeAddress), [codeSize] "d" (codeSize) );
	
	/* should not happen */
	return -1;
}
