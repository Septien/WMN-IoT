/**
 * A small API for handling timer in linux (probably RIOT as well), 
 * inspired in the library by Nominal Animal at stackoverflow: 
 * https://stackoverflow.com/questions/12764771/timers-in-linux-c
*/

#ifndef TIMERS_H
#define TIMERS_H

#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#define TIMEOUTS        16
#define TIMEOUT_SIGNAL (SIGRTMIN+0)

#define TIMEOUT_USED    1
#define TIMEOUT_ARMED   2
#define TIMEOUT_PASSED  4

static timer_t                  timeout_timer;
static volatile sig_atomic_t    timeout_state[TIMEOUTS] = { 0 };
static struct timespec          timeout_time[TIMEOUTS];

inline double timespec_diff(const struct timespec after, const struct timespec before);
inline void timespec_add(struct timespec *const to, const double seconds);
inline void timespec_set(struct timespec *const to, const double seconds);
inline int timeout_passed(const int timeout);
inline int timeout_unset(const int timeout);
int timeout_set(const double seconds);
void timeout_signal_handler(int signum __attribute__((unused)), siginfo_t *info, void *context __attribute__((unused)));
int timeout_init(void);
int timeout_done(void);

#endif      // TIMERS_H