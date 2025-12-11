#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#include "../interrupt_handler/tick.h"
#include "../task/task_struct.h"
#include "../utils/utils.h"

#define LT 6.0 // latency: s
#define GR 7.5 // granularity: s
#define WGR 1  // wakeup granularity: s

#define QUANTUM(ready_tasks, load_contrib) ((uint32_t)ceil(PER(ready_tasks) * load_contrib))

typedef struct sched_task
{
    task_t task;
    struct sched_task *next;
    struct sched_task *prev;
} sched_task;

typedef struct queue
{
    sched_task *head;
    sched_task *tail;
} queue;

typedef struct runqueue
{
    queue q;

    int ready_tasks;
    uint64_t total_load;
    double vruntime_min;
} runqueue;

typedef struct scheduler
{
    runqueue *running_queue;
    sched_task *current;
} scheduler;

extern scheduler *sched;
extern pthread_mutex_t runqueue_lock;

static inline double PER(int ready_tasks)
{
    return MAX(LT, ready_tasks * GR);
}

void init_scheduler();
void calc_created_sched_info(task_t *task, uint32_t load);
void enqueue_task(task_t task);
sched_task *dequeue_task();
void schedule();
void task_tick();

#endif