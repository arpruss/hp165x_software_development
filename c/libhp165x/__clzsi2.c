#include <stdint.h>

int __clzsi2 (unsigned int a) {
	int c = 0;
	while (a) {
		if (0x80000000 & a)
			c++;
		a <<= 1;
	}
	return c;
}