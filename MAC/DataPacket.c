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
    (SINGLE_POINTER pkt)->type = -1;
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
    pkt->type = -1;
#endif
    return;
}

void datapacket_create(DataPacket_t *pkt, int8_t type, uint16_t destination_id, ARRAY* data, uint8_t size)
{
    assert(pkt != NULL);
    assert(type >= 0 && type < 10);      // Only accept type 0, 1, or 2. -1 is for when no data is stored on the packet.
    assert(size > 0);
    assert(data != NULL);
#ifdef __LINUX__
    assert(*data != NULL);
#endif
    pkt->destination_id = destination_id;
    pkt->type = type;
    // The data size should be the same always
    pkt->size = size;
#ifdef __LINUX__
    pkt->data = (uint8_t *)malloc(pkt->size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&pkt->data, pkt->size);
#endif
    uint i = 0;
    for (i = 0; i < size; i++)
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
    pkt->type = -1;
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

void datapacket_get_packet_bytestring(DataPacket_t *pkt, ARRAY* byteString)
{
    assert(pkt != NULL);

    size_t sizeA = PACKET_SIZE_MAC;
    ARRAY byteStringA;
#ifdef __LINUX__
    byteStringA = (uint8_t *)malloc(sizeA * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteStringA, sizeA);
#endif

    WRITE_ARRAY(REFERENCE byteStringA, pkt->type, 0);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id & 0xff00) >> 8, 1);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id & 0x00ff), 2);
    WRITE_ARRAY(REFERENCE byteStringA, pkt->size, 3);
    uint i;
    for (i = 0; i < pkt->size; i++)
    {
        uint8_t element;
        element = READ_ARRAY(REFERENCE pkt->data, i);
        WRITE_ARRAY(REFERENCE byteStringA, element, i + 4);
    }
    if (pkt->size < PACKET_SIZE_MAC - 3)
    {
        for(; i < PACKET_SIZE_MAC - 3; i++)
            WRITE_ARRAY(REFERENCE byteStringA, 0, i + 4);
    }

    *byteString = byteStringA;
}

void datapacket_construct_from_bytestring(DataPacket_t *pkt, ARRAY* byteString)
{
    assert(pkt != NULL);
    assert(byteString != NULL);

    pkt->size = PACKET_SIZE_MAC - 3;
    pkt->destination_id = 0;
    pkt->type = READ_ARRAY(SINGLE_POINTER byteString, 0);
    pkt->destination_id |= ((uint16_t)READ_ARRAY(SINGLE_POINTER byteString, 1)) << 8;
    pkt->destination_id |= ((uint16_t)READ_ARRAY(SINGLE_POINTER byteString, 2));
    pkt->size = READ_ARRAY(SINGLE_POINTER byteString, 3);
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
    // Copy only the data, and discard the zeros.
    for (uint i = 0; i < pkt->size; i++)
    {
        uint8_t element = READ_ARRAY(SINGLE_POINTER byteString, i + 4);
        WRITE_ARRAY(REFERENCE pkt->data, element, i);
    }
}
