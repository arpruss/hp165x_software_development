#ifndef _KEYBOARD_H

#define _KEYBOARD_H
#include <hp165x.h>

#define KEYBOARD_LEFT '\x02' // ctrl-b
#define KEYBOARD_RIGHT '\x06' // ctrl-f
#define KEYBOARD_UP '\x10' // ctrl-p
#define KEYBOARD_DOWN '\x0E' // ctrl-n
#define KEYBOARD_BREAK '\x03' // ctrl-c

void initKeyboard(char useSerial);
char kbhit(void);
char getch(void);

#endif