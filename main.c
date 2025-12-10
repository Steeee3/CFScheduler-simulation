#include <pthread.h>
#include <sys/types.h>

#include "task/task_struct.h"
#include "interrupt_handler/fork_exec.h"
#include "./gui/htop.h"

#define MAX_TASKS 10

pthread_mutex_t cpu;

void boot();
void *create_N_tasks();
void start_gui();

int main() {
    boot();
    start_gui();

    return 0;
}

void start_gui() {
    pthread_t gui;

    pthread_create(&gui, NULL, htop, NULL);

    pthread_join(gui, NULL);
}

void boot() {
    create_custom_task(0, "IDLE", do_nothing);
    create_custom_task(1, "INIT", create_N_tasks);
}

void *create_N_tasks() {
    for (int i = 0; i < MAX_TASKS; i++) {
        char name[16];
        snprintf(name, sizeof(name), "task%d", i + 1);
        fork_exec(name);
    }
    do_nothing();
}