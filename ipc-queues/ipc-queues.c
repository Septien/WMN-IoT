#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "ipc-queues.h"

#ifdef __RIOT__
static char stack[MAX_QUEUES * THREAD_STACKSIZE_DEFAULT];
static msg_t queue[MAX_QUEUES * QUEUE_SIZE];
#endif

static IPC_Queues_t Queues;

void init_queues(void)
{
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
#endif
#ifdef __LINUX__
        q->queue = (mqd_t) -1;
        q->q_name = NULL;
#endif
        for (int i = 0; i < MAX_QUEUES; i++)
            Queues.queues_ids[i] = 0;
    }
#ifdef __RIOT__
    Queues.free_stack = stack;
    Queues.free_queue = queue;
#endif
}

void end_queues(void)
{
#ifdef __RIOT__
    Queues.free_stack = NULL;
    Queues.free_queue = NULL;
#endif
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        Queue_t *q = &Queues.queues[i];
        q->queue_id = 0;
        q->queue_size = 0;
        q->message_size = 0;
        q->msgs_allow = 0;
        q->msgs_on_queue = 0;
#ifdef __LINUX__
        if (q->queue != -1)
        {
            mq_unlink(q->q_name);
            mq_close(q->queue);
        }
        q->queue = (mqd_t) -1;
        if (q->q_name != NULL)
            free(q->q_name);
        q->q_name = NULL;
#endif
#ifdef __RIOT__
        q->stack = NULL;
        q->queue = NULL;
#endif
        Queues.queues_ids[i] = 0;
    }
}

uint32_t create_queue(size_t max_queue_size, size_t message_size, uint32_t msgs_allow, char **stack)
{
    assert(stack != NULL);
#ifdef __RIOT__
    assert(Queues.free_stack != NULL);
    assert(Queues.free_queue != NULL);
#endif

    if (max_queue_size > QUEUE_SIZE)
        return 0;
    if (message_size > MAX_MESSAGE_SIZE)
        return 0;
    if (msgs_allow > MAX_ELEMENTS_ON_QUEUE)
        return 0;

    // Find a new available id and get the pointers to the corresponding array entry
#ifdef __RIOT__
    char *_stack = NULL;
    msg_t *_queue = NULL;
#endif
    uint32_t q_id = 0;
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        if (Queues.queues_ids[i] == 0)
        {
            q_id = i + 1;
            Queues.queues_ids[i] = 1;
#ifdef __RIOT__
            _stack = Queues.free_stack + (i * THREAD_STACKSIZE_DEFAULT);
            _queue = Queues.free_queue + (i * QUEUE_SIZE);
#endif
            break;
        }
    }
    // No available queues found
    if (q_id == 0)
    {
        *stack = NULL;
        return 0;
    }
    Queue_t *q = &Queues.queues[q_id - 1];
    // Store the data
    q->queue_id = q_id;
    q->queue_size = max_queue_size;
    q->message_size = message_size;
    q->msgs_allow = msgs_allow;
    q->msgs_on_queue = 0;
#ifdef __RIOT__
    // Update the pointers
    *stack = _stack;
    q->stack = *stack;
    q->queue = _queue;
#endif
#ifdef __LINUX__
    q->attr.mq_maxmsg = q->msgs_allow;
    q->attr.mq_msgsize = q->message_size;
    q->attr.mq_curmsgs = 0;
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
    assert(q->q_name != NULL);
#endif
#ifdef __RIOT__
    assert(q->stack != NULL);
    assert(q->queue != NULL);
#endif
#ifdef __LINUX__
    //printf("Queue attribute:\nMax msgs: %ld\nMsg size: %ld\n", q->attr.mq_maxmsg, q->attr.mq_msgsize);
    q->queue = mq_open(q->q_name, O_RDWR | O_CREAT | O_CLOEXEC, S_IRWXU, &q->attr);
    int error = errno;
    if (q->queue == -1 && error == EEXIST)
    {
        // Queue already exists, just open it
        q->queue = mq_open(q->q_name, O_RDWR);
        error = 0;
    }
    struct mq_attr attr;
    mq_getattr(q->queue, &attr);
#endif
#ifdef __RIOT__
    msg_init_queue(q->queue, q->queue_size);
#endif
    return 1;
}

void close_queue(uint32_t queue_id)
{
    assert(queue_id > 0);
    assert(queue_id <= MAX_QUEUES);

    Queue_t *q = &Queues.queues[queue_id - 1];

    q->queue_id = 0;
    q->queue_size = 0;
    q->message_size = 0;
    q->msgs_allow = 0;
    q->msgs_on_queue = 0;
#ifdef __LINUX__
    if (q->q_name != NULL)
    {
        mq_unlink(q->q_name);
        free(q->q_name);
        q->q_name = NULL;
    }
    if (q->queue != (mqd_t) -1)
    {
        mq_close(q->queue);
        q->queue = (mqd_t) -1;
    }
    memset(&q->attr, 0, sizeof(q->attr));
#endif
#ifdef __RIOT__
    q->stack = NULL;
    q->queue = NULL;
#endif
    Queues.queues_ids[queue_id - 1] = 0;
}

uint32_t send_message(uint32_t queue_id, void *msg, size_t size
#ifdef __LINUX__
, int pid
#endif
#ifdef __RIOT__
, kernel_pid_t pid
#endif
)
{
    assert(msg != NULL);

    if (queue_id < 1 || queue_id > MAX_QUEUES)
        return 0;
    Queue_t *q = &Queues.queues[queue_id - 1];

#ifdef __LINUX__
    assert(q->queue_id != 0);
    assert(q->queue != -1);
    (void) pid;
#endif
#ifdef __RIOT__
    assert(q->stack != NULL);
    assert(q->queue != NULL);
#endif
    assert(size <= q->message_size);

#ifdef __LINUX__
    // Set the priority to 1 for all messages
    unsigned int priority = 1;
    char *_msg = (char *)msg;
    mq_send(q->queue, _msg, size, priority);
#endif
#ifdef __RIOT__
    msg_t _msg;
    _msg.content.ptr = msg;
    _msg.type = (uint16_t) size;
    msg_send(&_msg, pid);
#endif
    return 1;
}

uint32_t recv_message(uint32_t queue_id, 
#ifdef __LINUX__
uint8_t **msg
#endif
#ifdef __RIOT__
msg_t *msg
#endif
, size_t size
)
{
    assert(msg != NULL);
#ifdef __LINUX__
    assert(*msg != NULL);
#endif

    if (queue_id < 1 || queue_id > MAX_QUEUES)
        return 0;
    if (size == 0)
        return 0;

    Queue_t *q = &Queues.queues[queue_id - 1];
#ifdef __LINUX__
    assert(q->queue != -1);
#endif
#ifdef __RIOT__
    assert(q->stack != NULL);
    assert(q->queue != NULL);
#endif
#ifdef __LINUX__
    unsigned int priority = 0;
    ssize_t length = mq_receive(q->queue, (char *)*msg, size, &priority);
    int err = errno;
    if (length == -1)
    {
        switch (err)
        {
        case EINTR:
            /* Interrupted by signal, try again */
            length = mq_receive(q->queue, (char *)*msg, size, &priority);
            break;
        case EBADF:
            /* Invalid queue descriptor */
            return -1;
        default:
            break;
        }
    }
#endif
#ifdef __RIOT__
    msg_receive(msg);
    // Invalid message, discard
    if (msg->type != q->message_size)
    {
        msg = NULL;
        return 0;
    }
#endif

    return 1;
}

uint32_t elements_on_queue(uint32_t queue_id)
{
    assert(queue_id > 0);
    assert(queue_id <= MAX_QUEUES);

    Queue_t *q = &Queues.queues[queue_id - 1];
    (void) q;

    return 0;
}

// ---------------------------------- Testing functions ----------------------------------------

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
