#include <stdio.h>
#include <stdlib.h>

#include "utils_tests.h"
#include "memory_tests.h"

void utils_tests(void)
{
    printf("\nTesting the utils module.\n");

    memory_tests();

    return;
}