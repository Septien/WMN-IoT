#include "DataPacket.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void datapacket_init(DataPacket_t DOUBLE_POINTER pkt)
{
#ifdef __LINUX__
    (*pkt) = (DataPacket_t *)malloc(sizeof(DataPacket_t));
    (*pkt)->data = NULL;  // Initialize until data is loaded
#endif
    memset(SINGLE_POINTER pkt, 0, sizeof(DataPacket_t));
}

void datapacket_destroy(DataPacket_t DOUBLE_POINTER pkt)
{
    assert(pkt != NULL);
#ifdef __LINUX__
    assert(*pkt != NULL);

    if ((*pkt)->data != NULL)
        free((*pkt)->data);

    free(*pkt);
    *pkt = NULL;
#endif
#ifdef __RIOT__
    if (pkt->data.size > 0)
        free_array(&pkt->data);
    memset(pkt, 0, sizeof(DataPacket_t));
#endif
}

void datapacket_create(DataPacket_t *pkt, bool isFragment, uint8_t totalFragments, uint8_t fragmentNumber, ARRAY* data, size_t size)
{
    assert(pkt != NULL);

    pkt->isFragment = isFragment;
    pkt->totalFragments = totalFragments;
    pkt->fragmentNumber = fragmentNumber;
    pkt->dataLength = size;
    if (size > 0 && data != NULL)
    {
#ifdef __LINUX__
        pkt->data = (uint8_t *)malloc(pkt->dataLength * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&pkt->data, pkt->dataLength);
#endif  
        for (int i = 0; i < pkt->dataLength; i++)
        {
            uint8_t element;
            element = READ_ARRAY(SINGLE_POINTER data, i);
            WRITE_ARRAY(REFERENCE pkt->data, element, i);
        }
    }
#ifdef __LINUX__
    else
        pkt->data = NULL;
#endif
}

void datapacket_clear(DataPacket_t *pkt)
{
    assert(pkt != NULL);

    pkt->isFragment = false;
    pkt->totalFragments = 0;
    pkt->fragmentNumber = 0;
#ifdef __LINUX__
    if (pkt->data != NULL)
    {
        free(pkt->data);
        pkt->data = NULL;
    }
#endif
#ifdef __RIOT__
    if (pkt->data.size > 0)
    {
        free_array(&pkt->data);
    }
#endif
    pkt->dataLength = 0;
}

void datapacket_set_isFragment(DataPacket_t *pkt, bool isFragment)
{
    assert(pkt != NULL);

    pkt->isFragment = isFragment;
}

bool datapacket_get_isFragment(DataPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->isFragment;
}

void datapacket_set_total_fragments(DataPacket_t *pkt, uint8_t totalFragments)
{
    assert(pkt != NULL);

    pkt->totalFragments = totalFragments;
}

uint8_t datapacket_get_total_fragments(DataPacket_t *pkt)
{
    assert(pkt != NULL);
    
    return pkt->totalFragments;
}

void datapacket_set_fragment_number(DataPacket_t *pkt, uint8_t fragmentNumber)
{
    assert(pkt != NULL);
    assert(pkt->totalFragments > fragmentNumber);

    pkt->fragmentNumber = fragmentNumber;
}

uint8_t datapacket_get_fragment_number(DataPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->fragmentNumber;
}

void datapacket_set_data(DataPacket_t *pkt, ARRAY* data, uint8_t size)
{
    assert(pkt != NULL);
    assert(data != NULL);
#ifdef __LINUX__
    assert(*data != NULL);
#endif
    assert(size > 0 && size < 250);

    // if packet already contains data, delete it
#ifdef __LINUX__
    if (pkt->data != NULL)
    {
        free(pkt->data);
        pkt->data = NULL;
    }
#endif
#ifdef __RIOT__
    if (pkt->data.size > 0)
    {
        free_array(&pkt->data);
    }
#endif

    pkt->dataLength = size;
#ifdef __LINUX__
    pkt->data = (uint8_t *)malloc(pkt->dataLength * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&pkt->data, pkt->dataLength);
#endif
    for (int i = 0; i < size; i++)
    {
        uint8_t element = READ_ARRAY(SINGLE_POINTER data, i);
        WRITE_ARRAY(REFERENCE pkt->data, element, i);
    }
}

void datapacket_delete_data(DataPacket_t *pkt)
{
    assert(pkt != NULL);
    assert(pkt->dataLength > 0);
#ifdef __LINUX__
    assert(pkt->data != NULL);
#endif
#ifdef __RIOT__
    assert(pkt->data.size > 0);
#endif

    pkt->dataLength = 0;
#ifdef __LINUX__
    free(pkt->data);
    pkt->data = NULL;
#endif
#ifdef __RIOT__
    free_array(&pkt->data);
#endif
}

void datapacket_clear_data(DataPacket_t *pkt)
{
    assert(pkt != NULL);
#ifdef __LINUX__
    assert(pkt->data != NULL);
#endif
#ifdef __RIOT__
    assert(pkt->data.size > 0);
#endif
    assert(pkt->dataLength > 0);

    for (int i = 0; i <pkt->dataLength; i++)
        WRITE_ARRAY(REFERENCE pkt->data, 0, i);
}

void datapacket_get_data(DataPacket_t *pkt, ARRAY* data, uint8_t *size)
{
    assert(pkt != NULL);

    uint8_t sizeA = pkt->dataLength;
    ARRAY dataA;
#ifdef __LINUX__
    dataA = (uint8_t *)malloc(sizeA * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&dataA, sizeA);
#endif
    for (int i = 0; i < sizeA; i++)
    {
        uint8_t element;
        element = READ_ARRAY(REFERENCE pkt->data, i);
        WRITE_ARRAY(REFERENCE dataA, element, i);
    }

    *size = sizeA;
    *data = dataA;
}

uint8_t datapacket_get_data_length(DataPacket_t *pkt)
{
    assert(pkt != NULL);

    return pkt->dataLength;
}

size_t datapacket_get_packet_length(DataPacket_t *pkt)
{
    assert(pkt != NULL);
#ifdef __LINUX__
    assert(pkt->data != NULL);
#endif

    size_t size = 0;
    size += sizeof(pkt->isFragment);
    size += sizeof(pkt->totalFragments);
    size += sizeof(pkt->fragmentNumber);
    size += sizeof(pkt->dataLength);
    size += pkt->dataLength;

    pkt->packetLength = size;
    return pkt->packetLength;
}

void datapacket_get_packet_bytestring(DataPacket_t *pkt, ARRAY* byteString, size_t *size)
{
    assert(pkt != NULL);

    size_t sizeA = datapacket_get_packet_length(pkt);
    ARRAY byteStringA;
#ifdef __LINUX__
    byteStringA = (uint8_t *)malloc(sizeA * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteStringA, sizeA);
#endif

    WRITE_ARRAY(REFERENCE byteStringA, (uint8_t)pkt->isFragment, 0);
    WRITE_ARRAY(REFERENCE byteStringA, pkt->totalFragments, 1);
    WRITE_ARRAY(REFERENCE byteStringA, pkt->fragmentNumber, 2);
    WRITE_ARRAY(REFERENCE byteStringA, pkt->dataLength, 3);
    int i;
    for (i = 4; i < pkt->dataLength + 4; i++)
    {
        uint8_t element;
        element = READ_ARRAY(REFERENCE pkt->data, i - 4);
        WRITE_ARRAY(REFERENCE byteStringA, element, i);
    }

    *byteString = byteStringA;
    *size = sizeA;
}

void datapacket_construct_from_bytestring(DataPacket_t *pkt, ARRAY* byteString, size_t size)
{
    assert(pkt != NULL);
    assert(byteString != NULL);
    assert(size >= 6 && size <= 256);

    pkt->packetLength = size;
    pkt->isFragment = READ_ARRAY(SINGLE_POINTER byteString, 0);
    pkt->totalFragments = READ_ARRAY(SINGLE_POINTER byteString, 1);
    pkt->fragmentNumber = READ_ARRAY(SINGLE_POINTER byteString, 2);;
    pkt->dataLength = READ_ARRAY(SINGLE_POINTER byteString, 3);
#ifdef __LINUX__
    pkt->data = (uint8_t *)malloc(pkt->dataLength * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&pkt->data, pkt->dataLength);
#endif
    int i;
    for (i = 0; i < pkt->dataLength; i++)
    {
        uint8_t element;
        element = READ_ARRAY(SINGLE_POINTER byteString, i + 4);
        WRITE_ARRAY(REFERENCE pkt->data, element, i);
    }
}
