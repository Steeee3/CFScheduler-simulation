#ifndef DO_NOTHING_H
#define DO_NOTHING_H

#include <semaphore.h>
#include <time.h>

#include "../scheduler/sched_info.h"
#include "../utils/utils.h"

#define CPU_PERIOD_US 1000000 // 1s

void *do_nothing(void *arg);

#endif