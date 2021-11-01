#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CFPacket.h"

void cfpacket_init(CFPacket_t DOUBLE_POINTER pkt)
{
#ifdef __LINUX__
    (*pkt) = (CFPacket_t *)malloc(sizeof(CFPacket_t));
#endif
    memset(SINGLE_POINTER pkt, 0, sizeof(CFPacket_t));

}

void cfpacket_destroy(CFPacket_t DOUBLE_POINTER pkt)
{
    assert(pkt != NULL);
#ifdef __LINUX__
    assert(*pkt != NULL);
    free((void *) *pkt);
    *pkt = NULL;
#endif
#ifdef __RIOT__
    memset(pkt, 0, sizeof(CFPacket_t));
#endif
}

void cfpacket_create(CFPacket_t *pkt, uint16_t nodeID, uint16_t destinationID, uint32_t frequency)
{
    assert(pkt != NULL);

    pkt->nodeID = nodeID;
    pkt->destinationID = destinationID;
    pkt->frequency = frequency;
}

void cfpacket_clear(CFPacket_t *pkt)
{
    assert(pkt != NULL);

    pkt->nodeID = 0;
    pkt->destinationID = 0;
}

void cfpacket_set_nodeid(CFPacket_t *pkt, uint16_t nodeID)
{
    assert(pkt != NULL);

    pkt->nodeID = nodeID;
}

uint16_t cfpacket_get_nodeid(CFPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->nodeID;
}

void cfpacket_set_destinationid(CFPacket_t *pkt, uint16_t destinationID)
{
    assert(pkt != NULL);
    
    pkt->destinationID = destinationID;
}

uint16_t cfpacket_get_destinationid(CFPacket_t *pkt)
{
    assert(pkt != NULL);
    
    return pkt->destinationID;
}

void cfpacket_set_frequency(CFPacket_t *pkt, uint32_t frequency)
{
    assert(pkt != NULL);
    assert(frequency > 0);

    pkt->frequency = frequency;
}

uint32_t cfpacket_get_frequency(CFPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->frequency;
}

void cfpacket_get_packet_bytestring(CFPacket_t *pkt, ARRAY* byteString, size_t *size)
{
    assert(pkt != NULL);

    size_t sizeA = sizeof(pkt->nodeID) + sizeof(pkt->destinationID);
    sizeA += sizeof(pkt->frequency);
#ifdef __LINUX__
    (*byteString) = (uint8_t *)malloc(sizeA * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(byteString, sizeA);
#endif
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->nodeID & 0xff00) >> 8,          0);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->nodeID & 0x00ff),               1);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destinationID & 0xff00) >> 8,   2);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destinationID & 0x00ff),        3);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->frequency & 0xff000000 >> 24),  4);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->frequency & 0x00ff0000 >> 16),  5);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->frequency & 0x0000ff00 >> 8),   6);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->frequency & 0x000000ff),        7);

    *size = sizeA;
}

void cfpacket_construct_packet_from_bytestring(CFPacket_t *pkt, ARRAY* byteString, size_t size)
{
    assert(pkt != NULL);
    assert(byteString != NULL);
#ifdef __LINUX__
    assert(*byteString != NULL);
#endif
#ifdef __RIOT__
    assert(byteString->size > 0);
#endif
    assert(size > 0);

    pkt->nodeID = (READ_ARRAY(SINGLE_POINTER byteString, 0) << 8) | (READ_ARRAY(SINGLE_POINTER byteString, 1));
    pkt->destinationID = (READ_ARRAY(SINGLE_POINTER byteString, 2) << 8) | READ_ARRAY(SINGLE_POINTER byteString, 3);
    uint32_t frequency = 0;
    frequency |= READ_ARRAY(SINGLE_POINTER byteString, 4) << 24;
    frequency |= READ_ARRAY(SINGLE_POINTER byteString, 5) << 16;
    frequency |= READ_ARRAY(SINGLE_POINTER byteString, 6) << 8;
    frequency |= READ_ARRAY(SINGLE_POINTER byteString, 7);
    pkt->frequency = frequency;
}
