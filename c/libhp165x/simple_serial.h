#ifndef _SIMPLE_SERIAL_H

#define _SIMPLE_SERIAL_H
#include <hp165x.h>

void simple_serial_set_circular(char c);
int16_t simple_serial_getchar(void);
void simple_serial_write(const void* data, uint32_t size);
void simple_serial_init(uint16_t baud);
void simple_serial_close();
uint16_t simple_serial_available(void);
int16_t simple_serial_peek(void);

#endif