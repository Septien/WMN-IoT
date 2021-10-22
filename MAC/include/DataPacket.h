/**
 * @ingroup     MAC
 * @{
 * @file
 * @brief       The data packet implementation required by the selected MAC protocol
 * 
 * @author      José Antonio Septién Hernández.
 * @}
 */
#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "memory_macros.h"
#ifdef __RIOT__
#include "memory.h"
#endif

typedef struct {
    bool     isFragment;
    uint8_t  totalFragments;
    uint8_t  fragmentNumber;
    size_t   packetLength;
    uint8_t  dataLength;
    ARRAY    data;
}DataPacket_t;

void datapacket_init(DataPacket_t DOUBLE_POINTER pkt);
void datapacket_destroy(DataPacket_t DOUBLE_POINTER pkt);
void datapacket_create(DataPacket_t *pkt, bool isFragment, uint8_t totalFragments, uint8_t fragmentNumber, ARRAY* data, size_t size);
void datapacket_clear(DataPacket_t *pkt);
void datapacket_set_isFragment(DataPacket_t *pkt, bool isFragment);
bool datapacket_get_isFragment(DataPacket_t *pkt);
void datapacket_set_total_fragments(DataPacket_t *pkt, uint8_t totalFragments);
uint8_t datapacket_get_total_fragments(DataPacket_t *pkt);
void datapacket_set_fragment_number(DataPacket_t *pkt, uint8_t fragmentNumber);
uint8_t datapacket_get_fragment_number(DataPacket_t *pkt);
void datapacket_set_data(DataPacket_t *pkt, ARRAY* data, uint8_t size);
void datapacket_delete_data(DataPacket_t *pkt);
void datapacket_clear_data(DataPacket_t *pkt);
void datapacket_get_data(DataPacket_t *pkt, ARRAY* data, uint8_t *size);
uint8_t datapacket_get_data_length(DataPacket_t *pkt);
size_t datapacket_get_packet_length(DataPacket_t *pkt);
void datapacket_get_packet_bytestring(DataPacket_t *pkt, ARRAY* byteString, size_t *size);
void datapacket_construct_from_bytestring(DataPacket_t *pkt, ARRAY* byteString, size_t size);
#endif  // DATAPACKET_H
