#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "config.h"
#include "graph_tests.h"
#include "graph.h"
#include "cUnit.h"

#ifdef __RIOT__
#include "ztimer.h"
#endif

static void setup(void *arg)
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
    uint64_t node_id[2] = {1234567890, 9876543210};
    data_t data = {0};
    request_t request = NONE;
    mutex_t mtx_data, mtx_req;
#ifdef __LINUX__
    mtx_data = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    mtx_req = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
#endif
#ifdef __RIOT__
    mtx_data = (mutex_t) MUTEX_INIT;
    mtx_req = (mutex_t) MUTEX_INIT;
#endif
    node_t node = {.pid = 0, .data = &data, .request = &request,
                    .mtx_data = &mtx_data, .mtx_req = &mtx_req};

    int ret = graph_insert_node(g, &node, node_id, execute_rema);
#ifdef __LINUX__
    usleep(10);
#endif
    char str[200];
    bool passed = true;
    passed = check_condition(passed, ret == 1, "Function return 1", str);
    passed = check_condition(passed, g->index == 1, "Index incremented by 1", str);
    // Check a new entry exists at position 0
    passed = check_condition(passed, g->nodes[0] != NULL, "Node is stored", str);
    passed = check_condition(passed, g->adj[0] == NULL, "Adj list is empty", str);
    passed = check_condition(passed, g->nodes[0]->pid != 0, "pid is non zero", str);
    passed = check_condition(passed, memcmp(g->nodes[0]->_node_id, node_id, 2*sizeof(uint64_t)) == 0,
                            "node_id is correct", str);
    if (!passed) {
        printf("%s\n", str);
    }

    return passed;
}
#if 0
bool test_insert_nodes_same_ids(void *arg)
{
    graph_t *g = (graph_t *)arg;
    uint64_t node_id[2] = {1234567890, 9876543210};
    node_t node = {.pid = 0, .args = &arguments[0]};
    node_t node1 = {.pid = 0, .args = &arguments[1]};

    int ret = graph_insert_node(g, &node, node_id, execute_rema);
    ret = graph_insert_node(g, &node1, node_id, execute_rema);
    
    return ret == 0;
}

bool test_insert_several_nodes(void *arg)
{
    graph_t *g = (graph_t *)arg;
    uint64_t ids[2] = {1234567890 ,9876543210};

    int n = 10;
    node_t nodes[n];

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
        nodes[i] = (node_t) {.pid = 0, .args = &arguments[i]};
        int ret = graph_insert_node(g, &nodes[i], ids, execute_rema);
        ids[0]++;
        ids[1]--;
        // Check success
        passed = passed && (ret == 1);
        // Check the index is updated
        passed = passed && (g->index == index+1);
        passed = passed && (g->nodes[index] != NULL);
        passed = passed && (g->nodes[index] == &nodes[i]);
        passed = passed && (g->adj[index] == NULL);
        index++;
    }
    sleep(10);
    return passed;
}

bool test_insert_MAX_NODES_1_nodes(void *arg)
{
    // Insert MAX_NUMBER_NODES + 1, return 0 on the last attemp
    graph_t *g = (graph_t *)arg;
    node_t nodes[MAX_NUMBER_NODES+1];
    uint64_t ids[2] = {1234567890 ,9876543210};

    /**
     * Insert the MAX_NUMBER_NODES into the graphs.
     * When inserting the MAX_NUMBER_NODES+1 node,
     * return 0.
     */
    int ret = 1;
    bool passed = true;
    for (uint32_t i = 0; i < MAX_NUMBER_NODES; i++) {
        nodes[i] = (node_t) {.pid = 0, .args = &arguments[i]};
        ret = graph_insert_node(g, &nodes[i], ids, execute_rema);
        passed = passed && (ret == 1);
        ids[0]++;
        ids[1]++;
    }
    nodes[MAX_NUMBER_NODES+1] = (node_t) {.pid = 0, .args = NULL};
    ret = graph_insert_node(g, &nodes[MAX_NUMBER_NODES+1], ids, execute_rema);
    passed = passed && (ret == 0);

    return passed;
}

bool test_insert_neighbor_empty_graph(void *arg)
{
    graph_t *g = (graph_t *)arg;

    REMA_t node1, node2;
    uint64_t id1[2] = {1234567890, 9876543210};
    uint64_t id2[2] = {1234567891, 9876543211};
    memcpy(node1._node_id, id1, sizeof(id1));
    memcpy(node2._node_id, id2, sizeof(id2));
    vertex_t v1 = {.node = &node1, .next = NULL};
    vertex_t v2 = {.node = &node2, .next = NULL};
    /**
     * Try to insert a neighbor into an empty graph (no nodes).
     * Return 0.
     */
    int ret = graph_insert_neighbor(g, &v1, &v2);

    return ret == 0;
}

bool test_insert_non_existing_neighbor_single_node(void *arg)
{
    graph_t *g = (graph_t *)arg;
    REMA_t node1, node2;
    uint64_t id1[2] = {1234567890, 9876543210};
    uint64_t id2[2] = {1234567891, 9876543211};
    memcpy(node1._node_id, id1, sizeof(id1));
    memcpy(node2._node_id, id2, sizeof(id2));
    vertex_t v1 = {.node = &node1, .next = NULL};
    vertex_t v2 = {.node = &node2, .next = NULL};

    int ret = graph_insert_node(g, &node1);
    
    ret = graph_insert_neighbor(g, &v1, &v2);
    
    return ret == 0;
}

bool test_insert_no_existing_node(void *arg)
{
    graph_t *g = (graph_t *)arg;

    REMA_t node1, node2;
    uint64_t id1[2] = {1234567890, 9876543210};
    uint64_t id2[2] = {1234567891, 9876543211};
    memcpy(node1._node_id, id1, sizeof(id1));
    memcpy(node2._node_id, id2, sizeof(id2));
    vertex_t v1 = {.node = &node1, .next = NULL};
    vertex_t v2 = {.node = &node2, .next = NULL};

    int ret = graph_insert_node(g, &node2);
    ret = graph_insert_neighbor(g, &v1, &v2);

    return ret == 0;
}

bool test_insert_existing_neighbor_single_node(void *arg)
{
    graph_t *g = (graph_t *)arg;

    REMA_t node1, node2;
    uint64_t id1[2] = {1234567890, 9876543210};
    uint64_t id2[2] = {1234567891, 9876543211};
    memcpy(node1._node_id, id1, sizeof(id1));
    memcpy(node2._node_id, id2, sizeof(id2));
    vertex_t v1 = {.node = &node1, .next = NULL};
    vertex_t v2 = {.node = &node2, .next = NULL};

    int ret = graph_insert_node(g, &node1);
    ret = graph_insert_node(g, &node2);

    ret = graph_insert_neighbor(g, &v1, &v2);
    bool passed = true;
    passed = ret == 1;
    // Verify the adjacence list was correctly updated
    passed = passed && (g->adj[0] != NULL);
    passed = passed && (g->adj[1] != NULL);
    passed = passed && (g->adj[0] == &v2);
    passed = passed && (g->adj[1] == &v1);
    passed = passed && (g->adj[0]->next == NULL);
    passed = passed && (g->adj[1]->next == NULL);
    return passed;
}

bool test_insert_multiple_neighbors(void *arg)
{
    graph_t *g = (graph_t *)arg;

    int n = 5;
    REMA_t node, neighbors[n];
    uint64_t id1[2] = {1234567890, 9876543210};
    memcpy(node._node_id, id1, sizeof(id1));
    vertex_t v1 = {.node = &node, .next = NULL};
    vertex_t v[n];

    int ret = graph_insert_node(g, &node);

    char str[500] = {0};
    bool passed = true;
    for (int i = 0; i < n; i++) {
        id1[0]++;
        id1[1]--;
        memcpy(neighbors[i]._node_id, id1, sizeof(id1));
        v[i] = (vertex_t){.node = &neighbors[i], .next = NULL};
        ret = graph_insert_node(g, &neighbors[i]);
        ret = graph_insert_neighbor(g, &v1, &v[i]);
        passed = check_condition(passed, ret == 1, "\nReturn value of graph_insert_neighbor is 1", str);
    }

    int count = 0;
    // For the first node
    vertex_t *v2 = g->adj[0];
    while (v2 != NULL) {
        v2 = v2->next;
        count++;
    }
    passed = check_condition(passed, count == n, "Check the number of neighbors", str);
    // The rest of the nodes were inserted in consecutive order. Check each one has exactly 1 neighbor
    for (int i = 0; i < n; i++) {
        v2 = g->adj[i+1];
        passed = check_condition(passed, v2 != NULL, "Node has no neighbor", str);
        if (v2 != NULL) {
            passed = check_condition(passed, v2->next == NULL, "Node has no more neighbors", str);
        }
    }

    if (!passed) {
        printf("%s", str);
    }
    return passed;
}

bool test_inset_vertex_with_next_not_null(void *arg)
{
    graph_t *g = (graph_t *)arg;

    REMA_t node1, node2;
    uint64_t id1[2] = {1234567890, 9876543210};
    uint64_t id2[2] = {1234567891, 9876543211};
    memcpy(node1._node_id, id1, sizeof(id1));
    memcpy(node2._node_id, id2, sizeof(id2));
    vertex_t v1 = {.node = &node1, .next = NULL};
    vertex_t v2 = {.node = &node2, .next = &v1};

    int ret = graph_insert_node(g, &node1);
    ret = graph_insert_node(g, &node2);

    ret = graph_insert_neighbor(g, &v1, &v2);
    char str[100];
    bool passed = true;
    passed = check_condition(passed, ret == 0, "insert neighbor with next != NULL\0", str);
    if (!passed) {
        printf("%s\n", str);
    }

    return passed;
}

// insert twice the same neighbor
bool test_insert_same_neighbor_twice(void *arg)
{
    graph_t *g = (graph_t *)arg;

    REMA_t node1, node2;
    uint64_t id1[2] = {1234567890, 9876543210};
    uint64_t id2[2] = {1234567891, 9876543211};
    memcpy(node1._node_id, id1, sizeof(id1));
    memcpy(node2._node_id, id2, sizeof(id2));
    vertex_t v1 = {.node = &node1, .next = NULL};
    vertex_t v2 = {.node = &node2, .next = NULL};

    int ret = graph_insert_node(g, &node1);
    ret = graph_insert_node(g, &node2);

    /* Try inserting the same neighbor to the same node
     twice, it should return 0 */
    ret = graph_insert_neighbor(g, &v1, &v2);
    ret = graph_insert_neighbor(g, &v1, &v2);

    return ret == 0;
}

// insert neighbor with more than two neighbors
bool insert_neighbor_several_neighbors(void *arg)
{
    graph_t *g = (graph_t *)arg;

    REMA_t node1, node2, node3;
    uint64_t id1[2] = {1234567890, 9876543210};
    uint64_t id2[2] = {1234567891, 9876543211};
    uint64_t id3[2] = {1234567892, 9876543212};
    memcpy(node1._node_id, id1, sizeof(id1));
    memcpy(node2._node_id, id2, sizeof(id2));
    memcpy(node3._node_id, id3, sizeof(id3));
    vertex_t v1 = {.node = &node1, .next = NULL};
    vertex_t v2 = {.node = &node2, .next = NULL};
    vertex_t v3 = {.node = &node3, .next = NULL};

    int ret = graph_insert_node(g, &node1);
    ret = graph_insert_node(g, &node2);
    ret = graph_insert_node(g, &node3);

    ret = graph_insert_neighbor(g, &v2, &v3);
    ret = graph_insert_neighbor(g, &v1, &v2);

    char str[100];
    bool passed = true;
    passed = check_condition(passed, ret == 1, "insertion of neighbor with several neighbors\0", str);
    int count = 0;
    vertex_t *v = g->adj[1];    // v2
    while (v != NULL) {
        count++;
        v = v->next;
    }
    passed = check_condition(passed, count == 2, "adjacence list has all the neighbors\0", str);
    if (!passed) {
        printf("%s\n", str);
    }
    return passed;
}
#endif
static void teardown(void *arg)
{
    graph_t *g = (graph_t *)arg;
    graph_destroy(&g);
}

void graph_tests(void)
{
    cUnit_t *tests;
    graph_t graph;

    cunit_init(&tests, &setup, &teardown, (void *)&graph);

    cunit_add_test(tests, &test_graph_init,                                 "graph_init\0");
    cunit_add_test(tests, &test_graph_destroy,                              "graph_destroy\0");
    cunit_add_test(tests, &test_graph_insert_first,                         "insertion of first node\0");
    /*cunit_add_test(tests, &test_insert_nodes_same_ids,                      "inserting 2 nodes with same id\0");
    cunit_add_test(tests, &test_insert_several_nodes,                       "insertion of several nodes\0");
    cunit_add_test(tests, &test_insert_MAX_NODES_1_nodes,                   "insertion of max+1 nodes\0");
    cunit_add_test(tests, &test_insert_neighbor_empty_graph,                "insertion of neighbor into emtpy graph\0");
    cunit_add_test(tests, &test_insert_non_existing_neighbor_single_node,   "insertion of non existing neighbor\0");
    cunit_add_test(tests, &test_insert_no_existing_node,                    "insertion of neighbor with none-existing node\0");
    cunit_add_test(tests, &test_insert_existing_neighbor_single_node,       "successful insertion of neighbor\0");
    cunit_add_test(tests, &test_insert_multiple_neighbors,                  "insertion of several neighbors\0");
    cunit_add_test(tests, &test_inset_vertex_with_next_not_null,            "insert vertex with next different from NULL\0");
    cunit_add_test(tests, &test_insert_same_neighbor_twice,                 "insertion of same neighbor twice\0");
    cunit_add_test(tests, &insert_neighbor_several_neighbors,               "insertion of new neighbor with several neighbors\0");*/

    printf("\nTesting the graph's implementation.\n");
    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
