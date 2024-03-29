/**
 * @ingroup     MAC
 * @{
 * @file
 * @brief       The control packet implementation required by the selected MAC protocol
 * 
 * @author      José Antonio Septién Hernández.
 * @}
 */

#ifndef CTRLPKT_H
#define CTRLPKT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "config_mac.h"
#include "memory_macros.h"
#ifdef __RIOT__
#include "memory.h"
#endif

typedef struct 
{
    uint8_t     currentSlot;
    uint8_t     collisionSlot;
    uint16_t    hopCount;
    uint32_t    currentFrame;
    uint32_t    collisionFrequency;
    uint32_t    initTime;
    uint64_t    networkTime;
    uint64_t    node_id[2];
    uint8_t     occupied_slots[MAX_NUMBER_FREQS][(MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0)];
} ControlPacket_t;

void controlpacket_init(ControlPacket_t DOUBLE_POINTER pkt);
void controlpacket_destroy(ControlPacket_t DOUBLE_POINTER pkt);
void controlpacket_create(ControlPacket_t *pkt, uint64_t *node_id, uint32_t frame, uint8_t slot, uint8_t collisionSlots, uint32_t collisionFrequency, uint16_t hopCount, uint64_t netTime, uint32_t initTime, uint8_t *occupied_slots);
void controlpacket_clear(ControlPacket_t *pkt);
void controlpacket_set_current_frame(ControlPacket_t *pkt, uint32_t frame);
uint32_t controlpacket_get_current_frame(ControlPacket_t *pkt);
void controlpacket_set_current_slot(ControlPacket_t *pkt, uint8_t slot);
uint8_t controlpacket_get_current_slot(ControlPacket_t *pkt);
uint8_t controlpacket_get_collision_slot(ControlPacket_t *pkt);
uint32_t controlpacket_get_collision_frequency(ControlPacket_t *pkt);
void controlpacket_set_hop_count(ControlPacket_t *pkt, uint16_t count);
uint16_t controlpacket_get_hop_count(ControlPacket_t *pkt);
void controlpacket_set_network_time(ControlPacket_t *pkt, uint64_t time);
uint64_t controlpacket_get_network_time(ControlPacket_t *pkt);
void controlpacket_set_init_time(ControlPacket_t *pkt, uint32_t time);
uint32_t controlpacket_get_init_time(ControlPacket_t *pkt);
void controlpacket_get_occupied_slots(ControlPacket_t *pkt, uint8_t *occupied_slots);
void controlpacket_get_packet_bytestring(ControlPacket_t *pkt, ARRAY* byteStr);
void controlpacket_construct_packet_from_bytestring(ControlPacket_t *pkt, ARRAY* byteString);

uint8_t get_number_bytes(uint8_t n);
#endif // CTRLPKT_H
