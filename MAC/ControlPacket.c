#include "ControlPacket.h"
#include <assert.h>
#include <string.h>

// Given an arbitrary number n, get the next multiple of 8 greater than n, and
// return the multiple.
uint8_t get_number_bytes(uint8_t n)
{
    uint8_t mod = 8;
    uint8_t res = n % mod;
    uint8_t add = (res == 0 ? 0 : mod - res);
    return (n + add) / 8;
}

void controlpacket_init(ControlPacket_t DOUBLE_POINTER pkt)
{
#ifdef __LINUX__
    (*pkt) = (ControlPacket_t *)malloc(sizeof(ControlPacket_t));
    if ((*pkt) == NULL)
    {
        printf("Unable to create packet.\n");
        exit(-1);
    }
#endif
    memset(SINGLE_POINTER pkt, 0, sizeof(ControlPacket_t));
}

void controlpacket_destroy(ControlPacket_t DOUBLE_POINTER pkt)
{
    assert(pkt != NULL);
#ifdef __LINUX__
    assert(*pkt != NULL);
    free(*pkt);

    *pkt = NULL;
#endif
#ifdef __RIOT__
    memset(SINGLE_POINTER pkt, 0, sizeof(ControlPacket_t));
#endif
}

// Fill the packet with the given parameters
void controlpacket_create(ControlPacket_t *pkt, uint64_t *node_id, uint32_t frame, uint8_t slot,uint8_t collisionSlots, 
                          uint32_t collisionFrequency, uint16_t hopCount, uint64_t netTime, uint32_t initTime, uint8_t *occupied_slots)
{
    assert(pkt != NULL);

    pkt->node_id[0] = node_id[0];
    pkt->node_id[1] = node_id[1];
    pkt->currentFrame = frame;
    pkt->currentSlot = slot;
    pkt->collisionSlot = collisionSlots;
    pkt->collisionFrequency = collisionFrequency;
    pkt->hopCount = hopCount;
    pkt->networkTime = netTime;
    pkt->initTime = initTime;
    memcpy(&pkt->occupied_slots, occupied_slots, sizeof(pkt->occupied_slots));
}

// Set all values of the packet to zero, and free the arrays
void controlpacket_clear(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    memset(pkt, 0, sizeof(ControlPacket_t));
}

void controlpacket_set_current_frame(ControlPacket_t *pkt, uint32_t frame)
{
    assert(pkt != NULL);

    pkt->currentFrame = frame;
}

uint32_t controlpacket_get_current_frame(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->currentFrame;
}

void controlpacket_set_current_slot(ControlPacket_t *pkt, uint8_t slot)
{
    assert(pkt != NULL);
    
    pkt->currentSlot = slot;
}

uint8_t controlpacket_get_current_slot(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->currentSlot;
}

uint8_t controlpacket_get_collision_slot(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->collisionSlot;
}

uint32_t controlpacket_get_collision_frequency(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->collisionFrequency;;
}

void controlpacket_set_hop_count(ControlPacket_t *pkt, uint16_t count)
{
    assert(pkt != NULL);
    
    pkt->hopCount = count;
}

uint16_t controlpacket_get_hop_count(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->hopCount;;
}

void controlpacket_set_network_time(ControlPacket_t *pkt, uint64_t time)
{
    assert(pkt != NULL);

    pkt->networkTime = time;
}

uint64_t controlpacket_get_network_time(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->networkTime;
}

void controlpacket_set_init_time(ControlPacket_t *pkt, uint32_t time)
{
    assert(pkt != NULL);

    pkt->initTime = time;
}

uint32_t controlpacket_get_init_time(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->initTime;
}

void controlpacket_get_occupied_slots(ControlPacket_t *pkt, uint8_t *occupied_slots)
{
    assert(pkt != NULL);
    assert(occupied_slots != NULL);

    memcpy(occupied_slots, &pkt->occupied_slots, sizeof(pkt->occupied_slots));
}

void controlpacket_get_packet_bytestring(ControlPacket_t *pkt, ARRAY* byteStr)
{
    assert(pkt != NULL);

    size_t size1 = PACKET_SIZE_MAC;

#ifdef __LINUX__
    *byteStr = (uint8_t *)malloc(size1 * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(byteStr, size1 + 1);
#endif

    // Copy the fields
    WRITE_ARRAY(SINGLE_POINTER byteStr, 1,                                               0);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[0] & 0xff00000000000000) >> 56,    1);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[0] & 0x00ff000000000000) >> 48,    2);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[0] & 0x0000ff0000000000) >> 40,    3);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[0] & 0x000000ff00000000) >> 32,    4);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[0] & 0x00000000ff000000) >> 24,    5);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[0] & 0x0000000000ff0000) >> 16,    6);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[0] & 0x000000000000ff00) >> 8,     7);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[0] & 0x00000000000000ff),          8);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[1] & 0xff00000000000000) >> 56,    9);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[1] & 0x00ff000000000000) >> 48,    10);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[1] & 0x0000ff0000000000) >> 40,    11);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[1] & 0x000000ff00000000) >> 32,    12);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[1] & 0x00000000ff000000) >> 24,    13);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[1] & 0x0000000000ff0000) >> 16,    14);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[1] & 0x000000000000ff00) >> 8,     15);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->node_id[1] & 0x00000000000000ff),          16);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0xff000000) >> 24,          17);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0x00ff0000) >> 16,          18);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0x0000ff00) >> 8,           19);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0x000000ff),                20);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->currentSlot,                                21);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->collisionSlot,                              22);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0xff000000) >> 24,    23);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x00ff0000) >> 16,    24);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x0000ff00) >> 8,     25);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x000000ff),          26);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->hopCount & 0xff00) >> 8,                   27);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->hopCount & 0x00ff),                        28);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0xff00000000000000) >> 56,   29);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x00ff000000000000) >> 48,   30);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x0000ff0000000000) >> 40,   31);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x000000ff00000000) >> 32,   32);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x00000000ff000000) >> 24,   33);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x0000000000ff0000) >> 16,   34);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x000000000000ff00) >> 8,    35);
    WRITE_ARRAY(SINGLE_POINTER byteStr,  pkt->networkTime & 0x00000000000000ff,          36);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->initTime & 0xff000000) >> 24,              37);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->initTime & 0x00ff0000) >> 16,              38);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->initTime & 0x0000ff00) >> 8,               39);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->initTime & 0x000000ff),                    40);
    int freqs = MAX_NUMBER_FREQS;
    int slots = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    int i = 0, j = 0;
    for (i = 0; i < freqs; i++) {
        for (j = 0; j < slots; j++) {
            WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->occupied_slots[i][j], (i * slots) + j + 41);
        }
    }
    // Fill the rest of the string with a random value.
    i = 41 + (freqs * slots);
    uint n = rand();
    for (; i < PACKET_SIZE_MAC; i++)
        WRITE_ARRAY(SINGLE_POINTER byteStr, n, i);
}

void controlpacket_construct_packet_from_bytestring(ControlPacket_t *pkt, ARRAY* byteString)
{
    assert(pkt != NULL);
    assert(byteString != NULL);

    if (READ_ARRAY(SINGLE_POINTER byteString, 0) != 1)
        return;
    
    // Fill the content of the packet
    pkt->node_id[0] = 0;
    pkt->node_id[0]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        1)) << 56;
    pkt->node_id[0]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        2)) << 48;
    pkt->node_id[0]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        3)) << 40;
    pkt->node_id[0]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        4)) << 32;
    pkt->node_id[0]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        5)) << 24;
    pkt->node_id[0]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        6)) << 16;
    pkt->node_id[0]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        7)) << 8;
    pkt->node_id[0]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        8));
    pkt->node_id[1] = 0;
    pkt->node_id[1]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        9)) << 56;
    pkt->node_id[1]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        10)) << 48;
    pkt->node_id[1]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        11)) << 40;
    pkt->node_id[1]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        12)) << 32;
    pkt->node_id[1]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        13)) << 24;
    pkt->node_id[1]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        14)) << 16;
    pkt->node_id[1]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        15)) << 8;
    pkt->node_id[1]         |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString,        16));
    pkt->currentFrame = 0;
    pkt->currentFrame       |= READ_ARRAY(SINGLE_POINTER byteString,                    17) << 24;
    pkt->currentFrame       |= READ_ARRAY(SINGLE_POINTER byteString,                    18) << 16;
    pkt->currentFrame       |= READ_ARRAY(SINGLE_POINTER byteString,                    19) << 8;
    pkt->currentFrame       |= READ_ARRAY(SINGLE_POINTER byteString,                    20);
    pkt->currentSlot        = READ_ARRAY(SINGLE_POINTER byteString,                     21);
    pkt->collisionSlot      = READ_ARRAY(SINGLE_POINTER byteString,                     22);
    pkt->collisionFrequency = 0;
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString,                    23)  << 24; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString,                    24) << 16; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString,                    25) << 8; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString,                    26);
    pkt->hopCount = 0;
    pkt->hopCount           |= READ_ARRAY(SINGLE_POINTER byteString,                    27) << 8;
    pkt->hopCount           |= READ_ARRAY(SINGLE_POINTER byteString,                    28);
    pkt->networkTime = 0;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString,         29) << 52;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString,         30) << 48;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString,         31) << 40;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString,         32) << 32;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString,         33) << 24;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString,         34) << 16;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString,         35) << 8;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString,         36);
    pkt->initTime = 0;
    pkt->initTime           |= READ_ARRAY(SINGLE_POINTER byteString,                    37) << 24;
    pkt->initTime           |= READ_ARRAY(SINGLE_POINTER byteString,                    38) << 16;
    pkt->initTime           |= READ_ARRAY(SINGLE_POINTER byteString,                    39) << 8;
    pkt->initTime           |= READ_ARRAY(SINGLE_POINTER byteString,                    40);
    int freqs = MAX_NUMBER_FREQS;
    int slots = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < freqs; i++) {
        for (int j = 0; j < slots; j++) {
            pkt->occupied_slots[i][j] = READ_ARRAY(SINGLE_POINTER byteString, (i * slots) + j + 41);
        }
    }
}
