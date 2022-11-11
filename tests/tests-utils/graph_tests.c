#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "graph_tests.h"
#include "graph.h"
#include "cUnit.h"

void setup_graph(void *arg)
{
    (void) arg;
}

bool test_graph_init(void *arg)
{
    graph_t *g = (graph_t *)arg;
    graph_init(&g);

    bool passed = true;
    passed = passed && (g->max_nodes == MAX_NUMBER_NODES);
    for (unsigned int i = 0; i < g->max_nodes; i++) {
        passed = passed && (g->nodes[i] == NULL);
        passed = passed && (g->adj[i] == NULL);
    }

    return passed;
}

bool test_graph_destroy(void *arg)
{
    graph_t *g = (graph_t *)arg;

    graph_init(&g);
    graph_destroy(&g);

    bool passed = true;
    passed = passed && (g->max_nodes == 0);
    for (unsigned int i = 0; i < g->max_nodes; i++) {
        passed = passed && (g->nodes[i] == NULL);
        passed = passed && (g->adj[i] == NULL);
    }

    return passed;
}

void teardown_graph(void *arg)
{
    (void) arg;
}

void graph_tests(void)
{
    cUnit_t *tests;
    graph_t graph;

    cunit_init(&tests, &setup_graph, &teardown_graph, (void *)&graph);

    cunit_add_test(tests, &test_graph_init,     "graph_init\0");
    cunit_add_test(tests, &test_graph_destroy,  "graph_destroy\0");

    printf("\nTesting the graph implementation.\n");
    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
