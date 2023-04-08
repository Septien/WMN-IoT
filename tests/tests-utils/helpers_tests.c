#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "cUnit.h"
#include "REMA.h"
#include "ipc-queues.h"
#include "helpers.h"
#include "printbinary.h"

static void setup(void *arg)
{
    (void) arg;
}

static void teardown(void *arg)
{
    (void) arg;
}

bool test_execute_rema(void *arg)
{
    (void) arg;

    return true;
}

void helpers_tests(void)
{
    cUnit_t *tests;
    int data = 0;
    init_queues();

    cunit_init(&tests, &setup, &teardown, &data);

    cunit_add_test(tests, &test_execute_rema,               "execution of rema protocol\0");

    printf("Testing the helpers function.\n");
    cunit_execute_tests(tests);

    cunit_terminate(&tests);

    end_queues();
}
