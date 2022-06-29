#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "ControlPacket.h"

#include "cUnit.h"

#define ITERATIONS 1000

struct controlpacket_data {
    ControlPacket_t SINGLE_POINTER ctrlpkt;
};

void setup_controlpacket(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    controlpacket_init(&data->ctrlpkt);
}

void teardown_controlpacket(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    controlpacket_destroy(&data->ctrlpkt);
}

void test_controlpacket_init(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

#ifdef __LINUX__
    assert(pkt != NULL);
#endif
#ifdef __RIOT__
    assert(pkt.node_id[0] == 0);
    assert(pkt.node_id[1] == 0);
    assert(pkt.currentFrame == 0);
    assert(pkt.currentSlot == 0);
    assert(pkt.collisionSlot == 0);
    assert(pkt.hopCount == 0);
    assert(pkt.networkTime == 0);
    assert(pkt.initTime == 0);
#endif
}

void test_controlpacket_destroy(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;    

    controlpacket_destroy(&data->ctrlpkt);

#ifdef __LINUX__
    assert(data->ctrlpkt == NULL);
#endif
#ifdef __RIOT__
    assert(data->ctrlpkt.node_id[0] == 0);
    assert(data->ctrlpkt.node_id[1] == 0);
    assert(data->ctrlpkt.currentFrame == 0);
    assert(data->ctrlpkt.currentSlot == 0);
    assert(data->ctrlpkt.collisionSlot == 0);
    assert(data->ctrlpkt.hopCount == 0);
    assert(data->ctrlpkt.networkTime == 0);
    assert(data->ctrlpkt.initTime == 0);
#endif
    controlpacket_init(&data->ctrlpkt);
}

void test_controlpacket_create(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    uint64_t node_id[2] = {0};
    node_id[0] = rand();
    node_id[1] = rand();
    uint32_t frame = 20;
    uint8_t slot = 8;
    uint8_t collisionSlot = 10;
    uint32_t collisionFrequency = 915;
    uint16_t hopCount = 3;
    uint64_t netTime = rand();
    uint32_t initTime = rand();
    int freqs = MAX_NUMBER_FREQS;
    int slots = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    uint8_t occupied_slots[freqs][slots];
    for (int i = 0; i < freqs; i++) {
        for (int j = 0; j < slots; j++) {
            occupied_slots[i][j] = rand();
        }
    }

    controlpacket_create(REFERENCE pkt, node_id, frame, slot, collisionSlot, collisionFrequency, 
                        hopCount, netTime, initTime, (uint8_t *)occupied_slots);

#ifdef __LINUX__
    assert(pkt != NULL);
#endif
    assert(ARROW(pkt)node_id[0] == node_id[0]);
    assert(ARROW(pkt)node_id[1] == node_id[1]);
    assert(ARROW(pkt)currentFrame == frame);
    assert(ARROW(pkt)currentSlot == slot);
    assert(ARROW(pkt)collisionSlot == collisionSlot);
    assert(ARROW(pkt)collisionFrequency == collisionFrequency);
    assert(ARROW(pkt)hopCount == hopCount);
    assert(ARROW(pkt)networkTime == netTime);
    assert(ARROW(pkt)initTime == initTime);
    assert(memcmp(ARROW(pkt)occupied_slots, occupied_slots, sizeof(occupied_slots)) == 0);
}

void test_controlpacket_clear(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    uint64_t node_id[2];
    node_id[0] = rand();
    node_id[1] = rand();
    uint32_t frame = 20;
    uint8_t slot = 8;
    uint8_t collisionSlot = 10;
    uint32_t collisionFrequency = 915;
    uint16_t hopCount = 3;
    uint64_t netTime = rand();
    uint32_t initTime = rand();
    int freqs = MAX_NUMBER_FREQS;
    int slots = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    uint8_t occupied_slots[freqs][slots];
    for (int i = 0; i < freqs; i++) {
        for (int j = 0; j < slots; j++) {
            occupied_slots[i][j] = rand();
        }
    }

    controlpacket_create(REFERENCE pkt, node_id, frame, slot, collisionSlot, collisionFrequency, hopCount, netTime, initTime, (uint8_t *)occupied_slots);
    
    controlpacket_clear(REFERENCE pkt);

    assert(ARROW(pkt)node_id[0] == 0);
    assert(ARROW(pkt)node_id[0] == 0);
    assert(ARROW(pkt)collisionFrequency == 0);
    assert(ARROW(pkt)collisionSlot == 0);
    assert(ARROW(pkt)hopCount == 0);
    assert(ARROW(pkt)networkTime == 0);
    assert(ARROW(pkt)initTime == 0);
}

void test_controlpacket_set_current_frame(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    int n = rand() % 1024;
    for (int i = 0; i < n; i++)
    {
        uint32_t frame = rand();
        controlpacket_set_current_frame(REFERENCE pkt, frame);
        assert(ARROW(pkt)currentFrame == frame);
    }
}

void test_controlpacket_get_current_frame(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    int n = rand() % 1024;
    for (int i = 0; i < n; i++)
    {
        uint32_t frame = rand();
        controlpacket_set_current_frame(REFERENCE pkt, frame);
        uint32_t frameA = controlpacket_get_current_frame(REFERENCE pkt);
        assert(frameA == frame);
    }
}

void test_controlpacket_set_current_slot(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    int n = rand()%1024;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = rand();
        controlpacket_set_current_slot(REFERENCE pkt, slot);
        assert(ARROW(pkt)currentSlot == slot);
    }
}

void test_controlpacket_get_current_slot(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    int n = rand()%1024;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = rand();
        controlpacket_set_current_slot(REFERENCE pkt, slot);
        uint8_t slotA = controlpacket_get_current_slot(REFERENCE pkt);
        assert(slotA == slot);
    }
}

void test_controlpacket_get_collision_slot(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    uint8_t slot = 10;
    ARROW(pkt)collisionSlot = slot;

    slot = controlpacket_get_collision_slot(REFERENCE pkt);
    assert(slot == ARROW(pkt)collisionSlot);
}

void test_controlpacket_get_collision_frequency(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    uint32_t freq = 915000000;
    ARROW(pkt)collisionFrequency = freq;
    
    uint32_t freqr = controlpacket_get_collision_frequency(REFERENCE pkt);
    assert(freqr == ARROW(pkt)collisionFrequency);
    assert(freqr == freq);
}

void test_controlpacket_set_hop_count(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    uint16_t hops = 8;
    controlpacket_set_hop_count(REFERENCE pkt, hops);
    assert(ARROW(pkt)hopCount == hops);
}

void test_controlpacket_get_hop_count(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    uint16_t hops = 8;
    controlpacket_set_hop_count(REFERENCE pkt, hops);
    
    uint8_t hops2 = controlpacket_get_hop_count(REFERENCE pkt);
    assert(hops2 == ARROW(pkt)hopCount);
    assert(hops2 == hops);
}

void test_controlpacket_set_network_time(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint64_t netTime = rand();
        controlpacket_set_network_time(REFERENCE pkt, netTime);
        assert(ARROW(pkt)networkTime == netTime);
    }
}

void test_controlpacket_get_network_time(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint64_t time = (uint32_t)rand();
        controlpacket_set_network_time(REFERENCE pkt, time);
        uint64_t timeA = controlpacket_get_network_time(REFERENCE pkt);
        assert(timeA == time);
    }
}

void test_controlpacket_set_init_time(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t initTime = (uint32_t)rand();
        controlpacket_set_init_time(REFERENCE pkt, initTime);
        assert(ARROW(pkt)initTime == initTime);
    }
}

void test_controlpacket_get_init_time(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t initTime = (uint32_t)rand();
        controlpacket_set_init_time(REFERENCE pkt, initTime);
        uint32_t initTimeT = controlpacket_get_init_time(REFERENCE pkt);
        assert(initTimeT == initTime);
    }
}

void test_controlpacket_get_occupied_slots(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    uint64_t node_id[2] = {0};
    node_id[0] = rand();
    node_id[1] = rand();
    uint32_t frame = 20;
    uint8_t slot = 8;
    uint8_t collisionSlot = 10;
    uint32_t collisionFrequency = 915;
    uint16_t hopCount = 3;
    uint64_t netTime = rand();
    uint32_t initTime = rand();
    int freqs = MAX_NUMBER_FREQS;
    int slots = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    uint8_t occupied_slots[freqs][slots];
    for (int i = 0; i < freqs; i++) {
        for (int j = 0; j < slots; j++) {
            occupied_slots[i][j] = rand();
        }
    }

    controlpacket_create(REFERENCE pkt, node_id, frame, slot, collisionSlot, collisionFrequency, 
                        hopCount, netTime, initTime, (uint8_t *)occupied_slots);

    uint8_t occupied_slots_r[freqs][slots];
    controlpacket_get_occupied_slots(REFERENCE pkt, (uint8_t *)occupied_slots_r);
    assert(memcmp(occupied_slots, occupied_slots_r, sizeof(occupied_slots)) == 0);
}

void test_controlpacket_get_packet_bytestring(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    uint64_t node_id[2] = {0};
    node_id[0] = rand();
    node_id[1] = rand();
    uint32_t frame = 0x0f0f0f0f;
    uint8_t frames[4];
    uint8_t slot = 8;
    uint8_t collisionSlots = 0x01;
    uint32_t collisionFrequency = 0xf0f0f0f0;
    uint8_t freq[4];
    uint16_t hopCount = rand();
    uint8_t hops[2];
    uint64_t netTime = rand();
    uint8_t time[8];
    uint32_t initTime = rand();
    uint8_t inittime[4];
    int freqs = MAX_NUMBER_FREQS;
    int slots = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    uint8_t occupied_slots[freqs][slots];
    for (int i = 0; i < freqs; i++) {
        for (int j = 0; j < slots; j++) {
            occupied_slots[i][j] = rand();
        }
    }
    
    controlpacket_create(REFERENCE pkt, node_id, frame, slot, collisionSlots, collisionFrequency, hopCount, netTime, initTime, (uint8_t *)occupied_slots);

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
    uint64_t node_id1 = 0;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 1)) << 56;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 2)) << 48;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 3)) << 40;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 4)) << 32;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 5)) << 24;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 6)) << 16;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 7)) << 8;
    node_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 8));
    assert(node_id[0] == node_id1);
    uint64_t node_id2 = 0;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 9)) << 56;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 10)) << 48;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 11)) << 40;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 12)) << 32;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 13)) << 24;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 14)) << 16;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 15)) << 8;
    node_id2 |= ((uint64_t) READ_ARRAY(REFERENCE byteStr, 16));
    assert(node_id[1] == node_id2);
    assert(READ_ARRAY(REFERENCE byteStr, 17)     == frames[0]);
    assert(READ_ARRAY(REFERENCE byteStr, 18)     == frames[1]);
    assert(READ_ARRAY(REFERENCE byteStr, 19)     == frames[2]);
    assert(READ_ARRAY(REFERENCE byteStr, 20)     == frames[3]);
    assert(READ_ARRAY(REFERENCE byteStr, 21)     == slot);
    assert(READ_ARRAY(REFERENCE byteStr, 22)     == collisionSlots);
    assert(READ_ARRAY(REFERENCE byteStr, 23)     == freq[0]);
    assert(READ_ARRAY(REFERENCE byteStr, 24)     == freq[1]);
    assert(READ_ARRAY(REFERENCE byteStr, 25)     == freq[2]);
    assert(READ_ARRAY(REFERENCE byteStr, 26)     == freq[3]);
    assert(READ_ARRAY(REFERENCE byteStr, 27)     == hops[0]);
    assert(READ_ARRAY(REFERENCE byteStr, 28)     == hops[1]);
    assert(READ_ARRAY(REFERENCE byteStr, 29)     == time[0]);
    assert(READ_ARRAY(REFERENCE byteStr, 30)     == time[1]);
    assert(READ_ARRAY(REFERENCE byteStr, 31)     == time[2]);
    assert(READ_ARRAY(REFERENCE byteStr, 32)     == time[3]);
    assert(READ_ARRAY(REFERENCE byteStr, 33)     == time[4]);
    assert(READ_ARRAY(REFERENCE byteStr, 34)     == time[5]);
    assert(READ_ARRAY(REFERENCE byteStr, 35)     == time[6]);
    assert(READ_ARRAY(REFERENCE byteStr, 36)     == time[7]);
    assert(READ_ARRAY(REFERENCE byteStr, 37)     == inittime[0]);
    assert(READ_ARRAY(REFERENCE byteStr, 38)     == inittime[1]);
    assert(READ_ARRAY(REFERENCE byteStr, 39)     == inittime[2]);
    assert(READ_ARRAY(REFERENCE byteStr, 40)     == inittime[3]);
    for (int i = 0; i < freqs; i++) {
        for (int j = 0; j < slots; j++) {
            assert(occupied_slots[i][j] == READ_ARRAY(REFERENCE byteStr, (i * slots) + j + 41));
        }
    }

#ifdef __LINUX__
    free(byteStr);
#endif
#ifdef __RIOT__
    free_array(&byteStr);
#endif
}

void test_controlpacket_construct_packet_from_bytestring(void *arg)
{
    struct controlpacket_data *data = (struct controlpacket_data *) arg;
    ControlPacket_t SINGLE_POINTER pkt = data->ctrlpkt;

    uint64_t node_id[2] = {0};
    node_id[0] = rand();
    node_id[1] = rand();
    uint8_t currentSlot = 1;
    uint32_t currentFrame = 0x0f0f0f0f;
    uint8_t collisionSlots = 0x01;
    uint32_t collisionFrequency = 0xf0f0f0f0;
    uint16_t hopCount = 3;
    uint64_t netTime = (uint64_t)rand();
    uint32_t initTime = rand();
    int freqs = MAX_NUMBER_FREQS;
    int slots = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    uint8_t occupied_slots[freqs][slots];
    for (int i = 0; i < freqs; i++) {
        for (int j = 0; j < slots; j++) {
            occupied_slots[i][j] = rand();
        }
    }

    ARRAY byteStr;
#ifdef __LINUX__
     byteStr = (uint8_t *)malloc(PACKET_SIZE_MAC * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteStr, PACKET_SIZE_MAC);
#endif

    WRITE_ARRAY(REFERENCE byteStr, 1,                                               0);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[0] & 0xff00000000000000) >> 56,    1);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[0] & 0x00ff000000000000) >> 48,    2);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[0] & 0x0000ff0000000000) >> 40,    3);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[0] & 0x000000ff00000000) >> 32,    4);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[0] & 0x00000000ff000000) >> 24,    5);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[0] & 0x0000000000ff0000) >> 16,    6);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[0] & 0x000000000000ff00) >> 8,     7);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[0] & 0x00000000000000ff),          8);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[1] & 0xff00000000000000) >> 56,    9);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[1] & 0x00ff000000000000) >> 48,    10);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[1] & 0x0000ff0000000000) >> 40,    11);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[1] & 0x000000ff00000000) >> 32,    12);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[1] & 0x00000000ff000000) >> 24,    13);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[1] & 0x0000000000ff0000) >> 16,    14);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[1] & 0x000000000000ff00) >> 8,     15);
    WRITE_ARRAY(REFERENCE byteStr, (node_id[1] & 0x00000000000000ff),          16);
    WRITE_ARRAY(REFERENCE byteStr, (currentFrame & 0xff000000) >> 24,               17);
    WRITE_ARRAY(REFERENCE byteStr, (currentFrame & 0x00ff0000) >> 16,               18);
    WRITE_ARRAY(REFERENCE byteStr, (currentFrame & 0x0000ff00) >> 8,                19);
    WRITE_ARRAY(REFERENCE byteStr, (currentFrame & 0x000000ff),                     20);
    WRITE_ARRAY(REFERENCE byteStr, currentSlot,                                     21);
    WRITE_ARRAY(REFERENCE byteStr, collisionSlots,                                  22);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0xff000000) >> 24,         23);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0x00ff0000) >> 16,         24);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0x0000ff00) >> 8,          25);
    WRITE_ARRAY(REFERENCE byteStr, (collisionFrequency & 0x000000ff),               26);
    WRITE_ARRAY(REFERENCE byteStr, (hopCount & 0xff00) >> 8,                        27);
    WRITE_ARRAY(REFERENCE byteStr, hopCount & 0x00ff,                               28);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0xff00000000000000) >> 56,            29);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x00ff000000000000) >> 48,            30);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x0000ff0000000000) >> 40,            31);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x000000ff00000000) >> 32,            32);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x00000000ff000000) >> 24,            33);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x0000000000ff0000) >> 16,            34);
    WRITE_ARRAY(REFERENCE byteStr, (netTime & 0x000000000000ff00) >> 8,             35);
    WRITE_ARRAY(REFERENCE byteStr,  netTime & 0x00000000000000ff,                   36);
    WRITE_ARRAY(REFERENCE byteStr, (initTime & 0xff000000) >> 24,                   37);
    WRITE_ARRAY(REFERENCE byteStr, (initTime & 0x00ff0000) >> 16,                   38);
    WRITE_ARRAY(REFERENCE byteStr, (initTime & 0x0000ff00) >> 8,                    39);
    WRITE_ARRAY(REFERENCE byteStr, (initTime & 0x000000ff),                         40);
    int i, j;
    for (i = 0; i < freqs; i++) {
        for (j = 0; j < slots; j++) {
            WRITE_ARRAY(REFERENCE byteStr, occupied_slots[i][j], (i * slots) + j + 41);
        }
    }
    i = 41 + (freqs * slots);
    uint m = rand();
    for (; i < PACKET_SIZE_MAC; i++)
        WRITE_ARRAY(REFERENCE byteStr, m, i);
    
    controlpacket_construct_packet_from_bytestring(REFERENCE pkt, &byteStr);

    assert(ARROW(pkt)node_id[0]         == node_id[0]);
    assert(ARROW(pkt)node_id[1]         == node_id[1]);
    assert(ARROW(pkt)currentFrame       == currentFrame);
    assert(ARROW(pkt)currentSlot        == currentSlot);
    assert(ARROW(pkt)collisionSlot      == collisionSlots);
    assert(ARROW(pkt)collisionFrequency == collisionFrequency);
    assert(ARROW(pkt)hopCount           == hopCount);
    assert(ARROW(pkt)networkTime        == netTime);
    assert(ARROW(pkt)initTime           == initTime);
    assert(memcmp(ARROW(pkt)occupied_slots, occupied_slots, sizeof(occupied_slots)) == 0);

#ifdef __LINUX__
    free(byteStr);
#endif
#ifdef __RIOT__
    free_array(&byteStr);
#endif
}

void executeTestsCP(void)
{
    cUnit_t *tests;
    struct controlpacket_data data;

    cunit_init(&tests, &setup_controlpacket, &teardown_controlpacket, (void *)&data);

    cunit_add_test(tests, &test_controlpacket_init, "controlpacket_init\0");
    cunit_add_test(tests, &test_controlpacket_destroy, "controlpacket_destroy\0");
    cunit_add_test(tests, &test_controlpacket_create, "controlpacket_create\0");
    cunit_add_test(tests, &test_controlpacket_clear, "controlpacket_clear\0");
    cunit_add_test(tests, &test_controlpacket_set_current_frame, "controlpacket_set_current_frame\0");
    cunit_add_test(tests, &test_controlpacket_get_current_frame, "controlpacket_get_current_frame\0");
    cunit_add_test(tests, &test_controlpacket_set_current_slot, "controlpacket_set_current_slot\0");
    cunit_add_test(tests, &test_controlpacket_get_current_slot, "controlpacket_get_current_slot\0");
    cunit_add_test(tests, &test_controlpacket_get_collision_slot, "controlpacket_get_collision_slot\0");
    cunit_add_test(tests, &test_controlpacket_get_collision_frequency, "controlpacket_get_collision_frequency\0");
    cunit_add_test(tests, &test_controlpacket_set_hop_count, "controlpacket_set_hop_count\0");
    cunit_add_test(tests, &test_controlpacket_get_hop_count, "controlpacket_set_hop_count\0");
    cunit_add_test(tests, &test_controlpacket_set_network_time, "controlpacket_set_network_time\0");
    cunit_add_test(tests, &test_controlpacket_get_network_time, "controlpacket_get_network_time\0");
    cunit_add_test(tests, &test_controlpacket_set_init_time, "controlpacket_set_init_time\0");
    cunit_add_test(tests, &test_controlpacket_get_init_time, "controlpacket_get_init_time\0");
    cunit_add_test(tests, &test_controlpacket_get_occupied_slots, "controlpacket_get_occupied_slots\0");
    cunit_add_test(tests, &test_controlpacket_get_packet_bytestring, "controlpacket_get_packet_bytestring\0");
    cunit_add_test(tests, &test_controlpacket_construct_packet_from_bytestring, "controlpacket_construct_packet_from_bytestring\0");

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
