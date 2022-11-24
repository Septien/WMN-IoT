#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "config.h"
#include "cUnit.h"
#include "REMA.h"
#include "ipc-queues.h"
#include "helpers.h"

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

    char *stack = NULL;
    uint32_t queue_size = QUEUE_SIZE;
    uint32_t msgs_allow = MAX_ELEMENTS_ON_QUEUE;
    uint32_t message_size = MAX_MESSAGE_SIZE;
    uint32_t qid1 = create_queue(queue_size, message_size, msgs_allow, &stack);

#ifdef __LINUX__
    pthread_t pid;
    pthread_create(&pid, NULL, execute_rema, (void *)&qid1);
    pthread_join(pid, NULL);
#endif
#ifdef __RIOT__
    kernel_pid_t pid = thread_create(stack, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
    execute_rema, (void *)&qid1, "REMA");
#endif
    return true;
}

void helpers_tests(void)
{
    cUnit_t *tests;
    int data = 0;
    init_queues();

    cunit_init(&tests, &setup, &teardown, &data);

    cunit_add_test(tests, &test_execute_rema, "execution of rema protocol\0");

    printf("Testing the helpers function.\n");
    cunit_execute_tests(tests);

    cunit_terminate(&tests);

    end_queues();
}
