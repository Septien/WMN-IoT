#include "controlPacket.h"
#include <assert.h>
#include <string.h>

// Given an arbitrary number n, get the next multiple of 8 greater than n, and
// return the multiple.
uint8_t getNumberBytes(uint8_t n)
{
    uint8_t mod = 8;
    uint8_t res = n % mod;
    uint8_t add = (res == 0 ? 0 : mod - res);
    return (n + add) / 8;
}

void initCP(ControlPacket_t **pkt, uint8_t nSlots, uint8_t nChannels)
{
    ControlPacket_t *pktA = NULL;
    pktA = (ControlPacket_t *)malloc(sizeof(ControlPacket_t));
    if (pktA == NULL)
    {
        printf("Unable to create packet.\n");
        exit(-1);
    }
    pktA->_nSlots = nSlots;
    pktA->_nChannels = nChannels;
    // Create a one-dimension array of size nSlot*nChannels to store the occupied slots per channel
    uint8_t bytes = getNumberBytes(nSlots * nChannels), i;
    pktA->occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    *pkt = pktA;
}

void destroyPacketCP(ControlPacket_t **pkt)
{
    assert(pkt != NULL);
    assert(*pkt != NULL);

    free((*pkt)->occupiedSlots);
    free(*pkt);

    *pkt = NULL;
    return;
}

// Fill the packet with the given parameters
void createPacketCP(ControlPacket_t *pkt, uint8_t nodeID, uint8_t *occupiedSlots, uint8_t collisionSlot, uint32_t collisionFrequency, uint8_t hopCount, uint8_t ack)
{
    assert(pkt != NULL);
    assert(occupiedSlots != NULL);

    pkt->nodeID = nodeID;
    uint8_t bytes = getNumberBytes(pkt->_nChannels * pkt->_nSlots);
    for (int i = 0; i < bytes; i++)
        pkt->occupiedSlots[i] = occupiedSlots[i];
    pkt->collisionSlot = collisionSlot;
    pkt->collisionFrequency = collisionFrequency;
    pkt->hopCount = hopCount;
    pkt->ack = ack;
}

// Set all values of the packet to zero, and free the arrays
void clearPacktCP(ControlPacket_t *pkt)
{
    pkt->nodeID = 0;
    free(pkt->occupiedSlots);
    pkt->occupiedSlots = NULL;
    pkt->collisionSlot = 0;
    pkt->collisionFrequency = 0;
    pkt->hopCount = 0;
    pkt->ack = 0;
    pkt->_nSlots = 0;
}

void setNodeIDCP(ControlPacket_t *pkt, uint8_t nodeID)
{
    assert(pkt != NULL);
    assert(0 <= nodeID && nodeID <= 255);

    pkt->nodeID = nodeID;
}

uint8_t getNodeIDCP(ControlPacket_t *pkt)
{
    assert(pkt != NULL);
    
    return pkt->nodeID;
}

void setCollisionSlotCP(ControlPacket_t *pkt, uint8_t slot)
{
    assert(pkt != NULL);

    pkt->collisionSlot = slot;
}

uint8_t getCollisionSlotCP(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->collisionSlot;
}

void setOccupiedSlotsCP(ControlPacket_t *pkt, uint8_t *occupiedSlots)
{
    assert(pkt != NULL);
    assert(occupiedSlots != NULL);

    memcpy(pkt->occupiedSlots, occupiedSlots, getNumberBytes(pkt->_nChannels * pkt->_nSlots));
}

void getOccupiedSlotsCP(ControlPacket_t *pkt, uint8_t *occupiedSlots)
{
    assert(pkt != NULL);
    assert(occupiedSlots != NULL);
    assert(sizeof(pkt->occupiedSlots) == sizeof(occupiedSlots) );

    memcpy(occupiedSlots, pkt->occupiedSlots, getNumberBytes(pkt->_nChannels * pkt->_nSlots));
}

void setCollisionFrequencyCP(ControlPacket_t *pkt, uint32_t frequency)
{
    assert(pkt != NULL);

    pkt->collisionFrequency = frequency;
}

uint32_t getColissionFrequencyCP(ControlPacket_t *pkt)
{
    uint32_t freq = pkt->collisionFrequency;
    return freq;
}

void setHopCountCP(ControlPacket_t *pkt, uint8_t count)
{
    assert(pkt != NULL);
    pkt->hopCount = count;
}

uint8_t getHopCountCP(ControlPacket_t *pkt)
{
    assert(pkt != NULL);
    uint8_t hops = pkt->hopCount;

    return hops;
}

void setACKCP(ControlPacket_t *pkt, uint8_t ack)
{
    assert(pkt != NULL);

    pkt->ack = ack;
}

uint8_t getACKCP(ControlPacket_t *pkt)
{
    assert(pkt != NULL);

    uint8_t ack = pkt->ack;
    return ack;
}

void getPacketByteStringCP(ControlPacket_t *pkt, uint8_t **byteStr, size_t *size)
{
    assert(pkt != NULL);
    // Get string size and allocate memory
    size_t size1 = 0;
    uint8_t bytes = getNumberBytes(pkt->_nSlots * pkt->_nChannels);
    size1 += sizeof(pkt->nodeID),
    size1 += (size_t)bytes;
    size1 += sizeof(pkt->collisionSlot);
    size1 += sizeof(pkt->collisionFrequency);
    size1 += sizeof(pkt->hopCount);
    size1 += sizeof(pkt->ack);

    uint8_t *byteStrA = (uint8_t *)malloc(size1 * sizeof(uint8_t));

    // Copy the fields
    byteStrA[0] = pkt->nodeID;
    int i;
    for (i = 0; i < bytes; i++)
    {
        byteStrA[i + 1] = pkt->occupiedSlots[i];
    }
    byteStrA[i + 2] = pkt->collisionSlot;
    byteStrA[i + 3] = (pkt->collisionFrequency & 0xff000000) >> 24;
    byteStrA[i + 4] = (pkt->collisionFrequency & 0x00ff0000) >> 16;
    byteStrA[i + 5] = (pkt->collisionFrequency & 0x0000ff00) >> 8;
    byteStrA[i + 6] = (pkt->collisionFrequency & 0x000000ff);
    byteStrA[i + 7] = pkt->hopCount;
    byteStrA[i + 8] = pkt->ack;
    *byteStr = byteStrA;
    *size = size1;
}

void constructPacketFromByteStringCP(ControlPacket_t *pkt, uint8_t *byteString, size_t size)
{
    assert(pkt != NULL);
    assert(byteString != NULL);
    assert(size > 0);

    // Get size of collisionSlots array and initialize array
    uint8_t bytes = getNumberBytes(pkt->_nSlots * pkt->_nChannels);
    pkt->occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    
    // Fill the content of the packet
    pkt->nodeID = byteString[0];
    for (int i = 0; i < bytes; i++)
        pkt->occupiedSlots[i] = byteString[i + 1];
    pkt->collisionSlot = byteString[bytes + 2];
    pkt->collisionFrequency = 0;
    pkt->collisionFrequency |= byteString[bytes + 3] << 24;
    pkt->collisionFrequency |= byteString[bytes + 4] << 16;
    pkt->collisionFrequency |= byteString[bytes + 5] << 8;
    pkt->collisionFrequency |= byteString[bytes + 6];
    pkt->hopCount = byteString[bytes + 7];
    pkt->ack = byteString[bytes + 8];
}
