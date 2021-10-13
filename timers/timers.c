#include <stdio.h>
#include "timers.h"

#ifdef __RIOT__
#include "ztimer.h"

extern ztimer_t         timeout_timer;
extern uint32_t         timeout_time[TIMEOUTS];
extern ztimer_clock_t   *clock;
#endif

/**
 * Return the number of seconds between before and after (after - before).
 * This must be async-signal safe, so it cannot use difftime()
*/
#ifdef __LINUX__
double timespec_diff(const struct timespec after, const struct timespec before)
#endif
#ifdef __RIOT__
uint32_t timespec_diff(const uint32_t after, const uint32_t before)
#endif
{
#ifdef __LINUX__
    return (double)(after.tv_sec - before.tv_sec)
            + ((double)(after.tv_nsec - before.tv_nsec) / 1000000000.0); 
#endif
#ifdef __RIOT__
    return after - before;
#endif
}

/**
 * Add positive seconds to a timespec, nothing if seconds is negative.
 * This must be async-signal safe.
*/
void timespec_add(
#ifdef __LINUX__
    struct timespec *const to, const double seconds
#endif
#ifdef __RIOT__
    uint32_t *const to, const uint32_t seconds
#endif
)
{
    if (to && seconds > 0.0)
    {
#ifdef __LINUX__
        long s = (long) seconds;
        long ns = (long)(0.5 + 1000000000.0 * (seconds - (double)s));

        /* Adjust for rounding errors */
        if (ns < 0L)
            ns = 0L;
        else
        if (ns > 999999999L)
            ns = 999999999L;

        to->tv_sec += (time_t)s;
        to->tv_nsec += ns;

        if (to ->tv_nsec >= 1000000000L)
        {
            to->tv_nsec -= 1000000000L;
            to->tv_sec++;
        }
#endif
#ifdef __RIOT__
        /* For RIOT, seconds represent 'microseconds', as that is the selected 
        clock. */
        *to += seconds;
#endif
    }
}

/**
 * Set the timespec to the specified number of seconds, or zero if negative seconds.
*/
void timespec_set(
#ifdef __LINUX__
    struct timespec *const to, const double seconds
#endif
#ifdef __RIOT__
    uint32_t *const to, const uint32_t seconds
#endif
)
{
    if (to)
    {
        if (seconds > 0.0)
        {
#ifdef __LINUX__
            const long s = (long)seconds;
            long      ns = (long)(0.5 + 1000000000.0 * (seconds - (double)s));

            if (ns < 0L)
            ns = 0L;
            else
            if (ns > 999999999L)
                ns = 999999999L;
            
            to->tv_sec = (time_t)s;
            to->tv_nsec = ns;
#endif
#ifdef __RIOT__
            /* For RIOT, seconds represent 'microseconds', as that is the selected 
            clock. */
            *to = seconds;
#endif
        }
        else 
        {
#ifdef __LINUX__
            to->tv_sec = (time_t)0;
            to->tv_nsec = 0L;
#endif
#ifdef __RIOT__
            *to = 0;
#endif
        }
    }
}

/**
 * Return nonzero value if timeout has occurred.
*/
int timeout_passed(
#ifdef __LINUX__
    const int timeout
#endif
#ifdef __RIOT__
    const uint32_t timeout
#endif
)
{
    if (timeout < TIMEOUTS)
    {
#ifdef __LINUX__
        if (timeout >= 0)
            const int state = __sync_or_and_fetch(&timeout_state[timeout], 0);
#endif
#ifdef __RIOT__
        const int state = __sync_or_and_fetch(&timeout_state[timeout], 0);
#endif

        /* Refers to an unused timeout? */
        if (!(state & TIMEOUT_USED))
            return -1;

        /* Not armed? */
        if (!(state & TIMEOUT_ARMED))
            return -1;

        /* Return 1 if timeout passed, 0 otherwise */
        return (state & TIMEOUT_PASSED) ? 1 : 0;
    }
    else
    {
        /* Invalid timeout number */
        return -1;
    }
}

/**
 * Release the timeout.
 * Returns 0 if the timeout had not fired yet, 1 if it had.
*/
int timeout_unset(
#ifdef __LINUX__
    const int timeout
#endif
#ifdef __RIOT__
    const uint32_t timeout
#endif
)
{
    if (timeout < TIMEOUTS)
    {
        /** Obtain the current timeout state to 'state',
         * the clear all but the TIMEOUT_PASSED flag
         * for the specified timeout.
        */
#ifdef __LINUX__
        if (timeout >= 0)
            const int state = __sync_fetch_and_and(&timeout_state[timeout], TIMEOUT_PASSED);
#endif
#ifdef __RIOT__
        const int state = __sync_fetch_and_and(&timeout_state[timeout], TIMEOUT_PASSED);
#endif

        /* Invalid timeout? */
        if (!(state & TIMEOUT_USED))
            return -1;
        
        /* Not armed? */
        if (!(state & TIMEOUT_ARMED))
            return -1;

        /* Return 1 if passed, 0 otherwise */
        return (state & TIMEOUT_PASSED) ? 1 : 0;
    }
    else
    {
        /* Invalid timeout */
        return -1;
    }
}

#ifdef __LINUX__
int timeout_set(const double seconds)
#endif
#ifdef __RIOT__
uint32_t timeout_set(const uint32_t seconds)
#endif
{
#ifdef __LINUX__
    struct timespec     now, then;
    struct itimerspec   when;
    double              next;
    int                 timeout, i;
#endif
#ifdef __RIOT__
    uint32_t            now, then;
    uint32_t            when;
    uint32_t            next;
    uint32_t            timeout, i;
#endif

    /* Timeout must be in the future. */
    if (seconds <= 0.0)
        return -1;

    /* Get the current time, */
#ifdef __LINUX__
    if (clock_gettime(CLOCK_REALTIME, &now))
        return -1;
#endif
#ifdef __RIOT__
    now = ztimer_now(clock);
#endif

    /* and calculate when the timeout should be fired. */
    then = now;
    timespec_add(&then, seconds);

    /* Find an unused timeout. */
    for (timeout = 0; timeout < TIMEOUTS; timeout++)
        if(!(__sync_fetch_and_or(&timeout_state[timeout], TIMEOUT_USED) & TIMEOUT_USED))
            break;
    
    /* No unused timeouts? */
    if (timeout >= TIMEOUTS)
        return -1;

    /* Clear all but the TIMEOUT_USED from the state, */
    __sync_and_and_fetch(&timeout_state[timeout], TIMEOUT_USED);

    /* update the timeout details, */
    timeout_time[timeout] = then;

    /* and mark the timeout armeable. */
    __sync_or_and_fetch(&timeout_state[timeout], TIMEOUT_ARMED);

    /* How long till the next timeout? */
    next = seconds;
    for (i = 0; i < TIMEOUTS; i++)
        if ((__sync_fetch_and_or(&timeout_state[i], 0) & (TIMEOUT_USED | TIMEOUT_ARMED | TIMEOUT_PASSED)) == (TIMEOUT_USED | TIMEOUT_ARMED))
        {
#ifdef __LINUX__
            const double secs = timespec_diff(timeout_time[i], now);
#endif
#ifdef __RIOT__
            const uint32_t secs = timespec_diff(timeout_time[i], now);
#endif
            if (secs >= 0.0 && secs < next)
                next = secs;
        }

    /* Calculate the duration when to fire the timeout next, */
#ifdef __LINUX__
    timespec_set(&when.it_value, next);
    when.it_interval.tv_sec = 0;
    when.it_interval.tv_nsec = 0L;
#endif
#ifdef __RIOT__
    timespec_set(&when, next);
#endif

    /* and arm the timer. */
#ifdef __LINUX__
    if (timer_settime(timeout_timer, 0, &when, NULL))
    {
        /* Failed. */
        __sync_and_and_fetch(&timeout_state[timeout], 0);
        return -1;
    }
#endif
#ifdef __RIOT__
    ztimer_set(clock, &timeout_timer, when);
#endif

    /* Return timeout number. */
    return timeout;
}

void timeout_signal_handler(
#ifdef __LINUX__
    int signum __attribute__((unused)), siginfo_t *info, void *context __attribute__((unused))
#endif
#ifdef __RIOT__
    void *arg
#endif
)
{
#ifdef __LINUX__
    struct timespec     now;
    struct itimerspec   when;
    int                 saved_errno, i;
    double              next;

    /* Not a timer signal? */
    if (!info || info->si_code != SI_TIMER)
        return;

    /* Save errno; some of the functions used may modify errno. */
    saved_errno = errno;

    if (clock_gettime(CLOCK_REALTIME, &now))
    {
        errno = saved_errno;
        return;
    }

    /* Assume no next timeout. */
    next = -1.0;

    /* Check all the timeouts that are used and armed, but not passed yet. */
    for (i = 0; i < TIMEOUTS; i++)
        if ((__sync_or_and_fetch(&timeout_state[i], 0) & (TIMEOUT_USED | TIMEOUT_ARMED | TIMEOUT_PASSED)) == (TIMEOUT_USED | TIMEOUT_ARMED))
        {
            const double seconds = timespec_diff(timeout_time[i], now);
            if (seconds <= 0.0)
            {
                /* Timeout [i] fires! */
                __sync_or_and_fetch(&timeout_state[i], TIMEOUT_PASSED);
            }
            else
            if (next <= 0.0 || seconds < next)
            {
                /* This is the soonest timeout in the future. */
                next = seconds;
            }
        }
    
    /**
     * Note: timespec_set() will return the time to zero if next <= 0.0,
     *          which in turn will disarm the timer.
     * The timer is one-shot; it_interval == 0.
    */
   timespec_set(&when.it_value, next);
   when.it_interval.tv_sec = 0;
   when.it_interval.tv_nsec = 0L;
   timer_settime(timeout_timer, 0, &when, NULL);

   /* Restore errno. */
   errno = saved_errno;
#endif
#ifdef __RIOT__
    (void) arg;
    uint32_t    now;
    uint32_t    when;
    uint32_t    i;
    uint32_t    next;

    now = ztimer_now(clock);

    /* Assume no next timeout. */
    next = 0.0;

    /* Check all the timeouts that are used and armed, but not passed yet. */
    for (i = 0; i < TIMEOUTS; i++)
        if ((__sync_or_and_fetch(&timeout_state[i], 0) & (TIMEOUT_USED | TIMEOUT_ARMED | TIMEOUT_PASSED)) == (TIMEOUT_USED | TIMEOUT_ARMED))
        {
            const uint32_t seconds = timespec_diff(timeout_time[i], now);
            if (seconds <= 0.0)
            {
                /* Timeout [i] fires! */
                __sync_or_and_fetch(&timeout_state[i], TIMEOUT_PASSED);
            }
            else
            if (next <= 0.0 || seconds < next)
            {
                /* This is the soonest timeout in the future. */
                next = seconds;
            }
        }

     /**
     * Note: timespec_set() will return the time to zero if next <= 0.0,
     *          which in turn will disarm the timer.
     * The timer is one-shot; it_interval == 0.
    */
   timespec_set(&when, next);
   ztimer_set(clock, &timeout_timer, when);
#endif
}

int timeout_init(
#ifdef __LINUX__
void)
#endif
#ifdef __RIOT__
ztimer_clock_t *clockA)
#endif
{
#ifdef __LINUX__
    struct sigaction    act;
    struct sigevetn     evt;
    struct itimerspec   arm;

    /* Install timeout signal_handler. */
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = timeout_signal_handler;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(TIMEOUT_SIGNAL, &act, NULL))
        return errno;

    /* Create a timer that will signal to timeout_signal_handler. */
    evt.sigev_notify = SIGEV_SIGNAL;
    evt.sigev_signo = TIMEOUT_SIGNAL;
    evt.sifev_value.sival_ptr = NULL;
    if (timer_create(CLOCK_REALTIME, &evt, &timeout_timer))
        return errno;

    /* Disarm the timeout timer (for now). */
    arm.it_value.tv_sec = 0;
    arm.it_value.tv_nsec = 0L;
    arm.it_interval.tv_sec = 0;
    arm.it_interval.tv_nsec = 0L;
    if (timer_settime(timeout_timer, 0, &arm, NULL))
        return errno;
#endif
#ifdef __RIOT__
    /* Initialize board-specific ztimer configuration */
    ztimer_init();
    clock = clockA;

    /* Set callback and arguments */
    timeout_timer.callback = timeout_signal_handler;
    timeout_timer.arg = NULL;

    /* Disarm timer */
    uint32_t arm = 0;
    ztimer_set(clock, &timeout_timer, arm);
#endif
    return 0;
}

int timeout_done(void)
{
#ifdef __LINUX__
    struct sigaction    act;
    struct itimerspec   arm;
    int                 errors = 0;

    /* Ignore the timeout signals. */
    sigemptyset(&act.sa_mask);
    act.sa_handler = SIG_IGN;
    if (sigaction(TIMEOUT_SIGNAL, &act, NULL))
        if (!errors) errors = errno;

    /* Disarm any current timeouts. */
    arm.it_value.tv_sec = 0;
    arm.it_value.tv_nsec = 0L;
    arm.it_interval.tv_sec = 0;
    arm.it_interval.tv_nsec = 0L;
    if (timer_settime(timeout_timer, 0, &arm, NULL))
        if (!errors) errors = errno;

    /* Destroy the timer itself. */
    if (timer_delete(timeout_timer))
        if (!errors) errors = errno;
    
    /* If any error ocurred, set errno. */
    if (errors)
        errno = errors;

    /* Return 0 if success, errno otherwise. */
    return errors;
#endif
#ifdef __RIOT__
    ztimer_remove(clock, &timeout_timer);
    return 0;
#endif
}
