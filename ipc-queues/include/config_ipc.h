/**
 * @file config_ipc.h
 * @author José Antonio Septién Hernández.
 * @brief The configuration file for the IPC Queues module. This file contains the limit on the 
 * maximum number of queues that can be created; the maximum number of elements a queue can hold, 
 * and the maximum size of each message.
 * @version 0.1
 * @date 2022-02-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef IPC_QUEUES_CONFIG_HEADER
#define IPC_QUEUES_CONFIG_HEADER

#ifndef MAX_QUEUES
#define MAX_QUEUES              100
#endif

#ifndef MAX_ELEMENTS_ON_QUEUE
#define MAX_ELEMENTS_ON_QUEUE   100
#endif

#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE        100
#endif

#ifndef QUEUE_SIZE
#define QUEUE_SIZE              10
#endif

#endif      // IPC_QUEUES_CONFIG_HEADER