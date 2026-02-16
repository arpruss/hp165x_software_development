#ifndef _MC6845_H
#define _MC6845_H

#define MC6845_REGISTER_ADDRESS ((volatile uint8_t*)0x0020c001)
#define MC6845_REGISTER_VALUE ((volatile uint8_t*)0x0020c003)

#define MC6845_H_TOTAL     0
#define MC6845_H_DISPLAYED 1
#define MC6845_H_SYNC      2
#define MC6845_V_TOTAL	   5
#define MC6845_V_DISPLAYED 6
#define MC6845_V_SYNC	   7

void resetMC6845(void);
void setScreenHeight(uint16_t height);
void _setScreenWidth(void);
uint16_t ticksPerSecond(void);

#endif
