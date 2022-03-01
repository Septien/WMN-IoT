/**
 * @file ipc-queues.h
 * @author José Antonio Septién Hernández (ja.septienhernandez@ugto.mx)
 * @brief This API handles the queues necessary for IPC among the different layers of the network 
 * stack. It works on both RIOT OS and on Linux. It hides away the implementation details necessary
 * for making the queues work on each OS, and provides a clean API that allows to use the different
 * queues with different requirements.
 * @version 0.1
 * @date 2022-02-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef IPC_QUEUES_H
#define IPC_QUEUES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "config_ipc.h"

#ifdef __LINUX__
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#endif

#ifdef __RIOT__
#include "msg.h"
#include "thread.h"

static char stack[MAX_QUEUES * THREAD_STACKSIZE_DEFAULT];
static msg_t queue[MAX_QUEUES * QUEUE_SIZE];
#endif

typedef struct queue
{
    uint32_t        queue_id;
#ifdef __LINUX__
    mqd_t           queue;
    struct mq_attr  attr;
#endif
#ifdef __RIOT__
    char            *stack;
    msg_t           *queue;
#endif
    size_t          queue_size;
    size_t          message_size;
    uint32_t        msgs_allow;
    uint32_t        msgs_on_queue;
}Queue_t;

typedef struct ipc_queues
{
    Queue_t         queues[MAX_QUEUES];
    uint32_t        last_queue_id;
#ifdef __RIOT__
    char            *free_stack;
    msg_t           *free_queue;
#endif
}IPC_Queues_t;

/**
 * @brief Initialize the IPC Queues API. Set the array *queues* to zero, the variable
 * *last_queue_id* to 1, and for RIOT set the pointers *free_stack* and *free_queue* to 
 * start of its corresponding array. For each Queue_t element, it initialize its memebers
 * to zero, and set the pointers to NULL;
 */
void init_queues(void);

/**
 * @brief Create a queue object with a queue lenght of *max_queue_size*, with a maximum of 
 * *msgs_allow* number of messages, and a message size of *message_size*. It returns a
 * pointer to the start of the stack for further usage (only for RIOT), and the queue's id
 * for further usage.
 * 
 * @param max_queue_size 
 * @param stack 
 * @return uint32_t 
 */
uint32_t create_queue(size_t max_queue_size, size_t message_size, uint32_t msgs_allow, char *stack);

uint32_t elements_on_queue(uint32_t queue_id);
uint32_t send_message(uint32_t queue_id, void *msg, size_t size);
uint32_t recv_message(uint32_t queue_id, void **msg, size_t *size);
#ifdef __RIOT__
uint32_t send_uint_message(uint32_t queue_id, uint32_t data);
uint32_t recv_uint_message(uint32_t queue_id, uint32_t *data);
#endif
void end_queues(void);

#ifdef TESTING
// Only for testing purposes
IPC_Queues_t *get_queues_pointer(void);

#ifdef __RIOT__
char *_get_stack_pointer(void);
msg_t *_get_queue_pointer(void);
#endif  // __RIOT__

#endif  // TESTING

#endif      // IPC_QUEUES_H
