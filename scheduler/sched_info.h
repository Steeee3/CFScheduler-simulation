#ifndef SCHED_INFO_H
#define SCHED_INFO_H

#include <stdint.h>
#include "task_info.h"

#define DEFAULT_LOAD 1024u

typedef struct sched_info {
    task_state_t state;

    uint32_t load;
    double load_contrib;
    
    uint32_t quantum;
    
    double vruntime;
    double vruntime_coeff;
    
    uint64_t exec_ticks;
} sched_info;

#endif