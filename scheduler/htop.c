#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "task_struct.h"

#define MAX_TASKS 10

pthread_mutex_t lock;
task_t *tasks;
int created_tasks = 0;

void *init();
void set_default_sched_info(sched_info *sched);

int main() {
    pthread_t init_thread;

    pthread_mutex_init(&lock, NULL);

    pthread_create(&init_thread, NULL, init, NULL);
    while(1) {
        // Move cursor to top-left corner
        printf("\033[H");

        printf(" PID  NAME  STATE      VRT       exec_ticks       \n");
        printf("--------------------------------------------      \n");

        pthread_mutex_lock(&lock);
        for (int i = 0; i < created_tasks; i++)
        {
            printf("%4d %-6s %-10s %-9.2f %-15lu\n",
                   tasks[i].pid,
                   tasks[i].name,
                   state_to_string(tasks[i].sched.state),
                   tasks[i].sched.vruntime,
                   tasks[i].sched.exec_ticks);
        }
        pthread_mutex_unlock(&lock);

        // Simulate a delay
        for (volatile long i = 0; i < 100000000; i++);
    }
    return 0;
}

void *init() {
    tasks = (task_t *) malloc(MAX_TASKS * sizeof(task_t));

    for (int i = 0; i < MAX_TASKS; i++)
    {
        tasks[i].pid = i + 1;
        snprintf(tasks[i].name, sizeof(tasks[i].name), "task%d", i + 1);

        set_default_sched_info(&tasks[i].sched);

        pthread_mutex_lock(&lock);
        created_tasks++;
        pthread_mutex_unlock(&lock);
    }
}

void set_default_sched_info(sched_info *sched) {
    sched->state = READY;
    sched->load = DEFAULT_LOAD;
    sched->load_contrib = 0.0;
    sched->quantum = 100;
    sched->vruntime = 0.0;
    sched->exec_ticks = 0;
}

const char* state_to_string(task_state_t state) {
    switch (state) {
        case RUNNING: return "RUNNING";
        case READY:   return "READY";
        case WAITING: return "WAITING";
        default:      return "UNKNOWN";
    }
}