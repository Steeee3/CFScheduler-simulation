#ifndef TASK_INFO_H
#define TASK_INFO_H

typedef enum task_state_t {
    RUNNING,
    READY,
    WAITING
} task_state_t;

const char* state_to_string(task_state_t state);

#endif