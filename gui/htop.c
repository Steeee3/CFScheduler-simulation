#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ncurses.h>
#include "htop.h"

volatile sig_atomic_t interrupted = 0;

//TODO: refactor
void cleanup(void)
{
    curs_set(1);
    echo();
    endwin();
}

void handle_sigint(int sig)
{
    (void)sig;
    interrupted = 1;
}

void *htop()
{

    //Register signal handler for SIGINT
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    atexit(cleanup);

    initscr();
    timeout(100);
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    if (has_colors())
    {
        start_color();
        use_default_colors();

        init_pair(1, COLOR_BLUE, -1);
        init_pair(2, COLOR_GREEN, -1);
        init_pair(3, COLOR_WHITE, -1);
    }

    while (!interrupted)
    {
        // erase();

        // mvprintw(0, 0, " PID  NAME  STATE      VRT       LOAD       exec_ticks");
        // mvprintw(1, 0, "-------------------------------------------------------");

        // pthread_mutex_lock(&lock);
        // for (int i = 0; i < created_tasks; i++)
        // {
        //     int row = 2 + i;

        //     const char *state_str = state_to_string(tasks[i].sched.state);

        //     move(row, 0);
        //     printw("%4d %-6s ", tasks[i].pid, tasks[i].name);

        //     if (has_colors())
        //     {
        //         if (tasks[i].sched.state == RUNNING)
        //             attron(COLOR_PAIR(1));
        //         else if (tasks[i].sched.state == READY)
        //             attron(COLOR_PAIR(2));
        //         else if (tasks[i].sched.state == WAITING)
        //             attron(COLOR_PAIR(3));
        //     }
        //     printw("%-10s", state_str);
        //     if (has_colors())
        //     {
        //         attroff(COLOR_PAIR(1));
        //         attroff(COLOR_PAIR(2));
        //         attroff(COLOR_PAIR(3));
        //     }

        //     printw(" %-9.2f %-10u %-15llu",
        //            tasks[i].sched.vruntime,
        //            tasks[i].sched.load,
        //            (unsigned long long) tasks[i].sched.exec_ticks);
        // }
        // pthread_mutex_unlock(&lock);

        // refresh();

        // int ch = getch();
        // if (ch == 'q' || ch == 'Q')
        // {
        //     break;
        // }
        printf("htop running...\n");
    }

    cleanup();
    return;
}

// void *init()
// {
//     tasks = (task_t *)malloc(MAX_TASKS * sizeof(task_t));

//     for (int i = 0; i < MAX_TASKS; i++)
//     {
//         tasks[i].pid = i + 1;
//         snprintf(tasks[i].name, sizeof(tasks[i].name), "task%d", i + 1);

//         set_default_sched_info(&tasks[i].sched);

//         pthread_mutex_lock(&lock);
//         created_tasks++;
//         pthread_mutex_unlock(&lock);
//     }
// }

// void set_default_sched_info(sched_info *sched)
// {
//     sched->state = READY;
//     sched->load = DEFAULT_LOAD;
//     sched->load_contrib = 0.0;
//     sched->quantum = 100;
//     sched->vruntime = 0.0;
//     sched->exec_ticks = 0;
// }

// const char *state_to_string(task_state_t state)
// {
//     switch (state)
//     {
//     case RUNNING:
//         return "RUNNING";
//     case READY:
//         return "READY";
//     case WAITING:
//         return "WAITING";
//     default:
//         return "UNKNOWN";
//     }
// }