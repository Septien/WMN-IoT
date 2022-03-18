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
#include <pthread.h>
#include <unistd.h>
#endif

#ifdef __RIOT__
#include "msg.h"
#include "thread.h"
#endif

typedef struct queue
{
    uint32_t        queue_id;
#ifdef __LINUX__
    mqd_t           queue;
    struct mq_attr  attr;
    char            *q_name;
#endif
#ifdef __RIOT__
    char            *stack;
    msg_t           *queue;
#endif
    size_t          queue_size;
    size_t          message_size;
    uint32_t        msgs_allow;
}Queue_t;

typedef struct queue_thread_id
{
    uint32_t queue_id;
#ifdef __LINUX__
    pthread_t       pid;
#endif
#ifdef __RIOT__
    kernel_pid_t    pid;
#endif
}queue_thread_t;

typedef struct ipc_queues
{
    Queue_t         queues[MAX_QUEUES];
    queue_thread_t  queues_ids[MAX_QUEUES];
#ifdef __RIOT__
    char            *free_stack;
    msg_t           *free_queue;
#endif
}IPC_Queues_t;

/**
 * @brief Initialize the IPC Queues API. Set the array *queues* to zero, and for RIOT set 
 * the pointers *free_stack* and *free_queue* to start of its corresponding array. For 
 * each Queue_t element, it initialize its memebers to zero, and set the pointers to NULL.
 */
void init_queues(void);

/**
 * @brief Finish the queues API by closing any remaining opened queue, and freeing any
 * necessary memory. It clears as well any static memory in use as well as all the variables 
 * used.
 */
void end_queues(void);

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
uint32_t create_queue(size_t max_queue_size, size_t message_size, uint32_t msgs_allow, char **stack);

/**
 * @brief Given a queue_id, which was previously created with the create_queue function, this
 * function opens the queue once the threads are already initialize, and stores the process id
 * of the thread that will use the function.
 * 
 * @param queue_id 
 * @param pid
 * @return uint32_t 
 */
uint32_t open_queue(uint32_t queue_id,
#ifdef __LINUX__
pthread_t pid
#endif
#ifdef __RIOT__
kernel_pid_t pid
#endif
);

/**
 * @brief Close the queue associated with the *queue_id*, cleaning all its related content 
 * and variables.
 * 
 * @param queue_id 
 */
void close_queue(uint32_t queue_id);

/**
 * @brief Send a message *msg* of size *size* to the queue identified by queue_id. It returns
 * 1 in case of success, and 0 in case of failure. In RIOT, pid is the process id of the 
 * destination thread; in Linux it means nothing.
 * 
 * @param queue_id 
 * @param msg 
 * @param size 
 * @param pid
 * @return uint32_t 
 */
uint32_t send_message(uint32_t queue_id, void *msg, size_t size
#ifdef __LINUX__
, pthread_t pid
#endif
#ifdef __RIOT__
, kernel_pid_t pid
#endif
);

/**
 * @brief Receive the messages present on the queue identified by *queue_id*. Store the 
 * message on *msg* and its size on *size*. On *pid* returns the process id that sent 
 * the message.
 * 
 * @param queue_id 
 * @param msg 
 * @param pid
 * @param size 
 * @return uint32_t 
 */
uint32_t recv_message(uint32_t queue_id, 
#ifdef __LINUX__
char *msg, pthread_t *pid
#endif
#ifdef __RIOT__
msg_t *msg, kernel_pid_t *pid
#endif
, size_t size
);

/**
 * @brief It returns the number of elements available on the queue.
 * 
 * @param queue_id 
 * @return uint32_t 
 */
uint32_t elements_on_queue(uint32_t queue_id);

#ifdef TESTING
// Only for testing purposes
IPC_Queues_t *get_queues_pointer(void);

#ifdef __RIOT__
char *_get_stack_pointer(void);
msg_t *_get_queue_pointer(void);
#endif  // __RIOT__

#endif  // TESTING

#endif      // IPC_QUEUES_H
