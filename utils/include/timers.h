/**
 * A small API for handling timer in linux (probably RIOT as well), 
 * inspired in the library by Nominal Animal at stackoverflow: 
 * https://stackoverflow.com/questions/12764771/timers-in-linux-in-c
*/

#ifndef TIMERS_H
#define TIMERS_H

#ifdef __LINUX__
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#endif

#ifdef __RIOT__
#include "ztimer.h"
#endif

#ifndef TIMEOUTS
#define TIMEOUTS        4
#endif

#ifdef __LINUX__
#define TIMEOUT_SIGNAL (SIGRTMIN+0)
#endif

#define TIMEOUT_USED    1
#define TIMEOUT_ARMED   2
#define TIMEOUT_PASSED  4

#ifdef __LINUX__
static timer_t                  timeout_timer;
static volatile sig_atomic_t    timeout_state[TIMEOUTS] = { 0 };
static struct timespec          timeout_time[TIMEOUTS];
static ztimer_clock_t           *clock;
#endif

#ifdef __RIOT__
static ztimer_t                 timeout_timer;
static volatile uint8_t         timeout_state[TIMEOUTS] = { 0 };
static uint32_t                 timeout_time[TIMEOUTS];
#endif

#ifdef __LINUX__
double timespec_diff(const struct timespec after, const struct timespec before);
#endif
#ifdef __RIOT__
uint32_t timespec_diff(const uint32_t after, const uint32_t before);
#endif

void timespec_add(
#ifdef __LINUX__
    struct timespec *const to, const double seconds
#endif
#ifdef __RIOT__
    uint32_t *const to, const uint32_t seconds
#endif
);

void timespec_set(
#ifdef __LINUX__
    struct timespec *const to, const double seconds
#endif
#ifdef __RIOT__
    uint32_t *const to, const uint32_t seconds
#endif
);

int timeout_passed(
#ifdef __LINUX__
    const int timeout
#endif
#ifdef __RIOT__
    const uint32_t timeout
#endif
);

int timeout_unset(
#ifdef __LINUX__
    const int timeout
#endif
#ifdef __RIOT__
    const uint32_t timeout
#endif
);

#ifdef __LINUX__
int timeout_set(const double seconds
#endif
#ifdef __RIOT__
uint32_t timeout_set(const uint32_t seconds
#endif
);

void timeout_signal_handler(
#ifdef __LINUX__
    int signum __attribute__((unused)), siginfo_t *info, void *context __attribute__((unused))
#endif
#ifdef __RIOT__
    void *arg
#endif
);

int timeout_init(
#ifdef __LINUX__
void);
#endif
#ifdef __RIOT__
ztimer_clock_t *clockA);
#endif

int timeout_done(void);

#endif      // TIMERS_H
