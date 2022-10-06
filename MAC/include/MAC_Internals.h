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
#include "net/netdev.h"
#include "net/netopt.h"

// Header files for the nRF24l01+
#include "nrf24l01p_ng.h"
#include "nrf24l01p_ng_communication.h"
#include "nrf24l01p_ng_constants.h"
#include "nrf24l01p_ng_diagnostics.h"
#include "nrf24l01p_ng_netdev.h"
#include "nrf24l01p_ng_params.h"
#include "nrf24l01p_ng_registers.h"
#include "nrf24l01p_ng_states.h"
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
#ifdef __LINUX__
    uint8_t *radio;
    /* TODO: Add an implementation that runs the radio from a common computer */
#endif
#ifdef __RIOT__
    netdev_t *netdev;
#endif
    // Packets used by the protocol
    ControlPacket_t SINGLE_POINTER ctrlpkt;
    ControlPacket_t SINGLE_POINTER ctrlpkt_recv;
    Frame_t         SINGLE_POINTER frame;
    // Status variables
    uint8_t         selectedSlot;
    uint32_t        transmitChannel;
    uint32_t        receiveChannel;
    uint32_t        cfChannel;
    uint64_t        transmitter_id[2];
    // Collision
    bool            _collisionDetected;
    bool            _is_internal_collision;
    uint8_t         _collisionSlot;
    uint32_t        _collisionFrequency;
    /* CF messages */
    bool            _cf_message_received;
    /* On position 0, store the cf message to send, on pisition 1, the cf messages to receive. */
    CFPacket_t      _cf_messages[MAX_NUMBER_CF_PACKETS];
    /* Packets to send. */
    uint64_t        _destination_id[2];
    uint8_t         _packets_to_send_message;
    uint8_t         _first_send_message;
    uint8_t         _last_send_message;
    DataPacket_t    _message_packets_to_send[MAX_NUMBER_DATA_PACKETS];
    uint8_t         _packets_to_send_control;
    uint8_t         _first_send_control;
    uint8_t         _last_send_control;
    DataPacket_t    _control_packets_to_send[MAX_NUMBER_DATA_PACKETS];
    /* Messages received from other nodes */
    uint8_t         _first_received;
    uint8_t         _last_received;
    /* A node can send up to 2 * MAX_NUMBER_DATA_PACKETS, so that is the number of packets
    a node should receive. */
    DataPacket_t    _packets_received[2 * MAX_NUMBER_DATA_PACKETS];
    uint16_t        _number_packets_received;
}MAC_Internals_t;

void MAC_internals_init(MAC_Internals_t DOUBLE_POINTER mac, 
#ifdef __LINUX__
    uint8_t *radio
#endif
#ifdef __RIOT__
    netdev_t *netdev
#endif
);
void MAC_internals_clear(MAC_Internals_t *mac);
void MAC_internals_destroy(MAC_Internals_t DOUBLE_POINTER mac);

#endif  // MAC_INTERNALS_H
