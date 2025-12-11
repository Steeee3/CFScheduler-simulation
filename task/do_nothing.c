#include "do_nothing.h"

void *do_nothing(void *arg)
{
    sched_info *task = (sched_info *)arg;
    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)pthread_self();

    while (1)
    {
        if (task->state != RUNNING)
        {
            sem_wait(&(task->running));
        }
        else if (task->load != 1)
        {
            int free_buffer = rand_r(&seed) % WAKEUP_FREQUENCY;

            if (free_buffer == 0)
            {
                wake_up();
            }

            int wait = rand_r(&seed) % WAIT_FREQUENCY;
            if (wait == 0)
            {
#ifdef DEBUG_THREAD
                printf("WAITING\n");
#endif
                task->state = WAITING;
            }
            else
            {
#ifdef DEBUG_THREAD
                printf("RUNNING\n");
#endif
            }
        }
        usleep(CPU_PERIOD_US);
    }
}