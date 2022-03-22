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
    (*pkt)->data = NULL;
    free((*pkt));
    *pkt = NULL;
#endif
#ifdef __RIOT__
    if (pkt->data.size > 0)
        free_array(&pkt->data);
#endif
    return;
}

void datapacket_create(DataPacket_t *pkt, uint16_t destination_id, ARRAY* data, size_t size)
{
    assert(pkt != NULL);
    assert(size > 0);
    assert(data != NULL);
#ifdef __LINUX__
    assert(*data != NULL);
#endif
    pkt->destination_id = destination_id;
    pkt->size = size;
#ifdef __LINUX__
    pkt->data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&pkt->data, size);
#endif
    for (uint i = 0; i < size; i++)
    {
        uint8_t e = READ_ARRAY((SINGLE_POINTER data), i);
        WRITE_ARRAY(REFERENCE pkt->data, e, i);
    }
}

void datapacket_clear(DataPacket_t *pkt)
{
    assert(pkt != NULL);

    pkt->destination_id = 0;
    pkt->size = 0;
#ifdef __LINUX__
    if (pkt->data != NULL)
        free(pkt->data);
    pkt->data = NULL;
#endif
#ifdef __RIOT__
    if (pkt->data.size > 0)
        free_array(&pkt->data);
#endif
}

void datapacket_get_packet_bytestring(DataPacket_t *pkt, ARRAY* byteString, size_t *size)
{
    assert(pkt != NULL);
    (void) pkt;
    (void) byteString;
    (void) size;

    size_t sizeA = pkt->size + sizeof(uint16_t);
    ARRAY byteStringA;
#ifdef __LINUX__
    byteStringA = (uint8_t *)malloc(sizeA * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteStringA, sizeA);
#endif

    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id & 0xff00) >> 8, 0);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id & 0x00ff), 1);
    for (uint i = 0; i < pkt->size; i++)
    {
        uint8_t element;
        element = READ_ARRAY(REFERENCE pkt->data, i);
        WRITE_ARRAY(REFERENCE byteStringA, element, i + 2);
    }

    *byteString = byteStringA;
    *size = sizeA;
}

void datapacket_construct_from_bytestring(DataPacket_t *pkt, ARRAY* byteString, size_t size)
{
    assert(pkt != NULL);
    assert(byteString != NULL);
    assert(size >= 6 && size <= 256);

    pkt->size = size;
    pkt->destination_id = 0;
    pkt->destination_id |= ((uint16_t)READ_ARRAY(SINGLE_POINTER byteString, 0)) << 8;
    pkt->destination_id |= ((uint16_t)READ_ARRAY(SINGLE_POINTER byteString, 1));
#ifdef __LINUX__
    if (pkt->data != NULL)
        free(pkt->data);
    pkt->data = (uint8_t *)malloc(pkt->size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    if (pkt->data.size > 0)
        free_array(&pkt->data);
    create_array(&pkt->data, pkt->size);
#endif
    for (uint i = 0; i < pkt->size; i++)
    {
        uint8_t element = READ_ARRAY(SINGLE_POINTER byteString, i + 2);
        WRITE_ARRAY(REFERENCE pkt->data, element, i);
    }
}
