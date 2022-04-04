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
    free((*pkt)->ack);
    free(*pkt);

    *pkt = NULL;
#endif
#ifdef __RIOT__
    free_array(&pkt->ack);
    pkt->nodeID = 0;
    pkt->currentFrame = 0;
    pkt->currentSlot = 0;
    pkt->collisionSlot = 0;
    pkt->hopCount = 0;
    pkt->networkTime = 0;
    pkt->initTime = 0;
#endif
}

// Fill the packet with the given parameters
void controlpacket_create(ControlPacket_t *pkt, uint16_t nodeID, uint32_t frame, uint8_t slot,uint8_t collisionSlots, 
                          uint32_t collisionFrequency, uint8_t hopCount, uint64_t netTime, uint32_t initTime, ARRAY* ack, int n)
{
    assert(pkt != NULL);

    pkt->nodeID = nodeID;
    pkt->currentFrame = frame;
    pkt->currentSlot = slot;
    pkt->collisionSlot = collisionSlots;
    pkt->collisionFrequency = collisionFrequency;
    pkt->hopCount = hopCount;
    pkt->networkTime = netTime;
    pkt->initTime = initTime;
    pkt->ackSize = n;
    // Fill ack array; release in case it contains data.
#ifdef __LINUX__
    if (pkt->ack != NULL)
    {
        free(pkt->ack);
        pkt->ack = NULL;
    }
    if (ack == NULL)
        return;
    pkt->ack = (uint8_t *)malloc(n * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    if (pkt->ack.size != 0)
        free_array(&pkt->ack);
    if (ack == NULL)
        return;
    create_array(&pkt->ack, n);
#endif
    for (int i = 0; i < n; i++)
    {
        uint8_t e = READ_ARRAY(REFERENCE (*ack), i);
        WRITE_ARRAY(REFERENCE pkt->ack, e, i);
    }
}

// Set all values of the packet to zero, and free the arrays
void controlpacket_clear(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    memset(pkt, 0, sizeof(ControlPacket_t));
}

void controlpacket_set_nodeID(ControlPacket_t *pkt, uint16_t nodeID)
{
    assert(pkt != NULL);

    pkt->nodeID = nodeID;
}

uint16_t controlpacket_get_nodeID(ControlPacket_t *pkt)
{
    assert(pkt != NULL);
    
    return pkt->nodeID;
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

void controlpacket_set_collision_slot(ControlPacket_t *pkt, uint8_t slot)
{
    assert(pkt != NULL);

    pkt->collisionSlot = slot;
}

uint8_t controlpacket_get_collision_slot(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->collisionSlot;
}

void controlpacket_set_collision_frequency(ControlPacket_t *pkt, uint32_t frequency)
{
    assert(pkt != NULL);

    pkt->collisionFrequency = frequency;
}

uint32_t controlpacket_get_collision_frequency(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->collisionFrequency;;
}

void controlpacket_set_hop_count(ControlPacket_t *pkt, uint8_t count)
{
    assert(pkt != NULL);
    
    pkt->hopCount = count;
}

uint8_t controlpacket_get_hop_count(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->hopCount;;
}

void controlpacket_set_network_time(ControlPacket_t *pkt, uint64_t time)
{
    assert(pkt != NULL);
    assert(time > 0);

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

void controlpacket_set_ACK(ControlPacket_t *pkt, ARRAY* ack, int n)
{
    assert(pkt != NULL);
    assert(n > 0);

#ifdef __LINUX__
    if (pkt->ack != NULL)
        free(pkt->ack);
    pkt->ack = (uint8_t *)malloc(n * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    if (pkt->ack.size != 0)
        free_array(&pkt->ack);
    create_array(&pkt->ack, n);
#endif

    for (int i = 0; i < n; i++)
    {
        uint8_t e = READ_ARRAY(REFERENCE (*ack), i);
        WRITE_ARRAY(REFERENCE pkt->ack, e, i);
    }
}

void controlpacket_get_ACK(ControlPacket_t *pkt, ARRAY* ack, int n)
{
    assert(pkt != NULL);
    assert(n > 0);
#ifdef __LINUX__
    assert(pkt->ack != NULL);
#endif
#ifdef __RIOT__
    assert(pkt->ack.size > 0);
#endif
    for (int i = 0; i < n; i++)
    {
        uint8_t e = READ_ARRAY(REFERENCE pkt->ack, i);
        WRITE_ARRAY(REFERENCE (*ack), e, i);
    }
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
    WRITE_ARRAY(SINGLE_POINTER byteStr, 1,                                              0);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->nodeID & 0xff00) >> 8,                    1);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->nodeID & 0x00ff),                         2);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0xff000000) >> 24,         3);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0x00ff0000) >> 16,         4);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0x0000ff00) >> 8,          5);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0x000000ff),               6);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->currentSlot,                               7);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->collisionSlot,                             8);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0xff000000) >> 24,   9);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x00ff0000) >> 16,   10);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x0000ff00) >> 8,    11);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x000000ff),         12);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->hopCount,                                  13);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0xff00000000000000) >> 56,  14);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x00ff000000000000) >> 48,  15);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x0000ff0000000000) >> 40,  16);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x000000ff00000000) >> 32,  17);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x00000000ff000000) >> 24,  18);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x0000000000ff0000) >> 16,  19);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x000000000000ff00) >> 8,   20);
    WRITE_ARRAY(SINGLE_POINTER byteStr,  pkt->networkTime & 0x00000000000000ff,         21);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->initTime & 0xff000000) >> 24,             22);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->initTime & 0x00ff0000) >> 16,             23);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->initTime & 0x0000ff00) >> 8,              24);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->initTime & 0x000000ff),                   25);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->ackSize,                                   26);
    uint i = 0;
    if (pkt->ackSize != 0)
    {
        for (i = 0; i < pkt->ackSize; i++)
        {
            uint8_t e = READ_ARRAY(REFERENCE pkt->ack, i);
            WRITE_ARRAY(REFERENCE (*byteStr), e, 27 + i);
        }
    }
    // Fill the rest of the string with a random value.
    i += 27;
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
    pkt->nodeID = 0;
    pkt->nodeID             |= READ_ARRAY(SINGLE_POINTER byteString,            1) << 8;
    pkt->nodeID             |= READ_ARRAY(SINGLE_POINTER byteString,            2);
    pkt->currentFrame = 0;
    pkt->currentFrame       |= READ_ARRAY(SINGLE_POINTER byteString,            3) << 24;
    pkt->currentFrame       |= READ_ARRAY(SINGLE_POINTER byteString,            4) << 16;
    pkt->currentFrame       |= READ_ARRAY(SINGLE_POINTER byteString,            5) << 8;
    pkt->currentFrame       |= READ_ARRAY(SINGLE_POINTER byteString,            6);
    pkt->currentSlot        = READ_ARRAY(SINGLE_POINTER byteString,             7);
    pkt->collisionSlot      = READ_ARRAY(SINGLE_POINTER byteString,             8);
    pkt->collisionFrequency = 0;
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString,            9) << 24; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString,            10) << 16; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString,            11) << 8; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString,            12);
    pkt->hopCount           = READ_ARRAY(SINGLE_POINTER byteString,             13);
    pkt->networkTime = 0;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 14) << 52;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 15) << 48;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 16) << 40;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 17) << 32;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 18) << 24;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 19) << 16;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 20) << 8;
    pkt->networkTime        |= (uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 21);
    pkt->initTime = 0;
    pkt->initTime           |= READ_ARRAY(SINGLE_POINTER byteString,            22) << 24;
    pkt->initTime           |= READ_ARRAY(SINGLE_POINTER byteString,            23) << 16;
    pkt->initTime           |= READ_ARRAY(SINGLE_POINTER byteString,            24) << 8;
    pkt->initTime           |= READ_ARRAY(SINGLE_POINTER byteString,            25);
    pkt->ackSize = 0;
    pkt->ackSize            = READ_ARRAY(SINGLE_POINTER byteString,             26);
    if (pkt->ackSize > 0)
    {
#ifdef __LINUX__
        assert(*byteString != NULL);
        if (pkt->ack != NULL)
            free(pkt->ack);
        pkt->ack = (uint8_t *)malloc(pkt->ackSize * sizeof(uint8_t));
#endif
#ifdef __RIOT__
        if (pkt->ack.size > 0)
            free_array(&pkt->ack);
        create_array(&pkt->ack, pkt->ackSize);
#endif
        for (int i = 0; i < pkt->ackSize; i++)
        {
            uint8_t e = READ_ARRAY(SINGLE_POINTER byteString, i + 27);
            WRITE_ARRAY(REFERENCE pkt->ack, e, i);
        }
    }
}
