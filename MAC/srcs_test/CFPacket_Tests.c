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
    printf("Hello\n");
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
    assert(cfpkt.nodeID == 0);
    assert(cfpkt.destinationID == 0);
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
    assert(data->cfpkt.nodeID == 0);
    assert(data->cfpkt.destinationID == 0);
#endif
    cfpacket_init(&data->cfpkt);
}

void test_cfpacket_create(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint16_t nodeID = (uint16_t)rand();
    uint16_t destinationID = (uint16_t)rand();

    cfpacket_create(REFERENCE cfpkt, nodeID, destinationID);

    assert(ARROW(cfpkt)nodeID == nodeID);
    assert(ARROW(cfpkt)destinationID == destinationID);
}

void test_cfpacket_clear(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint16_t nodeID = (uint16_t)rand();
    uint16_t destinationID = (uint16_t)rand();

    cfpacket_create(REFERENCE cfpkt, nodeID, destinationID);

    cfpacket_clear(REFERENCE cfpkt);

    assert(ARROW(cfpkt)nodeID == 0);
    assert(ARROW(cfpkt)destinationID == 0);
}

void test_cfpacket_set_nodeid(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint16_t nodeID = rand();
    cfpacket_set_nodeid(REFERENCE cfpkt, nodeID);
    assert(ARROW(cfpkt)nodeID == nodeID);
}

void test_cfpacket_get_nodeid(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint16_t nodeID = rand();
    cfpacket_set_nodeid(REFERENCE cfpkt, nodeID);
    
    uint16_t nodeIDR;
    nodeIDR = cfpacket_get_nodeid(REFERENCE cfpkt);
    assert(nodeIDR == ARROW(cfpkt)nodeID);
}

void test_cfpacket_set_destinationid(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint16_t destinationID = (uint16_t)rand();
    cfpacket_set_destinationid(REFERENCE cfpkt, destinationID);
    assert(ARROW(cfpkt)destinationID == destinationID);
}

void test_cfpacket_get_destinationid(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint16_t destinationID = (uint16_t)rand();
    cfpacket_set_destinationid(REFERENCE cfpkt, destinationID);

    uint16_t dIDF = cfpacket_get_destinationid(REFERENCE cfpkt);
    assert(dIDF == destinationID);
    assert(dIDF == ARROW(cfpkt)destinationID);
}

void test_cfpacket_get_packet_bytestring(void *arg)
{
    struct cfpacket_data *data = (struct cfpacket_data *)arg;
    CFPacket_t SINGLE_POINTER cfpkt = data->cfpkt;

    uint16_t nodeID = (uint16_t)rand();
    uint16_t destinationID = (uint16_t)rand();
    cfpacket_create(REFERENCE cfpkt, nodeID, destinationID);

    ARRAY byteString;

    cfpacket_get_packet_bytestring(REFERENCE cfpkt, &byteString);
#ifdef __LINUX__
    assert(byteString != NULL);
#endif

    assert(READ_ARRAY(REFERENCE byteString, 0) == 0);   // Type 0 packet, cf
    assert(READ_ARRAY(REFERENCE byteString, 1) == ((nodeID & 0xff00) >> 8));
    assert(READ_ARRAY(REFERENCE byteString, 2) == (nodeID & 0x00ff));
    assert(READ_ARRAY(REFERENCE byteString, 3) == (destinationID & 0xff00) >> 8);
    assert(READ_ARRAY(REFERENCE byteString, 4) == (destinationID & 0x00ff));

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
    uint16_t nodeid = (uint16_t) rand();
    uint16_t destinationid = (uint16_t) rand();
    WRITE_ARRAY(REFERENCE byteString, 0,                                   0);
    WRITE_ARRAY(REFERENCE byteString, (nodeid & 0xff00) >> 8,              1);
    WRITE_ARRAY(REFERENCE byteString, (nodeid & 0x00ff),                   2);
    WRITE_ARRAY(REFERENCE byteString, (destinationid & 0xff00) >> 8,       3);
    WRITE_ARRAY(REFERENCE byteString, (destinationid & 0x00ff),            4);
    int n = rand();
    for (int i = 5; i < PACKET_SIZE_MAC; i++)
        WRITE_ARRAY(REFERENCE byteString, n, i);

    cfpacket_construct_packet_from_bytestring(REFERENCE cfpkt, &byteString);
    assert(ARROW(cfpkt)nodeID        == nodeid);
    assert(ARROW(cfpkt)destinationID == destinationid);

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
