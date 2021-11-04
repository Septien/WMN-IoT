#include <stdio.h>

#include "timeouts.h"

void test_timeout(void)
{
    timeout_init();

#ifdef __LINUX__
    int timeout1, timeout2;
    double useconds1 = 2, useconds2 = 5;
#endif
#ifdef __RIOT__
    uint32_t timeout1, timeout2;
    uint32_t useconds1 = 1000000, useconds2 = 5000000;
#endif

#ifdef __LINUX__
    printf("Setting timeouts:\ntimeout1 = %f, timeout2 = %f usec.\n", useconds1, useconds2);
#endif
#ifdef __RIOT__
    printf("Setting timeouts:\ntimeout1 = %d, timeout2 = %d usec.\n", useconds1, useconds2);
#endif
    timeout1 = timeout_set(useconds1);
    timeout2 = timeout_set(useconds2);
    printf("timeout1 = %d, timeout2 = %d\n", timeout1, timeout2);

    while (1)
    {
        if (timeout_passed(timeout1) == 1)
        {
            printf("Timeout1 passed. Removing\n");
            timeout_unset(timeout1);
        }

        if (timeout_passed(timeout2) == 1)
        {
            printf("Timeout2 passed. Removing and Breaking.\n");
            timeout_unset(timeout2);
            break;
        }
    }

    printf("Terminating timeouts.\n");
    timeout_done();
}

void timeouts_test(void)
{
    printf("\nTestint the timeouts API.\n");
    test_timeout();
    return;
}