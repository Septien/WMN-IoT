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

void cfpacket_create(CFPacket_t *pkt, uint64_t *node_id, uint64_t *destination_id)
{
    assert(pkt != NULL);
    assert(node_id != NULL);
    assert(destination_id != NULL);

    pkt->node_id[0] = node_id[0];
    pkt->node_id[1] = node_id[1];
    pkt->destination_id[0] = destination_id[0];
    pkt->destination_id[1] = destination_id[1];
}

void cfpacket_clear(CFPacket_t *pkt)
{
    assert(pkt != NULL);

    memset(pkt, 0, sizeof(CFPacket_t));
}

void cfpacket_set_nodeid(CFPacket_t *pkt, uint64_t *node_id)
{
    assert(pkt != NULL);

    pkt->node_id[0] = node_id[0];
    pkt->node_id[1] = node_id[1];
}

void cfpacket_get_nodeid(CFPacket_t *pkt, uint64_t *node_id)
{
    assert(pkt != NULL);

    node_id[0] = pkt->node_id[0];
    node_id[1] = pkt->node_id[1];
}

void cfpacket_set_destinationid(CFPacket_t *pkt, uint64_t *destination_id)
{
    assert(pkt != NULL);
    
    pkt->destination_id[0] = destination_id[0];
    pkt->destination_id[1] = destination_id[1];
}

void cfpacket_get_destinationid(CFPacket_t *pkt, uint64_t *destination_id)
{
    assert(pkt != NULL);
    
    destination_id[0] = pkt->destination_id[0];
    destination_id[1] = pkt->destination_id[1];
}

void cfpacket_get_packet_bytestring(CFPacket_t *pkt, ARRAY* byteString)
{
    assert(pkt != NULL);

#ifdef __LINUX__
    (*byteString) = (uint8_t *)malloc(PACKET_SIZE_MAC * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(byteString, PACKET_SIZE_MAC);
#endif
    WRITE_ARRAY(SINGLE_POINTER byteString, 0,                                                       0);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[0] & 0xff00000000000000) >> 56,            1);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[0] & 0x00ff000000000000) >> 48,            2);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[0] & 0x0000ff0000000000) >> 40,            3);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[0] & 0x000000ff00000000) >> 32,            4);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[0] & 0x00000000ff000000) >> 24,            5);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[0] & 0x0000000000ff0000) >> 16,            6);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[0] & 0x000000000000ff00) >> 8,             7);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[0] & 0x00000000000000ff),                  8);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[1] & 0xff00000000000000) >> 56,            9);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[1] & 0x00ff000000000000) >> 48,            10);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[1] & 0x0000ff0000000000) >> 40,            11);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[1] & 0x000000ff00000000) >> 32,            12);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[1] & 0x00000000ff000000) >> 24,            13);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[1] & 0x0000000000ff0000) >> 16,            14);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[1] & 0x000000000000ff00) >> 8,             15);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->node_id[1] & 0x00000000000000ff),                  16);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[0] & 0xff00000000000000) >> 56,     17);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[0] & 0x00ff000000000000) >> 48,     18);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[0] & 0x0000ff0000000000) >> 40,     19);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[0] & 0x000000ff00000000) >> 32,     20);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[0] & 0x00000000ff000000) >> 24,     21);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[0] & 0x0000000000ff0000) >> 16,     22);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[0] & 0x000000000000ff00) >> 8,      23);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[0] & 0x00000000000000ff),           24);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[1] & 0xff00000000000000) >> 56,     25);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[1] & 0x00ff000000000000) >> 48,     26);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[1] & 0x0000ff0000000000) >> 40,     27);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[1] & 0x000000ff00000000) >> 32,     28);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[1] & 0x00000000ff000000) >> 24,     29);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[1] & 0x0000000000ff0000) >> 16,     30);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[1] & 0x000000000000ff00) >> 8,      31);
    WRITE_ARRAY(SINGLE_POINTER byteString, (pkt->destination_id[1] & 0x00000000000000ff),           32);
    uint n = rand();
    for (uint i = 33; i < PACKET_SIZE_MAC; i++)
        WRITE_ARRAY(SINGLE_POINTER byteString, n, i);
}

void cfpacket_construct_packet_from_bytestring(CFPacket_t *pkt, ARRAY* byteString)
{
    assert(pkt != NULL);
    assert(byteString != NULL);
#ifdef __LINUX__
    assert(*byteString != NULL);
#endif
#ifdef __RIOT__
    assert(byteString->size > 0);
#endif
    if (READ_ARRAY(SINGLE_POINTER byteString, 0) != 0)
        return;

    uint64_t node_id1 = 0;
    node_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 1)) << 56;
    node_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 2)) << 48;
    node_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 3)) << 40;
    node_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 4)) << 32;
    node_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 5)) << 24;
    node_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 6)) << 16;
    node_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 7)) << 8;
    node_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 8));
    uint64_t node_id2 = 0;
    node_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 9)) << 56;
    node_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 10)) << 48;
    node_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 11)) << 40;
    node_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 12)) << 32;
    node_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 13)) << 24;
    node_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 14)) << 16;
    node_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 15)) << 8;
    node_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 16));
    uint64_t destination_id1 = 0;
    destination_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 17)) << 56;
    destination_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 18)) << 48;
    destination_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 19)) << 40;
    destination_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 20)) << 32;
    destination_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 21)) << 24;
    destination_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 22)) << 16;
    destination_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 23)) << 8;
    destination_id1 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 24));
    uint64_t destination_id2 = 0;
    destination_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 25)) << 56;
    destination_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 26)) << 48;
    destination_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 27)) << 40;
    destination_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 28)) << 32;
    destination_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 29)) << 24;
    destination_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 30)) << 16;
    destination_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 31)) << 8;
    destination_id2 |= ((uint64_t) READ_ARRAY(SINGLE_POINTER byteString, 32));

    pkt->node_id[0] = node_id1;
    pkt->node_id[1] = node_id2;
    pkt->destination_id[0] = destination_id1;
    pkt->destination_id[1] = destination_id2;
}
