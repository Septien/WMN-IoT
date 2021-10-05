#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#include "ControlPacket.h"

#define ITERATIONS 1000
#if 0
void testInit(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    assert(pkt != NULL);
    assert(pkt->occupiedSlots != NULL);
    assert(pkt->_nSlots == slots);
    assert(pkt->_nChannels == channels);

    destroyPacketCP(&pkt);
}

void testDestroyPacket(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    destroyPacketCP(&pkt);

    assert(pkt == NULL);
}

void testCreatePacket(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    uint8_t nodeID = 1;
    uint8_t bytes = getNumberBytes(slots * channels);
    uint8_t *occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    for (int i = 0; i < bytes; i++)
        occupiedSlots[i] = 0xf0;
    uint8_t collisionSlot = 10;
    uint32_t collisionFrequency = 915;
    uint8_t hopCount = 3;
    uint8_t ack = 0;
    uint32_t netTime = rand();

    createPacketCP(pkt, nodeID, occupiedSlots, collisionSlot, collisionFrequency, hopCount, netTime, ack);

    assert(pkt != NULL);
    assert(pkt->occupiedSlots != NULL && sizeof(pkt->occupiedSlots) == slots);
    for (int i = 0; i < bytes; i++)
        assert(pkt->occupiedSlots[i] == 0xf0);
    assert(pkt->collisionSlot == collisionSlot);
    assert(pkt->collisionFrequency == collisionFrequency);
    assert(pkt->hopCount == hopCount);
    assert(pkt->networkTime == netTime);
    assert(pkt->ack == ack);

    free(occupiedSlots);
    destroyPacketCP(&pkt);
}

void testClearPacket(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;;
    initCP(&pkt, slots, channels);

    uint8_t nodeID = 1;
    uint8_t bytes = getNumberBytes(pkt->_nChannels * pkt->_nSlots);
    uint8_t *occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    for (int i = 0; i < slots; i++)
        occupiedSlots[i] = 0xf0;
    uint8_t collisionSlots = 0x01;
    uint32_t collisionFrequency = 915;
    uint8_t hopCount = 3;
    uint32_t netTime = rand();
    uint8_t ack = 1;

    createPacketCP(pkt, nodeID, occupiedSlots, collisionSlots, collisionFrequency, hopCount, netTime, ack);

    clearPacktCP(pkt);

    assert(pkt->nodeID == 0);
    assert(pkt->occupiedSlots == NULL);
    assert(pkt->collisionFrequency == 0);
    assert(pkt->collisionSlot == 0);
    assert(pkt->hopCount == 0);
    assert(pkt->ack == 0);
    assert(pkt->_nSlots == 0);

    free(occupiedSlots);
    destroyPacketCP(&pkt);
}

void testsetNodeID(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;;
    initCP(&pkt, slots, channels);

    for (uint8_t i = 0; i < 255; i++)
    {
        setNodeIDCP(pkt, i);
        assert(pkt->nodeID == i);
    }

    destroyPacketCP(&pkt);
}

void testgetNodeID(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;;
    initCP(&pkt, slots, channels);

    uint8_t nodeid = 0;
    for (uint8_t i = 0; i < 255; i++)
    {
        setNodeIDCP(pkt, i);
        nodeid = getNodeIDCP(pkt);
        assert(nodeid == i);
    }

    destroyPacketCP(&pkt);
}

void testsetCollisionSlot(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;;
    initCP(&pkt, slots, channels);

    uint8_t slot = 10;
    setCollisionSlotCP(pkt, slot);

    assert(pkt->collisionSlot == slot);

    destroyPacketCP(&pkt);
}

void testgetCollisionSlot(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;;
    initCP(&pkt, slots, channels);

    uint8_t slot = 10;
    setCollisionSlotCP(pkt, slot);

    slot = getCollisionSlotCP(pkt);
    assert(slot == pkt->collisionSlot);

    destroyPacketCP(&pkt);
}

void testsetOccupiedSlots(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    uint8_t bytes = getNumberBytes(slots * channels);
    uint8_t *occSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    uint8_t i;
    for (i = 0; i < bytes; i++)
        occSlots[i] = 0xff;
    setOccupiedSlotsCP(pkt, occSlots);

    for (i = 0; i < bytes; i++)
        assert(pkt->occupiedSlots[i] == occSlots[i]);

    free(occSlots);
    destroyPacketCP(&pkt);
}

void testgetOccupiedSlots(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    uint8_t bytes = getNumberBytes(slots * channels);
    uint8_t *occSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    uint8_t i;
    for (i = 0; i < bytes; i++)
        occSlots[i] = 0xff;
    setOccupiedSlotsCP(pkt, occSlots);

    uint8_t *occSlots2 = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    getOccupiedSlotsCP(pkt, occSlots2);
    for (uint8_t i = 0; i < bytes; i++)
        assert(occSlots2[i] == pkt->occupiedSlots[i]);

    free(occSlots);
    free(occSlots2);
    destroyPacketCP(&pkt);
}

void testsetCollisionFrequency(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    uint32_t freq = 915;
    setCollisionFrequencyCP(pkt, freq);
    assert(pkt->collisionFrequency == freq);

    destroyPacketCP(&pkt);
}

void testgetCollisionFrequency(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    uint32_t freq = 915;
    setCollisionFrequencyCP(pkt, freq);
    
    uint32_t freqr = getColissionFrequencyCP(pkt);
    assert(freqr == pkt->collisionFrequency);
    assert(freqr == freq);

    destroyPacketCP(&pkt);
}

void testsetHopCount(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    uint8_t hops = 8;
    setHopCountCP(pkt, hops);
    assert(pkt->hopCount == hops);

    destroyPacketCP(&pkt);
}

void testgetHopCount(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    uint8_t hops = 8;
    setHopCountCP(pkt, hops);
    
    uint8_t hops2 = getHopCountCP(pkt);
    assert(hops2 == pkt->hopCount);
    assert(hops2 == hops);

    destroyPacketCP(&pkt);
}

void testsetNetworkTimeCP(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    uint32_t netTime;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        netTime = rand();
        setNetworkTimeCP(pkt, netTime);
        assert(pkt->networkTime == netTime);
    }

    destroyPacketCP(&pkt);
}

void testgetNetworkTimeCP(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    uint32_t time, timeA;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        time = (uint32_t)rand();
        setNetworkTimeCP(pkt, time);
        timeA = getNetworkTimeCP(pkt);
        assert(timeA == time);
    }

    destroyPacketCP(&pkt);
}

void testsetACK(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    uint8_t ack = 1;
    setACKCP(pkt, ack);
    assert(pkt->ack == ack);

    destroyPacketCP(&pkt);
}

void testgetACK(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    initCP(&pkt, slots, channels);

    uint8_t ack = 1;
    setACKCP(pkt, ack);
    
    uint8_t ack2 = getACKCP(pkt);
    assert(ack2 == pkt->ack);
    assert(ack2 == ack);

    destroyPacketCP(&pkt);
}

void testgetByteString(void)
{
    ControlPacket_t *pkt;
    size_t slots = 9, channels = 17;
    initCP(&pkt, slots, channels);

    int i;
    uint8_t bytes = getNumberBytes(slots * channels);
    uint8_t nodeID = 1;
    uint8_t *occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    for (i = 0; i < bytes; i++)
        occupiedSlots[i] = 0xff;
    uint8_t collisionSlots = 0x01;
    uint32_t collisionFrequency = 0xf0f0f0f0;
    uint8_t freq[4];
    uint8_t hopCount = 3;
    uint32_t netTime = rand();
    uint8_t time[4];
    uint8_t ack = 0;

    createPacketCP(pkt, nodeID, occupiedSlots, collisionSlots, collisionFrequency, hopCount, netTime, ack);

    freq[0] = (collisionFrequency & 0xff000000) >> 24;
    freq[1] = (collisionFrequency & 0x00ff0000) >> 16;
    freq[2] = (collisionFrequency & 0x0000ff00) >> 8;
    freq[3] = (collisionFrequency & 0x000000ff);
    time[0] = (netTime & 0xff000000) >> 24;
    time[1] = (netTime & 0x00ff0000) >> 16;
    time[2] = (netTime & 0x0000ff00) >> 8;
    time[3] = (netTime & 0x000000ff);
    uint8_t *byteStr;
    size_t size;
    getPacketByteStringCP(pkt, &byteStr, &size);
    assert(byteStr[0] == nodeID);
    for (i = 0; i < bytes; i++)
        assert(byteStr[i+1] == occupiedSlots[i]);
    assert(byteStr[bytes+2] == collisionSlots);
    assert(byteStr[bytes+3] == freq[0]);
    assert(byteStr[bytes+4] == freq[1]);
    assert(byteStr[bytes+5] == freq[2]);
    assert(byteStr[bytes+6] == freq[3]);
    assert(byteStr[bytes+7] == hopCount);
    assert(byteStr[bytes+8] == time[0]);
    assert(byteStr[bytes+9] == time[1]);
    assert(byteStr[bytes+10] == time[2]);
    assert(byteStr[bytes+11] == time[3]);
    assert(byteStr[bytes+12] == ack);

    free(byteStr);
    free(occupiedSlots);
    destroyPacketCP(&pkt);
}

void testconstructPacketFromByteString(void)
{
    ControlPacket_t *pkt;
    size_t slots = 9, channels = 17;
    initCP(&pkt, slots, channels);

    int i;
    uint8_t bytes = getNumberBytes(slots * channels);
    uint8_t nodeID = 1;
    uint8_t *occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    for (i = 0; i < bytes; i++)
        occupiedSlots[i] = 0xff;
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
    uint8_t *byteStr = (uint8_t *)malloc(size * sizeof(uint8_t));

    byteStr[0] = nodeID;
    for (i = 0; i < bytes; i++)
        byteStr[i + 1] = occupiedSlots[i];
    byteStr[bytes + 2] = collisionSlots;
    byteStr[bytes + 3] = (collisionFrequency & 0xff000000) >> 24;
    byteStr[bytes + 4] = (collisionFrequency & 0x00ff0000) >> 16;
    byteStr[bytes + 5] = (collisionFrequency & 0x0000ff00) >> 8;
    byteStr[bytes + 6] = (collisionFrequency & 0x000000ff);
    byteStr[bytes + 7] = (netTime & 0xff000000) >> 24;
    byteStr[bytes + 8] = (netTime & 0x00ff0000) >> 16;
    byteStr[bytes + 9] = (netTime & 0x0000ff00) >> 8;
    byteStr[bytes + 10] = (netTime & 0x000000ff);
    byteStr[bytes + 11] = hopCount;
    byteStr[bytes + 12] = ack;

    constructPacketFromByteStringCP(pkt, byteStr, size);

    assert(pkt->nodeID == byteStr[0]);
    for (i = 0; i < bytes; i++)
        assert(pkt->occupiedSlots[i] == byteStr[i + 1]);
    assert(pkt->collisionSlot == byteStr[bytes + 2]);
    assert(pkt->collisionFrequency == collisionFrequency);
    assert(pkt->hopCount == hopCount);
    assert(pkt->networkTime == netTime);
    assert(pkt->ack == ack);

    free(byteStr);
    free(occupiedSlots);
    destroyPacketCP(&pkt);
}

void executeTestsCP(void)
{
    srand(time(NULL));

    printf("Testing init function.\n");
    testInit();
    printf("Test passed\n");

    printf("Testing destroyPacket function.\n");
    testDestroyPacket();
    printf("Test passed.\n");

    printf("Testing createPacket function.\n");
    testCreatePacket();
    printf("Test passed.\n");

    printf("Testing clearPacket function.\n");
    testClearPacket();
    printf("Test passed.\n");
    
    printf("Testing setNodeID function.\n");
    testsetNodeID();
    printf("Test passed.\n");

    printf("Testing getNodeID function.\n");
    testgetNodeID();
    printf("Test passed.\n");

    printf("Testing setSlot function.\n");
    testsetCollisionSlot();
    printf("Test passed.\n");

    printf("Testing clearSlot function.\n");
    testgetCollisionSlot();
    printf("Test passed.\n");

    printf("Testing setOccupiedSlots function.\n");
    testsetOccupiedSlots();
    printf("Test passed.\n");

    printf("Testing getOccupiedSlots function.\n");
    testgetOccupiedSlots();
    printf("Test passed.\n");

    printf("Testing setCollisionFrequency function.\n");
    testsetCollisionFrequency();
    printf("Test passed.\n");

    printf("Testing getCollisionFrequency function.\n");
    testgetCollisionFrequency();
    printf("Test passed.\n");

    printf("Testing setHopCount function.\n");
    testsetHopCount();
    printf("Test passed.\n");

    printf("Testing getHopCount function.\n");
    testgetHopCount();
    printf("Test passed.\n");

    printf("Testing setNetworkTimeCP function.\n");
    testsetNetworkTimeCP();
    printf("Test passed.\n");

    printf("Testing getNetworkTimeCP function.\n");
    testgetNetworkTimeCP();
    printf("Test passed.\n");

    printf("Testing setACK function.\n");
    testsetACK();
    printf("Test passed.\n");

    printf("Testing getACK function.\n");
    testgetACK();
    printf("Test passed.\n");

    printf("Testing getPacketByteString function.\n");
    testgetByteString();
    printf("Test passed.\n");

    printf("Testing constructPacketFromByteString function.\n");
    testconstructPacketFromByteString();
    printf("Test passed.\n");
}
#endif