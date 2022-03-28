/**
 * @ingroup     MAC
 * @{
 * @file
 * @brief       This data structure stores all the packets received by upper layers,
 *              for sending to the medium. It contains only the destinatio id, the data
 *              to send (treated as a string of uint8_t) and the data's size.
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

#include "config_mac.h"

typedef struct {
    int8_t      type;
    uint16_t    destination_id;
    size_t      size;
    ARRAY       data;
}DataPacket_t;

void datapacket_init(DataPacket_t DOUBLE_POINTER pkt);
void datapacket_destroy(DataPacket_t DOUBLE_POINTER pkt);
void datapacket_create(DataPacket_t *pkt, int8_t type, uint16_t destination_id, ARRAY* data, size_t size);
void datapacket_clear(DataPacket_t *pkt);
void datapacket_get_packet_bytestring(DataPacket_t *pkt, ARRAY* byteString, size_t *size);
void datapacket_construct_from_bytestring(DataPacket_t *pkt, ARRAY* byteString, size_t size);

#endif  // DATAPACKET_H
