#ifndef UTILS_H
#define UTILS_H

// #define DEBUG_SCHEDULER
// #define DEBUG_THREAD

#define TIMER_PERIOD_US 100000 // us

#define CPU_PERIOD_US 100000 // us
#define WAIT_FREQUENCY 5     // 1 in n chances
#define WAKEUP_FREQUENCY 5   // 1 in n chances

#define LT 6.0 // latency: s
#define GR 7.5 // granularity: s
#define WGR 1  // wakeup granularity: s

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#endif