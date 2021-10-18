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

    uint8_t nodeID = (uint8_t)rand();
    uint8_t destinationID = (uint8_t)rand();
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

    uint8_t nodeID = (uint8_t)rand();
    uint8_t destinationID = (uint8_t)rand();
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

    uint8_t nodeID = rand();
    cfpacket_set_nodeid(REFERENCE cfpkt, nodeID);
    assert(ARROW(cfpkt)nodeID == nodeID);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_get_nodeid(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint8_t nodeID = rand();
    cfpacket_set_nodeid(REFERENCE cfpkt, nodeID);
    
    uint8_t nodeIDR;
    nodeIDR = cfpacket_get_nodeid(REFERENCE cfpkt);
    assert(nodeIDR == ARROW(cfpkt)nodeID);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_set_destinationid(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    srand(time(NULL));
    uint8_t destinationID = (uint8_t)rand();
    cfpacket_set_destinationid(REFERENCE cfpkt, destinationID);
    assert(ARROW(cfpkt)destinationID == destinationID);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_get_destinationid(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint8_t destinationID = (uint8_t)rand();
    cfpacket_set_destinationid(REFERENCE cfpkt, destinationID);

    uint8_t dIDF = cfpacket_get_destinationid(REFERENCE cfpkt);
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

void test_cfpacket_get_packet_byte_string(void)
{
    CFPacket_t SINGLE_POINTER cfpkt;
    cfpacket_init(&cfpkt);

    uint8_t nodeID = (uint8_t)rand();
    uint8_t destinationID = (uint8_t)rand();
    uint32_t frequency = (uint32_t)rand();
    cfpacket_create(REFERENCE cfpkt, nodeID, destinationID, frequency);

    size_t size = sizeof(nodeID) + sizeof(destinationID);
    size += sizeof(frequency);
    ARRAY byteString;
    size_t size2;
#ifdef __LINUX__
    byteString = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteString, size);
#endif
    cfpacket_get_packet_byte_string(REFERENCE cfpkt, &byteString, &size2);
#ifdef __LINUX__
    assert(byteString != NULL);
    assert(byteString[0] == nodeID);
    assert(byteString[1] == destinationID);
    assert(byteString[2] == (frequency & 0xff000000 >> 24));
    assert(byteString[3] == (frequency & 0x00ff0000 >> 16));
    assert(byteString[4] == (frequency & 0x0000ff00 >> 8));
    assert(byteString[5] == (frequency & 0x000000ff));

    free(byteString);
#endif
#ifdef __RIOT__
    uint8_t r;
    int ret = read_element(&byteString, &r, 0);
    assert(r == nodeID);
    ret = read_element(&byteString, &r, 1);
    assert(r == destinationID);
    ret = read_element(&byteString, &r, 2);
    assert(r == (frequency & 0xff000000 >> 24));
    ret = read_element(&byteString, &r, 3);
    assert(r == (frequency & 0x00ff0000 >> 16));
    ret = read_element(&byteString, &r, 4);
    assert(r == (frequency & 0x0000ff00 >> 8));
    ret = read_element(&byteString, &r, 5);
    assert(r == (frequency & 0x000000ff));
    assert(ret == 1);
    write_element(&byteString, 1, 0);

    free_array(&byteString);
#endif
    assert(size2 == size);

    cfpacket_destroy(&cfpkt);
}

void test_cfpacket_construct_packet_from_byte_string(void)
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
    uint32_t frequency = (uint32_t)rand();
#ifdef __LINUX__
    byteString[0] = (uint8_t)rand();
    byteString[1] = (uint8_t)rand();
    byteString[2] = ((frequency & 0xff000000) >> 24);
    byteString[3] = ((frequency & 0x00ff0000) >> 16);
    byteString[4] = ((frequency & 0x0000ff00) >> 8);
    byteString[5] = (frequency & 0x000000ff);
#endif
#ifdef __RIOT__
    write_element(&byteString, (uint8_t)rand(),                     0);
    write_element(&byteString, (uint8_t)rand(),                     1);
    write_element(&byteString, ((frequency & 0xff000000) >> 24),    2);
    write_element(&byteString, ((frequency & 0x00ff0000) >> 16),    3);
    write_element(&byteString, ((frequency & 0x0000ff00) >> 8),     4);
    write_element(&byteString, (frequency & 0x000000ff),            5);
#endif

    cfpacket_construct_packet_from_byte_string(REFERENCE cfpkt, &byteString, size);
#ifdef __LINUX__
    assert(cfpkt->nodeID        == byteString[0]);
    assert(cfpkt->destinationID == byteString[1]);
    assert(cfpkt->frequency     == frequency);
#endif
#ifdef __RIOT__
    uint8_t element;
    read_element(&byteString, &element, 0);
    assert(cfpkt.nodeID == element);
    read_element(&byteString, &element, 1);
    assert(cfpkt.destinationID == element);
    assert(cfpkt.frequency == frequency);
#endif

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
#ifdef __LINUX__
    srand(time(NULL));
#endif
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

    printf("Testing cfpacket_get_packet_byte_string function.\n");
    test_cfpacket_get_packet_byte_string();
    printf("Test passed.\n");

    printf("Testing cfpacket_construct_packet_from_byte_string function.\n");
    test_cfpacket_construct_packet_from_byte_string();
    printf("Test passed.\n");
}
