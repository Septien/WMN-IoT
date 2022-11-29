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

data_t rema_data;
request_t request;
#ifdef __LINUX__
pthread_mutex_t mtx_req;
pthread_mutex_t mtx_data;
#endif
#ifdef __RIOT__
mutex_t mtx_req;
mutex_t mtx_data;

char thread_stack[THREAD_STACKSIZE_MAIN];
#endif

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

#ifdef __LINUX__
    pthread_t pid;
    pthread_create(&pid, NULL, execute_rema, NULL);
    while (_mutex_lock(&mtx_req) != 1) ;
    request = STOP;
    _mutex_unlock(&mtx_req);
    pthread_join(pid, NULL);
#endif
#ifdef __RIOT__
    memset(thread_stack, 0, sizeof(thread_stack));
    kernel_pid_t pid = thread_create(thread_stack, sizeof(thread_stack), THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_SLEEPING,
    execute_rema, NULL, "REMA");
    int ret = 0;
    do {
        ret = _mutex_lock(&mtx_req);
    } while(ret != 1);
    request = STOP;
    _mutex_unlock(&mtx_req);
    thread_wakeup(pid);
#endif
    return true;
}

bool test_handle_get_nodeid_request(void *arg)
{
    (void) arg;

    while (_mutex_lock(&mtx_req) != 1) ;
    request = GET_NODEID;
    _mutex_unlock(&mtx_req);
#ifdef __LINUX__
    pthread_t pid;
    pthread_create(&pid, NULL, execute_rema, NULL);
    usleep(1000);
#endif
#ifdef __RIOT__
    memset(thread_stack, 0, sizeof(thread_stack));
    kernel_pid_t pid = thread_create(thread_stack, sizeof(thread_stack), THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_SLEEPING,
    execute_rema, NULL, "REMA2");
    thread_wakeup(pid);
#endif
    while (_mutex_lock(&mtx_req) != 1) ;
    request = STOP;
    _mutex_unlock(&mtx_req);
#ifdef __LINUX__
    pthread_join(pid, NULL);
#endif
#ifdef __RIOT__
    thread_wakeup(pid);
#endif
    // Verify node_id is correct
    uint64_t nid[2] = {0};
    while (_mutex_lock(&mtx_req) != 1) ;
    memcpy(nid, rema_data._node_id, 2*sizeof(uint64_t));
    _mutex_unlock(&mtx_data);
    uint64_t id[2] = {1234567890, 9876543210};

    return memcmp(nid, id, 2*sizeof(uint64_t)) == 0;;
}

void helpers_tests(void)
{
#ifdef __LINUX__
    pthread_mutex_init(&mtx_req, NULL);
    pthread_mutex_init(&mtx_data, NULL);
#endif
#ifdef __RIOT__
    mutex_init(&mtx_req);
    mutex_init(&mtx_data);
#endif
    _mutex_lock(&mtx_req);
    request = NONE;
    _mutex_unlock(&mtx_req);
    _mutex_lock(&mtx_data);
    memset(rema_data._node_id, 0, 2*sizeof(uint64_t));
    _mutex_unlock(&mtx_data);

    cUnit_t *tests;
    int data = 0;
    init_queues();

    cunit_init(&tests, &setup, &teardown, &data);

    cunit_add_test(tests, &test_execute_rema,               "execution of rema protocol\0");
    cunit_add_test(tests, &test_handle_get_nodeid_request,  "node_id request\0");

    printf("Testing the helpers function.\n");
    cunit_execute_tests(tests);

    cunit_terminate(&tests);

    end_queues();
}
