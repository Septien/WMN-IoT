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

    controlpacket_init(&pkt);
#ifdef __LINUX__
    assert(pkt != NULL);
    assert(pkt->ack == NULL);
#endif
#ifdef __RIOT__
    assert(pkt.nodeID == 0);
    assert(pkt.currentFrame == 0);
    assert(pkt.currentSlot == 0);
    assert(pkt.collisionSlot == 0);
    assert(pkt.hopCount == 0);
    assert(pkt.networkTime == 0);
    assert(pkt.initTime == 0);
    assert(pkt.ackSize == 0);
    assert(pkt.ack.size == 0);
#endif

    controlpacket_destroy(&pkt);
}

void test_controlpacket_destroy(void)
{
    ControlPacket_t  SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    controlpacket_destroy(&pkt);

#ifdef __LINUX__
    assert(pkt == NULL);
#endif
#ifdef __RIOT__
    assert(pkt.nodeID == 0);
    assert(pkt.currentFrame == 0);
    assert(pkt.currentSlot == 0);
    assert(pkt.collisionSlot == 0);
    assert(pkt.hopCount == 0);
    assert(pkt.networkTime == 0);
    assert(pkt.initTime == 0);
    assert(pkt.ack.size == 0);
#endif
}

void test_controlpacket_create(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    uint16_t nodeID = 1;
    uint32_t frame = 20;
    uint8_t slot = 8;
    uint8_t collisionSlot = 10;
    uint32_t collisionFrequency = 915;
    uint16_t hopCount = 3;
    uint64_t netTime = rand();
    uint32_t initTime = rand();
    int n = rand() % 200;
    ARRAY ack;
#ifdef __LINUX__
    ack = (uint8_t *)malloc(n * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&ack, n);
#endif
    for (int i = 0; i < n; i++)
        WRITE_ARRAY(REFERENCE ack, rand(), i);

    controlpacket_create(REFERENCE pkt, nodeID, frame, slot, collisionSlot, collisionFrequency, hopCount, netTime, initTime, &ack, n);

#ifdef __LINUX__
    assert(pkt != NULL);
#endif
    assert(ARROW(pkt)nodeID == nodeID);
    assert(ARROW(pkt)currentFrame == frame);
    assert(ARROW(pkt)currentSlot == slot);
    assert(ARROW(pkt)collisionSlot == collisionSlot);
    assert(ARROW(pkt)collisionFrequency == collisionFrequency);
    assert(ARROW(pkt)hopCount == hopCount);
    assert(ARROW(pkt)networkTime == netTime);
    assert(ARROW(pkt)initTime == initTime);
    assert(ARROW(pkt)ackSize == n);
    for (int i = 0; i < n; i++)
        assert(READ_ARRAY(REFERENCE ARROW(pkt)ack, i) == READ_ARRAY(REFERENCE ack, i));

    controlpacket_destroy(&pkt);
}

void test_controlpacket_clear(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

uint16_t nodeID = 1;
    uint32_t frame = 20;
    uint8_t slot = 8;
    uint8_t collisionSlot = 10;
    uint32_t collisionFrequency = 915;
    uint16_t hopCount = 3;
    uint64_t netTime = rand();
    uint32_t initTime = rand();
    int n = rand() % 200;
    ARRAY ack;
#ifdef __LINUX__
    ack = (uint8_t *)malloc(n * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&ack, n);
#endif
    for (int i = 0; i < n; i++)
        WRITE_ARRAY(REFERENCE ack, rand(), i);

    controlpacket_create(REFERENCE pkt, nodeID, frame, slot, collisionSlot, collisionFrequency, hopCount, netTime, initTime, &ack, n);
    
    controlpacket_clear(REFERENCE pkt);

    assert(ARROW(pkt)nodeID == 0);
    assert(ARROW(pkt)collisionFrequency == 0);
    assert(ARROW(pkt)collisionSlot == 0);
    assert(ARROW(pkt)hopCount == 0);
    assert(ARROW(pkt)networkTime == 0);
    assert(ARROW(pkt)initTime == 0);
#ifdef __RIOT__
    assert(ARROW(pkt)ack.size == 0);
#endif
    
    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_nodeID(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    for (uint16_t i = 0; i < 255; i++)
    {
        controlpacket_set_nodeID(REFERENCE pkt, i);
        assert(ARROW(pkt)nodeID == i);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_nodeID(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    for (uint16_t i = 0; i < 255; i++)
    {
        controlpacket_set_nodeID(REFERENCE pkt, i);
        uint16_t nodeid = controlpacket_get_nodeID(REFERENCE pkt);
        assert(nodeid == i);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_current_frame(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    int n = rand() % 1024;
    for (int i = 0; i < n; i++)
    {
        uint32_t frame = rand();
        controlpacket_set_current_frame(REFERENCE pkt, frame);
        assert(ARROW(pkt)currentFrame == frame);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_current_frame(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    int n = rand() % 1024;
    for (int i = 0; i < n; i++)
    {
        uint32_t frame = rand();
        controlpacket_set_current_frame(REFERENCE pkt, frame);
        uint32_t frameA = controlpacket_get_current_frame(REFERENCE pkt);
        assert(frameA == frame);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_current_slot(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    int n = rand()%1024;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = rand();
        controlpacket_set_current_slot(REFERENCE pkt, slot);
        assert(ARROW(pkt)currentSlot == slot);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_current_slot(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    int n = rand()%1024;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = rand();
        controlpacket_set_current_slot(REFERENCE pkt, slot);
        uint8_t slotA = controlpacket_get_current_slot(REFERENCE pkt);
        assert(slotA == slot);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_collision_slot(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    uint8_t slot = 10;
    controlpacket_set_collision_slot(REFERENCE pkt, slot);

    assert(ARROW(pkt)collisionSlot == slot);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_collision_slot(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    uint8_t slot = 10;
    controlpacket_set_collision_slot(REFERENCE pkt, slot);

    slot = controlpacket_get_collision_slot(REFERENCE pkt);
    assert(slot == ARROW(pkt)collisionSlot);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_collision_frequency(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    uint32_t freq = 915000000;
    controlpacket_set_collision_frequency(REFERENCE pkt, freq);
    assert(ARROW(pkt)collisionFrequency == freq);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_collision_frequency(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    uint32_t freq = 915000000;
    controlpacket_set_collision_frequency(REFERENCE pkt, freq);
    
    uint32_t freqr = controlpacket_get_collision_frequency(REFERENCE pkt);
    assert(freqr == ARROW(pkt)collisionFrequency);
    assert(freqr == freq);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_hop_count(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    uint16_t hops = 8;
    controlpacket_set_hop_count(REFERENCE pkt, hops);
    assert(ARROW(pkt)hopCount == hops);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_hop_count(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    uint16_t hops = 8;
    controlpacket_set_hop_count(REFERENCE pkt, hops);
    
    uint8_t hops2 = controlpacket_get_hop_count(REFERENCE pkt);
    assert(hops2 == ARROW(pkt)hopCount);
    assert(hops2 == hops);

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_network_time(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint64_t netTime = rand();
        controlpacket_set_network_time(REFERENCE pkt, netTime);
        assert(ARROW(pkt)networkTime == netTime);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_network_time(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint64_t time = (uint32_t)rand();
        controlpacket_set_network_time(REFERENCE pkt, time);
        uint64_t timeA = controlpacket_get_network_time(REFERENCE pkt);
        assert(timeA == time);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_init_time(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t initTime = (uint32_t)rand();
        controlpacket_set_init_time(REFERENCE pkt, initTime);
        assert(ARROW(pkt)initTime == initTime);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_init_time(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t initTime = (uint32_t)rand();
        controlpacket_set_init_time(REFERENCE pkt, initTime);
        uint32_t initTimeT = controlpacket_get_init_time(REFERENCE pkt);
        assert(initTimeT == initTime);
    }

    controlpacket_destroy(&pkt);
}

void test_controlpacket_set_ACK(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    ARRAY ack;
    int n = rand() % 200;
#ifdef __LINUX__
    ack = (uint8_t *)malloc(n * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(REFERENCE ack, n);
#endif
    for (int i = 0; i < n; i++)
        WRITE_ARRAY(REFERENCE ack, rand(), i);

    controlpacket_set_ACK(REFERENCE pkt, &ack, n);

    for (int i = 0; i < n; i++)
    {
        assert(READ_ARRAY(REFERENCE ARROW(pkt)ack, i) == READ_ARRAY(REFERENCE ack, i));
    }

#ifdef __LINUX__
    free(ack);
#endif
#ifdef __RIOT__
    free_array(&ack);
#endif
    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_ACK(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    ARRAY ack;
    ARRAY ack2;
    int n = rand() % 200;
#ifdef __LINUX__
    ack = (uint8_t *)malloc(n * sizeof(uint8_t));
    ack2 = (uint8_t *)malloc(n * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&ack, n);
    create_array(&ack2, n);
#endif
    for (int i = 0; i < n; i++)
        WRITE_ARRAY(REFERENCE ack, rand(), i);

    controlpacket_set_ACK(REFERENCE pkt, &ack, n);

    controlpacket_get_ACK(REFERENCE pkt, &ack2, n);
    for (int i = 0; i < n; i++)
        assert(READ_ARRAY(REFERENCE ack, i) == READ_ARRAY(REFERENCE ack2, i));

#ifdef __LINUX__
    free(ack);
    free(ack2);
#endif
#ifdef __RIOT__
    free_array(&ack);
    free_array(&ack2);
#endif
    controlpacket_destroy(&pkt);
}

void test_controlpacket_get_packet_bytestring(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    uint16_t nodeID = (uint16_t) rand();
    uint32_t frame = 0x0f0f0f0f;
    uint8_t frames[4];
    uint8_t slot = 8;
    uint8_t collisionSlots = 0x01;
    uint32_t collisionFrequency = 0xf0f0f0f0;
    uint8_t freq[4];
    uint16_t hopCount = 3;
    uint8_t hops[2];
    uint64_t netTime = rand();
    uint8_t time[8];
    uint32_t initTime = rand();
    uint8_t inittime[4];
    ARRAY ack;
    int n = rand() % 200;
#ifdef __LINUX__
    ack = (uint8_t *)malloc(n * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(REFERENCE ack, n);
#endif
    for (int i = 0; i < n; i++)
        WRITE_ARRAY(REFERENCE ack, rand(), i);
    
    controlpacket_create(REFERENCE pkt, nodeID, frame, slot, collisionSlots, collisionFrequency, hopCount, netTime, initTime, &ack, n);

    frames[0] = (frame & 0xff000000) >> 24;
    frames[1] = (frame & 0x00ff0000) >> 16;
    frames[2] = (frame & 0x0000ff00) >> 8;
    frames[3] = (frame & 0x000000ff);
    freq[0] = (collisionFrequency & 0xff000000) >> 24;
    freq[1] = (collisionFrequency & 0x00ff0000) >> 16;
    freq[2] = (collisionFrequency & 0x0000ff00) >> 8;
    freq[3] = (collisionFrequency & 0x000000ff);
    hops[0] = (hopCount & 0xff00) >> 8;
    hops[1] = (hopCount & 0x00ff);
    time[0] = (netTime & 0xff00000000000000) >> 56;
    time[1] = (netTime & 0x00ff000000000000) >> 48;
    time[2] = (netTime & 0x0000ff0000000000) >> 40;
    time[3] = (netTime & 0x000000ff00000000) >> 32;
    time[4] = (netTime & 0x00000000ff000000) >> 24;
    time[5] = (netTime & 0x0000000000ff0000) >> 16;
    time[6] = (netTime & 0x000000000000ff00) >> 8;
    time[7] = (netTime & 0x00000000000000ff);
    inittime[0] = (initTime & 0xff000000) >> 24;
    inittime[1] = (initTime & 0x00ff0000) >> 16;
    inittime[2] = (initTime & 0x0000ff00) >> 8;
    inittime[3] = (initTime & 0x000000ff);
    ARRAY byteStr;
    controlpacket_get_packet_bytestring(REFERENCE pkt, &byteStr);

    assert(READ_ARRAY(REFERENCE byteStr, 0)     == 1);      // Control packet MAC, type one
    assert(READ_ARRAY(REFERENCE byteStr, 1)     == (nodeID & 0xff00) >> 8);
    assert(READ_ARRAY(REFERENCE byteStr, 2)     == (nodeID & 0x00ff));
    assert(READ_ARRAY(REFERENCE byteStr, 3)     == frames[0]);
    assert(READ_ARRAY(REFERENCE byteStr, 4)     == frames[1]);
    assert(READ_ARRAY(REFERENCE byteStr, 5)     == frames[2]);
    assert(READ_ARRAY(REFERENCE byteStr, 6)     == frames[3]);
    assert(READ_ARRAY(REFERENCE byteStr, 7)     == slot);
    assert(READ_ARRAY(REFERENCE byteStr, 8)     == collisionSlots);
    assert(READ_ARRAY(REFERENCE byteStr, 9)     == freq[0]);
    assert(READ_ARRAY(REFERENCE byteStr, 10)    == freq[1]);
    assert(READ_ARRAY(REFERENCE byteStr, 11)    == freq[2]);
    assert(READ_ARRAY(REFERENCE byteStr, 12)    == freq[3]);
    assert(READ_ARRAY(REFERENCE byteStr, 13)    == hops[0]);
    assert(READ_ARRAY(REFERENCE byteStr, 14)    == hops[1]);
    assert(READ_ARRAY(REFERENCE byteStr, 15)    == time[0]);
    assert(READ_ARRAY(REFERENCE byteStr, 16)    == time[1]);
    assert(READ_ARRAY(REFERENCE byteStr, 17)    == time[2]);
    assert(READ_ARRAY(REFERENCE byteStr, 18)    == time[3]);
    assert(READ_ARRAY(REFERENCE byteStr, 19)    == time[4]);
    assert(READ_ARRAY(REFERENCE byteStr, 20)    == time[5]);
    assert(READ_ARRAY(REFERENCE byteStr, 21)    == time[6]);
    assert(READ_ARRAY(REFERENCE byteStr, 22)    == time[7]);
    assert(READ_ARRAY(REFERENCE byteStr, 23)    == inittime[0]);
    assert(READ_ARRAY(REFERENCE byteStr, 24)    == inittime[1]);
    assert(READ_ARRAY(REFERENCE byteStr, 25)    == inittime[2]);
    assert(READ_ARRAY(REFERENCE byteStr, 26)    == inittime[3]);
    assert(READ_ARRAY(REFERENCE byteStr, 27)    == n);
    for (int i = 0; i < n; i++)
    {
        assert(READ_ARRAY(REFERENCE byteStr, i + 28) == READ_ARRAY(REFERENCE ack, i));
    }

#ifdef __LINUX__
    free(byteStr);
    free(ack);
#endif
#ifdef __RIOT__
    free_array(&byteStr);
    free_array(&ack);
#endif

    controlpacket_destroy(&pkt);
}

void test_controlpacket_construct_packet_from_bytestring(void)
{
    ControlPacket_t SINGLE_POINTER pkt;
    controlpacket_init(&pkt);

    uint16_t nodeID = rand();
    uint8_t currentSlot = 1;
    uint32_t currentFrame = 0x0f0f0f0f;
    uint8_t collisionSlots = 0x01;
    uint32_t collisionFrequency = 0xf0f0f0f0;
    uint16_t hopCount = 3;
    uint64_t netTime = (uint64_t)rand();
    uint32_t initTime = rand();
    uint8_t n = rand() % 200;
    ARRAY ack;
#ifdef __LINUX__
    ack = (uint8_t *)malloc(n * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&ack, n);
#endif
    for (int i = 0; i < n; i++)
        WRITE_ARRAY(REFERENCE ack, rand() % 256, i);

    ARRAY byteStr;
#ifdef __LINUX__
     byteStr = (uint8_t *)malloc(PACKET_SIZE_MAC * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteStr, PACKET_SIZE_MAC);
#endif

    WRITE_ARRAY(REFERENCE byteStr, 1,                                       0);
    WRITE_ARRAY(REFERENCE byteStr, (nodeID & 0xff00) >> 8,                  1);
    WRITE_ARRAY(REFERENCE byteStr, (nodeID & 0x00ff),                       2);
    WRITE_ARRAY(REFERENCE byteStr, (currentFrame & 0xff000000) >> 24,       3);
    WRITE_ARRAY(REFERENCE byteStr, (currentFrame & 0x00ff0000) >> 16,       4);
    WRITE_ARRAY(REFERENCE byteStr, (currentFrame & 0x0000ff00) >> 8,        5);
    WRITE_ARRAY(REFERENCE byteStr, (currentFrame & 0x000000ff),             6);
    WRITE_ARRAY(REFERENCE byteStr, currentSlot,                             7);
    WRITE_ARRAY(REFERENCE byteStr, collisionSlots,                          8);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0xff000000) >> 24, 9);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0x00ff0000) >> 16, 10);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0x0000ff00) >> 8,  11);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0x000000ff),       12);
    WRITE_ARRAY(REFERENCE byteStr, (hopCount & 0xff00) >> 8,                13);
    WRITE_ARRAY(REFERENCE byteStr, hopCount & 0x00ff,                       14);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0xff00000000000000) >> 56,    15);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x00ff000000000000) >> 48,    16);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x0000ff0000000000) >> 40,    17);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x000000ff00000000) >> 32,    18);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x00000000ff000000) >> 24,    19);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x0000000000ff0000) >> 16,    20);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x000000000000ff00) >> 8,     21);
    WRITE_ARRAY(REFERENCE byteStr,  netTime & 0x00000000000000ff,           22);
    WRITE_ARRAY(REFERENCE byteStr, (initTime & 0xff000000) >> 24,           23);
    WRITE_ARRAY(REFERENCE byteStr, (initTime & 0x00ff0000) >> 16,           24);
    WRITE_ARRAY(REFERENCE byteStr, (initTime & 0x0000ff00) >> 8,            25);
    WRITE_ARRAY(REFERENCE byteStr, (initTime & 0x000000ff),                 26);
    WRITE_ARRAY(REFERENCE byteStr, n,                                       27);
    int i;
    for (i = 0; i < n; i++)
    {
        uint8_t e = READ_ARRAY(REFERENCE ack, i);
        WRITE_ARRAY(REFERENCE byteStr, e, i + 28);
    }
    i += 28;
    uint m = rand();
    for (; i < PACKET_SIZE_MAC; i++)
        WRITE_ARRAY(REFERENCE byteStr, m, i);
    
    controlpacket_construct_packet_from_bytestring(REFERENCE pkt, &byteStr);

    assert(ARROW(pkt)nodeID             == nodeID);
    assert(ARROW(pkt)currentFrame       == currentFrame);
    assert(ARROW(pkt)currentSlot        == currentSlot);
    assert(ARROW(pkt)collisionSlot      == collisionSlots);
    assert(ARROW(pkt)collisionFrequency == collisionFrequency);
    assert(ARROW(pkt)hopCount           == hopCount);
    assert(ARROW(pkt)networkTime        == netTime);
    assert(ARROW(pkt)initTime           == initTime);
    assert(ARROW(pkt)ackSize            == n);
    for (int i = 0; i < n; i++)
        assert(READ_ARRAY(REFERENCE ARROW(pkt)ack, i) == READ_ARRAY(REFERENCE ack, i));

#ifdef __LINUX__
    free(byteStr);
    free(ack);
#endif
#ifdef __RIOT__
    free_array(&byteStr);
    free_array(&ack);
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

    printf("Testing controlpacket_set_current_frame function.\n");
    test_controlpacket_set_current_frame();
    printf("Test passed.\n");

    printf("Teting controlpacket_get_current_frame function.\n");
    test_controlpacket_get_current_frame();
    printf("Test passed.\n");

    printf("Testing controlpacket_set_current_slot function.\n");
    test_controlpacket_set_current_slot();
    printf("Test passed.\n");
    
    printf("Testing controlpacket_get_current_slot function.\n");
    test_controlpacket_get_current_slot();
    printf("Test passed.\n");

    printf("Testing controlpacket_set_collision_slot function.\n");
    test_controlpacket_set_collision_slot();
    printf("Test passed.\n");

    printf("Testing controlpacket_get_collision_slot function.\n");
    test_controlpacket_get_collision_slot();
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

    printf("Testing controlpacket_set_init_time function.\n");
    test_controlpacket_set_init_time();
    printf("Test passed.\n");

    printf("Testing controlpacket_get_init_time function.\n");
    test_controlpacket_get_init_time();
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
