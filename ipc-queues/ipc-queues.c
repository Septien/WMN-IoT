#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

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
#ifdef __LINUX__
        q->q_name = NULL;
#endif
        Queues.last_queue_id = 1;
    }
}

uint32_t create_queue(size_t max_queue_size, size_t message_size, uint32_t msgs_allow, char **stack)
{
    assert(stack != NULL);

    if (max_queue_size > QUEUE_SIZE)
        return 0;
    if (message_size > MAX_MESSAGE_SIZE)
        return 0;
    if (msgs_allow > MAX_ELEMENTS_ON_QUEUE)
        return 0;
    // Generate the new id
    uint32_t q_id = Queues.last_queue_id;
    Queues.last_queue_id++;
    Queue_t *q = &Queues.queues[q_id - 1];
    // Store the data
    q->queue_id = q_id;
    q->queue_size = max_queue_size;
    q->message_size = message_size;
    q->msgs_allow = msgs_allow;
    q->msgs_on_queue = 0;
#ifdef __RIOT__
    // Update the pointers
    *stack = Queues.free_stack;
    q->stack = *stack;
    q->queue = Queues.free_queue;
    Queues.free_stack = Queues.free_stack + THREAD_STACKSIZE_DEFAULT;
    Queues.free_queue = Queues.free_queue + QUEUE_SIZE;
#endif
#ifdef __LINUX__
    q->attr.mq_maxmsg = q->msgs_allow;
    q->attr.mq_msgsize = q->message_size;
    // For the string's name, just use the queue id
    q->q_name = (char *)malloc(4 * sizeof(char));
    sprintf(q->q_name, "/%d", q->queue_id);
    *stack = NULL;
    (void) stack; // Don't use
#endif

    return q_id;
}

uint32_t open_queue(uint32_t queue_id)
{
    assert(queue_id != 0);
    if (queue_id > MAX_QUEUES)
        return 0;
    Queue_t *q = &Queues.queues[queue_id - 1];
#ifdef __LINUX__
    q->queue = mq_open(q->q_name, O_RDWR | O_CREAT, S_IRWXG, &q->attr);
    int error = errno;
    if (q->queue == -1 && error == EEXIST)
    {
        // Queue already exists, just open it
        q->queue = mq_open(q->q_name, O_RDWR);
        error = 0;
    }
#endif
#ifdef __RIOT__
    msg_init_queue(q->queue, q->queue_size);
#endif
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