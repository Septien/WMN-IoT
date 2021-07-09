#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "../controlPacket.h"

void testInit()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    init(&pkt, slots, channels);

    assert(pkt != NULL);
    assert(pkt->occupiedSlots != NULL);
    assert(pkt->_nSlots == slots);
    assert(pkt->_nChannels == channels);

    destroyPacket(&pkt);
}

void testDestroyPacket()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    init(&pkt, slots, channels);

    destroyPacket(&pkt);

    assert(pkt == NULL);
}

void testCreatePacket(void)
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    init(&pkt, slots, channels);

    uint8_t nodeID = 1;
    uint8_t bytes = getNumberBytes(slots * channels);
    uint8_t *occupiedSlots = (uint8_t *)malloc(sizeof(uint8_t) * bytes);
    for (int i = 0; i < bytes; i++)
        occupiedSlots[i] = 0xf0;
    uint8_t collisionSlot = 10;
    uint32_t collisionFrequency = 915;
    uint8_t hopCount = 3;
    uint8_t ack = 0;

    createPacket(pkt, nodeID, occupiedSlots, collisionSlot, collisionFrequency, hopCount, ack);

    assert(pkt != NULL);
    assert(pkt->occupiedSlots != NULL && sizeof(pkt->occupiedSlots) == slots);
    for (int i = 0; i < bytes; i++)
        assert(pkt->occupiedSlots[i] == 0xf0);
    assert(pkt->collisionSlot == collisionSlot);
    assert(pkt->collisionFrequency == collisionFrequency);
    assert(pkt->hopCount == hopCount);
    assert(pkt->ack == ack);

    free(occupiedSlots);
    destroyPacket(&pkt);
}

void testClearPacket()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;;
    init(&pkt, slots, channels);

    uint8_t nodeID = 1;
    uint8_t *occupiedSlots = (uint8_t *)malloc(sizeof(uint8_t) * slots);
    for (int i = 0; i < slots; i++)
        occupiedSlots[i] = 0xf0;
    uint8_t collisionSlots = 0x01;
    uint32_t collisionFrequency = 915;
    uint8_t hopCount = 3;
    uint8_t ack = 1;

    createPacket(pkt, nodeID, occupiedSlots, collisionSlots, collisionFrequency, hopCount, ack);

    clearPackt(pkt);

    assert(pkt->nodeID == 0);
    assert(pkt->occupiedSlots == NULL);
    assert(pkt->collisionFrequency == 0);
    assert(pkt->collisionSlot == 0);
    assert(pkt->hopCount == 0);
    assert(pkt->ack == 0);
    assert(pkt->_nSlots == 0);

    free(occupiedSlots);
    destroyPacket(&pkt);
}

void testsetNodeID()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;;
    init(&pkt, slots, channels);

    for (uint8_t i = 0; i < 255; i++)
    {
        setNodeID(pkt, i);
        assert(pkt->nodeID == i);
    }

    destroyPacket(&pkt);
}

void testgetNodeID()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;;
    init(&pkt, slots, channels);

    uint8_t nodeid = 0;
    for (uint8_t i = 0; i < 255; i++)
    {
        setNodeID(pkt, i);
        nodeid = getNodeID(pkt);
        assert(nodeid == i);
    }

    destroyPacket(&pkt);
}

void testsetCollisionSlot()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;;
    init(&pkt, slots, channels);

    uint8_t slot = 10;
    setCollisionSlot(pkt, slot);

    assert(pkt->collisionSlot == slot);

    destroyPacket(&pkt);
}

void testgetCollisionSlot()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;;
    init(&pkt, slots, channels);

    uint8_t slot = 10;
    setCollisionSlot(pkt, slot);

    slot = getCollisionSlot(pkt);
    assert(slot == pkt->collisionSlot);

    destroyPacket(&pkt);
}

void testsetOccupiedSlots()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    init(&pkt, slots, channels);

    uint8_t bytes = getNumberBytes(slots * channels);
    uint8_t *occSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    uint8_t i;
    for (i = 0; i < bytes; i++)
        occSlots[i] = 0xff;
    setOccupiedSlots(pkt, occSlots);

    for (i = 0; i < bytes; i++)
        assert(pkt->occupiedSlots[i] == occSlots[i]);

    free(occSlots);
    destroyPacket(&pkt);
}

void testgetOccupiedSlots()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    init(&pkt, slots, channels);

    uint8_t bytes = getNumberBytes(slots * channels);
    uint8_t *occSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    uint8_t i;
    for (i = 0; i < bytes; i++)
        occSlots[i] = 0xff;
    setOccupiedSlots(pkt, occSlots);

    uint8_t *occSlots2 = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    getOccupiedSlots(pkt, occSlots2);
    for (uint8_t i = 0; i < bytes; i++)
        assert(occSlots2[i] == pkt->occupiedSlots[i]);

    free(occSlots);
    free(occSlots2);
    destroyPacket(&pkt);
}

void testsetCollisionFrequency()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    init(&pkt, slots, channels);

    uint32_t freq = 915;
    setCollisionFrequency(pkt, freq);
    assert(pkt->collisionFrequency == freq);

    destroyPacket(&pkt);
}

void testgetCollisionFrequency()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    init(&pkt, slots, channels);

    uint32_t freq = 915;
    setCollisionFrequency(pkt, freq);
    
    uint32_t freqr = getColissionFrequency(pkt);
    assert(freqr == pkt->collisionFrequency);
    assert(freqr == freq);

    destroyPacket(&pkt);
}

void testsetHopCount()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    init(&pkt, slots, channels);

    uint8_t hops = 8;
    setHopCount(pkt, hops);
    assert(pkt->hopCount == hops);

    destroyPacket(&pkt);
}

void testgetHopCount()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    init(&pkt, slots, channels);

    uint8_t hops = 8;
    setHopCount(pkt, hops);
    
    uint8_t hops2 = getHopCount(pkt);
    assert(hops2 == pkt->hopCount);
    assert(hops2 == hops);

    destroyPacket(&pkt);
}

void testsetACK()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    init(&pkt, slots, channels);

    uint8_t ack = 1;
    setACK(pkt, ack);
    assert(pkt->ack == ack);

    destroyPacket(&pkt);
}

void testgetACK()
{
    ControlPacket_t *pkt;
    size_t slots = 8, channels = 17;
    init(&pkt, slots, channels);

    uint8_t ack = 1;
    setACK(pkt, ack);
    
    uint8_t ack2 = getACK(pkt);
    assert(ack2 == pkt->ack);
    assert(ack2 == ack);

    destroyPacket(&pkt);
}

void testgetByteString()
{
    ControlPacket_t *pkt;
    size_t slots = 9, channels = 17;
    init(&pkt, slots, channels);

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
    uint8_t ack = 0;

    createPacket(pkt, nodeID, occupiedSlots, collisionSlots, collisionFrequency, hopCount, ack);

    freq[0] = (collisionFrequency & 0xff000000) >> 24;
    freq[1] = (collisionFrequency & 0x00ff0000) >> 16;
    freq[2] = (collisionFrequency & 0x0000ff00) >> 8;
    freq[3] = (collisionFrequency & 0x000000ff);
    uint8_t *byteStr;
    getPacketByteString(pkt, &byteStr);
    assert(byteStr[0] == nodeID);
    for (i = 0; i < bytes; i++)
        assert(byteStr[i+1] == occupiedSlots[i]);
    assert(byteStr[bytes+2] == collisionSlots);
    assert(byteStr[bytes+3] == freq[0]);
    assert(byteStr[bytes+4] == freq[1]);
    assert(byteStr[bytes+5] == freq[2]);
    assert(byteStr[bytes+6] == freq[3]);
    assert(byteStr[bytes+7] == hopCount);
    assert(byteStr[bytes+8] == ack);

    free(occupiedSlots);
    destroyPacket(&pkt);
}

void testconstructPacketFromByteString()
{
    ControlPacket_t *pkt;
    size_t slots = 9, channels = 17;
    init(&pkt, slots, channels);

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
    uint8_t ack = 0;

    size_t size = sizeof(nodeID);
    size += (size_t)bytes;
    size += sizeof(collisionSlots);
    size += sizeof(collisionFrequency);
    size += sizeof(hopCount);
    size += sizeof(ack);
    uint8_t *byteStr = (uint8_t *)malloc(size * sizeof(uint8_t));

    byteStr[0] = nodeID;
    for (i = 0; i < bytes; i++)
        byteStr[i + 1] = occupiedSlots[i];
    byteStr[bytes + 2] = collisionSlots;
    byteStr[bytes + 3] = (collisionFrequency & 0xff000000) >> 24;
    byteStr[bytes + 4] = (collisionFrequency & 0x00ff0000) >> 16;
    byteStr[bytes + 5] = (collisionFrequency & 0x0000ff00) >> 8;
    byteStr[bytes + 6] = (collisionFrequency & 0x000000ff);
    byteStr[bytes + 7] = hopCount;
    byteStr[bytes + 8] = ack;

    consturctPacketFromByteString(pkt, byteStr, size);

    assert(pkt->nodeID == byteStr[0]);
    for (i = 0; i < bytes; i++)
        assert(pkt->occupiedSlots[i] == byteStr[i + 1]);
    assert(pkt->collisionSlot == byteStr[bytes + 2]);
    assert(pkt->collisionFrequency == collisionFrequency);
    assert(pkt->hopCount == byteStr[bytes + 7]);
    assert(pkt->ack == byteStr[bytes + 8]);
}

void executeTests()
{
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

    printf("Testing setACK function.\n");
    testsetACK();
    printf("Test passed.\n");

    printf("Testing getACK function.\n");
    testgetACK();
    printf("Test passed.\n");

    printf("Testing getPacketBytString function.\n");
    testgetByteString();
    printf("Test passed.\n");

    printf("Testing constructPacketFromByteString function.\n");
    testconstructPacketFromByteString();
    printf("Test passed.\n");
}