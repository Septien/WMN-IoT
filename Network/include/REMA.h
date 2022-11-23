/**
 * @file REMA.h
 * @author Jose Antonio Septien Hernandez (ja.septienhernandez@ugto.mx)
 * @brief Main header for the implementation of the REMA routing protocol, 
 * which implements the stacks' Network layer. It provides the main data 
 * structure along with the main API for interacting with the protocol.
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef INCLUDE_REMA_H
#define INCLUDE_REMA_H

#include <stdint.h>

typedef struct REMA {
    uint64_t _node_id[2];
}REMA_t;

void rema_init(REMA_t **rema);
void rema_terminate(REMA_t **rema);

/* Just a dummy function, to simulate the execution of the protocol. */
void rema_dummy(REMA_t *rema);
#endif  // INCLUDE_REMA_H
