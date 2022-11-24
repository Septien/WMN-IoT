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
#include <thread.h>
#endif

#include "config.h"
#include "ipc-queues.h"
#include "REMA.h"

void *execute_rema(void *arg);

#endif      // INCLUDE_HELPERS_H
