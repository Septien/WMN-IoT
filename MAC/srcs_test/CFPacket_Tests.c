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
    assert(cfpkt.frequency == 0);
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
    assert(cfpkt.frequency == 0);
#endif
}

void test_cfpacket_create(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint16_t nodeID = (uint16_t)rand();
    uint16_t destinationID = (uint16_t)rand();
    uint32_t frequency = (uint32_t)rand();

    cfpacket_create(REFERENCE cfpkt, nodeID, destinationID, frequency);

    assert(ARROW(cfpkt)nodeID == nodeID);
    assert(ARROW(cfpkt)destinationID == destinationID);
    assert(ARROW(cfpkt)frequency == frequency);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_clear(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint16_t nodeID = (uint16_t)rand();
    uint16_t destinationID = (uint16_t)rand();
    uint32_t netTime = (uint32_t)rand();

    cfpacket_create(REFERENCE cfpkt, nodeID, destinationID, netTime);

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

void test_cfpacket_set_frequency(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint32_t frequency = rand();
    cfpacket_set_frequency(REFERENCE cfpkt, frequency);
    assert(ARROW(cfpkt)frequency == frequency);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_get_frequency(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint32_t frequency = rand();
    cfpacket_set_frequency(REFERENCE cfpkt, frequency);
    
    uint32_t freqA;
    freqA = cfpacket_get_frequency(REFERENCE cfpkt);
    assert(freqA == frequency);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_get_packet_bytestring(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint16_t nodeID = (uint16_t)rand();
    uint16_t destinationID = (uint16_t)rand();
    uint32_t frequency = (uint32_t)rand();
    cfpacket_create(REFERENCE cfpkt, nodeID, destinationID, frequency);

    size_t size = sizeof(nodeID) + sizeof(destinationID);
    size += sizeof(frequency);
    ARRAY byteString;
    size_t size2;

    cfpacket_get_packet_bytestring(REFERENCE cfpkt, &byteString, &size2);
#ifdef __LINUX__
    assert(byteString != NULL);
#endif

    assert(READ_ARRAY(REFERENCE byteString, 0) == ((nodeID & 0xff00) >> 8));
    assert(READ_ARRAY(REFERENCE byteString, 1) == (nodeID & 0x00ff));
    assert(READ_ARRAY(REFERENCE byteString, 2) == (destinationID & 0xff00) >> 8);
    assert(READ_ARRAY(REFERENCE byteString, 3) == (destinationID & 0x00ff));
    assert(READ_ARRAY(REFERENCE byteString, 4) == (frequency & 0xff000000 >> 24));
    assert(READ_ARRAY(REFERENCE byteString, 5) == (frequency & 0x00ff0000 >> 16));
    assert(READ_ARRAY(REFERENCE byteString, 6) == (frequency & 0x0000ff00 >> 8));
    assert(READ_ARRAY(REFERENCE byteString, 7) == (frequency & 0x000000ff));
    assert(size2 == size);

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

    size_t size = 2 * sizeof(uint8_t);
    size += sizeof(uint32_t);

    ARRAY byteString;
#ifdef __LINUX__
    byteString = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteString, size);
#endif
    uint32_t frequency = (uint32_t) rand();
    uint16_t nodeid = (uint16_t) rand();
    uint16_t destinationid = (uint16_t) rand();
    WRITE_ARRAY(REFERENCE byteString, (nodeid & 0xff00) >> 8,              0);
    WRITE_ARRAY(REFERENCE byteString, (nodeid & 0x00ff),                   1);
    WRITE_ARRAY(REFERENCE byteString, (destinationid & 0xff00) >> 8,       2);
    WRITE_ARRAY(REFERENCE byteString, (destinationid & 0x00ff),            3);
    WRITE_ARRAY(REFERENCE byteString, ((frequency & 0xff000000) >> 24),    4);
    WRITE_ARRAY(REFERENCE byteString, ((frequency & 0x00ff0000) >> 16),    5);
    WRITE_ARRAY(REFERENCE byteString, ((frequency & 0x0000ff00) >> 8),     6);
    WRITE_ARRAY(REFERENCE byteString, (frequency & 0x000000ff),            7);

    cfpacket_construct_packet_from_bytestring(REFERENCE cfpkt, &byteString, size);
    assert(ARROW(cfpkt)nodeID        == nodeid);
    assert(ARROW(cfpkt)destinationID == destinationid);
    assert(ARROW(cfpkt)frequency     == frequency);

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

    printf("Testing _cfpacket_set_frequency function.\n");
    test_cfpacket_set_frequency();
    printf("Test passed.\n");

    printf("Testing cfpacket_get_frequency function.\n");
    test_cfpacket_get_frequency();
    printf("Test passed.\n");

    printf("Testing cfpacket_get_packet_bytestring function.\n");
    test_cfpacket_get_packet_bytestring();
    printf("Test passed.\n");

    printf("Testing cfpacket_construct_packet_from_bytestring function.\n");
    test_cfpacket_construct_packet_from_bytestring();
    printf("Test passed.\n");
}
