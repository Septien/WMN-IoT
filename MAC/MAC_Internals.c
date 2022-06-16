#include "MAC_Internals.h"
#include <assert.h>
#include <string.h>

void MAC_internals_init(MAC_Internals_t DOUBLE_POINTER mac,
#ifdef __LINUX__
    uint8_t *radio
#endif
#ifdef __RIOT__
    sx127x_t *radio
#endif
)
{
#ifdef __LINUX__
    *mac = (MAC_Internals_t *)malloc(sizeof(MAC_Internals_t));
#endif
    memset(SINGLE_POINTER mac, 0, sizeof(MAC_Internals_t));
#ifdef __LINUX__
    (*mac)->radio = NULL;
    (*mac)->ctrlpkt = NULL;
    (*mac)->frame = (Frame_t *)malloc(sizeof(Frame_t));
    if ((*mac)->frame == NULL)
        return;
#endif
    memset(REFERENCE (SINGLE_POINTER mac)->frame, 0, sizeof(Frame_t));

    (SINGLE_POINTER mac)->radio = radio;
    (SINGLE_POINTER mac)->cfChannel = CF_FREQUENCY;

    // Initialize the Control packet
    controlpacket_init(&(SINGLE_POINTER mac)->ctrlpkt);
    controlpacket_init(&(SINGLE_POINTER mac)->ctrlpkt_recv);

    /* Initialize the data packets queues. These packets are not pointers, so just fill 
    the memory space with zeros. */
    for (int i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
    {
        DataPacket_t *pkt;
        pkt = &(SINGLE_POINTER mac)->_message_packets_to_send[i];
        memset(pkt, 0, sizeof(DataPacket_t));
        pkt = &(SINGLE_POINTER mac)->_control_packets_to_send[i];
        memset(pkt, 0, sizeof(DataPacket_t));
        pkt = &(SINGLE_POINTER mac)->_packets_received[i];
        memset(pkt, 0, sizeof(DataPacket_t));
    }

    // Initialize the cf packets
    for (int i = 0; i < MAX_NUMBER_CF_PACKETS; i++)
    {
        CFPacket_t *cfpkt = &(SINGLE_POINTER mac)->_cf_messages[i];
        memset(cfpkt, 0, sizeof(CFPacket_t));
    }
}

void MAC_internals_destroy(MAC_Internals_t DOUBLE_POINTER mac)
{
    assert(mac != NULL);
    
    for (int i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
    {
        // As these packets are not pointers, just release the array's associated memory.
        DataPacket_t *pkt = &(SINGLE_POINTER mac)->_message_packets_to_send[i];
        datapacket_clear(pkt);
        pkt = &(SINGLE_POINTER mac)->_control_packets_to_send[i];
        datapacket_clear(pkt);
        pkt = &(SINGLE_POINTER mac)->_packets_received[i];
        datapacket_clear(pkt);
    }
    for (int i = 0; i < MAX_NUMBER_CF_PACKETS; i++)
    {
        CFPacket_t *pkt = &(SINGLE_POINTER mac)->_cf_messages[i];
        cfpacket_clear(pkt);
    }
#ifdef __LINUX__
    assert(*mac != NULL);

    /* Destroy packets data structures */
    if ((*mac)->ctrlpkt != NULL)
        controlpacket_destroy(&(*mac)->ctrlpkt);
    if ((*mac)->ctrlpkt_recv != NULL)
        controlpacket_destroy(&(*mac)->ctrlpkt_recv);
    // Destroy Frame
    if ((*mac)->frame != NULL)
        free((*mac)->frame);
    free((*mac));
    *mac = NULL;
#endif
#ifdef __RIOT__
    controlpacket_destroy(&mac->ctrlpkt);
    controlpacket_destroy(&mac->ctrlpkt_recv);
#endif
}

void MAC_internals_clear(MAC_Internals_t *mac)
{
    // Keep radio, it is not physically possible to change the radio once deployed.
    assert(mac != NULL);

    ARROW(mac->frame)current_slot = 0;
    ARROW(mac->frame)current_frame = 0;
    ARROW(mac->frame)current_cf_slot = 0;
    ARROW(mac->frame)cf_slots_number = 0;
    ARROW(mac->frame)slots_number = 0;
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
