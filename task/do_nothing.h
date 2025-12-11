#ifndef DO_NOTHING_H
#define DO_NOTHING_H

#include <semaphore.h>
#include <time.h>

#include "../scheduler/scheduler.h"
#include "../scheduler/sched_info.h"
#include "../utils/utils.h"

void *do_nothing(void *arg);

#endif