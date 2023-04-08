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

#include "config.h"
#include "ipc-queues.h"
#include "REMA.h"

/* Requests sent by the main thread to its childs. */
typedef enum {STOP, GET_NODEID, NONE} request_t;

void *execute_rema(void *arg);

#endif      // INCLUDE_HELPERS_H
