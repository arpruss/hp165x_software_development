#include <hp165x.h>


// _data must be even aligned and _len divisible by 4
uint32_t 
rotatedSumAligned(const void* _data, uint16_t _len) {
    uint32_t out;
    const void* data = _data;
    uint16_t len = _len;
    asm volatile("  lsr.l #2, %[len]\n"
        "  subq.w #1, %[len]\n"
        "  moveq.l #0, %[out]\n"
        "1:\n"
        "  move.l (%[data])+,%%d1\n"
        "  eor.l %%d1,%[out]\n"
        "  rol.l #7,%[out]\n"
        "  dbra %[len],1b\n"
        : [out] "=&d" (out) : [data] "a" (data), [len] "d" (len) : "d1" );
    return out;
}

// _data must be even aligned and _len divisible by 4
uint32_t 
xorShift32Aligned(const void* _data, uint16_t _len) {
    uint32_t out;
    const void* data = _data;
    uint16_t len = _len;
    asm volatile("  lsr.l #2, %[len]\n"
        "  subq.w #1, %[len]\n"
        "  moveq.l #0, %[out]\n"
        "1:\n"
        "  move.l (%[data])+,%%d1\n"
        "  eor.l %%d1,%[out]\n"
        "  move.l %[out],%%d1\n"
        "  lsl.l #8,%%d1\n"
        "  lsl.l #5,%%d1\n"
        "  eor.l %%d1,%[out]\n"
        "  move.l %[out],%%d1\n"
        "  lsr.l #8,%%d1\n" 
        "  lsr.l #8,%%d1\n" // todo:use SWAP?
        "  lsr.l #1,%%d1\n"
        "  eor.l %%d1,%[out]\n"
        "  move.l %[out],%%d1\n"
        "  lsl.l #5,%%d1\n"
        "  eor.l %%d1,%[out]\n"
        "  dbra %[len],1b\n"
        : [out] "=&d" (out) : [data] "a" (data), [len] "d" (len) : "d1" );
    return out;
}

uint32_t getSeed32(void) {
    uint32_t n = getVBLCounter();
    while (n==getVBLCounter());
    *SCREEN_MEMORY_CONTROL=0b1110;
    uint32_t x = xorShift32Aligned((void*)SCREEN,64000);
    x = x ^ xorShift32Aligned((void*)0xA70020,0x40) ^ xorShift32Aligned((void*)0xA70800,0xb6);
    *SCREEN_MEMORY_CONTROL=WRITE_WHITE;
    return x;
}
