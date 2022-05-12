#include <stdio.h>
#include "assert.h"

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
    printf("Setting timeouts:\ntimeout1 = %u, timeout2 = %u usec.\n", useconds1, useconds2);
#endif
    timeout1 = timeout_set(useconds1);
    timeout2 = timeout_set(useconds2);
    #ifdef __LINUX__
    printf("timeout1 = %d, timeout2 = %d\n", timeout1, timeout2);
#endif
#ifdef __RIOT__
    printf("timeout1 = %u, timeout2 = %u\n", timeout1, timeout2);
#endif

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

    timeout1 = timeout_set(useconds1);
    assert(timeout1 == 0);

    while (1)
    {
        if (timeout_passed(timeout1) == 1)
        {
            printf("Second test passed.\n");
            timeout_unset(timeout1);
            break;
        }
    }

    printf("Terminating timeouts.\n");
    timeout_done();
}

void timeouts_test(void)
{
    printf("\nTesting the timeouts API.\n");
    test_timeout();
    return;
}