#include <string.h>
#include "helpers.h"

void *execute_rema(void *arg)
{
    uint32_t *qid = (uint32_t *)arg;
#ifdef __LINUX__
    pthread_t pid = pthread_self();
#endif
#ifdef __RIOT__
    kernel_pid_t pid = thread_getpid();
#endif
    (void) pid;
    (void) qid;
    REMA_t rema, *prema;
    uint64_t nid[2] = {1234567890, 9876543210};
    memcpy(rema._node_id, nid, 2 * sizeof(uint64_t));

    prema = &rema;
    rema_init(&prema);

    rema_dummy(prema);

    rema_terminate(&prema);
    return NULL;
}
