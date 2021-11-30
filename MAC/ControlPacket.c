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
    // Create a one-dimension array of size nSlot*nChannels to store the occupied slots per channel
    uint8_t bytes = MAX_NUMBER_FREQS * (MAX_NUMBER_SLOTS + (MAX_NUMBER_SLOTS % 8));
#ifdef __LINUX__
    (*pkt)->occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&pkt->occupiedSlots, bytes);
#endif
}

void controlpacket_destroy(ControlPacket_t DOUBLE_POINTER pkt)
{
    assert(pkt != NULL);
#ifdef __LINUX__
    assert(*pkt != NULL);

    free((*pkt)->occupiedSlots);
    (*pkt)->occupiedSlots = NULL;
    free(*pkt);

    *pkt = NULL;
#endif
#ifdef __RIOT__
    free_array(&pkt->occupiedSlots);
    memset(pkt, 0, sizeof(ControlPacket_t));
#endif
}

// Fill the packet with the given parameters
void controlpacket_create(ControlPacket_t *pkt, uint16_t nodeID, ARRAY* occupiedSlots, uint32_t frame, uint8_t slot,uint8_t collisionSlots, 
                          uint32_t collisionFrequency, uint8_t hopCount, uint32_t netTime, uint8_t ack)
{
    assert(pkt != NULL);
    assert(occupiedSlots != NULL);
#ifdef __LINUX__
    assert(*occupiedSlots != NULL);
#endif

    pkt->nodeID = nodeID;
    uint8_t bytes = MAX_NUMBER_FREQS * (MAX_NUMBER_SLOTS + (MAX_NUMBER_SLOTS % 8));
    for (int i = 0; i < bytes; i++)
    {
        uint8_t element = READ_ARRAY(SINGLE_POINTER occupiedSlots, i);
        WRITE_ARRAY(REFERENCE pkt->occupiedSlots, element, i);
    }
    pkt->currentFrame = frame;
    pkt->currentSlot = slot;
    pkt->collisionSlot = collisionSlots;
    pkt->collisionFrequency = collisionFrequency;
    pkt->hopCount = hopCount;
    pkt->networkTime = netTime;
    pkt->ack = ack;
}

// Set all values of the packet to zero, and free the arrays
void controlpacket_clear(ControlPacket_t *pkt)
{
    assert(pkt != NULL);
#ifdef __LINUX__
    free(pkt->occupiedSlots);
#endif
#ifdef __RIOT__
    free_array(&pkt->occupiedSlots);
#endif
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

void controlpacket_set_occupied_slots(ControlPacket_t *pkt, ARRAY* occupiedSlots)
{
    assert(pkt != NULL);
    assert(occupiedSlots != NULL);

    uint8_t bytes = MAX_NUMBER_FREQS * (MAX_NUMBER_SLOTS + (MAX_NUMBER_SLOTS % 8));
    for (uint8_t i = 0; i < bytes; i++)
    {
        uint8_t element = READ_ARRAY(SINGLE_POINTER occupiedSlots, i);
        WRITE_ARRAY(REFERENCE pkt->occupiedSlots, element, i);
    }
}

void controlpacket_get_occupied_slots(ControlPacket_t *pkt, ARRAY* occupiedSlots)
{
    assert(pkt != NULL);
    assert(occupiedSlots != NULL);
#ifdef __LINUX__
    assert(*occupiedSlots != NULL);
#endif

    uint8_t bytes = MAX_NUMBER_FREQS * (MAX_NUMBER_SLOTS + (MAX_NUMBER_SLOTS % 8));
    for (uint8_t i = 0; i < bytes; i++)
    {
        uint8_t element = READ_ARRAY(REFERENCE pkt->occupiedSlots, i);
        WRITE_ARRAY(SINGLE_POINTER occupiedSlots, element, i);
    }
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

void controlpacket_set_network_time(ControlPacket_t *pkt, uint32_t time)
{
    assert(pkt != NULL);
    assert(time > 0);

    pkt->networkTime = time;
}

uint32_t controlpacket_get_network_time(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->networkTime;
}

void controlpacket_set_ACK(ControlPacket_t *pkt, uint8_t ack)
{
    assert(pkt != NULL);

    pkt->ack = ack;
}

uint8_t controlpacket_get_ACK(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->ack;
}

void controlpacket_get_packet_bytestring(ControlPacket_t *pkt, ARRAY* byteStr, size_t *size)
{
    assert(pkt != NULL);

    int n = MAX_NUMBER_FREQS;
    int m = MAX_NUMBER_SLOTS + (MAX_NUMBER_SLOTS % 8);
    // Get string size and allocate memory
    size_t size1 = 0;
    uint8_t bytes = n * m;
    size1 += sizeof(pkt->nodeID),
    size1 += (size_t)bytes;
    size1 += sizeof(pkt->currentFrame);
    size1 += sizeof(pkt->currentSlot);
    size1 += sizeof(pkt->collisionSlot);
    size1 += sizeof(pkt->collisionFrequency);
    size1 += sizeof(pkt->hopCount);
    size1 += sizeof(pkt->networkTime);
    size1 += sizeof(pkt->ack);

#ifdef __LINUX__
    *byteStr = (uint8_t *)malloc(size1 * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(byteStr, size1 + 1);
#endif

    // Copy the fields
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->nodeID & 0xff00) >> 8, 0);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->nodeID & 0x00ff),      1);
    int i;
    for (i = 0; i < bytes; i++)
    {
        uint8_t element = READ_ARRAY(REFERENCE pkt->occupiedSlots, i);
        WRITE_ARRAY(SINGLE_POINTER byteStr, element, i + 2);
    }
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0xff000000) >> 24,        i + 3);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0x00ff0000) >> 16,        i + 4);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0x0000ff00) >> 8,         i + 5);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->currentFrame & 0x000000ff),              i + 6);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->currentSlot,                              i + 7);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->collisionSlot,                            i + 8);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0xff000000) >> 24,  i + 9);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x00ff0000) >> 16,  i + 10);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x0000ff00) >> 8,   i + 11);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x000000ff),        i + 12);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->hopCount,                                 i + 13);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0xff000000) >> 24,         i + 14);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x00ff0000) >> 16,         i + 15);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x0000ff00) >> 8,          i + 16);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x000000ff),               i + 17);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->ack,                                      i + 18);

    *size = size1;
}

void controlpacket_construct_packet_from_bytestring(ControlPacket_t *pkt, ARRAY* byteString, size_t size)
{
    assert(pkt != NULL);
    assert(byteString != NULL);
#ifdef __LINUX__
    assert(*byteString != NULL);
#endif
    assert(size > 0);

    // Get size of collisionSlots array and initialize array
    int n = MAX_NUMBER_FREQS;
    int m = MAX_NUMBER_SLOTS + (MAX_NUMBER_SLOTS % 8);
    uint8_t bytes = n * m;
    
    // Fill the content of the packet
    pkt->nodeID = (READ_ARRAY(SINGLE_POINTER byteString, 0) << 8) | READ_ARRAY(SINGLE_POINTER byteString, 1);
    for (int i = 0; i < bytes; i++)
    {
        uint8_t element;
        element = READ_ARRAY(SINGLE_POINTER byteString, i + 2);
        WRITE_ARRAY(REFERENCE pkt->occupiedSlots, element, i);
    }
    pkt->currentFrame = READ_ARRAY(SINGLE_POINTER byteString, bytes + 3);
    pkt->currentSlot = READ_ARRAY(SINGLE_POINTER byteString, bytes + 4);
    pkt->collisionSlot = READ_ARRAY(SINGLE_POINTER byteString, bytes + 5);
    pkt->collisionFrequency = 0;
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 6) << 24; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 7) << 16; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 8) << 8; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 9); 
    pkt->networkTime = 0;
    pkt->networkTime |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 10) << 24;
    pkt->networkTime |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 11) << 16;
    pkt->networkTime |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 12) << 8;
    pkt->networkTime |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 13);
    pkt->hopCount = READ_ARRAY(SINGLE_POINTER byteString, bytes + 14);
    pkt->ack = READ_ARRAY(SINGLE_POINTER byteString, bytes + 15);
}
