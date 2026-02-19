#include <hp165x.h>

int strncasecmp(const char* s1, const char* s2, long n) {
	while (*s1 && *s2 && n>0) {
		int c1 = *s1;
		int c2 = *s2;
		if (c1 != c2) {
			if ('a' <= c1 && c1 <= 'z') 
				c1 += 'A'-'a';
			if ('a' <= c2 && c2 <= 'z') 
				c2 += 'A'-'a';
			if (c1 != c2)
				return c1-c2;
		}
		s1++;
		s2++;
		n--;
	}
	if (n > 0)
		return *s1 - *s2; 
	return 0;
}

