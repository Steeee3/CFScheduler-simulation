#include "scheduler.h"

scheduler *sched = NULL;
pthread_mutex_t runqueue_lock = PTHREAD_MUTEX_INITIALIZER;

void init_scheduler()
{
    sched = (scheduler *)malloc(sizeof(scheduler));

    sched->current = NULL;

    sched->running_queue = (runqueue *)malloc(sizeof(runqueue));
    sched->running_queue->q.head = NULL;
    sched->running_queue->q.tail = NULL;
    sched->running_queue->ready_tasks = 0;
    sched->running_queue->total_load = 0;
    sched->running_queue->vruntime_min = 0.0;

    sched->waiting_queue = (waitqueue *)malloc(sizeof(waitqueue));
    sched->waiting_queue->q.head = NULL;
    sched->waiting_queue->q.tail = NULL;
    sched->waiting_queue->waiting_tasks = 0;
}

void calc_created_sched_info(task_t *task, uint32_t load)
{
    task->sched.state = READY;
    task->sched.load = load;

    task->sched.load_contrib = (double)task->sched.load / (double)(sched->running_queue->total_load + load);
    task->sched.quantum = QUANTUM(sched->running_queue->ready_tasks + 1, task->sched.load_contrib);

    task->sched.vruntime_coeff = 1 / (double)load;
    task->sched.vruntime = sched->running_queue->vruntime_min + task->sched.quantum * task->sched.vruntime_coeff;

#ifdef DEBUG_SCHEDULER
    printf("NEW TASK: nrt=%d, load=%u, total_load=%u, LT=%.6f, GR=%.6f, PER=%.6f, "
           "contrib=%.6f, quantum=%u, coeff=%.9f, vruntime=%.9f\n",
           sched->running_queue->ready_tasks + 1,
           load,
           sched->running_queue->total_load + load,
           (double)LT,
           (double)GR,
           PER(sched->running_queue->ready_tasks + 1),
           task->sched.load_contrib,
           task->sched.quantum,
           task->sched.vruntime_coeff,
           task->sched.vruntime);
#endif

    task->sched.exec_ticks = 0;
    task->sched.delta = 0;

    sem_init(&(task->sched.running), 0, 0);
}

void order_new_task()
{
    if (sched->running_queue->ready_tasks <= 1)
    {
        return;
    }

    for (sched_task *t = sched->running_queue->q.tail; t != sched->running_queue->q.head;)
    {
        sched_task *prev = t->prev;

        if (t->task->sched.vruntime >= prev->task->sched.vruntime)
        {
            return;
        }

        if (prev == sched->running_queue->q.head)
        {
            sched->running_queue->q.head = t;
        }
        else
        {
            prev->prev->next = t;
        }
        prev->next = t->next;
        t->next = prev;

        if (t == sched->running_queue->q.tail)
        {
            sched->running_queue->q.tail = prev;
        }
        else
        {
            prev->next->prev = prev;
        }
        t->prev = prev->prev;
        prev->prev = t;
    }
}

void recompute_rq_tasks_parameters()
{
    for (sched_task *t = sched->running_queue->q.head; t != NULL; t = t->next)
    {
        t->task->sched.load_contrib = (double)t->task->sched.load / (double)sched->running_queue->total_load;
        t->task->sched.quantum = QUANTUM(sched->running_queue->ready_tasks, t->task->sched.load_contrib);

#ifdef DEBUG_SCHEDULER
        printf("RECALC: contrib=%.6f, quantum=%u\n", t->task->sched.load_contrib, t->task->sched.quantum);
#endif
    }
}

void enqueue_task(task_t *task)
{
    pthread_mutex_lock(&runqueue_lock);

    sched_task *new_rq_node = (sched_task *)malloc(sizeof(sched_task));

    new_rq_node->task = task;
    new_rq_node->next = NULL;
    new_rq_node->prev = NULL;

    if (sched->running_queue->ready_tasks == 0)
    {
        sched->running_queue->q.head = new_rq_node;
        sched->running_queue->q.tail = new_rq_node;

        new_rq_node->next = NULL;
        new_rq_node->prev = NULL;

        sched->running_queue->vruntime_min = task->sched.vruntime;
    }
    else
    {
        sched->running_queue->q.tail->next = new_rq_node;
        new_rq_node->prev = sched->running_queue->q.tail;
        new_rq_node->next = NULL;

        sched->running_queue->q.tail = new_rq_node;
    }
    sched->running_queue->ready_tasks++;
    sched->running_queue->total_load += task->sched.load;
    order_new_task();
    recompute_rq_tasks_parameters();

    pthread_mutex_unlock(&runqueue_lock);
}

sched_task *dequeue_task()
{
    pthread_mutex_lock(&runqueue_lock);

    if (sched->running_queue->ready_tasks == 0)
    {
        pthread_mutex_unlock(&runqueue_lock);
        return NULL;
    }

    sched_task *node_to_dequeue = sched->running_queue->q.head;
    sched_task *new_head = node_to_dequeue->next;

    if (new_head == NULL)
    {
        sched->running_queue->q.head = NULL;
        sched->running_queue->q.tail = NULL;
    }
    else
    {
        new_head->prev = NULL;
        sched->running_queue->q.head = new_head;
    }

    sched->running_queue->ready_tasks--;
    sched->running_queue->total_load -= node_to_dequeue->task->sched.load;

    recompute_rq_tasks_parameters();

    pthread_mutex_unlock(&runqueue_lock);
    return node_to_dequeue;
}

void enqueue_waiting_task(task_t *task)
{
    pthread_mutex_lock(&runqueue_lock);

    sched_task *new_wq_node = (sched_task *)malloc(sizeof(sched_task));

    new_wq_node->task = task;
    new_wq_node->next = NULL;
    new_wq_node->prev = NULL;

    if (sched->waiting_queue->waiting_tasks == 0)
    {
        sched->waiting_queue->q.head = new_wq_node;
        sched->waiting_queue->q.tail = new_wq_node;

        new_wq_node->next = NULL;
        new_wq_node->prev = NULL;
    }
    else
    {
        sched->waiting_queue->q.tail->next = new_wq_node;
        new_wq_node->prev = sched->waiting_queue->q.tail;
        new_wq_node->next = NULL;

        sched->waiting_queue->q.tail = new_wq_node;
    }
    sched->waiting_queue->waiting_tasks++;

    pthread_mutex_unlock(&runqueue_lock);
}

sched_task *dequeue_waiting_task()
{
    pthread_mutex_lock(&runqueue_lock);

    if (sched->waiting_queue->waiting_tasks == 0)
    {
        pthread_mutex_unlock(&runqueue_lock);
        return NULL;
    }

    sched_task *node_to_dequeue = sched->waiting_queue->q.head;
    sched_task *new_head = node_to_dequeue->next;

    if (new_head == NULL)
    {
        sched->waiting_queue->q.head = NULL;
        sched->waiting_queue->q.tail = NULL;
    }
    else
    {
        new_head->prev = NULL;
        sched->waiting_queue->q.head = new_head;
    }

    sched->waiting_queue->waiting_tasks--;

    pthread_mutex_unlock(&runqueue_lock);
    return node_to_dequeue;
}

void context_switch(sched_task *task_to_run)
{
    if (sched->current != NULL)
    {
        sched->current->task->sched.delta = 0;
        if (sched->current->task->sched.state == RUNNING)
        {
            sched->current->task->sched.state = READY;
            enqueue_task(sched->current->task);
        }
        else
        {
            enqueue_waiting_task(sched->current->task);
        }
    }

    sched->current = task_to_run;
    sched->current->task->sched.state = RUNNING;
    sem_post(&(sched->current->task->sched.running));
}

void schedule()
{
    if (sched->running_queue->ready_tasks == 0)
    {
        sched->current = NULL;
        return;
    }

    if (sched->current == NULL || sched->current->task->sched.state == WAITING)
    {
        sched_task *task_to_run = dequeue_task();
        context_switch(task_to_run);
    }
}

void wake_up()
{
    if (sched->waiting_queue->waiting_tasks == 0)
    {
        return;
    }

    sched_task *ready_task = dequeue_waiting_task();

    double current_vruntime = sched->running_queue->q.head != NULL ? MIN(sched->current->task->sched.vruntime, sched->running_queue->q.head->task->sched.vruntime)
                                                                   : sched->current->task->sched.vruntime;
    sched->running_queue->vruntime_min = MAX(sched->running_queue->vruntime_min, current_vruntime);
    ready_task->task->sched.vruntime = MAX(ready_task->task->sched.vruntime, sched->running_queue->vruntime_min - LT / 2);

    if (ready_task->task->sched.vruntime + WGR * ready_task->task->sched.load_contrib < sched->running_queue->vruntime_min)
    {
        context_switch(ready_task);
    }
    else
    {
        ready_task->task->sched.state = READY;
        enqueue_task(ready_task->task);
    }
}

void task_tick()
{
    if (sched == NULL || sched->current == NULL)
    {
        return;
    }

    sched->current->task->sched.exec_ticks++;
    sched->current->task->sched.delta++;
    sched->current->task->sched.vruntime += sched->current->task->sched.vruntime_coeff;

    if (sched->running_queue->ready_tasks == 0)
    {
        return;
    }

    if (sched->current->task->sched.delta >= sched->current->task->sched.quantum || sched->current->task->sched.state == WAITING)
    {
        sched_task *task_to_run = dequeue_task();
        context_switch(task_to_run);
#ifdef DEBUG_THREAD
        printf("--------------------------");
        printf("THREAD %d\n", sched->current->task->pid);
#endif
    }

    double current_vruntime = sched->running_queue->q.head != NULL ? MIN(sched->current->task->sched.vruntime, sched->running_queue->q.head->task->sched.vruntime)
                                                                   : sched->current->task->sched.vruntime;
    sched->running_queue->vruntime_min = MAX(sched->running_queue->vruntime_min, current_vruntime);
}