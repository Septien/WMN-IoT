#include "MAC_Internals.h"
#include <assert.h>
#include <string.h>

void MAC_internals_init(MAC_Internals_t *mac,
#ifdef __LINUX__
    uint8_t *radio
#endif
#ifdef __RIOT__
    netdev_t *netdev
#endif
)
{
    memset(mac, 0, sizeof(MAC_Internals_t));
#ifdef __LINUX__
    mac->radio = radio;
#endif
    memset(&mac->frame, 0, sizeof(Frame_t));

#ifdef __RIOT__
    mac->netdev = netdev;
#endif
    mac->cfChannel = CF_FREQUENCY;

    // Initialize the Control packet
    ControlPacket_t *ctrl, *ctrl_recv;
    ctrl = &mac->ctrlpkt;
    ctrl_recv = &mac->ctrlpkt_recv;
    controlpacket_init(&ctrl);
    controlpacket_init(&ctrl_recv);

    /* Initialize the data packets queues. These packets are not pointers, so just fill 
    the memory space with zeros. */
    for (int i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
    {
        DataPacket_t *pkt;
        pkt = &mac->_message_packets_to_send[i];
        memset(pkt, 0, sizeof(DataPacket_t));
        pkt = &mac->_control_packets_to_send[i];
        memset(pkt, 0, sizeof(DataPacket_t));
        pkt = &mac->_packets_received[i];
        memset(pkt, 0, sizeof(DataPacket_t));
    }

    // Initialize the cf packets
    for (int i = 0; i < MAX_NUMBER_CF_PACKETS; i++)
    {
        CFPacket_t *cfpkt = &mac->_cf_messages[i];
        memset(cfpkt, 0, sizeof(CFPacket_t));
    }
}

void MAC_internals_destroy(MAC_Internals_t *mac)
{
    assert(mac != NULL);
    
    for (int i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
    {
        // As these packets are not pointers, just release the array's associated memory.
        DataPacket_t *pkt = &mac->_message_packets_to_send[i];
        datapacket_clear(pkt);
        pkt = &mac->_control_packets_to_send[i];
        datapacket_clear(pkt);
        pkt = &mac->_packets_received[i];
        datapacket_clear(pkt);
    }
    for (int i = 0; i < MAX_NUMBER_CF_PACKETS; i++)
    {
        CFPacket_t *pkt = &mac->_cf_messages[i];
        cfpacket_clear(pkt);
    }

    /* Destroy packets data structures */
    ControlPacket_t *ctrl, *ctrl_recv;
    ctrl = &mac->ctrlpkt;
    ctrl_recv = &mac->ctrlpkt_recv;
    controlpacket_destroy(&ctrl);
    controlpacket_destroy(&ctrl_recv);
    // Destroy Frame
    memset(&mac->frame, 0, sizeof(Frame_t));
}

void MAC_internals_clear(MAC_Internals_t *mac)
{
    // Keep radio, it is not physically possible to change the radio once deployed.
    assert(mac != NULL);

    mac->frame.current_slot = 0;
    mac->frame.current_frame = 0;
    mac->frame.current_cf_slot = 0;
    mac->frame.cf_slots_number = 0;
    mac->frame.slots_number = 0;
    mac->selectedSlot = 0;
    mac->transmitChannel = 0;
    mac->receiveChannel = 0;
    mac->cfChannel = 0;
    mac->_collisionDetected = false;
    mac->_collisionSlot = 0;
    mac->_collisionFrequency = 0;
    mac->_cf_message_received = false;
        for (int i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
    {
        // As these packets are not pointers, just release the array's associated memory.
        DataPacket_t *pkt = &mac->_message_packets_to_send[i];
        datapacket_clear(pkt);
        pkt = &mac->_control_packets_to_send[i];
        datapacket_clear(pkt);
        pkt = &mac->_packets_received[i];
        datapacket_clear(pkt);
    }
    for (int i = 0; i < MAX_NUMBER_CF_PACKETS; i++)
    {
        CFPacket_t *pkt = &mac->_cf_messages[i];
        cfpacket_clear(pkt);
    }
}
