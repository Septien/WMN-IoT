/**
 * @ingroup     MAC
 * @{
 * @file
 * @brief       The CFs packet implementation required by the selected MAC protocol
 * 
 * @author      José Antonio Septién Hernández.
 * @}
 */
#ifndef CFPKT_H
#define CFPKT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __RIOT__
#include "memory.h"
#endif
#include "memory_macros.h"
#include "config_mac.h"

typedef struct CFPacket
{
    uint64_t node_id[2];
    uint64_t destination_id[2];
}CFPacket_t;

void cfpacket_init(CFPacket_t *pkt);
void cfpacket_destroy(CFPacket_t *pkt);
void cfpacket_create(CFPacket_t *pkt, uint64_t *node_id, uint64_t *destination_id);
void cfpacket_clear(CFPacket_t *pkt);
void cfpacket_set_nodeid(CFPacket_t *pkt, uint64_t *node_id);
void cfpacket_get_nodeid(CFPacket_t *pkt, uint64_t *node_id);
void cfpacket_set_destinationid(CFPacket_t *pkt, uint64_t *destination_id);
void cfpacket_get_destinationid(CFPacket_t *pkt, uint64_t *destination_id);
void cfpacket_get_packet_bytestring(CFPacket_t *pkt, ARRAY* byteString);
void cfpacket_construct_packet_from_bytestring(CFPacket_t *pkt, ARRAY* byteString);

#endif // CFPKT_H
