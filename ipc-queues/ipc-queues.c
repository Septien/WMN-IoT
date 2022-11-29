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
#ifdef __RIOT__
        q->stack = NULL;
        q->queue = NULL;
#endif
#ifdef __LINUX__
        q->queue = (mqd_t) -1;
        q->q_name = NULL;
#endif
        for (int i = 0; i < MAX_QUEUES; i++)
        {
            Queues.queues_ids[i].queue_id = 0;
            Queues.queues_ids[i].pid = 0;
        }
    }
#ifdef __RIOT__
    Queues.free_stack = stack;
    Queues.free_queue = queue;
    // Initialize the message storage
    memset(Queues.msg_storage, 0, MAX_QUEUES * MAX_ELEMENTS_ON_QUEUE * MAX_MESSAGE_SIZE);
    Queues.start_storage = 0;
    Queues.end_storage = 0;
    Queues.stored_elements = 0;
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
#ifdef __LINUX__
        if (q->queue != -1)
        {
            mq_unlink(q->q_name);
            mq_close(q->queue);
        }
        q->queue = (mqd_t) -1;
        if (q->q_name != NULL) {
            free(q->q_name);
        }
        q->q_name = NULL;
#endif
#ifdef __RIOT__
        q->stack = NULL;
        q->queue = NULL;
        // Clear storage memory and set pointers to NULL
        memset(Queues.msg_storage, 0, MAX_QUEUES * MAX_ELEMENTS_ON_QUEUE * MAX_MESSAGE_SIZE);
        Queues.start_storage = 0;
        Queues.end_storage = 0;
        Queues.stored_elements = 0;
#endif
        Queues.queues_ids[i].queue_id = 0;
        Queues.queues_ids[i].pid = 0;
    }
}

uint32_t create_queue(size_t max_queue_size, size_t message_size, uint32_t msgs_allow, char **stack)
{
    assert(stack != NULL);
#ifdef __RIOT__
    assert(Queues.free_stack != NULL);
    assert(Queues.free_queue != NULL);
#endif

    if (max_queue_size > QUEUE_SIZE) {
        return 0;
    }
    if (message_size > MAX_MESSAGE_SIZE) {
        return 0;
    }
    if (msgs_allow > MAX_ELEMENTS_ON_QUEUE) {
        return 0;
    }

    // Find a new available id and get the pointers to the corresponding array entry
#ifdef __RIOT__
    char *_stack = NULL;
    msg_t *_queue = NULL;
#endif
    uint32_t q_id = 0;
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        if (Queues.queues_ids[i].queue_id == 0)
        {
            q_id = i + 1;
            Queues.queues_ids[i].queue_id = 1;
            Queues.queues_ids[i].pid = 0;
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
#ifdef __RIOT__
    // Update the pointers
    *stack = _stack;
    q->stack = _stack;
    q->queue = _queue;
#endif
#ifdef __LINUX__
    q->attr.mq_maxmsg = q->msgs_allow;
    // Add the width of pthread_t, so the pid of the thread can be stored on the message.
    q->attr.mq_msgsize = q->message_size + sizeof(pthread_t);
    q->attr.mq_curmsgs = 0;
    // For the string's name, just use the queue id
    q->q_name = (char *)malloc(4 * sizeof(char));
    sprintf(q->q_name, "/%d", q->queue_id);
    *stack = NULL;
    (void) stack; // Don't use
#endif

    return q_id;
}

uint32_t open_queue(uint32_t queue_id,
#ifdef __LINUX__
pthread_t pid
#endif
#ifdef __RIOT__
kernel_pid_t pid
#endif
)
{
    assert(queue_id != 0);

    if (queue_id > MAX_QUEUES)
        return 0;
    Queue_t *q = &Queues.queues[queue_id - 1];
    Queues.queues_ids[queue_id - 1].pid = pid;
#ifdef __LINUX__
    assert(q->q_name != NULL);
#endif
#ifdef __RIOT__
    assert(q->stack != NULL);
    assert(q->queue != NULL);
#endif
#ifdef __LINUX__
    q->queue = mq_open(q->q_name, O_RDWR | O_CREAT | O_CLOEXEC, S_IRWXU, &q->attr);
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
    Queues.queues_ids[queue_id - 1].queue_id = 0;
    Queues.queues_ids[queue_id - 1].pid = 0;
}

uint32_t send_message(uint32_t queue_id, uint8_t *msg, size_t size
#ifdef __LINUX__
, pthread_t pid
#endif
#ifdef __RIOT__
, kernel_pid_t pid
#endif
)
{
    assert(msg != NULL);
    if (queue_id < 1 || queue_id > MAX_QUEUES) {
        return 0;
    }
    if (pid == 0) {
        return 0;
    }
    Queue_t *q = &Queues.queues[queue_id - 1];
    // Get the associated queue
    Queue_t *recv_q = NULL;
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        if (Queues.queues_ids[i].pid == pid && Queues.queues_ids[i].queue_id == 1)
        {
            recv_q = &Queues.queues[i];
            break;
        }
    }
    if (recv_q == NULL) {
        return 0;
    }

#ifdef __LINUX__
    assert(recv_q->queue_id != 0);
    assert(recv_q->queue != -1);
    assert(q->queue_id != 0);
    assert(q->queue != -1);
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
    // Create the message to send
    char *out_msg = (char *)malloc((size + sizeof(pthread_t)) * sizeof(char));
    // Add the pid to the message
    out_msg[0] = (pid & 0xff00000000000000) >> 56;
    out_msg[1] = (pid & 0x00ff000000000000) >> 48;
    out_msg[2] = (pid & 0x0000ff0000000000) >> 40;
    out_msg[3] = (pid & 0x000000ff00000000) >> 32;
    out_msg[4] = (pid & 0x00000000ff000000) >> 24;
    out_msg[5] = (pid & 0x0000000000ff0000) >> 16;
    out_msg[6] = (pid & 0x000000000000ff00) >> 8;
    out_msg[7] = (pid & 0x00000000000000ff);
    // Copy the original message to the out_msg
    memcpy(out_msg + 8, _msg, sizeof(char) * size);
    int ret = 0;
    do {
        ret = mq_send(recv_q->queue, out_msg, size + sizeof(pthread_t), priority);
        int err = errno;
        if (ret == -1)
        {
            if (err == EAGAIN || err == EINTR)
                continue;
            switch (err)
            {
            case EBADF:
                ret = 0;
                printf("Invalid descriptor or no opened.\n");
                break;
            case EMSGSIZE:
                ret = 0;
                printf("msg_len was greater then mq_msgsize attribute of the queue.\n");
                break;
            default:
                ret = 0;
                printf("Non detected.\n");
                break;
            }
        }
    } while (ret == -1);
    free(out_msg);
#endif
#ifdef __RIOT__
    if (Queues.stored_elements == MAX_QUEUES * MAX_ELEMENTS_ON_QUEUE)
        return 0;
    msg_t _msg;
    uint last = Queues.end_storage;
    uint8_t *storage = Queues.msg_storage[last];
    memcpy(storage, msg, MAX_MESSAGE_SIZE);
    _msg.content.ptr = (void *)storage;
    _msg.type = (uint16_t) size;
    Queues.end_storage = (last++ >= MAX_QUEUES * MAX_ELEMENTS_ON_QUEUE ? 0 : last);
    Queues.stored_elements++;
    msg_send(&_msg, pid);
#endif
    return 1;
}

uint32_t recv_message(uint32_t queue_id, uint8_t *msg, size_t size,
#ifdef __LINUX__
pthread_t *pid
#endif
#ifdef __RIOT__
kernel_pid_t *pid
#endif
)
{
    assert(msg != NULL);
    assert(pid != NULL);

    if (queue_id < 1 || queue_id > MAX_QUEUES) {
        return 0;
    }
    if (size == 0) {
        return 0;
    }

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
    /* Create an array to temporarly store the message. */
    char *_msg = (char *)malloc((size + sizeof(pthread_t)) * sizeof(char));
    ssize_t length = mq_receive(q->queue, _msg, size + sizeof(pthread_t), &priority);
    int err = errno;
    if (length == -1)
    {
        switch (err)
        {
        case EINTR:
            /* Interrupted by signal, try again */
            length = mq_receive(q->queue, _msg, size, &priority);
            break;
        case EBADF:
            /* Invalid queue descriptor */
            return -1;
        case EMSGSIZE:
            /* Invalid message size */
            return -2;
        default:
            return 0;
            break;
        }
    }
    /* Get the pid. */
    uint8_t *_msg_ = (uint8_t *)_msg;
    pthread_t _pid = 0;
    _pid |= ((pthread_t)_msg_[0]) << 56;
    _pid |= ((pthread_t)_msg_[1]) << 48;
    _pid |= ((pthread_t)_msg_[2]) << 40;
    _pid |= ((pthread_t)_msg_[3]) << 32;
    _pid |= ((pthread_t)_msg_[4]) << 24;
    _pid |= ((pthread_t)_msg_[5]) << 16;
    _pid |= ((pthread_t)_msg_[6]) << 8;
    _pid |= ((pthread_t)_msg_[7]);
    /* Copy the message back to the return message. */
    uint8_t *p = (uint8_t *)(_msg_ + 8);
    memcpy(msg, p, sizeof(uint8_t) * size);
    *pid = _pid;

    free(_msg);
#endif
#ifdef __RIOT__
    if (Queues.stored_elements == 0) {
        return 0;
    }
    msg_t _msg;
    msg_receive(&_msg);
    // Invalid message, discard
    if (_msg.type != q->message_size)
    {
        msg = NULL;
        return 0;
    }
    // Copy the message to the output array
    memcpy(msg, _msg.content.ptr, sizeof(uint8_t) * size);
    *pid = _msg.sender_pid;
    // Clear memory and update pointers and data
    uint first = Queues.start_storage;
    memset(Queues.msg_storage[first], 0, MAX_MESSAGE_SIZE);
    Queues.start_storage = (first + 1) % (MAX_QUEUES * MAX_ELEMENTS_ON_QUEUE);
    Queues.stored_elements--;
#endif

    return 1;
}

uint32_t elements_on_queue(uint32_t queue_id)
{
    assert(queue_id > 0);
    assert(queue_id <= MAX_QUEUES);

    uint32_t n_messages;
#ifdef __LINUX__
    Queue_t *q = &Queues.queues[queue_id - 1];
    mq_getattr(q->queue, &q->attr);
    n_messages = q->attr.mq_curmsgs;
#endif
#ifdef __RIOT__
    n_messages = msg_avail_thread(Queues.queues_ids[queue_id-1].pid);
#endif

    return n_messages;
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
