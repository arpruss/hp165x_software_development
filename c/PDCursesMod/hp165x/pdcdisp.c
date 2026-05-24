#define USE_UNICODE_ACS_CHARS 0

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

short cursorX,cursorY;

int PDC_get_terminal_fd( void)
{
    return -1;
}


size_t PDC_puts_to_stdout( const char *buff)
{
    putText(buff);
    return strlen(buff);
}

void PDC_gotoyx(int y, int x)
{
    cursorX = x;
    cursorY = y;
}

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    static chtype prevAttr = ~(chtype)0;
    setTextXY(x,lineno);
    while (len > 0) {
        chtype attr = *srcp & ~A_CHARTEXT;
        if (attr != prevAttr) {
            if (attr & A_BLINK) {
                _setTextColors(WRITE_WHITE, WRITE_GRAY);
                setTextScrollBitplanes(0xF);
            }
            else if (attr & A_DIM) {
                _setTextColors(WRITE_GRAY, WRITE_BLACK);
                setTextScrollBitplanes(0xF);
            }
            else {
                _setTextColors(WRITE_WHITE, WRITE_BLACK);
            }
            setTextReverse((attr & A_REVERSE) != 0);
            setTextUnderline((attr & A_UNDERLINE) != 0);
            prevAttr = attr;
        }
        uint16_t ch = *srcp++ & A_CHARTEXT;
        len--;
        if( _is_altcharset(ch))
            ch = acs_map[ch & 0x7f];        
        putChar(ch);
    }
}

void PDC_doupdate(void)
{
    setTextCursorXY(cursorX,cursorY);
}
