/* Implement all the handles for the different packets. */
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

bool stub_mclmac_receive_ctrlpkt_sync(MCLMAC_t *mclmac, ControlPacket_t *ctrlpkt)
{
    assert(mclmac != NULL);
    assert(ctrlpkt != NULL);

    static uint32_t frame = 0;
    static uint8_t slot = 0;
    static uint8_t hopCount = 100;
    static uint32_t network_time = 220;
    static uint32_t init_time = 10;

    controlpacket_set_current_frame(ctrlpkt, frame);
    controlpacket_set_current_slot(ctrlpkt, slot);
    controlpacket_set_hop_count(ctrlpkt, hopCount);
    controlpacket_set_network_time(ctrlpkt, network_time);
    controlpacket_set_init_time(ctrlpkt, init_time);

    slot++;
    network_time++;
    if ((slot % MAX_NUMBER_SLOTS) == 0)
    {
        slot = 0;
        frame++;
    }

    if ((rand() % 1024) > 128)
        return false;

    return true;
}

void mclmac_create_control_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    ControlPacket_t *pkt = REFERENCE ARROW(mclmac->mac)ctrlpkt;
    controlpacket_clear(pkt);

    // Get the relevant data.
    uint16_t nodeID = mclmac->_nodeID;
    uint32_t currentFrame = ARROW(ARROW(mclmac->mac)frame)current_frame;
    uint8_t currentSlot = ARROW(ARROW(mclmac->mac)frame)current_slot;
    uint8_t collisionSlot = ARROW(mclmac->mac)_collisionSlot;
    uint32_t collisionFrequency = ARROW(mclmac->mac)_collisionFrequency;
    uint16_t hopCount = mclmac->_hopCount;
    uint64_t networkTime = mclmac->_networkTime;
    uint32_t initTime = mclmac->_initTime;

    controlpacket_create(pkt, nodeID, currentFrame, currentSlot, collisionSlot, collisionFrequency,
                        hopCount, networkTime, initTime);
}

void stub_mclmac_send_control_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->ctrlpkt != NULL);
#endif

    ControlPacket_t *pkt = REFERENCE ARROW(mclmac->mac)ctrlpkt;
    ARRAY byteStr;

    controlpacket_get_packet_bytestring(pkt, &byteStr);

    // Send byte string

    controlpacket_clear(pkt);

#ifdef __LINUX__
    free(byteStr);
#endif
#ifdef __RIOT__
    free_array(&byteStr);
#endif
}

void stub_mclmac_send_data_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    if (ARROW(mclmac->mac)_packets_to_send_message == 0)
        return;
    if (ARROW(mclmac->mac)_first_send_message > MAX_NUMBER_DATA_PACKETS)
        return;

    // Get the packet pointed to by _first_send_message
    uint8_t first_send = ARROW(mclmac->mac)_first_send_message;
    DataPacket_t *pkt = &ARROW(mclmac->mac)_message_packets_to_send[first_send];

    // Get the byte string
    ARRAY byteString;
    datapacket_get_packet_bytestring(pkt, &byteString);

    // copy byte string to radio.
    // Send byte string.

    ARROW(mclmac->mac)_first_send_message = (first_send + 1) % MAX_NUMBER_DATA_PACKETS;
    ARROW(mclmac->mac)_packets_to_send_message--;

    datapacket_clear(pkt);

#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
#endif
}

void stub_mclmac_send_layers_control_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    if (ARROW(mclmac->mac)_packets_to_send_control == 0)
        return;
    if (ARROW(mclmac->mac)_first_send_control > MAX_NUMBER_DATA_PACKETS)
        return;

    // Get the first control packet
    uint8_t first_send = ARROW(mclmac->mac)_first_send_control;
    DataPacket_t *pkt = &ARROW(mclmac->mac)_control_packets_to_send[first_send];

    // Get the byte string
    ARRAY byteString;
    datapacket_get_packet_bytestring(pkt, &byteString);

    // copy the packet to the radio
    // Send the packet

    ARROW(mclmac->mac)_first_send_control = (first_send + 1) % MAX_NUMBER_DATA_PACKETS;
    ARROW(mclmac->mac)_packets_to_send_control--;

    datapacket_clear(pkt);

#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
#endif
}
