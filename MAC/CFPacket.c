#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CFPacket.h"

void initCF(
#ifdef __LINUX__
    CFPacket_t **pkt
#endif
#ifdef __RIOT__
    CFPacket_t *pkt
#endif
)
{
#ifdef __LINUX__
    CFPacket_t *pktA = (CFPacket_t *)malloc(sizeof(CFPacket_t));
    memset(pktA, 0, sizeof(*pktA));
    *pkt = pktA;
#endif
#ifdef __RIOT__
    return;
#endif
}

void destroyPacketCF(CFPacket_t **pkt)
{
    assert(pkt != NULL);
    assert(*pkt != NULL);
    free((void *) *pkt);
    *pkt = NULL;
}

void createPacketCF(CFPacket_t *pkt, uint8_t nodeID, uint8_t destinationID, uint32_t frequency)
{
    assert(pkt != NULL);
    assert(nodeID > 0 && destinationID >= 0);

    pkt->nodeID = nodeID;
    pkt->destinationID = destinationID;
    pkt->frequency = frequency;
}

void clearPacketCF(CFPacket_t *pkt)
{
    assert(pkt != NULL);

    pkt->nodeID = 0;
    pkt->destinationID = 0;
}

void setNodeIDCF(CFPacket_t *pkt, uint8_t nodeID)
{
    assert(pkt != NULL);

    pkt->nodeID = nodeID;
}

uint8_t getNodeIDCF(CFPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->nodeID;
}

void setDestinationIDCF(CFPacket_t *pkt, uint8_t destinationID)
{
    assert(pkt != NULL);
    
    pkt->destinationID = destinationID;
}

uint8_t getDestinationIDCF(CFPacket_t *pkt)
{
    assert(pkt != NULL);
    
    return pkt->destinationID;
}

void setFrequencyCF(CFPacket_t *pkt, uint32_t frequency)
{
    assert(pkt != NULL);
    assert(frequency > 0);

    pkt->frequency = frequency;
}

uint32_t getFrequencyCF(CFPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->frequency;
}

void getPacketByteStringCF(CFPacket_t *pkt, uint8_t **byteString, size_t *size)
{
    assert(pkt != NULL);

    size_t sizeA = sizeof(pkt->nodeID) + sizeof(pkt->destinationID);
    sizeA += sizeof(pkt->frequency);
    uint8_t *byteStringA = (uint8_t *)malloc(sizeA * sizeof(uint8_t));

    byteStringA[0] = pkt->nodeID;
    byteStringA[1] = pkt->destinationID;
    byteStringA[2] = (pkt->frequency & 0xff000000 >> 24);
    byteStringA[3] = (pkt->frequency & 0x00ff0000 >> 16);
    byteStringA[4] = (pkt->frequency & 0x0000ff00 >> 8);
    byteStringA[5] = (pkt->frequency & 0x000000ff);

    *byteString = byteStringA;
    *size = sizeA;
}

void constructPktFromByteStringCF(CFPacket_t *pkt, uint8_t *byteString, size_t size)
{
    assert(pkt != NULL);
    assert(byteString != NULL);
    assert(size > 0);

    pkt->nodeID = byteString[0];
    pkt->destinationID = byteString[1];
    uint32_t frequency = 0;
    frequency |= byteString[2] << 24;
    frequency |= byteString[3] << 16;
    frequency |= byteString[4] << 8;
    frequency |= byteString[5];
    pkt->frequency = frequency;
}
