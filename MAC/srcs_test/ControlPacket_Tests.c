#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#include "ControlPacket.h"

#define ITERATIONS 1000

void test_controlpacket_init(void)
{
    ControlPacket_t SINGLE_POINTER pkt;

    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);
#ifdef __LINUX__
    assert(pkt != NULL);
    assert(ARROW(pkt)occupiedSlots != NULL);
#endif
#ifdef __RIOT__
    assert(pkt.occupiedSlots.size > 0);
#endif
    assert(ARROW(pkt)_nSlots == slots);
    assert(ARROW(pkt)_nChannels == channels);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_destroy(void)
{
    ControlPacket_t  SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    controlpacket_destroy(&pkt);

#ifdef __LINUX__
    assert(pkt == NULL);
#endif
#ifdef __RIOT__
    assert(pkt.occupiedSlots.size == 0);
    assert(pkt.nodeID == 0);
    assert(pkt.collisionSlot == 0);
    assert(pkt.hopCount == 0);
    assert(pkt.networkTime == 0);
    assert(pkt.ack == 0);
    assert(pkt._nSlots == 0);
    assert(pkt._nChannels == 0);
#endif
}

void test_controlpacket_create(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    uint8_t nodeID = 1;
    uint8_t bytes = get_number_bytes(slots * channels);
    ARRAY occupiedSlots;
#ifdef __LINUX__
    occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&occupiedSlots, bytes);
#endif
    int i;
    for (i = 0; i < bytes; i++)
        WRITE_ARRAY(REFERENCE occupiedSlots, 0xf0, i);
    uint8_t collisionSlot = 10;
    uint32_t collisionFrequency = 915;
    uint8_t hopCount = 3;
    uint8_t ack = 0;
    uint32_t netTime = rand();

    controlpacket_create(REFERENCE pkt, nodeID, &occupiedSlots, collisionSlot, collisionFrequency, hopCount, netTime, ack);

#ifdef __LINUX__
    assert(pkt != NULL);
    assert(pkt->occupiedSlots != NULL);
#endif
#ifdef __RIOT__
    assert(pkt.occupiedSlots.size == bytes);
#endif
    for (i = 0; i < bytes; i++)
        assert(READ_ARRAY(REFERENCE ARROW(pkt)occupiedSlots, i) == 0xf0);
    assert(ARROW(pkt)collisionSlot == collisionSlot);
    assert(ARROW(pkt)collisionFrequency == collisionFrequency);
    assert(ARROW(pkt)hopCount == hopCount);
    assert(ARROW(pkt)networkTime == netTime);
    assert(ARROW(pkt)ack == ack);

#ifdef __LINUX__
    free(occupiedSlots);
#endif
#ifdef __RIOT__
    free_array(&occupiedSlots);
#endif
    controlpacket_destroy(&pkt);
}

void test_controlpacket_clear(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;;
    controlpacket_init(&pkt, slots, channels);

    uint8_t nodeID = 1;
    uint8_t bytes = get_number_bytes(channels * slots);
    ARRAY occupiedSlots;
#ifdef __LINUX__
    occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
#endif

#ifdef __RIOT__
    int ret = create_array(&occupiedSlots, bytes);
    assert(ret == 1);
#endif
    for (unsigned int i = 0; i < slots; i++)
        WRITE_ARRAY(REFERENCE occupiedSlots, 0xf0, i);

    uint8_t collisionSlots = 0x01;
    uint32_t collisionFrequency = 915;
    uint8_t hopCount = 3;
    uint32_t netTime = rand();
    uint8_t ack = 1;

    controlpacket_create(REFERENCE pkt, nodeID, &occupiedSlots, collisionSlots, collisionFrequency, hopCount, netTime, ack);

    controlpacket_clear(REFERENCE pkt);

    assert(ARROW(pkt)nodeID == 0);
#ifdef __LINUX__
    assert(ARROW(pkt)occupiedSlots == NULL);
#endif
#ifdef __RIOT__
    assert(pkt.occupiedSlots.size == 0);
#endif
    assert(ARROW(pkt)collisionFrequency == 0);
    assert(ARROW(pkt)collisionSlot == 0);
    assert(ARROW(pkt)hopCount == 0);
    assert(ARROW(pkt)ack == 0);
    assert(ARROW(pkt)_nSlots == 0);
    assert(ARROW(pkt)_nChannels == 0);

#ifdef __LINUX__
    free(occupiedSlots);
#endif
#ifdef __RIOT__
    free_array(&occupiedSlots);
#endif
    
    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_nodeID(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;;
    controlpacket_init(&pkt, slots, channels);

    for (uint8_t i = 0; i < 255; i++)
    {
        controlpacket_set_nodeID(REFERENCE pkt, i);
        assert(ARROW(pkt)nodeID == i);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_nodeID(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;;
    controlpacket_init(&pkt, slots, channels);

    uint8_t nodeid = 0;
    for (uint8_t i = 0; i < 255; i++)
    {
        controlpacket_set_nodeID(REFERENCE pkt, i);
        nodeid = controlpacket_get_nodeID(REFERENCE pkt);
        assert(nodeid == i);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_collision_slot(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;;
    controlpacket_init(&pkt, slots, channels);

    uint8_t slot = 10;
    controlpacket_set_collision_slot(REFERENCE pkt, slot);

    assert(ARROW(pkt)collisionSlot == slot);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_collision_slot(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;;
    controlpacket_init(&pkt, slots, channels);

    uint8_t slot = 10;
    controlpacket_set_collision_slot(REFERENCE pkt, slot);

    slot = controlpacket_get_collision_slot(REFERENCE pkt);
    assert(slot == ARROW(pkt)collisionSlot);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_occupied_slots(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    uint8_t bytes = get_number_bytes(slots * channels);
    ARRAY occSlots;
    uint8_t i;
#ifdef __LINUX__
    occSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&occSlots, bytes);
#endif
    for (i = 0; i < bytes; i++)
        WRITE_ARRAY(REFERENCE occSlots, 0xff, i);
    controlpacket_set_occupied_slots(REFERENCE pkt, &occSlots);

    for (i = 0; i < bytes; i++)
        assert(READ_ARRAY(REFERENCE ARROW(pkt)occupiedSlots, i) == READ_ARRAY(REFERENCE occSlots, i));

#ifdef __LINUX__
    free(occSlots);
#endif
#ifdef __RIOT__
    free_array(&occSlots);
#endif
    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_occupied_slots(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    uint8_t bytes = get_number_bytes(slots * channels);
    ARRAY occSlots;
#ifdef __LINUX__
    occSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&occSlots, bytes);
#endif
    uint8_t i;
    for (i = 0; i < bytes; i++)
        WRITE_ARRAY(REFERENCE occSlots, 0xff, i);

    controlpacket_set_occupied_slots(REFERENCE pkt, &occSlots);

    ARRAY occSlots2;
#ifdef __LINUX__
    occSlots2 = (uint8_t *)malloc(bytes * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&occSlots2, bytes);
#endif
    controlpacket_get_occupied_slots(REFERENCE pkt, &occSlots2);
    for (uint8_t i = 0; i < bytes; i++)
        assert(READ_ARRAY(REFERENCE occSlots2, i) == READ_ARRAY(REFERENCE ARROW(pkt)occupiedSlots, i));

#ifdef __LINUX__
    free(occSlots);
    free(occSlots2);
#endif
#ifdef __RIOT__
    free_array(&occSlots);
    free_array(&occSlots2);
#endif
    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_collision_frequency(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    uint32_t freq = 915;
    controlpacket_set_collision_frequency(REFERENCE pkt, freq);
    assert(ARROW(pkt)collisionFrequency == freq);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_collision_frequency(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    uint32_t freq = 915;
    controlpacket_set_collision_frequency(REFERENCE pkt, freq);
    
    uint32_t freqr = controlpacket_get_collision_frequency(REFERENCE pkt);
    assert(freqr == ARROW(pkt)collisionFrequency);
    assert(freqr == freq);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_hop_count(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    uint8_t hops = 8;
    controlpacket_set_hop_count(REFERENCE pkt, hops);
    assert(ARROW(pkt)hopCount == hops);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_hop_count(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    uint8_t hops = 8;
    controlpacket_set_hop_count(REFERENCE pkt, hops);
    
    uint8_t hops2 = controlpacket_get_hop_count(REFERENCE pkt);
    assert(hops2 == ARROW(pkt)hopCount);
    assert(hops2 == hops);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_network_time(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    uint32_t netTime;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        netTime = rand();
        controlpacket_set_network_time(REFERENCE pkt, netTime);
        assert(ARROW(pkt)networkTime == netTime);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_network_time(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    uint32_t time, timeA;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        time = (uint32_t)rand();
        controlpacket_set_network_time(REFERENCE pkt, time);
        timeA = controlpacket_get_network_time(REFERENCE pkt);
        assert(timeA == time);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_ACK(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    uint8_t ack = 1;
    controlpacket_set_ACK(REFERENCE pkt, ack);
    assert(ARROW(pkt)ack == ack);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_ACK(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 8, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    uint8_t ack = 1;
    controlpacket_set_ACK(REFERENCE pkt, ack);
    
    uint8_t ack2 = controlpacket_get_ACK(REFERENCE pkt);
    assert(ack2 == ARROW(pkt)ack);
    assert(ack2 == ack);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_packet_bytestring(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 9, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    int i;
    uint8_t bytes = get_number_bytes(slots * channels);
    uint8_t nodeID = 1;
    ARRAY occupiedSlots;
#ifdef __LINUX__
     occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&occupiedSlots, bytes);
#endif
    for (i = 0; i < bytes; i++)
        WRITE_ARRAY(REFERENCE occupiedSlots, 0xff, i);
    uint8_t collisionSlots = 0x01;
    uint32_t collisionFrequency = 0xf0f0f0f0;
    uint8_t freq[4];
    uint8_t hopCount = 3;
    uint32_t netTime = rand();
    uint8_t time[4];
    uint8_t ack = 0;

    controlpacket_create(REFERENCE pkt, nodeID, &occupiedSlots, collisionSlots, collisionFrequency, hopCount, netTime, ack);

    freq[0] = (collisionFrequency & 0xff000000) >> 24;
    freq[1] = (collisionFrequency & 0x00ff0000) >> 16;
    freq[2] = (collisionFrequency & 0x0000ff00) >> 8;
    freq[3] = (collisionFrequency & 0x000000ff);
    time[0] = (netTime & 0xff000000) >> 24;
    time[1] = (netTime & 0x00ff0000) >> 16;
    time[2] = (netTime & 0x0000ff00) >> 8;
    time[3] = (netTime & 0x000000ff);
    ARRAY byteStr;
    size_t size;
    controlpacket_get_packet_bytestring(REFERENCE pkt, &byteStr, &size);

    assert(READ_ARRAY(REFERENCE byteStr, 0) == nodeID);
    for (i = 0; i < bytes; i++)
        assert(READ_ARRAY(REFERENCE byteStr, i + 1) == READ_ARRAY(REFERENCE occupiedSlots, i));
    assert(READ_ARRAY(REFERENCE byteStr, bytes + 2) == collisionSlots);
    assert(READ_ARRAY(REFERENCE byteStr, bytes + 3)           == freq[0]);
    assert(READ_ARRAY(REFERENCE byteStr, bytes + 4)           == freq[1]);
    assert(READ_ARRAY(REFERENCE byteStr, bytes + 5)           == freq[2]);
    assert(READ_ARRAY(REFERENCE byteStr, bytes + 6)           == freq[3]);
    assert(READ_ARRAY(REFERENCE byteStr, bytes + 7)           == hopCount);
    assert(READ_ARRAY(REFERENCE byteStr, bytes + 8)           == time[0]);
    assert(READ_ARRAY(REFERENCE byteStr, bytes + 9)           == time[1]);
    assert(READ_ARRAY(REFERENCE byteStr, bytes + 10)          == time[2]);
    assert(READ_ARRAY(REFERENCE byteStr, bytes + 11)          == time[3]);
    assert(READ_ARRAY(REFERENCE byteStr, bytes + 12)          == ack);

#ifdef __LINUX__
    free(byteStr);
    free(occupiedSlots);
#endif
#ifdef __RIOT__
    free_array(&byteStr);
    free_array(&occupiedSlots);
#endif

    controlpacket_destroy(&pkt);
}

void test_controlpacket_construct_packet_from_bytestring(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    size_t slots = 9, channels = 17;
    controlpacket_init(&pkt, slots, channels);

    int i;
    uint8_t bytes = get_number_bytes(slots * channels);
    uint8_t nodeID = 1;
    ARRAY occupiedSlots;
#ifdef __LINUX__
     occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&occupiedSlots, bytes);
#endif
    for (i = 0; i < bytes; i++)
        WRITE_ARRAY(REFERENCE occupiedSlots, 0xff, i);
    uint8_t collisionSlots = 0x01;
    uint32_t collisionFrequency = 0xf0f0f0f0;
    uint8_t hopCount = 3;
    uint32_t netTime = (uint32_t)rand();
    uint8_t ack = 0;

    size_t size = sizeof(nodeID);
    size += (size_t)bytes;
    size += sizeof(collisionSlots);
    size += sizeof(collisionFrequency);
    size += sizeof(hopCount);
    size += sizeof(netTime);
    size += sizeof(ack);
    size += 1;
    ARRAY byteStr;
#ifdef __LINUX__
     byteStr = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteStr, size);
#endif

    WRITE_ARRAY(REFERENCE byteStr, nodeID, 0);
    for (i = 0; i < bytes; i++)
    {
        uint8_t element = READ_ARRAY(REFERENCE occupiedSlots, i);
        WRITE_ARRAY(REFERENCE byteStr, element, i + 1);
    }
    WRITE_ARRAY(REFERENCE byteStr, collisionSlots, bytes + 2);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0xff000000) >> 24, bytes + 3);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0x00ff0000) >> 16, bytes + 4);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0x0000ff00) >> 8,  bytes + 5);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0x000000ff),       bytes + 6);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0xff000000) >> 24,            bytes + 7);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x00ff0000) >> 16,            bytes + 8);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x0000ff00) >> 8,             bytes + 9);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x000000ff),                  bytes + 10);
    WRITE_ARRAY(REFERENCE byteStr, hopCount,                                bytes + 11);
    WRITE_ARRAY(REFERENCE byteStr, ack,                                     bytes + 12);
    
    controlpacket_construct_packet_from_bytestring(REFERENCE pkt, &byteStr, size);

    assert(ARROW(pkt)nodeID == READ_ARRAY(REFERENCE byteStr, 0));
    for (i = 0; i < bytes; i++)
        assert(READ_ARRAY(REFERENCE ARROW(pkt)occupiedSlots, i) == READ_ARRAY(REFERENCE byteStr, i + 1));
    assert(ARROW(pkt)collisionSlot == READ_ARRAY(REFERENCE byteStr, bytes + 2));
    assert(ARROW(pkt)collisionFrequency == collisionFrequency);
    assert(ARROW(pkt)hopCount == hopCount);
    assert(ARROW(pkt)networkTime == netTime);
    assert(ARROW(pkt)ack == ack);

#ifdef __LINUX__
    free(byteStr);
    free(occupiedSlots);
#endif
#ifdef __RIOT__
    free_array(&byteStr);
    free_array(&occupiedSlots);
#endif
    controlpacket_destroy(&pkt);
}

void executeTestsCP(void)
{
    srand(time(NULL));

    printf("Testing controlpacket_init function.\n");
    test_controlpacket_init();
    printf("Test passed\n");

    printf("Testing controlpacket_destroy function.\n");
    test_controlpacket_destroy();
    printf("Test passed.\n");

    printf("Testing controlpacket_create function.\n");
    test_controlpacket_create();
    printf("Test passed.\n");

    printf("Testing controlpacket_clear function.\n");
    test_controlpacket_clear();
    printf("Test passed.\n");

    printf("Testing controlpacket_set_nodeID function.\n");
    test_controlpacket_set_nodeID();
    printf("Test passed.\n");

    printf("Testing controlpacket_get_nodeID function.\n");
    test_controlpacket_get_nodeID();
    printf("Test passed.\n");

    printf("Testing controlpacket_set_collision_slot function.\n");
    test_controlpacket_set_collision_slot();
    printf("Test passed.\n");

    printf("Testing controlpacket_get_collision_slot function.\n");
    test_controlpacket_get_collision_slot();
    printf("Test passed.\n");

    printf("Testing controlpacket_set_occupied_slots function.\n");
    test_controlpacket_set_occupied_slots();
    printf("Test passed.\n");

    printf("Testing controlpacket_get_occupied_slots function.\n");
    test_controlpacket_get_occupied_slots();
    printf("Test passed.\n");

    printf("Testing controlpacket_set_collision_frequency function.\n");
    test_controlpacket_set_collision_frequency();
    printf("Test passed.\n");

    printf("Testing _controlpacket_get_collision_frequency function.\n");
    test_controlpacket_get_collision_frequency();
    printf("Test passed.\n");

    printf("Testing controlpacket_set_hop_count function.\n");
    test_controlpacket_set_hop_count();
    printf("Test passed.\n");

    printf("Testing controlpacket_get_hop_count function.\n");
    test_controlpacket_get_hop_count();
    printf("Test passed.\n");

    printf("Testing controlpacket_set_network_time function.\n");
    test_controlpacket_set_network_time();
    printf("Test passed.\n");

    printf("Testing controlpacket_get_network_time function.\n");
    test_controlpacket_get_network_time();
    printf("Test passed.\n");

    printf("Testing controlpacket_set_ACK function.\n");
    test_controlpacket_set_ACK();
    printf("Test passed.\n");

    printf("Testing controlpacket_get_ACK function.\n");
    test_controlpacket_get_ACK();
    printf("Test passed.\n");

    printf("Testing controlpacket_get_packet_bytestring function.\n");
    test_controlpacket_get_packet_bytestring();
    printf("Test passed.\n");

    printf("Testing controlpacket_construct_packet_from_bytestring function.\n");
    test_controlpacket_construct_packet_from_bytestring();
    printf("Test passed.\n");
}
