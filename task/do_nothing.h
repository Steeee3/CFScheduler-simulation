#ifndef DO_NOTHING_H
#define DO_NOTHING_H

#include <semaphore.h>
#include <time.h>

#include "../scheduler/scheduler.h"
#include "../scheduler/sched_info.h"
#include "../utils/utils.h"

#define CPU_PERIOD_US 100000 // 0.1s
#define WAIT_FREQUENCY 5     // 1 in n chances
#define WAKEUP_FREQUENCY 5   // 1 in n chances

void *do_nothing(void *arg);

#endif