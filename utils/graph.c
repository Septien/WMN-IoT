#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "graph.h"

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
        (*g)->nodes[i] = NULL;
    }
    (*g)->max_nodes = 0;
    (*g)->index = 0;
}

int graph_insert_node(graph_t *g, REMA_t *node)
{
    assert(g != NULL);
    assert(node != NULL);

    if (g->index >= MAX_NUMBER_NODES) {
        return 0;
    }
    g->nodes[g->index] = node;
    g->index++;

    return 1;
}
