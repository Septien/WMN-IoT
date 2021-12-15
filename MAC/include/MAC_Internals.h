/**
 * @ingroup     MAC
 * @{
 * @file
 * @brief       The data structure containing all necessary variables for a 
 *              proper operation of the MAC protocol.
 * 
 * @author      José Antonio Septién Hernández.
 * @}
 */
#ifndef MAC_INTERNALS_H
#define MAC_INTERNALS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Frame.h"
#include "ControlPacket.h"
#include "CFPacket.h"
#include "DataPacket.h"
#include "timeouts.h"
#include "memory.h"
#include "memory_macros.h"

#ifdef __RIOT__
#include "sx127x.h"
#endif

/**
 * @brief Use this data structure within the MEDIUM_ACCESS state.
 *          It contains:
 *              -Queues for cf and data packets.
 *              -A control packet.
 *              -A frame data structure.
 *              -The selected slot.
 *              -The selected frequency.
 *              -The channel to transmit.
 *              -The channel to receive.
 *              -The cf channel.
 *              -Whether a collision is detected, if so, the slot and frequency.
 *              -The duration of frames, slots, and cfslots.
 */
typedef struct MAC_Internals
{
    // Physical device
#ifdef __RIOT__
    sx127x_t *radio;
#endif
#ifdef __LINUX__
    uint8_t *radio;
    /* TODO: Add an implementation that runs the radio from a common computer */
#endif
    // Packets used by the protocol
    ControlPacket_t SINGLE_POINTER ctrlpkt;
    CFPacket_t      SINGLE_POINTER cfpkt;
    Frame_t         SINGLE_POINTER frame;
    // Status variables
    uint8_t         selectedSlot;
    uint32_t        transmitChannel;
    uint32_t        receiveChannel;
    uint32_t        cfChannel;
    uint16_t        indexChannel;
    // Collision
    bool            _collisionDetected;
    uint8_t         _collisionSlot;
    uint32_t        _collisionFrequency;
    /* CF messages */
    bool            _cf_message_received;
    CFPacket_t      _cf_messages[MAX_NUMBER_CF_PACKETS];
    uint8_t         _max_cf_messages;
    /* Messages received from other nodes */
    uint8_t         _first_send;
    uint8_t         _last_send;
    uint8_t         _packets_read;
    uint16_t        _destination_ids[MAX_NUMBER_DATA_PACKETS];
    DataPacket_t    _packet_to_send[MAX_NUMBER_DATA_PACKETS];
    uint8_t         _first_received;
    uint8_t         _last_received;
    uint16_t        _max_data_packets;
    DataPacket_t    _packets_received[MAX_NUMBER_DATA_PACKETS];
    uint16_t        _number_packets_received;
}MAC_Internals_t;

void MAC_internals_init(MAC_Internals_t DOUBLE_POINTER mac, 
#ifdef __LINUX__
    uint8_t *radio
#endif
#ifdef __RIOT__
    sx127x_t *radio
#endif
);
void MAC_internals_clear(MAC_Internals_t *mac);
void MAC_internals_destroy(MAC_Internals_t DOUBLE_POINTER mac);

#endif  // MAC_INTERNALS_H