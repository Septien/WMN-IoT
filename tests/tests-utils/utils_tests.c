#include <stdio.h>
#include <stdlib.h>

#include "utils_tests.h"
#ifdef __RIOT__
#include "memory_tests.h"
#endif
#include "timeouts_test.h"

void utils_tests(void)
{
    printf("\nTesting the utils module.\n");

#ifdef __RIOT__
    memory_tests();
#endif

    timeouts_test();

    return;
}