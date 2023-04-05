/**
 * @file helpers.h
 * @author Jose Antonio Septien Hernandez (ja.septienhernande@ugto.mx)
 * @brief This translation unit contains all the necessary function to execute the behavioral
 * tests. From functions that handle requests coming from the main testing thread,
 * to functions that execute the different network protocols.
 * @version 0.1
 * @date 2022-11-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef INCLUDE_HELPERS_H
#define INCLUDE_HELPERS_H

#ifdef __LINUX__
#include <pthread.h>
#endif
#ifdef __RIOT__
#include "thread.h"
#include "mutex.h"
#endif

#include "config.h"
#include "ipc-queues.h"
#include "REMA.h"

/* Requests sent by the main thread to its childs. */
typedef enum {STOP, GET_NODEID, NONE} request_t;
/* Union for data sharing among threads. */
typedef union data
{
    uint64_t _node_id[2];
}data_t;

/* Declare the data. */
#ifdef __LINUX__
/* For consistency. */
typedef pthread_mutex_t mutex_t;
typedef pthread_t p_thread_t;   // Protocol's thread
#endif
#ifdef __RIOT__
typedef kernel_pid_t p_thread_t;
#endif

typedef struct args{
    p_thread_t  pid;
    data_t      *data;
    data_t      _node_id[2];
    request_t   *request;
    mutex_t     *mtx_data;
    mutex_t     *mtx_req;
}args_t;

int _mutex_lock(mutex_t *mtx);
void _mutex_unlock(mutex_t *mtx);

void *execute_rema(void *arg);

#endif      // INCLUDE_HELPERS_H
