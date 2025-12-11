#include "tick.h"

long long int ticks_count = 0;

void tick()
{
    while(1) {
        ticks_count++;
        task_tick();
        usleep(TIMER_PERIOD_US);
    }
}