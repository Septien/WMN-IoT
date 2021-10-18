#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CFPacket.h"

void cfpacket_init(CFPacket_t DOUBLE_POINTER pkt)
{
#ifdef __LINUX__
    CFPacket_t *pktA = (CFPacket_t *)malloc(sizeof(CFPacket_t));
    memset(pktA, 0, sizeof(*pktA));
    *pkt = pktA;
#endif
#ifdef __RIOT__
    memset(pkt, 0, sizeof(CFPacket_t));
#endif
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

void cfpacket_create(CFPacket_t *pkt, uint8_t nodeID, uint8_t destinationID, uint32_t frequency)
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

void cfpacket_set_nodeid(CFPacket_t *pkt, uint8_t nodeID)
{
    assert(pkt != NULL);

    pkt->nodeID = nodeID;
}

uint8_t cfpacket_get_nodeid(CFPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->nodeID;
}

void cfpacket_set_destinationid(CFPacket_t *pkt, uint8_t destinationID)
{
    assert(pkt != NULL);
    
    pkt->destinationID = destinationID;
}

uint8_t cfpacket_get_destinationid(CFPacket_t *pkt)
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

void cfpacket_get_packet_byte_string(CFPacket_t *pkt, ARRAY* byteString, size_t *size)
{
    assert(pkt != NULL);

    size_t sizeA = sizeof(pkt->nodeID) + sizeof(pkt->destinationID);
    sizeA += sizeof(pkt->frequency);
#ifdef __LINUX__
    uint8_t *byteStringA = (uint8_t *)malloc(sizeA * sizeof(uint8_t));
#endif

#ifdef __LINUX__
    byteStringA[0] = pkt->nodeID;
    byteStringA[1] = pkt->destinationID;
    byteStringA[2] = (pkt->frequency & 0xff000000 >> 24);
    byteStringA[3] = (pkt->frequency & 0x00ff0000 >> 16);
    byteStringA[4] = (pkt->frequency & 0x0000ff00 >> 8);
    byteStringA[5] = (pkt->frequency & 0x000000ff);

    *byteString = byteStringA;
#endif
#ifdef __RIOT__
    write_element(byteString, pkt->nodeID,                          0);
    write_element(byteString, pkt->destinationID,                   1);
    write_element(byteString, (pkt->frequency & 0xff000000 >> 24),  2);
    write_element(byteString, (pkt->frequency & 0x00ff0000 >> 16),  3);
    write_element(byteString, (pkt->frequency & 0x0000ff00 >> 8),   4);
    write_element(byteString, (pkt->frequency & 0x000000ff),        5);
#endif

    *size = sizeA;
}

void cfpacket_construct_packet_from_byte_string(CFPacket_t *pkt, ARRAY* byteString, size_t size)
{
    assert(pkt != NULL);
    assert(byteString != NULL);
#ifdef __LINUX__
    assert(*byteString != NULL);
#endif
    assert(size > 0);

    uint32_t frequency = 0;
#ifdef __LINUX__
    pkt->nodeID = *byteString[0];
    pkt->destinationID = *byteString[1];
    frequency |= *byteString[2] << 24;
    frequency |= *byteString[3] << 16;
    frequency |= *byteString[4] << 8;
    frequency |= *byteString[5];
#endif
#ifdef __RIOT__
    read_element(byteString, &pkt->nodeID, 0);
    read_element(byteString, &pkt->destinationID, 1);
    uint8_t element;
    read_element(byteString, &element, 2);
    frequency |= element << 24;
    read_element(byteString, &element, 3);
    frequency |= element << 16;
    read_element(byteString, &element, 4);
    frequency |= element << 8;
    read_element(byteString, &element, 5);
    frequency |= element;
#endif
    pkt->frequency = frequency;
}
