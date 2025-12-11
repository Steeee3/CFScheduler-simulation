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
        else
        {
            int waiting_chance = rand_r(&seed) % 5;
            if (waiting_chance == 0)
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