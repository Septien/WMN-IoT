#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "config.h"
#include "graph_tests.h"
#include "graph.h"
#include "cUnit.h"

void setup_graph(void *arg)
{
    graph_t *g = (graph_t *)arg;
    graph_init(&g);
}

bool test_graph_init(void *arg)
{
    graph_t *g = (graph_t *)arg;
    graph_destroy(&g);
    graph_init(&g);

    bool passed = true;
    passed = passed && (g->max_nodes == MAX_NUMBER_NODES);
    passed = passed && (g->index == 0);
    for (unsigned int i = 0; i < g->max_nodes; i++) {
        passed = passed && (g->nodes[i] == NULL);
        passed = passed && (g->adj[i] == NULL);
    }

    return passed;
}

bool test_graph_destroy(void *arg)
{
    graph_t *g = (graph_t *)arg;

    graph_destroy(&g);

    bool passed = true;
    passed = passed && (g->max_nodes == 0);
    for (unsigned int i = 0; i < g->max_nodes; i++) {
        passed = passed && (g->nodes[i] == NULL);
        passed = passed && (g->adj[i] == NULL);
    }

    graph_init(&g);
    return passed;
}

bool test_graph_insert_first(void *arg)
{
    graph_t *g = (graph_t *)arg;
    REMA_t node;
    node.node_id[0] = 1234567890;
    node.node_id[1] = 9876543210;

    int ret = graph_insert_node(g, &node);
    bool passed = true;
    passed = passed && (ret == 1);
    passed = passed && (g->index == 1);
    // Check a new entry exists at position 0
    passed = passed && (g->nodes[0] != NULL);
    passed = passed && (g->adj[0] == NULL);
    // Check the address of g->nodes[0] is the same as nodep
    passed = passed && (g->nodes[0] == &node);

    return passed;
}

bool test_insert_several_nodes(void *arg)
{
    graph_t *g = (graph_t *)arg;
    uint64_t ids[2] = {1234567890 ,9876543210};

    int n = 100;
    REMA_t nodes[n];
    // Store the nodes ids, and create the vertices
    for (int i = 0; i < n; i++) {
        memcpy(&nodes[i].node_id, ids, sizeof(ids));
        ids[0]++;
        ids[1]--;
    }

    /**
     * I have already initialize the nodes and fill the vertex array, and
     * determine the neighbors for each node. Now I have to test whether the
     * nodes' insertion is correctly performed. For that, I have to:
     *  -Check whether the function returned 1.
     *  -Check whether the graph index was incremented by one.
     *  -Check whether the entry at the corresponding index of the nodes array,
     *   corresponds to right node.
     */
    bool passed = true;
    unsigned int index = g->index;
    for (int i = 0; i < n; i++) {
        int ret = graph_insert_node(g, &nodes[i]);
        // Check success
        passed = passed && (ret == 1);
        // Check the index is updated
        passed = passed && (g->index == index+1);
        passed = passed && (g->nodes[index] != NULL);
        passed = passed && (g->nodes[index] == &nodes[i]);
        passed = passed && (g->adj[index] == NULL);
        index++;
    }
    return passed;
}

bool test_insert_MAX_NODES_1_nodes(void *arg)
{
    // Insert MAX_NUMBER_NODES + 1, return 0 on the last attemp
    graph_t *g = (graph_t *)arg;
    REMA_t nodes[MAX_NUMBER_NODES+1];
    uint64_t ids[2] = {1234567890 ,9876543210};
    // Store the nodes ids, and create the vertices
    for (unsigned int i = 0; i < MAX_NUMBER_NODES+1; i++) {
        memcpy(&nodes[i].node_id, ids, sizeof(ids));
        ids[0]++;
        ids[1]--;
    }

    /**
     * Insert the MAX_NUMBER_NODES into the graphs.
     * When inserting the MAX_NUMBER_NODES+1 node,
     * return 0.
     */
    int ret = 1;
    bool passed = true;
    for (uint32_t i = 0; i < MAX_NUMBER_NODES; i++) {
        ret = graph_insert_node(g, &nodes[i]);
        passed = passed && (ret == 1);
    }
    ret = graph_insert_node(g, &nodes[MAX_NUMBER_NODES+1]);
    passed = passed && (ret == 0);

    return passed;
}

void teardown_graph(void *arg)
{
    graph_t *g = (graph_t *)arg;
    graph_destroy(&g);
}

void graph_tests(void)
{
    cUnit_t *tests;
    graph_t graph;

    cunit_init(&tests, &setup_graph, &teardown_graph, (void *)&graph);

    cunit_add_test(tests, &test_graph_init,                 "graph_init\0");
    cunit_add_test(tests, &test_graph_destroy,              "graph_destroy\0");
    cunit_add_test(tests, &test_graph_insert_first,         "insertion of first node\0");
    cunit_add_test(tests, &test_insert_several_nodes,       "insertion of several nodes\0");
    cunit_add_test(tests, &test_insert_MAX_NODES_1_nodes,   "insertion of max+1 nodes\0");

    printf("\nTesting the graph's implementation.\n");
    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
