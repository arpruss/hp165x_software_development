#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>

#include "curspriv.h"
#include "pdcvt.h"
#include "../common/mouse.c"
#include "../common/xlates.h"

#undef getch
#include <hp165x.h>

void PDC_check_for_resize( void);
extern bool PDC_resize_occurred;

bool PDC_check_key( void)
{
    return haveInputEvent();
}

void PDC_flushinp( void)
{
    flushInputEvents();
}


#define MAX_COUNT 15

/* If possible,  we use the SGR mouse tracking modes.  These allow
for wheel mice and more than 224 columns and rows.  See

https://invisible-island.net/xterm/ctlseqs/ctlseqs.html

   for details on this and more on how 'traditional' mouse events are
encoded.

   'Traditional' mouse events include six bytes.  First three are

ESC [ M

   Next byte is 96 for mouse wheel up,  97 for down,  or (for more
"traditional" mouse events) 32 plus :

   0 for button 1
   1 for button 2
   2 for button 3
   3 for release
   4 if Shift is pressed
   8 if Alt (Meta) is pressed
   16 if Ctrl is pressed

   Note that 'release' doesn't tell you _which_ is released.  If only
one has been pressed (the usual case),  it's presumably the one you
released.  If two or more buttons are pressed simultaneously,  the
"releases" are reported in the numerical order of the buttons,  not
the order in which they're actually released (which we don't know).

   My tilt mouse reports 'tilt left' as a left button (1) and 'tilt right'
as a middle button press.  Wheel events get shift,  alt,  ctrl added in
(but that doesn't seem to be getting through in PDCurses... to be fixed).
Button events only get Ctrl (though I think you might get the other events
on some terminals).

   "Correct" mouse handling will require that we detect a button-down,
then hold off for SP->mouse_wait to see if we get a release event.  */

#define KEY_A1                (KEY_OFFSET + 0xc1) /* upper left on Virtual keypad */
#define KEY_A2                (KEY_OFFSET + 0xc2) /* upper middle on Virt. keypad */
#define KEY_A3                (KEY_OFFSET + 0xc3) /* upper right on Vir. keypad */
#define KEY_B1                (KEY_OFFSET + 0xc4) /* middle left on Virt. keypad */
#define KEY_B2                (KEY_OFFSET + 0xc5) /* center on Virt. keypad */
#define KEY_B3                (KEY_OFFSET + 0xc6) /* middle right on Vir. keypad */
#define KEY_C1                (KEY_OFFSET + 0xc7) /* lower left on Virt. keypad */
#define KEY_C2                (KEY_OFFSET + 0xc8) /* lower middle on Virt. keypad */
#define KEY_C3                (KEY_OFFSET + 0xc9) /* lower right on Vir. keypad */

static const struct {
    uint8_t character;
    uint16_t key;
} translate[] = {
    { KEYBOARD_LEFT, KEY_LEFT },
    { KEYBOARD_RIGHT, KEY_RIGHT },
    { KEYBOARD_UP, KEY_UP },
    { KEYBOARD_DOWN, KEY_DOWN },
    { KEYBOARD_F1, KEY_F0+1 },
    { KEYBOARD_F1+1, KEY_F0+2 },
    { KEYBOARD_F1+2, KEY_F0+3 },
    { KEYBOARD_F1+3, KEY_F0+4 },
    { KEYBOARD_F1+4, KEY_F0+5 },
    { KEYBOARD_F1+5, KEY_F0+6 },
    { KEYBOARD_F1+6, KEY_F0+7 },
    { KEYBOARD_F1+7, KEY_F0+8 },
    { KEYBOARD_F1+8, KEY_F0+9 },
    { KEYBOARD_F1+9, KEY_F0+10 },
    { KEYBOARD_F1+10, KEY_F0+11 },
    { KEYBOARD_F1+11, KEY_F0+12 },
    { KEYBOARD_HOME, KEY_HOME },
    { KEYBOARD_END, KEY_END },
    { KEYBOARD_PAGE_UP, KEY_PPAGE },
    { KEYBOARD_PAGE_DOWN, KEY_NPAGE },
};

static const struct {
    uint16_t native;
    uint16_t key;
} translateNative[] = {
    { KEYBOARD_KP_7, KEY_A1 },
    { KEYBOARD_KP_8, KEY_A2 },
    { KEYBOARD_KP_9, KEY_A3 },
    { KEYBOARD_KP_4, KEY_B1 },
    { KEYBOARD_KP_5, KEY_B2 },
    { KEYBOARD_KP_6, KEY_B3 },
    { KEYBOARD_KP_1, KEY_C1 },
    { KEYBOARD_KP_2, KEY_C2 },
    { KEYBOARD_KP_3, KEY_C3 },
};

int PDC_get_key( void)
{
    InputEvent_t e;
    SP->key_modifiers = 0;
    while (!getInputEvent(&e));
    if (e.type == INPUT_KEY) {
        if (e.data.key.modifiers & KEYBOARD_MODIFIER_SHIFT)
            SP->key_modifiers |= PDC_KEY_MODIFIER_SHIFT;

        if (e.data.key.modifiers & KEYBOARD_MODIFIER_CTRL)
            SP->key_modifiers |= PDC_KEY_MODIFIER_CONTROL;

        if (e.data.key.modifiers & KEYBOARD_MODIFIER_ALT)
            SP->key_modifiers |= PDC_KEY_MODIFIER_ALT;
        
        if (e.data.key.modifiers & KEYBOARD_MODIFIER_CTRL) {
            if (e.data.key.character == KEYBOARD_LEFT) 
                return CTL_LEFT;
            else if (e.data.key.character == KEYBOARD_RIGHT)
                return CTL_RIGHT;
        }

        for (uint16_t i=0; i<sizeof(translate)/sizeof(*translate); i++) {
            if (e.data.key.character == translate[i].character)
                return translate[i].key;
        }
        for (uint16_t i=0; i<sizeof(translateNative)/sizeof(*translateNative); i++) {
            if (e.data.key.nativeKey == translateNative[i].native)
                return translateNative[i].key;
        }
        return (uint16_t)e.data.key.character;
    }
    return -1;
    // TODO: mouse
}

int PDC_modifiers_set( void)
{
   return( OK);
}

bool PDC_has_mouse( void)
{
    return FALSE; // todo
}

/* Xterm defaults to reporting no mouse events.  If you request mouse movement
events even with no button pressed,  state 1003 is set ("report everything").
If you don't request such movements,  but _do_ want to know about movements
with one of the first three buttons down,  state 1002 is set.  If you just
want certain mouse events (clicks and doubleclicks,  say),  state 1000 is
set.  And if the mouse mask is zero ("don't tell me anything about the
mouse"),  mouse events are shut off.

At first,  this code just set state 1003.  Xterm reported bazillions of
events (which were filtered out according to SP->_trap_mbe).  I don't think
this really mattered much on my machine,  but I assume Xterm supports this
sort of filtering at a higher level for a reason.  */

int PDC_mouse_set( void)
{
   return(  OK);
}

void PDC_set_keyboard_binary( bool on)
{
   INTENTIONALLY_UNUSED_PARAMETER( on);
   return;
}
