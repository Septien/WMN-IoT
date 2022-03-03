#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#ifdef __RIOT__
#include "thread.h"
#endif

#include "ipc-queues_Tests.h"
#include "ipc-queues.h"
#include "config_ipc.h"

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
        assert(q->msgs_on_queue == 0);
#ifdef __RIOT__ 
        assert(q->stack == NULL);
        assert(q->queue == NULL);
#endif
#ifdef __LINUX__
        assert(q->q_name == NULL);
        assert(q->queue == (mqd_t) - 1);
#endif
    }
    assert(Queues->last_queue_id == 1);
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
    assert(Queues->last_queue_id == 0);
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        Queue_t *q = &Queues->queues[i];
        assert(q->queue_id == 0);
        assert(q->queue_size == 0);
        assert(q->message_size == 0);
        assert(q->msgs_allow == 0);
        assert(q->msgs_on_queue == 0);
#ifdef __LINUX__
        assert(q->queue == (mqd_t) -1);
        assert(q->q_name == NULL);
#endif
#ifdef __RIOT__
        assert(q->stack == NULL);
        assert(q->queue == NULL);
#endif
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
    msgs_allow = rand() % queue_size;
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
    qid = create_queue(queue_size, message_size, msgs_allow, &stack);
    assert(qid == Queues->last_queue_id - 1);
    Queue_t *q = &Queues->queues[qid - 1];
    assert(q->queue_size == queue_size);
    assert(q->message_size == message_size);
    assert(q->msgs_allow == msgs_allow);
    assert(q->msgs_on_queue == 0);
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
    char *new_stack = last_stack;
    new_stack = new_stack + THREAD_STACKSIZE_DEFAULT;
    msg_t *new_queue = last_queue;
    new_queue = last_queue + QUEUE_SIZE;
    assert(Queues->free_stack == new_stack);
    assert(Queues->free_queue == new_queue);
#endif
    end_queues();
}
#ifdef __RIOT__
static void *func(void *arg)
{
    uint32_t *qid = (uint32_t *)arg;
    printf("%d\n", *qid);
    open_queue(*qid);
    //msg_t msg;

    return NULL;
}
#endif

void test_open_queue(void)
{
    init_queues();
    IPC_Queues_t *Queues = get_queues_pointer();

    uint32_t qid = 0;
    uint32_t queue_size, msgs_allow, message_size;
    char *stack = NULL;
    queue_size = QUEUE_SIZE;
    msgs_allow = rand() % queue_size;
    message_size = MAX_MESSAGE_SIZE;
    qid = create_queue(queue_size, message_size, msgs_allow, &stack);

    /**
     * Open or initialize the queue, depending on which OS you are. For linux, call the function
     * mq_open, and for RIOT, call the function msg_init_queue. For this last one, it is necessary
     * to be in a threading context for the function to work. The queue should already be created.
     */
    int ret = open_queue(MAX_QUEUES + 1);
    assert(ret == 0);
    Queue_t *q = &Queues->queues[qid - 1];
#ifdef __LINUX__
    open_queue(qid);
    assert(q->queue != -1);
#endif

#ifdef __RIOT__
    thread_create(stack, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
    func, (void *)&qid, "Name");
    (void) q;
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

    return;
}
