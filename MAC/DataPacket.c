#include "DataPacket.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initDP(DataPacket_t **pkt)
{
    DataPacket_t *pktA;
    pktA = (DataPacket_t *)malloc(sizeof(DataPacket_t));
    pktA->data = NULL;  // Initialize until data is loaded
    *pkt = pktA;
}

void destroyPacketDP(DataPacket_t **pkt)
{
    assert(pkt != NULL);
    assert(*pkt != NULL);

    if ((*pkt)->data != NULL)
        free((*pkt)->data);

    free(*pkt);
    *pkt = NULL;
}

void createPacketDP(DataPacket_t *pkt, bool isFragment, uint8_t totalFragments, uint8_t fragmentNumber, void **data, size_t size)
{
    assert(pkt != NULL);

    pkt->isFragment = isFragment;
    pkt->totalFragments = totalFragments;
    pkt->fragmentNumber = fragmentNumber;
    pkt->dataLength = size;
    if (size > 0)
    {
        uint8_t *dataA = ((uint8_t *)*data);
        pkt->data = (uint8_t *)malloc(pkt->dataLength * sizeof(uint8_t));
        for (int i = 0; i < pkt->dataLength; i++)
            pkt->data[i] = dataA[i];
    }
    else
        pkt->data = NULL;
}

void clearPacketDP(DataPacket_t *pkt)
{
    assert(pkt != NULL);

    pkt->isFragment = false;
    pkt->totalFragments = 0;
    pkt->totalFragments = 0;
    pkt->fragmentNumber = 0;
    free(pkt->data);
    pkt->data = NULL;
    pkt->dataLength = 0;
}

void setIsFragmentDP(DataPacket_t *pkt, bool isFragment)
{
    assert(pkt != NULL);

    pkt->isFragment = isFragment;
}

bool isFragmentDP(DataPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->isFragment;
}

void setTotalFragmentsDP(DataPacket_t *pkt, uint8_t totalFragments)
{
    assert(pkt != NULL);

    pkt->totalFragments = totalFragments;
}

uint8_t getTotalFragmentsDP(DataPacket_t *pkt)
{
    assert(pkt != NULL);
    return pkt->totalFragments;
}

void setFragmentNumberDP(DataPacket_t *pkt, uint8_t fragmentNumber)
{
    assert(pkt != NULL);
    assert(pkt->totalFragments >= fragmentNumber);

    pkt->fragmentNumber = fragmentNumber;
}

uint8_t getFragmentNumberDP(DataPacket_t *pkt)
{
    return pkt->fragmentNumber;
}

void setPacketDataDP(DataPacket_t *pkt, void **data, uint8_t size)
{
    assert(pkt != NULL);
    assert(data != NULL);
    assert(size > 0 && size < 250);

    // if packet already contains data, delete it
    if (pkt->data != NULL)
    {
        free(pkt->data);
        pkt->data = NULL;
    }

    uint8_t *dataA = ((uint8_t *)*data);
    pkt->dataLength = size;
    pkt->data = (uint8_t *)malloc(pkt->dataLength * sizeof(uint8_t));
    for (int i = 0; i < size; i++)
        pkt->data[i] = dataA[i];
}

void deleteDataDP(DataPacket_t *pkt)
{
    assert(pkt != NULL);
    assert(pkt->dataLength > 0);
    assert(pkt->data != NULL);

    pkt->dataLength = 0;
    free(pkt->data);
    pkt->data = NULL;
}

void clearDataDP(DataPacket_t *pkt)
{
    assert(pkt != NULL);
    assert(pkt->data != NULL);
    assert(pkt->dataLength > 0);

    memset(pkt->data, 0, pkt->dataLength);
}

void getPacketDataDP(DataPacket_t *pkt, void **data, uint8_t *size)
{
    assert(pkt != NULL);

    uint8_t sizeA = pkt->dataLength;
    uint8_t *dataA = (uint8_t *)malloc(sizeA * sizeof(uint8_t));
    for (int i = 0; i < sizeA; i++)
        dataA[i] = pkt->data[i];

    *size = sizeA;
    *data = dataA;
}

uint8_t getDataLengthDP(DataPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->dataLength;
}

size_t getPacketLengthDP(DataPacket_t *pkt)
{
    assert(pkt != NULL);
    assert(pkt->data != NULL);
    assert(pkt->dataLength >= 0);

    size_t size = 0;
    size += sizeof(pkt->isFragment);
    size += sizeof(pkt->totalFragments);
    size += sizeof(pkt->fragmentNumber);
    size += sizeof(pkt->dataLength);
    size += pkt->dataLength;

    pkt->packetLength = size;
    return pkt->packetLength;
}

void getPacketByteStringDP(DataPacket_t *pkt, uint8_t **byteString, size_t *size)
{
    assert(pkt != NULL);

    size_t sizeA = getPacketLengthDP(pkt);
    uint8_t *byteStringA = (uint8_t *)malloc(sizeA * sizeof(uint8_t));

    byteStringA[0] = (uint8_t)pkt->isFragment;
    byteStringA[1] = pkt->totalFragments;
    byteStringA[2] = pkt->fragmentNumber;
    byteStringA[3] = pkt->dataLength;
    int i;
    for (i = 4; i < pkt->dataLength + 4; i++)
        byteStringA[i] = pkt->data[i - 4];

    *byteString = byteStringA;
    *size = sizeA;
}

void constructPktFromByteStringDP(DataPacket_t *pkt, uint8_t *byteString, size_t size)
{
    assert(pkt != NULL);
    assert(byteString != NULL);
    assert(size >= 6 && size <= 256);

    pkt->packetLength = size;
    pkt->isFragment = byteString[0];
    pkt->totalFragments = byteString[1];
    pkt->fragmentNumber = byteString[2];
    pkt->dataLength = byteString[3];
    pkt->data = (uint8_t *)malloc(pkt->dataLength * sizeof(uint8_t));
    int i;
    for (i = 0; i < pkt->dataLength; i++)
        pkt->data[i] = byteString[i + 4];
}
