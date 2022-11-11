#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "graph.h"

void graph_init(graph_t **g)
{
    assert(g != NULL);
    assert(*g != NULL);

    (*g)->max_nodes = MAX_NUMBER_NODES;
    unsigned int i;
    for (i = 0; i < (*g)->max_nodes; i++) {
        (*g)->adj[i] = NULL;
        (*g)->nodes[i] = NULL;
    }
}

void graph_destroy(graph_t **g)
{
    assert(g != NULL);
    assert(*g != NULL);

    for (unsigned int i = 0; i < (*g)->max_nodes; i++) {
        (*g)->adj[i] = NULL;
        (*g)->nodes[i] = NULL;
    }
    (*g)->max_nodes = 0;
}
