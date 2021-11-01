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

#include "ControlPacket.h"
#include "CFPacket.h"
#include "DataPacket.h"
#include "timeouts.h"
#include "memory.h"
#include "memory_macros.h"

#ifdef __RIOT__
#include "sx127x.h"
#endif

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
    // Packets send by the protocol
    ControlPacket_t SINGLE_POINTER ctrlpkt;
    CFPacket_t      SINGLE_POINTER cfpkt;
    DataPacket_t    SINGLE_POINTER datapkt;
    // Status variables
    uint8_t         numberChannels;
    ARRAY           channels;
    uint16_t         nodeID;
    uint16_t         destinationID;
    uint8_t         selectedSlot;
    uint32_t        transmitChannel;
    uint32_t        receiveChannel;
    uint32_t        cfChannel;
    ARRAY           slots;
    uint8_t         numberSlots;
    uint8_t         hopCount;
}MAC_Internals_t;

void MAC_internals_init(MAC_Internals_t DOUBLE_POINTER mac);
void MAC_internals_clear(MAC_Internals_t *mac);
void MAC_internals_destroy(MAC_Internals_t DOUBLE_POINTER mac);

#endif  // MAC_INTERNALS_H