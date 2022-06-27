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
#define MAX_QUEUES                  10
#endif

/* On Linux, the upper limit is 10. See mq_overview(7) for more details. */
#ifndef MAX_ELEMENTS_ON_QUEUE
#define MAX_ELEMENTS_ON_QUEUE       10
#endif

/* The minimum message size in Linux is 128 bytes, 
so set to 256 + 3 bytes, which includes the maximum 
message size allowed by LoRa plus the spaces for store
the type and destination id.*/
#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE            256
#endif

#ifndef QUEUE_SIZE
#define QUEUE_SIZE                  16
#endif

#ifdef __LINUX__
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT    2048
#endif
#endif // __LINUX__

#endif      // IPC_QUEUES_CONFIG_HEADER
