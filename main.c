#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>

#include "task/task_struct.h"
#include "interrupt_handler/fork_exec.h"
#include "interrupt_handler/tick.h"
#include "scheduler/scheduler.h"
#include "./gui/htop.h"
#include "utils/utils.h"

#define MAX_TASKS 10

pthread_mutex_t cpu;
sem_t boot_order;

void boot();
void *create_N_tasks(void *arg);
void start_gui();

int main()
{
    sem_init(&boot_order, 0, 0);
    boot();
    #ifndef DEBUG_THREAD
    start_gui();
    #endif

    return 0;
}

void start_gui()
{
    pthread_t gui;

    sem_wait(&boot_order);

    htop();
}

void boot()
{
    pthread_t tick_thread, start_scheduler;

    pthread_create(&tick_thread, NULL, tick, NULL);
    pthread_create(&start_scheduler, NULL, init_scheduler, NULL);

    pthread_join(start_scheduler, NULL);
    sem_post(&boot_order);

    create_custom_task(0, "IDLE", do_nothing, 1);
    create_custom_task(1, "INIT", create_N_tasks, DEFAULT_LOAD);

    schedule();

    #ifdef DEBUG_THREAD
    pthread_join(tick_thread, NULL);
    #endif
}

void *create_N_tasks(void *arg)
{
    for (int i = 0; i < MAX_TASKS; i++)
    {
        char name[16];

        snprintf(name, sizeof(name), "task%d", i + 1);
        fork_exec(name);
    }
    do_nothing(arg);
}