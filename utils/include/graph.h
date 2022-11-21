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
    uint32_t max_nodes;
    uint32_t index;
}graph_t;

void graph_init(graph_t **g);
void graph_destroy(graph_t **g);

/**
 * @brief Add a new node @node to the graph @g.
 *
 * @param g 
 * @param node 
 * @param neighbors 
 * @param n 
 * @return 1 on succes. 0 otherwise.
 */
int graph_insert_node(graph_t *g, REMA_t *node);

/**
 * @brief Insert @neighbor as neighbor of @node and viceversa. The function searches 
 * for both on the lists of nodes, and then inserts the corresponding neighbor on the
 * adjacence list of each node.It returns 0 when there are no nodes in the graph, 
 * when some of the nodes does not exist in the graph, or when the neighbor already exists
 * in the list of the node.
 *
 * @param g
 * @param node
 * @param neighbor
 * @return 1 on sucess, 0 otherwise.
 */
int graph_insert_neighbor(graph_t *g, vertex_t *node, vertex_t *neighbor);

#endif      // INCLUDE_GRAPH_H
