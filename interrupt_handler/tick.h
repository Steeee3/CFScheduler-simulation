#ifndef TICK_H
#define TICK_H

#include "../scheduler/scheduler.h"

#define TIMER_PERIOD_US 1000000 // 1s

extern long long int ticks_count;

void tick();

#endif