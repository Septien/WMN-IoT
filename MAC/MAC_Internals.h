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

#ifdef __RIOT__
#include "sx127x.h"
#include "sx127x_internals.h"
#include "sx127x_netdev.h"
#include "sx127x_registers.h"
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
    ControlPacket_t *ctrlpkt;
    CFPacket_t *cfpkt;
    DataPacket_t *datapkt;
    // Status variables
    uint8_t numberChannels;
    uint32_t *channels;
    uint8_t nodeID;
    uint8_t destinationID;
    uint8_t selectedSlot;
    uint32_t transmitChannel;
    uint32_t receiveChannel;
    uint32_t cfChannel;
    uint8_t *slots;
    uint8_t numberSlots;
    uint8_t hopCount;
}MAC_Internals_t;

void initMACIn(MAC_Internals_t **mac);
void clearMAC(MAC_Internals_t *mac);
void destroyMAC(MAC_Internals_t **mac);

#endif  // MAC_INTERNALS_H