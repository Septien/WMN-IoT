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

void controlpacket_init(ControlPacket_t DOUBLE_POINTER pkt, uint8_t nSlots, uint8_t nChannels)
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
    (SINGLE_POINTER pkt)->_nSlots = nSlots;
    (SINGLE_POINTER pkt)->_nChannels = nChannels;
    // Create a one-dimension array of size nSlot*nChannels to store the occupied slots per channel
    uint8_t bytes = get_number_bytes(nSlots * nChannels);
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
void controlpacket_create(ControlPacket_t *pkt, uint8_t nodeID, ARRAY* occupiedSlots, uint8_t collisionSlots, 
                          uint32_t collisionFrequency, uint8_t hopCount, uint32_t netTime, uint8_t ack)
{
    assert(pkt != NULL);
    assert(occupiedSlots != NULL);
#ifdef __LINUX__
    assert(*occupiedSlots != NULL);
#endif

    pkt->nodeID = nodeID;
    uint8_t bytes = get_number_bytes(pkt->_nChannels * pkt->_nSlots);
    for (int i = 0; i < bytes; i++)
    {
        uint8_t element = READ_ARRAY(SINGLE_POINTER occupiedSlots, i);
        WRITE_ARRAY(REFERENCE pkt->occupiedSlots, element, i);
    }
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
    pkt->occupiedSlots = NULL;
#endif
#ifdef __RIOT__
    free_array(&pkt->occupiedSlots);
#endif
    memset(pkt, 0, sizeof(ControlPacket_t));
}

void controlpacket_set_nodeID(ControlPacket_t *pkt, uint8_t nodeID)
{
    assert(pkt != NULL);

    pkt->nodeID = nodeID;
}

uint8_t controlpacket_get_nodeID(ControlPacket_t *pkt)
{
    assert(pkt != NULL);
    
    return pkt->nodeID;
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

    uint8_t bytes = get_number_bytes(pkt->_nChannels * pkt->_nSlots);
    for (uint8_t i = 0; i < bytes; i++)
    {
        uint8_t element = READ_ARRAY(SINGLE_POINTER occupiedSlots, i);
        WRITE_ARRAY(REFERENCE pkt->occupiedSlots, element, i);
    }
    //memcpy(REFERENCE pkt->occupiedSlots, SINGLE_POINTER occupiedSlots, get_number_bytes(pkt->_nChannels * pkt->_nSlots));
}

void controlpacket_get_occupied_slots(ControlPacket_t *pkt, ARRAY* occupiedSlots)
{
    assert(pkt != NULL);
    assert(occupiedSlots != NULL);
#ifdef __LINUX__
    assert(*occupiedSlots != NULL);
#endif

    uint8_t bytes = get_number_bytes(pkt->_nChannels * pkt->_nSlots);
    for (uint8_t i = 0; i < bytes; i++)
    {
        uint8_t element = READ_ARRAY(REFERENCE pkt->occupiedSlots, i);
        WRITE_ARRAY(SINGLE_POINTER occupiedSlots, element, i);
    }
    //memcpy(SINGLE_POINTER occupiedSlots, REFERENCE pkt->occupiedSlots, get_number_bytes(pkt->_nChannels * pkt->_nSlots));
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
    // Get string size and allocate memory
    size_t size1 = 0;
    uint8_t bytes = get_number_bytes(pkt->_nSlots * pkt->_nChannels);
    size1 += sizeof(pkt->nodeID),
    size1 += (size_t)bytes;
    size1 += sizeof(pkt->collisionSlot);
    size1 += sizeof(pkt->collisionFrequency);
    size1 += sizeof(pkt->hopCount);
    size1 += sizeof(pkt->networkTime);
    size1 += sizeof(pkt->ack);
    size1 += 1;

#ifdef __LINUX__
    *byteStr = (uint8_t *)malloc(size1 * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(byteStr, size1);
#endif

    // Copy the fields
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->nodeID, 0);
    int i;
    for (i = 0; i < bytes; i++)
    {
        uint8_t element = READ_ARRAY(REFERENCE pkt->occupiedSlots, i);
        WRITE_ARRAY(SINGLE_POINTER byteStr, element, i + 1);
    }
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->collisionSlot,                            i + 2);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0xff000000) >> 24,  i + 3);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x00ff0000) >> 16,  i + 4);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x0000ff00) >> 8,   i + 5);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->collisionFrequency & 0x000000ff),        i + 6);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->hopCount,                                 i + 7);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0xff000000) >> 24,         i + 8);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x00ff0000) >> 16,         i + 9);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x0000ff00) >> 8,          i + 10);
    WRITE_ARRAY(SINGLE_POINTER byteStr, (pkt->networkTime & 0x000000ff),               i + 11);
    WRITE_ARRAY(SINGLE_POINTER byteStr, pkt->ack,                                      i + 12);

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
    uint8_t bytes = get_number_bytes(pkt->_nSlots * pkt->_nChannels);
    
    // Fill the content of the packet
    pkt->nodeID = READ_ARRAY(SINGLE_POINTER byteString, 0);
    for (int i = 0; i < bytes; i++)
    {
        uint8_t element;
        element = READ_ARRAY(SINGLE_POINTER byteString, i + 1);
        WRITE_ARRAY(REFERENCE pkt->occupiedSlots, element, i);
    }
    pkt->collisionSlot = READ_ARRAY(SINGLE_POINTER byteString, bytes + 2);
    pkt->collisionFrequency = 0;
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 3) << 24; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 4) << 16; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 5) << 8; 
    pkt->collisionFrequency |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 6); 
    pkt->networkTime = 0;
    pkt->networkTime |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 7) << 24;
    pkt->networkTime |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 8) << 16;
    pkt->networkTime |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 9) << 8;
    pkt->networkTime |= READ_ARRAY(SINGLE_POINTER byteString, bytes + 10);
    pkt->hopCount = READ_ARRAY(SINGLE_POINTER byteString, bytes + 11);
    pkt->ack = READ_ARRAY(SINGLE_POINTER byteString, bytes + 12);
}
