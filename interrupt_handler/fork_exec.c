#include "fork_exec.h"

task_t *fork_exec(const char *name)
{
    pthread_t thread;
    task_t *task = (task_t *)malloc(sizeof(task_t));

    task->pid = next_pid++;
    snprintf(task->name, sizeof(task->name), "%s", name);
    calc_created_sched_info(task, DEFAULT_LOAD);

    enqueue_task(task);
    // TODO: schedule

    pthread_create(&thread, NULL, do_nothing, (void *)&(task->sched));
    return task;
}

task_t *create_custom_task(pid_t pid, const char *name, void *(*__start_routine)(void *), uint32_t load)
{
    pthread_t thread;
    task_t *task = (task_t *)malloc(sizeof(task_t));

    task->pid = pid;
    snprintf(task->name, sizeof(task->name), "%s", name);
    calc_created_sched_info(task, load);

    enqueue_task(task);
    // TODO: schedule

    pthread_create(&thread, NULL, __start_routine, (void *)&(task->sched));
    return task;
}