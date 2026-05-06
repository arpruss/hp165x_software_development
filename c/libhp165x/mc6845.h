#ifndef _MC6845_H
#define _MC6845_H

#define MC6845_REGISTER_ADDRESS ((volatile uint8_t*)0x0020c001)
#define MC6845_REGISTER_VALUE ((volatile uint8_t*)0x0020c003)

#define MC6845_H_TOTAL     0
#define MC6845_H_DISPLAYED 1
#define MC6845_H_SYNC      2
#define MC6845_SYNC_WIDTH  3
#define MC6845_V_TOTAL	   4
#define MC6845_V_ADJUST	   5
#define MC6845_V_DISPLAYED 6
#define MC6845_V_SYNC	   7
#define MC6845_MAX_SCANLINE 9 // character height - 1

void resetMC6845(void);
void setScreenHeight(uint16_t height);
void _setScreenWidth(void);
uint16_t ticksPerSecond(void);
extern uint16_t ticksPerSec;
extern uint32_t ticksPer1000Sec;
extern uint16_t msPerTick;
extern uint16_t usPerTick;

#endif
