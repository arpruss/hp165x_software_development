#ifndef _KEYBOARD_H

#define _KEYBOARD_H
#include <hp165x.h>

#define _CTRL(x) ((x)-'a'+1)
#define KEYBOARD_CTRL_LEFT _CTRL('l')
#define KEYBOARD_CTRL_RIGHT _CTRL('r')
#define KEYBOARD_LEFT '\x02' // ctrl-b
#define KEYBOARD_RIGHT '\x06' // ctrl-f
#define KEYBOARD_UP '\x10' // ctrl-p
#define KEYBOARD_DOWN '\x0E' // ctrl-n
#define KEYBOARD_BREAK '\x03' // ctrl-c
#define KEYBOARD_DELETE '\x07' // ctrl-g
#define KEYBOARD_HOME 0xD2
#define KEYBOARD_END  0xD5

void initKeyboard(char useSerial);
char kbhit(void);
char getch(void);

#endif