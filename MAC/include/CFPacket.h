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

typedef struct CFPacket
{
    uint8_t nodeID;
    uint8_t destinationID;
    uint32_t frequency;
}CFPacket_t;

void cfpacket_init(CFPacket_t DOUBLE_POINTER pkt);
void cfpacket_destroy(CFPacket_t DOUBLE_POINTER pkt);
void cfpacket_create(CFPacket_t *pkt, uint8_t nodeID, uint8_t destinationID, uint32_t frequency);
void cfpacket_clear(CFPacket_t *pkt);
void cfpacket_set_nodeid(CFPacket_t *pkt, uint8_t nodeID);
uint8_t cfpacket_get_nodeid(CFPacket_t *pkt);
void cfpacket_set_destinationid(CFPacket_t *pkt, uint8_t destinationID);
uint8_t cfpacket_get_destinationid(CFPacket_t *pkt);
void cfpacket_set_frequency(CFPacket_t *pkt, uint32_t frequency);
uint32_t cfpacket_get_frequency(CFPacket_t *pkt);
void cfpacket_get_packet_bytestring(CFPacket_t *pkt, ARRAY* byteString, size_t *size);
void cfpacket_construct_packet_from_bytestring(CFPacket_t *pkt, ARRAY* byteString, size_t size);

#endif // CFPKT_H