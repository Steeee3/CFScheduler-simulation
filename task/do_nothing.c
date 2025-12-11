#include "do_nothing.h"

void *do_nothing()
{
    while (1)
    {
        usleep(CPU_PERIOD_US);
    }
}