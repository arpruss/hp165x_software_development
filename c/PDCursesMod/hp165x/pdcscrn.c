#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#if !defined( _WIN32) && !defined( DOS)
#define USE_TERMIOS
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

static struct termios orig_term;
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef MOUSE_MOVED
#endif

#include <assert.h>
#include "curspriv.h"
#include "pdcvt.h"
#include "../common/pdccolor.h"
#include "../common/pdccolor.c"
#undef getch
#include <hp165x.h>


int PDC_rows = 28, PDC_cols = 80;
extern short cursorX, cursorY;
chtype _hpPrevAttr = ~(chtype)0;
 
static int PDC_get_screen_size( int *n_cols, int *n_rows)
{
    *n_cols = PDC_rows;
    *n_rows = PDC_cols;
    return( 0);
}

void PDC_check_for_resize( void)
{
}

void PDC_reset_prog_mode( void)
{
}

void PDC_reset_shell_mode( void)
{
}

int PDC_resize_screen(int nlines, int ncols)
{
   return( 0);
}

void PDC_restore_screen_mode(int i)
{
    INTENTIONALLY_UNUSED_PARAMETER( i);
}

void PDC_save_screen_mode(int i)
{
    INTENTIONALLY_UNUSED_PARAMETER( i);
}

void PDC_scr_close( void)
{
    setTextReverse(0);
    setTextUnderline(0);
    setTextColors(WRITE_WHITE, WRITE_BLACK);
    setTextXY(0,getTextRows()-1);
}

void PDC_scr_free( void)
{
}

int PDC_get_terminal_fd( void);        /* pdckbd.c */

int PDC_scr_open(void)
{
   atexit(reload);
   initScreen(392,WRITE_BLACK);
   initInputEvents(1);
   PDC_LOG(("PDC_scr_open exit\n"));
   SP->mouse_wait = PDC_CLICK_PERIOD;
   SP->visibility = 0;                /* no cursor,  by default */
   SP->curscol = SP->cursrow = 0;
   cursorX = cursorY = 0;
   setTextCursorXY(0,0);
   showTextCursor(0);
   SP->audible = TRUE;
   SP->mono = FALSE;
   SP->orig_attr = TRUE;
   SP->orig_fore = SP->orig_back = -1;
   SP->termattrs = A_UNDERLINE|A_REVERSE|A_STANDOUT|A_DIM|A_BOLD;
   return( 0);
}

void PDC_set_resize_limits( const int new_min_lines,
                            const int new_max_lines,
                            const int new_min_cols,
                            const int new_max_cols)
{
   INTENTIONALLY_UNUSED_PARAMETER( new_min_lines);
   INTENTIONALLY_UNUSED_PARAMETER( new_max_lines);
   INTENTIONALLY_UNUSED_PARAMETER( new_min_cols);
   INTENTIONALLY_UNUSED_PARAMETER( new_max_cols);
   return;
}


bool PDC_can_change_color(void)
{
    return FALSE;
}

int PDC_color_content( int color, int *red, int *green, int *blue)
{
    return OK;
}

int PDC_init_color( int color, int red, int green, int blue)
{
    return OK;
}
