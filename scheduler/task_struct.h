#ifndef TASK_STRUCT_H
#define TASK_STRUCT_H

#include <sys/types.h>
#include "sched_info.h"

typedef struct task_struct {
    pid_t pid;
    char name[16];
    
    sched_info sched;
} task_t;

#endif