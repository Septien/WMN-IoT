#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "graph.h"

#ifdef __RIOT__
// Thread's stack
static char thread_stack[MAX_NUMBER_NODES][THREAD_STACKSIZE_MAIN];
#endif

void graph_init(graph_t **g)
{
    assert(g != NULL);
    assert(*g != NULL);

    (*g)->max_nodes = MAX_NUMBER_NODES;
    (*g)->index = 0;
    for (uint32_t i = 0; i < (*g)->max_nodes; i++) {
        (*g)->adj[i] = NULL;
        (*g)->nodes[i] = NULL;
    }
}

void graph_destroy(graph_t **g)
{
    assert(g != NULL);
    assert(*g != NULL);

    for (uint32_t i = 0; i < (*g)->max_nodes; i++) {
        (*g)->adj[i] = NULL;
        if ((*g)->nodes[i] != NULL) {
            // Terminate associated thread
            while (_mutex_lock((*g)->nodes[i]->mtx_req) != 1) ;
            (*g)->nodes[i]->request = STOP;
            _mutex_unlock((*g)->nodes[i]->mtx_req);
            pthread_join((*g)->nodes[i]->pid, NULL);
        }
        (*g)->nodes[i] = NULL;
    }
    (*g)->max_nodes = 0;
    (*g)->index = 0;
}

int graph_insert_node(graph_t *g, node_t *new_node, uint64_t *node_id, void *func(void *))
{
    assert(g != NULL);
    assert(new_node != NULL);
    assert(node_id != NULL);
    assert(func != NULL);

    if (g->index >= MAX_NUMBER_NODES) {
        return 0;
    }
    // Check the node was not inserted previously
    for (uint32_t i = 0; i < g->index; i++) {
        if (memcmp(g->nodes[i]->_node_id, node_id, 2 * sizeof(uint64_t)) == 0) {
            return 0;
        }
    }
    g->nodes[g->index] = new_node;
    memcpy(g->nodes[g->index]->_node_id, node_id, 2 * sizeof(uint64_t));
    // Execute threads
#ifdef __LINUX__
    pthread_t pid;
    pthread_create(&pid, NULL, func, (void *)new_node);
    g->nodes[g->index]->pid = pid;
#endif
#ifdef __RIOT__
    kernel_pid_t pid = thread_create(thread_stack[g->index], THREAD_STACKSIZE_MAIN, THREAD_PRIORITY_MAIN-1,
    THREAD_CREATE_SLEEPING, func, (void *)new_node, "node");
    g->nodes[g->index]->pid = pid;
    if (pid < 0) {
        printf("No thread created.\n");
    }
    thread_wakeup(pid);
#endif
    g->index++;

    return 1;
}
#if 0
int graph_insert_neighbor(graph_t *g, vertex_t *node, vertex_t *neighbor)
{
    assert (g != NULL);
    assert(node != NULL);
    assert(neighbor != NULL);
    // An empty graph
    if (g->index == 0) {
        return 0;
    }
    if (neighbor->next != NULL) {
        return 0;
    }
    // Find the desired node and neighbor index
    int index_nd = -1, index_nbr = -1;
    for (unsigned int i = 0; i < g->index; i++) {
        if (memcmp(g->nodes[i]->_node_id, node->node->_node_id, 2*sizeof(uint64_t)) == 0) {
            index_nd = (int)i;
        }
        if (memcmp(g->nodes[i]->_node_id, neighbor->node->_node_id, 2*sizeof(uint64_t)) == 0) {
            index_nbr = (int)i;
        }
    }
    if (index_nd == -1) {
        return 0;
    }
    if (index_nbr == -1) {
        return 0;
    }
    
    /* We have the node's and the neighbbor's index, now insert the 
    new entry into the adjacence list for both the node and the neighbor */
    vertex_t *v = g->adj[index_nd];
    if (v == NULL) {
        g->adj[index_nd] = neighbor;
        g->adj[index_nd]->next = NULL;
    }
    else {
        if (memcmp(v->node->_node_id, neighbor->node->_node_id, 2*sizeof(uint64_t)) == 0) {
            return 0;
        }
        // Get to the list's end
        while (v->next != NULL) {
            // check if the neighbor already exists
            if (memcmp(v->node->_node_id, neighbor->node->_node_id, 2*sizeof(uint64_t)) == 0) {
                return 0;
            }
            v = v->next;
        }
        v->next = neighbor;
        v->next->next = NULL;
    }
    v = g->adj[index_nbr];
    if (v == NULL) {
        g->adj[index_nbr] = node;
    }
    else {
        while (v->next != NULL) {
            v = v->next;
        }
        v->next = node;
    }
    return 1;
}
#endif
int graph_transmit_message(graph_t *g)
{
    (void) g;
    return 0;
}
