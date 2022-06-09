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

void datapacket_create(DataPacket_t *pkt, int8_t type, uint64_t *destination_id, ARRAY* data, uint8_t size)
{
    assert(pkt != NULL);
    assert(type >= 0 && type < 10);      // Only accept type 0, 1, or 2. -1 is for when no data is stored on the packet.
    assert(size > 0);
    assert(data != NULL);
#ifdef __LINUX__
    assert(*data != NULL);
#endif
    pkt->destination_id[0] = destination_id[0];
    pkt->destination_id[1] = destination_id[1];
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

    pkt->destination_id[0] = 0;
    pkt->destination_id[1] = 0;
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
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[0] & 0xff00000000000000) >> 56, 1);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[0] & 0x00ff000000000000) >> 48, 2);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[0] & 0x0000ff0000000000) >> 40, 3);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[0] & 0x000000ff00000000) >> 32, 4);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[0] & 0x00000000ff000000) >> 24, 5);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[0] & 0x0000000000ff0000) >> 16, 6);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[0] & 0x000000000000ff00) >> 8,  7);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[0] & 0x00000000000000ff),       8);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[2] & 0xff00000000000000) >> 56, 9);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[2] & 0x00ff000000000000) >> 48, 10);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[2] & 0x0000ff0000000000) >> 40, 11);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[2] & 0x000000ff00000000) >> 32, 12);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[2] & 0x00000000ff000000) >> 24, 13);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[2] & 0x0000000000ff0000) >> 16, 14);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[2] & 0x000000000000ff00) >> 8,  15);
    WRITE_ARRAY(REFERENCE byteStringA, (pkt->destination_id[2] & 0x00000000000000ff),       16);
    WRITE_ARRAY(REFERENCE byteStringA, pkt->size, 17);
    uint i;
    for (i = 0; i < pkt->size; i++)
    {
        uint8_t element;
        element = READ_ARRAY(REFERENCE pkt->data, i);
        WRITE_ARRAY(REFERENCE byteStringA, element, i + 18);
    }
    if (pkt->size < PACKET_SIZE_MAC - 18)
    {
        for(; i < PACKET_SIZE_MAC - 18; i++)
            WRITE_ARRAY(REFERENCE byteStringA, 0, i + 18);
    }

    *byteString = byteStringA;
}

void datapacket_construct_from_bytestring(DataPacket_t *pkt, ARRAY* byteString)
{
    assert(pkt != NULL);
    assert(byteString != NULL);

    pkt->type = READ_ARRAY(SINGLE_POINTER byteString, 0);
    pkt->destination_id[0] = 0;
    pkt->destination_id[0] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 1)) << 56;
    pkt->destination_id[0] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 2)) << 48;
    pkt->destination_id[0] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 3)) << 40;
    pkt->destination_id[0] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 4)) << 32;
    pkt->destination_id[0] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 5)) << 24;
    pkt->destination_id[0] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 6)) << 16;
    pkt->destination_id[0] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 7)) << 8;
    pkt->destination_id[0] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 8));
    pkt->destination_id[1] = 0;
    pkt->destination_id[1] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 9)) << 56;
    pkt->destination_id[1] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 10)) << 48;
    pkt->destination_id[1] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 11)) << 40;
    pkt->destination_id[1] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 12)) << 32;
    pkt->destination_id[1] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 13)) << 24;
    pkt->destination_id[1] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 14)) << 16;
    pkt->destination_id[1] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 15)) << 8;
    pkt->destination_id[1] |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 16));
    pkt->size = READ_ARRAY(SINGLE_POINTER byteString, 17);
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
    // Copy only the data, and discard the rest.
    for (uint i = 0; i < pkt->size; i++)
    {
        uint8_t element = READ_ARRAY(SINGLE_POINTER byteString, i + 18);
        WRITE_ARRAY(REFERENCE pkt->data, element, i);
    }
}
