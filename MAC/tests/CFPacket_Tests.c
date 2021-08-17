#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#include "../CFPacket.h"

void testinitCF()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    assert(cfpkt != NULL);

    destroyPacketCF(&cfpkt);
}

void testdestroyPacketCF()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    destroyPacketCF(&cfpkt);
    assert(cfpkt == NULL);
}

void testcreatePacketCF()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    uint8_t nodeID = (uint8_t)rand();
    uint8_t destinationID = (uint8_t)rand();
    uint32_t frequency = (uint32_t)rand();

    createPacketCF(cfpkt, nodeID, destinationID, frequency);

    assert(cfpkt->nodeID == nodeID);
    assert(cfpkt->destinationID == destinationID);
    assert(cfpkt->frequency == frequency);

    destroyPacketCF(&cfpkt);
}

void testclearPacketCF()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    uint8_t nodeID = (uint8_t)rand();
    uint8_t destinationID = (uint8_t)rand();
    uint32_t netTime = (uint32_t)rand();

    createPacketCF(cfpkt, nodeID, destinationID, netTime);

    clearPacketCF(cfpkt);

    assert(cfpkt->nodeID == 0);
    assert(cfpkt->destinationID == 0);

    destroyPacketCF(&cfpkt);
}

void testsetNodeIDCF()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    uint8_t nodeID = rand();
    setNodeIDCF(cfpkt, nodeID);
    assert(cfpkt->nodeID == nodeID);

    destroyPacketCF(&cfpkt);
}

void testgetNodeIDCF()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    uint8_t nodeID = rand();
    setNodeIDCF(cfpkt, nodeID);
    
    uint8_t nodeIDR;
    nodeIDR = getNodeIDCF(cfpkt);
    assert(nodeIDR == cfpkt->nodeID);

    destroyPacketCF(&cfpkt);
}

void testsetDestinationIDCF()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    srand(time(NULL));
    uint8_t destinationID = (uint8_t)rand();
    setDestinationIDCF(cfpkt, destinationID);
    assert(cfpkt->destinationID == destinationID);

    destroyPacketCF(&cfpkt);
}

void testgetDestinationIDCF()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    uint8_t destinationID = (uint8_t)rand();
    setDestinationIDCF(cfpkt, destinationID);

    uint8_t dIDF = getDestinationIDCF(cfpkt);
    assert(dIDF == destinationID);
    assert(dIDF == cfpkt->destinationID);

    destroyPacketCF(&cfpkt);
}

void testsetFrequencyCF()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    uint32_t frequency = rand();
    setFrequencyCF(cfpkt, frequency);
    assert(cfpkt->frequency == frequency);

    destroyPacketCF(&cfpkt);
}

void testgetFrequencyCF()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    uint32_t frequency = rand();
    setFrequencyCF(cfpkt, frequency);
    
    uint32_t freqA;
    freqA = getFrequencyCF(cfpkt);
    assert(freqA == frequency);

    destroyPacketCF(&cfpkt);
}

void testgetPacketByteString()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    uint8_t nodeID = (uint8_t)rand();
    uint8_t destinationID = (uint8_t)rand();
    uint32_t frequency = (uint32_t)rand();
    createPacketCF(cfpkt, nodeID, destinationID, frequency);

    size_t size = sizeof(nodeID) + sizeof(destinationID);
    size += sizeof(frequency);

    uint8_t *byteString;
    size_t size2;
    getPacketByteStringCF(cfpkt, &byteString, &size2);
    assert(byteString != NULL);
    assert(byteString[0] == nodeID);
    assert(byteString[1] == destinationID);
    assert(byteString[2] == (frequency & 0xff000000 >> 24));
    assert(byteString[3] == (frequency & 0x00ff0000 >> 16));
    assert(byteString[4] == (frequency & 0x0000ff00 >> 8));
    assert(byteString[5] == (frequency & 0x000000ff));
    assert(size2 == size);

    free(byteString);
    destroyPacketCF(&cfpkt);
}

void testconstructPktFromByteString()
{
    CFPacket_t *cfpkt;
    initCF(&cfpkt);

    size_t size = 2 * sizeof(uint8_t);
    size += sizeof(uint32_t);
    printf("%ld\n", size);
    uint8_t *byteString = (uint8_t *)malloc(size * sizeof(uint8_t));
    uint32_t frequency = 915000000;//(uint32_t)rand();
    byteString[0] = (uint8_t)rand();
    byteString[1] = (uint8_t)rand();
    byteString[2] = ((frequency & 0xff000000) >> 24);
    byteString[3] = ((frequency & 0x00ff0000) >> 16);
    byteString[4] = ((frequency & 0x0000ff00) >> 8);
    byteString[5] = (frequency & 0x000000ff);

    constructPktFromByteStringCF(cfpkt, byteString, size);
    assert(cfpkt->nodeID == byteString[0]);
    assert(cfpkt->destinationID == byteString[1]);
    printf("%d\n", cfpkt->frequency);
    assert(cfpkt->frequency == frequency);

    free(byteString);
    destroyPacketCF(&cfpkt);
}

void executeTestsCF()
{
    srand(time(NULL));
    printf("Testing initCF function.\n");
    testinitCF();
    printf("Test passed.\n");

    printf("Testing destroyPacketCF function.\n");
    testdestroyPacketCF();
    printf("Test passed.\n");

    printf("Testing createPacketCF function.\n");
    testcreatePacketCF();
    printf("Test passed.\n");

    printf("Testing clearPacketCF function.\n");
    testclearPacketCF();
    printf("Test passed.\n");

    printf("Testing setNodeIDCF function.\n");
    testsetNodeIDCF();
    printf("Test passed.\n");
    
    printf("Testing getNodeIDCF function.\n");
    testgetNodeIDCF();
    printf("Test passed.\n");

    printf("Testing setDestinationIDCF function.\n");
    testsetDestinationIDCF();
    printf("Test passed.\n");

    printf("Testing getDestinationIDCF function.\n"),
    testgetDestinationIDCF();
    printf("Test passed.\n");

    printf("Testing setFrequencyCF function.\n");
    testsetFrequencyCF();
    printf("Test passed.\n");

    printf("Testing getFrequencyCF function.\n");
    testgetFrequencyCF();
    printf("Test passed.\n");

    printf("Testing getPacketByteString function.\n");
    testgetPacketByteString();
    printf("Test passed.\n");

    printf("Testing constructPktFromByteString function.\n");
    testconstructPktFromByteString();
    printf("Test passed.\n");
}
