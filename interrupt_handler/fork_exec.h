#ifndef FORK_EXEC_H
#define FORK_EXEC_H

#include <pthread.h>
#include <sys/types.h>

#include "../task/task_struct.h"
#include "../task/do_nothing.h"

static pid_t next_pid = 2; // Start from 2, as 0 and 1 are reserved for IDLE and INIT tasks

task_t fork_exec(const char *name);
task_t create_custom_task(pid_t pid, const char *name, void *(*__start_routine)(void *));

#endif