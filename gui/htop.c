#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ncurses.h>

#include "htop.h"

volatile sig_atomic_t interrupted = 0;

// TODO: refactor
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

    // Register signal handler for SIGINT
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
        erase();

        mvprintw(0, 0, " PID  NAME  STATE      VRT       LOAD       Q       exec_ticks       slice_ticks");
        mvprintw(1, 0, "---------------------------------------------------------------------------------");

        int row = 2;

        if (sched->current != NULL)
        {
            move(row, 0);
            printw("%4d %-6s ", sched->current->task->pid, sched->current->task->name);

            attron(COLOR_PAIR(1));
            printw("%-10s", "RUNNING");
            attroff(COLOR_PAIR(1));

            printw(" %-9.6f %-10u %-10u %-15llu %-10llu",
                   sched->current->task->sched.vruntime,
                   sched->current->task->sched.load,
                   sched->current->task->sched.quantum,
                   (unsigned long long)sched->current->task->sched.exec_ticks,
                   (unsigned long long)sched->current->task->sched.delta);
            row++;
        }

        for (sched_task *t = sched->running_queue->q.head; t != NULL; t = t->next)
        {
            const char *state_str = state_to_string(t->task->sched.state);

            move(row, 0);
            printw("%4d %-6s ", t->task->pid, t->task->name);

            if (has_colors())
            {
                if (t->task->sched.state == READY)
                    attron(COLOR_PAIR(2));
                else if (t->task->sched.state == WAITING)
                    attron(COLOR_PAIR(3));
            }
            printw("%-10s", state_str);
            if (has_colors())
            {
                attroff(COLOR_PAIR(1));
                attroff(COLOR_PAIR(2));
                attroff(COLOR_PAIR(3));
            }

            printw(" %-9.6f %-10u %-10u %-15llu /",
                   t->task->sched.vruntime,
                   t->task->sched.load,
                   t->task->sched.quantum,
                   (unsigned long long)t->task->sched.exec_ticks);

            row++;
        }
        for (sched_task *t = sched->waiting_queue->q.head; t != NULL; t = t->next)
        {
            const char *state_str = state_to_string(t->task->sched.state);

            move(row, 0);
            printw("%4d %-6s ", t->task->pid, t->task->name);

            if (has_colors())
            {
                if (t->task->sched.state == WAITING)
                    attron(COLOR_PAIR(3));
            }
            printw("%-10s", state_str);
            if (has_colors())
            {
                attroff(COLOR_PAIR(1));
                attroff(COLOR_PAIR(2));
                attroff(COLOR_PAIR(3));
            }

            printw(" %-9.6f %-10u %-10u %-15llu /",
                   t->task->sched.vruntime,
                   t->task->sched.load,
                   t->task->sched.quantum,
                   (unsigned long long)t->task->sched.exec_ticks);

            row++;
        }
        move(row + 1, 0);
        printw("Ticks: %lld", ticks_count);

        refresh();

        int ch = getch();
        if (ch == 'q' || ch == 'Q')
        {
            break;
        }
    }

    cleanup();
    return;
}

const char *state_to_string(task_state_t state)
{
    switch (state)
    {
    case RUNNING:
        return "RUNNING";
    case READY:
        return "READY";
    case WAITING:
        return "WAITING";
    default:
        return "UNKNOWN";
    }
}