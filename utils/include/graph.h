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
#include "helpers.h"
#include "config.h"

/**
 * @brief Holds information regarding the thread execution for each node.
 * @pid holds the new thread's process id.
 * @args contain all the arguments to pass to the thread.
 */
typedef args_t node_t;

/**
 * @brief For creating the node's adjacence list.
 * @node holds the node number (in nodes array.)
 */
typedef struct vertex {
    int             node;
    struct vertex   *next;
}vertex_t;

/**
 * @brief For the graph use in the BDD tests.
 * @nodes holds the information regarding each node.
 * @adj holds the adjacence of each node.
 * @index free position at nodes array, for inserting a new node.
 */
typedef struct graph {
    // Array of nodes
    node_t      *nodes[MAX_NUMBER_NODES];
    vertex_t    *adj[MAX_NUMBER_NODES];
    uint32_t    max_nodes;
    uint32_t    index;
}graph_t;

void graph_init(graph_t **g);
void graph_destroy(graph_t **g);

/**
 * @brief Add a new node @node to the graph @g.
 *
 * @param g The network's graph.
 * @param new_node  The node to insert.
 * @param func The function to pass to the new thread, for executing the protocol.
 * @return 1 on succes. 0 otherwise.
 */
int graph_insert_node(graph_t *g, node_t *new_node, uint64_t *node_id, void *func(void *));

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

/**
 * @brief Search through all the queues to verify any pending message. If one is found, it
 * retrives it from the queue, gets the destination's node_id, and searchs the adjacence list
 * for the intended node. If the node is not a neighbor, it discards the message and indicates
 * the sender.
 *
 * @param g
 * @param sender
 * @param recv
 * @return 1 on success
 */
int graph_transmit_message(graph_t *g);

#endif      // INCLUDE_GRAPH_H
