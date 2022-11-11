/**
 * @file graph.h
 * @author Jose Antonio Septien Hernandez (ja.septienhernandez@ugto.mx)
 * @brief This graph will be used for making the behavioral tests of the stack.
 * This will simulate a mesh network, and allow the nodes to communicate
 * among each other.
 * @version 0.1
 * @date 2022-11-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef INCLUDE_GRAPH_H
#define INCLUDE_GRAPH_H

#include <stdint.h>

#include "REMA.h"
#include "config.h"

typedef struct vertex {
    REMA_t *node;
    struct vertex *next;
}vertex_t;

typedef struct graph {
    // Array of nodes
    REMA_t *nodes[MAX_NUMBER_NODES];
    vertex_t *adj[MAX_NUMBER_NODES];
    unsigned int max_nodes;
}graph_t;

void graph_init(graph_t **g);
void graph_destroy(graph_t **g);

#endif      // INCLUDE_GRAPH_H
