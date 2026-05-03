#if defined( DOS) || (defined( _WIN32) && !defined( PDC_WIDE))
   #define USE_UNICODE_ACS_CHARS 0
#else
   #define USE_UNICODE_ACS_CHARS 1
#endif

#include <wchar.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "curspriv.h"
#include "pdcvt.h"
#define USE_UNICODE_ACS_CHARS 0
#include "../common/acs_defs.h"
#include "../common/pdccolor.h"
#undef getch
#include <hp165x.h>

int PDC_get_terminal_fd( void)
{
    return -1;
}


size_t PDC_puts_to_stdout( const char *buff)
{
}

void PDC_gotoyx(int y, int x)
{
    setTextCursorXY(x,y);
}


void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    setTextXY(x,lineno);
    while (len > 0) {
        setTextReverse((*srcp & A_REVERSE) != 0);
        setTextUnderline((*srcp & A_UNDERLINE) != 0);
        uint16_t ch = *srcp++;
        len--;
        if( _is_altcharset( ch))
            ch = acs_map[ch & 0x7f];        
        putChar(ch);
    }
    setTextReverse(0);
    setTextUnderline(0);
}

void PDC_doupdate(void)
{
}
