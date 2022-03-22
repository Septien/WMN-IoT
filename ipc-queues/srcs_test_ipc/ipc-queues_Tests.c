#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#ifdef __LINUX__
#include <unistd.h>
#include <errno.h>
#endif

#ifdef __RIOT__
#include "thread.h"
#include "ztimer.h"
#endif

#include "ipc-queues_Tests.h"
#include "ipc-queues.h"
#include "config_ipc.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
        ((byte) & 0x80 ? '1' : '0'), \
        ((byte) & 0x40 ? '1' : '0'), \
        ((byte) & 0x20 ? '1' : '0'), \
        ((byte) & 0x10 ? '1' : '0'), \
        ((byte) & 0x08 ? '1' : '0'), \
        ((byte) & 0x04 ? '1' : '0'), \
        ((byte) & 0x02 ? '1' : '0'), \
        ((byte) & 0x01 ? '1' : '0')

void test_init_queues(void)
{
    init_queues();
    IPC_Queues_t *Queues = get_queues_pointer();
    printf("Queues: %p\n", (void *)Queues);
#ifdef __RIOT__
    char *stack = _get_stack_pointer();
    msg_t *queue = _get_queue_pointer();
    printf("Stack %p\n", (void *)stack);
    printf("Queue: %p\n", (void *)queue);
#endif
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        Queue_t *q = &Queues->queues[i];
        assert(q->queue_id == 0);
        assert(q->queue_size == 0);
        assert(q->message_size == 0);
        assert(q->msgs_allow == 0);
#ifdef __RIOT__ 
        assert(q->stack == NULL);
        assert(q->queue == NULL);
#endif
#ifdef __LINUX__
        assert(q->q_name == NULL);
        assert(q->queue == (mqd_t) - 1);
#endif
    }
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        assert(Queues->queues_ids[i].queue_id == 0);
        assert(Queues->queues_ids[i].pid == 0);
    }
#ifdef __RIOT__
    assert(Queues->free_stack != NULL);
    assert(Queues->free_queue != NULL);
#endif
    end_queues();
}

void test_end_queues(void)
{
    init_queues();
    IPC_Queues_t *Queues = get_queues_pointer();

    /**
     * end_queue will finish the API by doing the following:
     *      -Sets the last_queue_id to 0.
     *      -For each queue, it clears its variables.
     * For Linux, it does the following:
     *      -Close the queue.
     *      -Free the name variable and set to NULL.
     *      -Set the mqd to -1.
     * For RIOT, it does the following:
     *      -Clears the static memory stack and queue.
     *      -For each queue, set the queue and stack pointer to NULL.
     *      -Sets the free_stack and free_queue pointers to zero.
     */
    end_queues();
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        Queue_t *q = &Queues->queues[i];
        assert(q->queue_id == 0);
        assert(q->queue_size == 0);
        assert(q->message_size == 0);
        assert(q->msgs_allow == 0);
#ifdef __LINUX__
        assert(q->queue == (mqd_t) -1);
        assert(q->q_name == NULL);
#endif
#ifdef __RIOT__
        assert(q->stack == NULL);
        assert(q->queue == NULL);
#endif
        assert(Queues->queues_ids[i].queue_id == 0);
        assert(Queues->queues_ids[i].pid == 0);
    }
#ifdef __RIOT__
    assert(Queues->free_stack == NULL);
    assert(Queues->free_queue == NULL);
#endif
}

void test_create_queue(void)
{
    init_queues();
    IPC_Queues_t *Queues = get_queues_pointer();

    uint32_t qid = 0;
    uint32_t queue_size, msgs_allow, message_size;
    char *stack = NULL;
    queue_size = QUEUE_SIZE;
    msgs_allow = MAX_ELEMENTS_ON_QUEUE;
    message_size = MAX_MESSAGE_SIZE;

    /**
     * This function creates a new queue. It should update the free_stack and free_queue
     * by the queue's requested size and the stack's predefined value (THREAD_STACKSIZE_DEFAULTS).
     * The id of the queue, is one more than the index of the queue on the queues array.
     * It should ckeck that there are queues available, and the size of the queue does not exceed
     * the maximum allowed size.
     * The funcion will do the following:
     *      -Create a new queue id, from the variable last_queue_id, and increase by one unit
     *      the latter.
     *      -Update the pointers free_stack by THREAD_STACKSIZE_DEFAULT units, and free_queue by 
     *      QUEUE_SIZE unit.
     *      -On linux, call the mq_open function with the O_RDWR flag.
     *      -On RIOT, call the msg_queue_init function.
     * On the corresponding Queue_t structure, hold:
     *      -The newly generated queue id.
     *      -The queue size.
     *      -The message_size.
     *      -The allowed messages.
     *      -On RIOT, hold the pointers to the correspoinding stack and queue positions.
     * It should return:
     *      -The queue's id.
     *      -Pointer to the stack.
     *      -The stack and queue pointers of the corresponding queue, should equal the previous
     * value of free_stack and free_queue.
     */
    /* When some of the input parameters exceed its maximuma allowed (defined on header file),
    return 0 from the function. */
    qid = create_queue(QUEUE_SIZE + 1, message_size, msgs_allow, &stack);
    assert(qid == 0);
    qid = create_queue(queue_size, MAX_MESSAGE_SIZE + 1, msgs_allow, &stack);
    assert(qid == 0);
    qid = create_queue(queue_size, message_size, MAX_ELEMENTS_ON_QUEUE + 1, &stack);
    assert(qid == 0);
#ifdef __RIOT__
    // Get the last position from stack and queue.
    char *last_stack = Queues->free_stack;
    msg_t *last_queue = Queues->free_queue;
    printf("last_stack = %p\n", (void *)last_stack);
    printf("last_queue = %p\n", (void *)last_queue);
    printf("stack = %p\n", (void *)stack);
#endif
    uint32_t _qid = 0;
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        if (Queues->queues_ids[i].queue_id == 0)
        {
            _qid = i + 1;
            break;
        }
    }
    qid = create_queue(queue_size, message_size, msgs_allow, &stack);
    assert(qid == _qid);
    assert(Queues->queues_ids[qid - 1].queue_id == 1);
    assert(Queues->queues_ids[qid - 1].pid == 0);
    Queue_t *q = &Queues->queues[qid - 1];
    assert(q->queue_size == queue_size);
    assert(q->message_size == message_size);
    assert(q->msgs_allow == msgs_allow);
#ifdef __LINUX__
    assert(stack == NULL);
    char name[4] = { 0 };
    sprintf(name, "/%d", qid);
    assert(strcmp(name, q->q_name) == 0);
#endif
#ifdef __RIOT__
    printf("last_stack = %p\n", (void *)last_stack);
    printf("last_queue = %p\n", (void *)last_queue);
    printf("stack = %p\n", (void *)stack);
    assert(stack == last_stack);
    assert(q->stack == last_stack);
    assert(q->queue == last_queue);
#endif

    /**
     * Test case 2:
     *      -We will now create MAX_QUEUES queues consequently. All should be valid, and the pointers
     * should be automatically updated.
     */
    for (int i = 1; i < MAX_QUEUES; i++)
    {
        qid = create_queue(queue_size, message_size, msgs_allow, &stack);
        _qid++;
        assert(qid == _qid);
        assert(Queues->queues_ids[i].queue_id == 1);
        assert(Queues->queues_ids[i].pid == 0);
#ifdef __RIOT__
        assert(stack != NULL);
        assert(stack == last_stack + (i * THREAD_STACKSIZE_DEFAULT));
        Queue_t *q = &Queues->queues[qid - 1];
        assert(q->stack == last_stack + (i * THREAD_STACKSIZE_DEFAULT));
        assert(q->queue == last_queue + (i * QUEUE_SIZE));
#endif
    }

    /**
     * Test case 3:
     *      -Attempt to create a last queue object. The function should fail and
     * return 0, stack should be NULL.
     */
    qid = create_queue(queue_size, message_size, msgs_allow, &stack);
    assert(qid == 0);
    assert(stack == NULL);

    end_queues();
}
#ifdef __RIOT__
static void *func(void *arg)
{
    uint32_t *qid = (uint32_t *)arg;
    kernel_pid_t pid = thread_getpid();
    open_queue(*qid, pid);
    IPC_Queues_t *Queues = get_queues_pointer();
    assert(Queues->queues_ids[*qid - 1].pid == pid);

    return NULL;
}
#endif
#ifdef __LINUX__
void *func(void *arg)
{
    uint32_t *qid = (uint32_t *)arg;
    pthread_t pid = pthread_self();
    open_queue(*qid, pid);
    IPC_Queues_t *Queues = get_queues_pointer();
    Queue_t *q = &Queues->queues[*qid - 1];
    assert(q->queue != -1);
    assert(Queues->queues_ids[*qid - 1].pid == pid);

    return (NULL);
}
#endif
void test_open_queue(void)
{
    init_queues();

    uint32_t qid = 0;
    uint32_t queue_size, msgs_allow, message_size;
    char *stack = NULL;
    queue_size = QUEUE_SIZE;
    msgs_allow = MAX_ELEMENTS_ON_QUEUE;
    message_size = MAX_MESSAGE_SIZE;
    qid = create_queue(queue_size, message_size, msgs_allow, &stack);

    /**
     * Open or initialize the queue, depending on which OS you are on. For linux, call the function
     * mq_open, and for RIOT, call the function msg_init_queue. For this last one, it is necessary
     * to be in a threading context for the function to work. The queue should already be created.
     */
    int ret = open_queue(MAX_QUEUES + 1, 0);
    assert(ret == 0);
#ifdef __LINUX__
    pthread_t pid;
    pthread_create(&pid, NULL, func, (void *)&qid);
    pthread_join(pid, NULL);
#endif

#ifdef __RIOT__
    kernel_pid_t pid = thread_create(stack, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
    func, (void *)&qid, "Name");
    //while (thread_getstatus(pid) != STATUS_STOPPED) ;
#endif
    end_queues();
}

#ifdef __LINUX__
void *recv(void *arg)
{
    uint32_t *qid = (uint32_t *)arg;
    
    pthread_t pid = pthread_self();
    pthread_t s_pid = 0;
    
    open_queue(*qid, pid);
    
    IPC_Queues_t *Queues = get_queues_pointer();
    Queue_t *q = &Queues->queues[(*qid) - 1];
    mqd_t qd = q->queue;
    
    size_t len = MAX_MESSAGE_SIZE + sizeof(pthread_t);
    char _msg[MAX_MESSAGE_SIZE] = {0};
    struct mq_attr attr;

    usleep(100U);
    mq_getattr(qd, &attr);
    int count = 0;
    while (attr.mq_curmsgs > 0)
    {
        int ret = mq_receive(qd, _msg, len, NULL);

        assert(ret == MAX_MESSAGE_SIZE + sizeof(pthread_t));
        s_pid = 0;
        s_pid |= ((pthread_t)_msg[0]) << 56;
        s_pid |= ((pthread_t)_msg[1]) << 48;
        s_pid |= ((pthread_t)_msg[2]) << 40;
        s_pid |= ((pthread_t)_msg[3]) << 32;
        s_pid |= ((pthread_t)_msg[4]) << 24;
        s_pid |= ((pthread_t)_msg[5]) << 16;
        s_pid |= ((pthread_t)_msg[6]) << 8;
        s_pid |= (pthread_t)_msg[7];
        assert(s_pid > 0);
        count++;
        mq_getattr(qd, &attr);
    }    
    assert(count > 0);
    return (NULL);
}
#endif
#ifdef __RIOT__
static void *recv(void *arg)
{
    uint32_t *qid = (uint32_t *)arg;
    kernel_pid_t pid = thread_getpid();
    open_queue(*qid, pid);
    int count = 0;
    thread_sleep();
    while (msg_avail())
    {
        msg_t msg;
        msg_receive(&msg);
        assert(msg.type > 0);
        assert(msg.content.ptr != NULL);
        printf("%d ", msg.type);
        count++;
    }
    assert(count > 0);

    return NULL;
}
#endif
void test_send_message(void)
{
    init_queues();

    uint32_t queue_size, msgs_allow, message_size;
    char *stack = NULL;
    queue_size = QUEUE_SIZE;
    msgs_allow = MAX_ELEMENTS_ON_QUEUE;
    message_size = MAX_MESSAGE_SIZE;
    uint32_t qid = create_queue(queue_size, message_size, msgs_allow, &stack);

    size_t msg_size = MAX_MESSAGE_SIZE;
    uint8_t msg[msg_size];
    for (uint i = 0; i < msg_size; i++)
        msg[i] = rand();
    /**    
     * We have now created and open a new queue. Now is time for sending a message. For the function
     * to work properly, the following should be true:
     *      -The queue must be open/initiated.
     *      -The queue_id must be valid [1, MAX_QUEUES].
     *      -Stack and queue pointers should not be NULL (RIOT.)
     *      -q_name should not be NULL and queue should be different to -1 (Linux.)
     *      -The message's size should not be greater than q->message_size.
     *      -The msg should not be a pointer.
     *      -In RIOT, the message should be a void pointer, while in Linux, it should be
     *       a string of characters.
     *      -It is assumed that there is space for sendign messages on the queue (the user shoudl
     *       verify it.)
     *      -In Linux, the messages have an associated priority, set the priority to 1 for all messages.
     * Steps to be executed by the function:
     *      -Check that the queue_id is valid.
     *      -Get the pointer to the corresponding queue.
     *      -Verify the message's size is within the limits.
     *      -Send the message to the queue.
     */
    /** Test case 1:
     *      The queue_id falls outside the allowed range. Return 0.
    */
    int ret = send_message(0, msg, msg_size, 0);
    assert(ret == 0);
    ret = send_message(MAX_QUEUES + 1, msg, msg_size, 0);
    assert(ret == 0);
    /** Test case 2:
     *      Create the queue, then send the message to the queue. Send it N times (random), then 
     *      receive the message on the other size. Verify that the received message is equal to 
     *      sent one. On RIOT, use the type variable to store the size of the message.
     */
    int n = 10; //rand() % MAX_ELEMENTS_ON_QUEUE;
#ifdef __LINUX__
    ret = send_message(qid, msg, msg_size, 0);
    assert(ret == 0);
    ret = send_message(qid, msg, msg_size, 1);
    assert(ret == 0);

    pthread_t pid;
    pthread_create(&pid, NULL, recv, (void *)&qid);
    usleep(50U);
    for (int i = 0; i < n; i++)
    {
        ret = send_message(qid, msg, msg_size, pid);
        assert(ret == 1);
    }
    pthread_join(pid, NULL);
#endif
#ifdef __RIOT__
    kernel_pid_t pid = thread_create(stack, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1, 
    THREAD_CREATE_SLEEPING, recv, (void *)&qid, "Name");
    thread_wakeup(pid);
    for (int i = 0; i < n; i++)
    {
        ret = send_message(qid, msg, msg_size, pid);
        assert(ret == 1);
    }
    thread_wakeup(pid);
    //while (thread_getstatus(pid) != STATUS_STOPPED) ;
#endif

    end_queues();
}

#ifdef __LINUX__
void *recv_recv(void *arg)
{
    uint32_t *qid = (uint32_t *)arg;
    pthread_t pid = pthread_self();
    pthread_t s_pid = 0;
    open_queue(*qid, pid);
    IPC_Queues_t *Queues = get_queues_pointer();
    Queue_t *q = &Queues->queues[(*qid) - 1];
    mqd_t qd = q->queue;
    size_t size = MAX_MESSAGE_SIZE;
    uint8_t *_msg = (uint8_t *)malloc(size * sizeof(uint8_t));
    struct mq_attr attr;

    usleep(100U);
    mq_getattr(qd, &attr);
    int count = 0;
    while (attr.mq_curmsgs > 0)
    {
        int ret = recv_message(*qid, _msg, size, &s_pid);
        assert(ret == 1);
        assert(s_pid > 0);
        for (uint i = 0; i < size; i++)
            assert(_msg[i] == 10);
        count++;
        mq_getattr(qd, &attr);
    }
    assert(count > 0);
    return (NULL);
}
#endif
#ifdef __RIOT__
static void *recv_recv(void *arg)
{
    uint32_t *qid = (uint32_t *)arg;
    kernel_pid_t pid = thread_getpid();
    kernel_pid_t s_pid = 0;
    open_queue(*qid, pid);
    int count = 0;
    thread_sleep();
    while (msg_avail())
    {
        size_t msg_size = MAX_MESSAGE_SIZE;
        uint8_t msg[msg_size];
        int ret = recv_message(*qid, msg, msg_size, &s_pid);
        assert(s_pid > 0);
        assert(ret == 1);
        for (uint i = 0; i < msg_size; i++)
            assert(msg[i] == 10);
        count++;
    }
    assert(count > 0);

    return NULL;
}
#endif
void test_recv_message(void)
{
    init_queues();

    uint32_t queue_size, msgs_allow, message_size;
    char *stack = NULL;
    queue_size = QUEUE_SIZE;
    msgs_allow = MAX_ELEMENTS_ON_QUEUE;
    message_size = MAX_MESSAGE_SIZE;
    uint32_t qid = create_queue(queue_size, message_size, msgs_allow, &stack);

    size_t msg_size = MAX_MESSAGE_SIZE;
    uint8_t msg[msg_size];
    for (uint i = 0; i < msg_size; i++)
        msg[i] = 10;
    /**
     * We have a way to send messages through the queues. We now need a way to receive them 
     * without using directly the different calls provided by the OSs. We now will test a function 
     * that will do so.
     * This function should hide the details of the different systems calls, and provide
     * an easy way to receive the messages.
     * The function will receive as parameters the following:
     *      -The queue_id, the queue's identifier.
     *      -A pointer of size msg_size (MAX_MESSAGE_SIZE on Linux), to store the incomming message.
     *      -A pointer to store the size of the message.
     * The following should comply:
     *      -The received message should be the same as the sent one.
     *      -The received size should be equal to the sent one.
     *      -The queue_id should point to a valid queue.
     */
    int n = 10;
#ifdef __LINUX__
    pthread_t pid;
    pthread_create(&pid, NULL, recv_recv, (void *)&qid);
    usleep(50U);
    for (int i = 0; i < n; i++)
        send_message(qid, msg, msg_size, pid);
    pthread_join(pid, NULL);
#endif
#ifdef __RIOT__
    kernel_pid_t pid = thread_create(stack, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1, 
    THREAD_CREATE_SLEEPING, recv_recv, (void *)&qid, "Name");
    thread_wakeup(pid);
    for (int i = 0; i < n; i++)
        send_message(qid, msg, msg_size, pid);
    thread_wakeup(pid);
    ztimer_sleep(ZTIMER_USEC, 100);
#endif

#ifdef __LINUX__
    uint8_t msg2[msg_size];
    uint8_t *_msg2 = msg2;
    pthread_t _pid;
#endif
#ifdef __RIOT__
    uint8_t msg2[msg_size];
    uint8_t *_msg2 = msg2;
    kernel_pid_t _pid;
#endif
    size_t size = message_size;
    /* Test case 1:
        -The queue id is invalid, that is, is outside the limits [1, MAX_QUEUES].
        The function should return 0.
        -The size is zero.
        The function should return 0.
    */
    int ret = recv_message(0, _msg2, size, &_pid);
    assert(ret == 0);
    ret = recv_message(MAX_QUEUES + 1, _msg2, size, &_pid);
    assert(ret == 0);
    ret = recv_message(MAX_QUEUES + 1, _msg2, 0, &_pid);
    assert(ret == 0);

    end_queues();
}

void test_close_queue(void)
{
    init_queues();

    uint32_t queue_size, msgs_allow, message_size;
    char *stack = NULL;
    queue_size = QUEUE_SIZE;
    msgs_allow = MAX_ELEMENTS_ON_QUEUE;
    message_size = MAX_MESSAGE_SIZE;
    uint32_t qid = create_queue(queue_size, message_size, msgs_allow, &stack);

    size_t msg_size = MAX_MESSAGE_SIZE;
    uint8_t msg[msg_size];
    for (uint i = 0; i < msg_size; i++)
        msg[i] = 10;

    int n = 10;
#ifdef __LINUX__
    pthread_t pid;
    pthread_create(&pid, NULL, recv_recv, (void *)&qid);
    usleep(50U);
    for (int i = 0; i < n; i++)
        send_message(qid, msg, msg_size, pid);
    pthread_join(pid, NULL);
#endif
#ifdef __RIOT__
    kernel_pid_t pid = thread_create(stack, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1, 
    THREAD_CREATE_SLEEPING, recv_recv, (void *)&qid, "Name");
    thread_wakeup(pid);
    for (int i = 0; i < n; i++)
        send_message(qid, msg, msg_size, pid);
    thread_wakeup(pid);
    ztimer_sleep(ZTIMER_USEC, 100);
#endif

    IPC_Queues_t *Queues = get_queues_pointer();

    /**
     * We have a way to open queues for using among the threads. Now we want a method for
     * closing them and releasing its resources. This method should check that the queue
     * indeed exists, and that the queue_id is valid (within the range). It should clean 
     * as well the corresponding queue and stack. Any thread associated to the queue, should
     * be already stopped.
     */
    /* Test case 1:
    *   We opened and used a queue for IPC, now we want to close it and assert that all
    * all its associated variables are clear. For Linux, the name should be freed, and for
    * for RIOT, the pointers should be set to NULL.
    */
    Queue_t *q = &Queues->queues[qid - 1];
    close_queue(qid);
    assert(q->queue_id == 0);
    assert(q->queue_size == 0);
    assert(q->message_size == 0);
    assert(q->msgs_allow == 0);
#ifdef __LINUX__
    assert(q->queue == -1);
    assert(q->q_name == NULL);
    assert(q->attr.mq_maxmsg == 0);
    assert(q->attr.mq_msgsize == 0);
#endif
#ifdef __RIOT__
    assert(q->stack == NULL);
    assert(q->queue == NULL);
#endif
    assert(Queues->queues_ids[qid - 1].queue_id == 0);
    assert(Queues->queues_ids[qid - 1].pid == 0);

    end_queues();
}

void test_open_close_queues(void)
{
    init_queues();
    IPC_Queues_t *Queues = get_queues_pointer();

    uint32_t queue_size, msgs_allow, message_size;
    char *stack = NULL;
    queue_size = QUEUE_SIZE;
    msgs_allow = MAX_ELEMENTS_ON_QUEUE;
    message_size = MAX_MESSAGE_SIZE;
    /**
     * This tests the creation and closing of several non-consecutive queues, for
     * knowing how the API handles such situations. 
     * The fist test does the following:
     *      -Create N queues.
     *      -Closes M queues at random (M < N);
     *      -Creates once again M queues.
     *      -The queues might be or might not be consecutive.
     */
    int n = rand() % MAX_QUEUES;
    int m = rand() % n;         // m < n
    int i;
    uint32_t qs[n];
    memset(qs, 0, n * sizeof(uint32_t));
    for (i = 0; i < n; i++)
    {
        qs[i] = create_queue(queue_size, message_size, msgs_allow, &stack);
    }
    int count = 0;
    for (i = 0; i < n; i++)
    {
        /* Close at random at most m queues. */
        if (count < m)
        {
            if (i % 2 == 0)
            {
                //uint8_t _qid = qs[i];
                Queue_t *q = &Queues->queues[qs[i] - 1];
#ifdef __LINUX__
                assert(q->q_name != NULL);
#endif
#ifdef __RIOT__
                assert(q->stack == Queues->free_stack + (i * THREAD_STACKSIZE_DEFAULT));
                assert(q->queue == Queues->free_queue + (i * QUEUE_SIZE));
#endif
                close_queue(qs[i]);
                //assert(Queues->queues_ids[_qid - 1] == 0);
#ifdef __LINUX__
                assert(q->queue == (mqd_t) -1);
                assert(q->q_name == NULL);
#endif
#ifdef __RIOT__
                assert(q->stack == NULL);
                assert(q->queue == NULL);
#endif
                qs[i] = 0;
                count++;
                
               ;
            }
        }
    }

    /* Re-open the closed queues. */
    for (i = 0; i < MAX_QUEUES; i++)
    {
        if (qs[i] == 0)
        {
            //assert(Queues->queues_ids[i] == 0);
            qs[i] = create_queue(queue_size, message_size, msgs_allow, &stack);
            assert(qs[i] != 0);
            uint8_t qid = qs[i];
            Queue_t *q = &Queues->queues[qid - 1];
            //assert(Queues->queues_ids[qid - 1] == 1);
#ifdef __LINUX__
            assert(q->q_name != NULL);
#endif
#ifdef __RIOT__
            assert(q->stack == Queues->free_stack + (i * THREAD_STACKSIZE_DEFAULT));
            assert(q->queue == Queues->free_queue + (i * QUEUE_SIZE));
#endif
            //assert(Queues->queues_ids[qid - 1] != 0);
        }
    }

    end_queues();
}

#ifdef __LINUX__
void *open_q(void *arg)
{
    uint32_t *qid = (uint32_t *)arg;
    pthread_t pid = pthread_self();
    open_queue(*qid, pid);
    /**
     * Test case 1:
     * No elements on the queue, the function should return 0.
     */
    uint32_t elements = elements_on_queue(*qid);
    assert(elements == 0);

    size_t size = MAX_MESSAGE_SIZE;
    char *msg = (char *)malloc(size * sizeof(char));
    for (uint i = 0; i < size; i++)
        msg[i] = 'a';
    
    send_message(*qid, (void *)msg, size, pid);
    elements = elements_on_queue(*qid);
    assert(elements == 1);

    uint n = rand() % QUEUE_SIZE;
    for (uint i = 0; i < n - 1; i++)
        send_message(*qid, (void *)msg, size, pid);
    elements = elements_on_queue(*qid);
    assert(elements == n);

    return (NULL);
}
#endif
#ifdef __RIOT__
static void *open_q(void *arg)
{
    uint32_t *qid = (uint32_t *)arg;
    kernel_pid_t pid = thread_getpid();
    open_queue(*qid, pid);
    /**
     * Test case 1:
     * No elements on the queue, the function should return 0.
     */
    uint32_t elements = elements_on_queue(*qid);
    assert(elements == 0);

    size_t size = MAX_MESSAGE_SIZE;
    char msg[size];
    for (uint i = 0; i < size; i++)
        msg[i] = 'a';
    char *_msg = msg;

    send_message(*qid, (void *)_msg, size, pid);
    elements = elements_on_queue(*qid);
    assert(elements == 1);

    uint n = rand() % QUEUE_SIZE;
    for (uint i = 0; i < n - 1; i++)
        send_message(*qid, (void *)_msg, size, pid);
    elements = elements_on_queue(*qid);
    assert(elements == n);
    thread_sleep();

    return NULL;
}
#endif
void test_elements_on_queue(void)
{
    init_queues();
    //IPC_Queues_t *Queues = get_queues_pointer();

    uint32_t queue_size, msgs_allow, message_size;
    char *stack = NULL;
    queue_size = QUEUE_SIZE;
    msgs_allow = MAX_ELEMENTS_ON_QUEUE;
    message_size = MAX_MESSAGE_SIZE;
    /**
     * This function will test that elements_on_queue returns the right number of 
     * elements present on a given queue.
     * The test cases are the following:
     *      -No messages on the queue.
     *      -One message on the queue.
     *      -Something between (1, MAX_ELEMENTS_ON_QUEUE).
     *      -MAX_ELEMENTS_ON_QUEUE.
     * For the test, we need to create and open a queue first, for RIOT we need to create a 
     * thread to call the function open_queue.
     */
    uint32_t qid = create_queue(queue_size, message_size, msgs_allow, &stack);

#ifdef __LINUX__
    pthread_t pid = 0;
    pthread_create(&pid, NULL, open_q, (void *)&qid);
    pthread_join(pid, NULL);
#endif

#ifdef __RIOT__
    kernel_pid_t pid = thread_create(stack, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
    open_q, (void *)&qid, "Name");
    ztimer_sleep(ZTIMER_USEC, 100);
    thread_wakeup(pid);
#endif

    end_queues();
}

void ipc_queues_tests(void)
{
    printf("Testing the IPC Queues API.\n");

    printf("Testing the init_queues function.\n");
    test_init_queues();
    printf("Test passed.\n");

    printf("Testing the end_queues function.\n");
    test_end_queues();
    printf("Test passed.\n");

    printf("Testing the create_queue function.\n");
    test_create_queue();
    printf("Test passed.\n");

    printf("Testing the open_queue function.\n");
    test_open_queue();
    printf("Test passed.\n");

    printf("Testing the send_message function.\n");
    test_send_message();
    printf("Test passed.\n");

    printf("Testing the recv_message function.\n");
    test_recv_message();
    printf("Test passed.\n");

    printf("Testing the close_queue function.\n");
    test_close_queue();
    printf("Test passed.\n");

    /*printf("Testing the creation and closing of non-consecutive queues.\n");
    test_open_close_queues();
    printf("Test passed.\n");*/

    printf("Testing the elements_on_queue function.\n");
    test_elements_on_queue();
    printf("Test passed.\n");

    return;
}
