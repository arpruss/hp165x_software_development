#ifndef _SIMPLE_SERIAL_H

#define _SIMPLE_SERIAL_H
#include <hp165x.h>

int16_t simple_serial_getchar(void);
uint16_t simple_serial_is_send_done(void);
void simple_serial_send(uint32_t size, const void* data);
void simple_serial_init(uint16_t baud);
void simple_serial_close();
uint16_t simple_serial_available(void);
int16_t simple_serial_peek(void);

#endif