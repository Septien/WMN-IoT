#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ipc-queues.h"

#ifdef __RIOT__
extern char stack[MAX_QUEUES * THREAD_STACKSIZE_DEFAULT];
extern msg_t queue[MAX_QUEUES * QUEUE_SIZE];
#endif

static IPC_Queues_t Queues;

void init_queues(void)
{
#ifdef __RIOT__
    memset(stack, 0, sizeof(char) * MAX_QUEUES * THREAD_STACKSIZE_DEFAULT);
    memset(queue, 0, sizeof(msg_t) * MAX_QUEUES * QUEUE_SIZE);
#endif
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        Queue_t *q = &Queues.queues[i];
        q->queue_id = 0;
        q->queue_size = 0;
        q->message_size = 0;
        q->msgs_allow = 0;
        q->msgs_on_queue = 0;
#ifdef __RIOT__
        q->stack = NULL;
        q->queue = NULL;
        Queues.free_stack = stack;
        Queues.free_queue = queue;
#endif
        Queues.last_queue_id = 1;
    }
}

uint32_t create_queue(size_t max_queue_size, size_t message_size, uint32_t msgs_allow, char *stack)
{
    (void) max_queue_size;
    (void) message_size;
    (void) msgs_allow;
    (void) stack;
    return 0;
}

#ifdef TESTING
IPC_Queues_t *get_queues_pointer(void)
{
    IPC_Queues_t *p = &Queues;
    printf("%p\n", (void *)p);
    return p;
}

#ifdef __RIOT__
char *_get_stack_pointer(void)
{
    return stack;
}

msg_t *_get_queue_pointer(void)
{
    return queue;
}
#endif  // __RIOT__

#endif