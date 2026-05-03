#include <stdio.h>
#include <curspriv.h>
#include "pdcvt.h"
#undef getch
#include <hp165x.h>

/*man-start**************************************************************

pdcsetsc
--------

### Synopsis

    int PDC_set_blink(bool blinkon);
    int PDC_set_bold(bool boldon);
    void PDC_set_title(const char *title);

### Description

   PDC_set_blink() toggles whether the A_BLINK attribute sets an actual
   blink mode (TRUE), or sets the background color to high intensity
   (FALSE). The default is platform-dependent (FALSE in most cases). It
   returns OK if it could set the state to match the given parameter,
   ERR otherwise.

   PDC_set_bold() toggles whether the A_BOLD attribute selects an actual
   bold font (TRUE), or sets the foreground color to high intensity
   (FALSE). It returns OK if it could set the state to match the given
   parameter, ERR otherwise.

   PDC_set_title() sets the title of the window in which the curses
   program is running. This function may not do anything on some
   platforms.

### Portability
                             X/Open  ncurses  NetBSD
    PDC_set_blink               -       -       -
    PDC_set_title               -       -       -

**man-end****************************************************************/


int PDC_curs_set( int visibility)
{
    showTextCursor(visibility != 0);
    return visibility;
}


int PDC_set_blink(bool blinkon)
{
}

int PDC_set_bold(bool boldon)
{
}

void PDC_set_title( const char *title)
{
    INTENTIONALLY_UNUSED_PARAMETER( title);
}
