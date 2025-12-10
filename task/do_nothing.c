#include "do_nothing.h"

void *do_nothing() {
    for(;;) {
        usleep(CPU_PERIOD_US);
    }
}