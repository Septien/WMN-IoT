#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "assert.h"

#include "CFPacket.h"
#include "memory.h"

#include "cUnit.h"

struct cfpacket_data {
    CFPacket_t SINGLE_POINTER cfpkt;
};

void setup_cfpacket(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    cfpacket_init(&data->cfpkt);
}

void teardown_cfpacket(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    cfpacket_destroy(&data->cfpkt);
}

void test_cfpacket_init(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

#ifdef __LINUX__
    assert(cfpkt != NULL);
#endif
#ifdef __RIOT__
    assert(data->cfpkt.node_id[0] == 0);
    assert(data->cfpkt.node_id[1] == 0);
    assert(data->cfpkt.destination_id[0] == 0);
    assert(data->cfpkt.destination_id[1] == 0);
#endif
}

void test_cfpacket_destroy(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;

    cfpacket_destroy(&data->cfpkt);
#ifdef __LINUX__
    assert(data->cfpkt == NULL);
#endif
#ifdef __RIOT__
    assert(data->cfpkt.node_id[0] == 0);
    assert(data->cfpkt.node_id[1] == 0);
    assert(data->cfpkt.destination_id[0] == 0);
    assert(data->cfpkt.destination_id[1] == 0);
#endif
    cfpacket_init(&data->cfpkt);
}

void test_cfpacket_create(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint64_t node_id[2] = {0};
    uint64_t destination_id[2] = {0};
    node_id[0] = rand();
    node_id[1] = rand();
    destination_id[0] = rand();
    destination_id[1] = rand();

    cfpacket_create(REFERENCE cfpkt, node_id, destination_id);

    assert(ARROW(cfpkt)node_id[0] == node_id[0]);
    assert(ARROW(cfpkt)node_id[1] == node_id[1]);
    assert(ARROW(cfpkt)destination_id[0] == destination_id[0]);
    assert(ARROW(cfpkt)destination_id[1] == destination_id[1]);
}

void test_cfpacket_clear(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint64_t node_id[2] = {0};
    uint64_t destination_id[2] = {0};
    node_id[0] = rand();
    node_id[1] = rand();
    destination_id[0] = rand();
    destination_id[1] = rand();

    cfpacket_create(REFERENCE cfpkt, node_id, destination_id);

    cfpacket_clear(REFERENCE cfpkt);

    assert(ARROW(cfpkt)node_id[0] == 0);
    assert(ARROW(cfpkt)node_id[1] == 0);
    assert(ARROW(cfpkt)destination_id[0] == 0);
    assert(ARROW(cfpkt)destination_id[1] == 0);
}

void test_cfpacket_set_nodeid(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint64_t node_id[2] = {0};
    node_id[0] = rand();
    node_id[1] = rand();
    cfpacket_set_nodeid(REFERENCE cfpkt, node_id);
    assert(ARROW(cfpkt)node_id[0] == node_id[0]);
    assert(ARROW(cfpkt)node_id[1] == node_id[1]);
}

void test_cfpacket_get_nodeid(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint64_t node_id[2] = {0};
    node_id[0] = rand();
    node_id[1] = rand();
    cfpacket_set_nodeid(REFERENCE cfpkt, node_id);
    
    uint64_t node_id_r[2];
    cfpacket_get_nodeid(REFERENCE cfpkt, node_id_r);
    assert(node_id_r[0] == ARROW(cfpkt)node_id[0]);
    assert(node_id_r[1] == ARROW(cfpkt)node_id[1]);
}

void test_cfpacket_set_destinationid(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint64_t destination_id[2] = {0};
    destination_id[0] = rand();
    destination_id[1] = rand();
    cfpacket_set_destinationid(REFERENCE cfpkt, destination_id);
    assert(ARROW(cfpkt)destination_id[0] == destination_id[0]);
    assert(ARROW(cfpkt)destination_id[1] == destination_id[1]);
}

void test_cfpacket_get_destinationid(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint64_t destination_id[2] = {0};
    destination_id[0] = rand();
    destination_id[1] = rand();
    cfpacket_set_destinationid(REFERENCE cfpkt, destination_id);

    uint64_t destination_id_df[2] = {0};
    cfpacket_get_destinationid(REFERENCE cfpkt, destination_id_df);
    assert(destination_id_df[0] == destination_id[0]);
    assert(destination_id_df[1] == destination_id[1]);
    assert(destination_id_df[0] == ARROW(cfpkt)destination_id[0]);
    assert(destination_id_df[1] == ARROW(cfpkt)destination_id[1]);
}

void test_cfpacket_get_packet_bytestring(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint64_t node_id[2] = {0};
    uint64_t destination_id[2] = {0};
    node_id[0] = rand();
    node_id[1] = rand();
    destination_id[0] = rand();
    destination_id[1] = rand();

    cfpacket_create(REFERENCE cfpkt, node_id, destination_id);

    ARRAY byteString;

    cfpacket_get_packet_bytestring(REFERENCE cfpkt, &byteString);
#ifdef __LINUX__
    assert(byteString != NULL);
#endif

    uint64_t node_id1 = 0;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 1)) << 56;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 2)) << 48;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 3)) << 40;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 4)) << 32;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 5)) << 24;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 6)) << 16;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 7)) << 8;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 8));
    uint64_t node_id2 = 0;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 9)) << 56;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 10)) << 48;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 11)) << 40;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 12)) << 32;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 13)) << 24;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 14)) << 16;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 15)) << 8;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 16));
    uint64_t destination_id1 = 0;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 17)) << 56;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 18)) << 48;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 19)) << 40;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 20)) << 32;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 21)) << 24;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 22)) << 16;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 23)) << 8;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 24));
    uint64_t destination_id2 = 0;
    destination_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 25)) << 56;
    destination_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 26)) << 48;
    destination_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 27)) << 40;
    destination_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 28)) << 32;
    destination_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 29)) << 24;
    destination_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 30)) << 16;
    destination_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 31)) << 8;
    destination_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 32));

    assert(node_id1 == node_id[0]);
    assert(node_id2 == node_id[1]);
    assert(destination_id1 == destination_id[0]);
    assert(destination_id2 == destination_id[1]);
#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
#endif
}

void test_cfpacket_construct_packet_from_bytestring(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    ARRAY byteString;
#ifdef __LINUX__
    byteString = (uint8_t *)malloc(PACKET_SIZE_MAC * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteString, PACKET_SIZE_MAC);
#endif
    uint64_t node_id[2] = {0};
    uint64_t destination_id[2] = {0};
    node_id[0] = rand();
    node_id[1] = rand();
    destination_id[0] = rand();
    destination_id[1] = rand();
    WRITE_ARRAY(REFERENCE byteString, 0,                                   0);
    WRITE_ARRAY(REFERENCE byteString, (node_id[0] & 0xff00000000000000) >> 56,    1);
    WRITE_ARRAY(REFERENCE byteString, (node_id[0] & 0x00ff000000000000) >> 48,    2);
    WRITE_ARRAY(REFERENCE byteString, (node_id[0] & 0x0000ff0000000000) >> 40,    3);
    WRITE_ARRAY(REFERENCE byteString, (node_id[0] & 0x000000ff00000000) >> 32,    4);
    WRITE_ARRAY(REFERENCE byteString, (node_id[0] & 0x00000000ff000000) >> 24,    5);
    WRITE_ARRAY(REFERENCE byteString, (node_id[0] & 0x0000000000ff0000) >> 16,    6);
    WRITE_ARRAY(REFERENCE byteString, (node_id[0] & 0x000000000000ff00) >> 8,     7);
    WRITE_ARRAY(REFERENCE byteString, (node_id[0] & 0x00000000000000ff),          8);
    WRITE_ARRAY(REFERENCE byteString, (node_id[1] & 0xff00000000000000) >> 56,    9);
    WRITE_ARRAY(REFERENCE byteString, (node_id[1] & 0x00ff000000000000) >> 48,    10);
    WRITE_ARRAY(REFERENCE byteString, (node_id[1] & 0x0000ff0000000000) >> 40,    11);
    WRITE_ARRAY(REFERENCE byteString, (node_id[1] & 0x000000ff00000000) >> 32,    12);
    WRITE_ARRAY(REFERENCE byteString, (node_id[1] & 0x00000000ff000000) >> 24,    13);
    WRITE_ARRAY(REFERENCE byteString, (node_id[1] & 0x0000000000ff0000) >> 16,    14);
    WRITE_ARRAY(REFERENCE byteString, (node_id[1] & 0x000000000000ff00) >> 8,     15);
    WRITE_ARRAY(REFERENCE byteString, (node_id[1] & 0x00000000000000ff),          16);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0xff00000000000000) >> 56,    17);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x00ff000000000000) >> 48,    18);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x0000ff0000000000) >> 40,    19);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x000000ff00000000) >> 32,    20);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x00000000ff000000) >> 24,    21);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x0000000000ff0000) >> 16,    22);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x000000000000ff00) >> 8,     23);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x00000000000000ff),          24);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0xff00000000000000) >> 56,    25);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x00ff000000000000) >> 48,    26);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x0000ff0000000000) >> 40,    27);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x000000ff00000000) >> 32,    28);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x00000000ff000000) >> 24,    29);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x0000000000ff0000) >> 16,    30);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x000000000000ff00) >> 8,     31);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x00000000000000ff),          32);
    
    int n = rand();
    for (int i = 33; i < PACKET_SIZE_MAC; i++)
        WRITE_ARRAY(REFERENCE byteString, n, i);

    cfpacket_construct_packet_from_bytestring(REFERENCE cfpkt, &byteString);
    assert(ARROW(cfpkt)node_id[0]        == node_id[0]);
    assert(ARROW(cfpkt)node_id[1]        == node_id[1]);
    assert(ARROW(cfpkt)destination_id[0] == destination_id[0]);
    assert(ARROW(cfpkt)destination_id[1] == destination_id[1]);

#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
#endif
}

void executeTestsCF(void)
{
    cUnit_t *tests;
    struct cfpacket_data data;

    cunit_init(&tests, &setup_cfpacket, &teardown_cfpacket, (void *)&data);

    cunit_add_test(tests, &test_cfpacket_init,      "cfpacket_init\0");
    cunit_add_test(tests, &test_cfpacket_destroy,   "cfpacket_destroy\0");
    cunit_add_test(tests, &test_cfpacket_create,    "cfpacket_create\0");
    cunit_add_test(tests, &test_cfpacket_clear,     "cfpacket_clear\0");
    cunit_add_test(tests, &test_cfpacket_set_nodeid, "cfpacket_set_nodeid\0");
    cunit_add_test(tests, &test_cfpacket_get_nodeid, "cfpacket_get_nodeid\0");
    cunit_add_test(tests, &test_cfpacket_set_destinationid, "cfpacket_set_destinationid\0");
    cunit_add_test(tests, &test_cfpacket_get_destinationid, "cfpacket_get_destinationid\0");
    cunit_add_test(tests, &test_cfpacket_get_packet_bytestring,             "cfpacket_get_packet_bytestring\0");
    cunit_add_test(tests, &test_cfpacket_construct_packet_from_bytestring,  "cfpacket_construct_packet_from_bytestring\0");

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
