#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
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

    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    if (has_colors()) {
        start_color();
        use_default_colors();

        init_pair(1, COLOR_BLUE, -1);
        init_pair(2, COLOR_GREEN, -1);
        init_pair(3, COLOR_WHITE, -1);
    }

    while(1) {
        erase();

        mvprintw(0, 0, " PID  NAME  STATE      VRT       exec_ticks");
        mvprintw(1, 0, "--------------------------------------------");

        pthread_mutex_lock(&lock);
        for (int i = 0; i < created_tasks; i++)
        {
            int row = 2 + i;

            const char *state_str = state_to_string(tasks[i].sched.state);

            if (has_colors()) {
                if (tasks[i].sched.state == RUNNING) attron(COLOR_PAIR(1));
                else if (tasks[i].sched.state == READY) attron(COLOR_PAIR(2));
                else if (tasks[i].sched.state == WAITING) attron(COLOR_PAIR(3));
            }

            mvprintw(row, 0, "%4d %-6s %-10s %-9.2f %-15lu",
                     tasks[i].pid,
                     tasks[i].name,
                     state_str,
                     tasks[i].sched.vruntime,
                     tasks[i].sched.exec_ticks);

            if (has_colors()) {
                attroff(COLOR_PAIR(1));
                attroff(COLOR_PAIR(2));
                attroff(COLOR_PAIR(3));
            }
        }
        pthread_mutex_unlock(&lock);

        refresh();

        // Simulate a delay
        for (volatile long i = 0; i < 100000000; i++);
    }

    endwin();
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