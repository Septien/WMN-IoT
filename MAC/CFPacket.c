#include "CFPacket.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void initCF(CFPacket_t **pkt)
{
    CFPacket_t *pktA = (CFPacket_t *)malloc(sizeof(CFPacket_t));
    *pkt = pktA;
}

void destroyPacketCF(CFPacket_t **pkt)
{
    assert(pkt != NULL);
    assert(*pkt != NULL);
    free((void *) *pkt);
    *pkt = NULL;
}

void createPacketCF(CFPacket_t *pkt, uint8_t nodeID, uint8_t destinationID)
{
    assert(pkt != NULL);
    assert(nodeID > 0 && destinationID > 0);

    pkt->nodeID = nodeID;
    pkt->destinationID = destinationID;
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

void getPacketByteStringCF(CFPacket_t *pkt, uint8_t **byteString, size_t *size)
{
    assert(pkt != NULL);

    size_t sizeA = sizeof(pkt->nodeID) + sizeof(pkt->destinationID);
    uint8_t *byteStringA = (uint8_t *)malloc(sizeA * sizeof(uint8_t));

    byteStringA[0] = pkt->nodeID;
    byteStringA[1] = pkt->destinationID;

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
}
