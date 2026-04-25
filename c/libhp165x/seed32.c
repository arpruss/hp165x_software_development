#include <hp165x.h>

// Paul Hsieh's super fast hash, but only for 32-bit data
// n=number of dwords
uint32_t 
superFastHashAligned32(const uint32_t* _data, uint16_t n) {
    uint32_t hash;
    const void* data = _data;
    uint16_t len = n;
    asm volatile(
        "  moveq  #0,%[hash]\n"
        "  move.l #0xFFFF,%%d2\n"
        "  move.w %[len],%[hash]\n"
        "  add.l  %[hash],%[hash]\n"
        "  add.l  %[hash],%[hash]\n" // hash=len in bytes
        "  subq.w #1, %[len]\n"
        "1:\n"
        "  moveq  #0,%%d1\n"
        "  move.w (%[data])+,%%d1\n"
        "  add.l  %%d1,%[hash]\n"  // hash += low16bits
        "  move.w (%[data])+,%%d1\n"
        "  lsl.l  #3,%%d1\n"     
        "  lsl.l  #8,%%d1\n"     
        "  eor.l  %[hash],%%d1\n"  // d1 = (high16bits<<11)^hash
        "  and.w  %%d2,%[hash]\n"
        "  swap   %[hash]\n"
        "  eor.l  %%d1,%[hash]\n"  // hash=(hash<<16)^d1
        "  move.l %[hash],%%d1\n" 
        "  lsr.l  #3,%%d1\n"
        "  lsr.l  #8,%%d1\n"
        "  add.l  %%d1,%[hash]\n"  // hash += hash >> 11
        "  dbra %[len],1b\n"
        : [hash] "=&d" (hash) : [data] "a" (data), [len] "d" (len) : "d1", "d2" );
        
    hash ^= hash<<3;
    hash += hash>>5;
    hash ^= hash<<4;
    hash += hash>>17;
    hash ^= hash<<25;
    hash += hash>>6;
        
    return hash;
}


uint32_t getSeed32(void) {
    uint32_t n = getVBLCounter();
    while (n==getVBLCounter());
    *SCREEN_MEMORY_CONTROL=0b1110;
    uint32_t x = superFastHashAligned32((void*)SCREEN,64000/4);
    x = x ^ superFastHashAligned32((void*)0xA70020,0x40/4) ^ superFastHashAligned32((void*)0xA70800,0xb8/4);
    *SCREEN_MEMORY_CONTROL=WRITE_WHITE;
    return x;
}
