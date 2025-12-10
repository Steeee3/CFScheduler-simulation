#include <stdio.h> //! remove
#include "tick.h"

void tick()
{
    while(1) {
        printf("Tick\n");
        usleep(TIMER_PERIOD_US);
    }
}