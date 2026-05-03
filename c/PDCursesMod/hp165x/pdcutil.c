#include <unistd.h>
#include <stdlib.h>
#include "curspriv.h"
#undef getch
#include <hp165x.h>

void PDC_napms(int ms)
{
    delayTicks(ticksPerSecond() * ms / 1000);
}

const char *PDC_sysname(void)
{
    return "HP165xB";
}

enum PDC_port PDC_port_val = PDC_PORT_DOS; // TODO
