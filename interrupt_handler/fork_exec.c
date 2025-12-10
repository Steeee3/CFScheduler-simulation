#include "fork_exec.h"

task_t fork_exec(const char *name)
{
    pthread_t thread;
    task_t task = {0};

    task.pid = next_pid++;
    snprintf(task.name, sizeof(task.name), "%s", name);
    //TODO: schedule

    pthread_create(&thread, NULL, do_nothing, NULL);
    return task;
}

task_t create_custom_task(pid_t pid, const char *name, void *(*__start_routine)(void *)) {
    pthread_t thread;
    task_t task = {0};

    task.pid = pid;
    snprintf(task.name, sizeof(task.name), "%s", name);
    //TODO: schedule

    pthread_create(&thread, NULL, __start_routine, NULL);
    return task;
}