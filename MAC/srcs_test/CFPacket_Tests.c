#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "assert.h"

#include "CFPacket.h"
#include "memory.h"

void test_cfpacket_init(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

#ifdef __LINUX__
    assert(cfpkt != NULL);
#endif
#ifdef __RIOT__
    assert(cfpkt.nodeID == 0);
    assert(cfpkt.destinationID == 0);
#endif

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_destroy(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    cfpacket_destroy(&cfpkt);
#ifdef __LINUX__
    assert(cfpkt == NULL);
#endif
#ifdef __RIOT__
    assert(cfpkt.nodeID == 0);
    assert(cfpkt.destinationID == 0);
#endif
}

void test_cfpacket_create(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint16_t nodeID = (uint16_t)rand();
    uint16_t destinationID = (uint16_t)rand();

    cfpacket_create(REFERENCE cfpkt, nodeID, destinationID);

    assert(ARROW(cfpkt)nodeID == nodeID);
    assert(ARROW(cfpkt)destinationID == destinationID);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_clear(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint16_t nodeID = (uint16_t)rand();
    uint16_t destinationID = (uint16_t)rand();

    cfpacket_create(REFERENCE cfpkt, nodeID, destinationID);

    cfpacket_clear(REFERENCE cfpkt);

    assert(ARROW(cfpkt)nodeID == 0);
    assert(ARROW(cfpkt)destinationID == 0);

    cfpacket_destroy(&cfpkt);
}

void tests_cfpacket_set_nodeid(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint16_t nodeID = rand();
    cfpacket_set_nodeid(REFERENCE cfpkt, nodeID);
    assert(ARROW(cfpkt)nodeID == nodeID);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_get_nodeid(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint16_t nodeID = rand();
    cfpacket_set_nodeid(REFERENCE cfpkt, nodeID);
    
    uint16_t nodeIDR;
    nodeIDR = cfpacket_get_nodeid(REFERENCE cfpkt);
    assert(nodeIDR == ARROW(cfpkt)nodeID);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_set_destinationid(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    srand(time(NULL));
    uint16_t destinationID = (uint16_t)rand();
    cfpacket_set_destinationid(REFERENCE cfpkt, destinationID);
    assert(ARROW(cfpkt)destinationID == destinationID);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_get_destinationid(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint16_t destinationID = (uint16_t)rand();
    cfpacket_set_destinationid(REFERENCE cfpkt, destinationID);

    uint16_t dIDF = cfpacket_get_destinationid(REFERENCE cfpkt);
    assert(dIDF == destinationID);
    assert(dIDF == ARROW(cfpkt)destinationID);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_get_packet_bytestring(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

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

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_construct_packet_from_bytestring(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

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
    
    cfpacket_destroy(&cfpkt);
}

void executeTestsCF(void)
{
    srand(time(NULL));

    printf("Testing cfpacket_init function.\n");
    test_cfpacket_init();
    printf("Test passed.\n");

    printf("Testing cfpacket_destroy function.\n");
    test_cfpacket_destroy();
    printf("Test passed.\n");

    printf("Testing createPacketCF function.\n");
    test_cfpacket_create();
    printf("Test passed.\n");

    printf("Testing cfpacket_clear function.\n");
    test_cfpacket_clear();
    printf("Test passed.\n");

    printf("Testing cfpacket_set_nodeid function.\n");
    tests_cfpacket_set_nodeid();
    printf("Test passed.\n");
    
    printf("Testing cfpacket_get_nodeid function.\n");
    test_cfpacket_get_nodeid();
    printf("Test passed.\n");

    printf("Testing cfpacket_set_destinationid function.\n");
    test_cfpacket_set_destinationid();
    printf("Test passed.\n");

    printf("Testing cfpacket_get_destinationid function.\n"),
    test_cfpacket_get_destinationid();
    printf("Test passed.\n");

    printf("Testing cfpacket_get_packet_bytestring function.\n");
    test_cfpacket_get_packet_bytestring();
    printf("Test passed.\n");

    printf("Testing cfpacket_construct_packet_from_bytestring function.\n");
    test_cfpacket_construct_packet_from_bytestring();
    printf("Test passed.\n");
}
