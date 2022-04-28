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

/* For the first test, I want this function to emulate properly the radio, that is, 
as if the radio is hearing the medium. But I want to always return false, or to pretend
that no message is received.
For the second test, I want to simulate a packet is received, and return the randomly created
packet.
For the third case, no packets are either received or to send.
The for case simulates a collision, where a packet should be sent and received on the same 
time slot.
The fifth case should be when two different packets are received on the same slot. A 
collision situation.
The function, or a part of it, will execute on an interrupt context. Meaning that 
the execution of the program will stop, to handle the hardware-generated interrupt.
How to simulate that? How to make it behave as the real one?
For the moment, a quick and simple solution would be to use a state variable on mclmac data structure.
For the state 1, always return false.
For the state 2, return true just once, and then false the other
For the state 3, return always false.
For the state 4, return once true, and the other false.
For the state 5, return true twice. 
Have a counter indicating in which function call the function is at.
Another variable for holding the last state of _cf_message_received. */
bool stub_mclmac_receive_cf_message(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    if (mclmac->_state == 1 || mclmac->_state == 3)
    {
        ARROW(mclmac->mac)_cf_message_received = false;
        return false;
    }
    if (mclmac->_state == 2)
    {
        if (mclmac->_trues == 1)
        {
            ARROW(mclmac->mac)_cf_message_received = false;
            return false;
        }
        
        ARROW(mclmac->mac)_cf_message_received = true;
        CFPacket_t *pkt = &ARROW(mclmac->mac)_cf_messages[1];
        if (ARROW(mclmac->mac)_cf_message_received)
        {
            uint16_t id = rand() % 256;
            id = (id == mclmac->_nodeID ? id + 1 : id);
            uint16_t nodeid = mclmac_get_nodeid(mclmac);
            cfpacket_create(pkt, id, nodeid);
            mclmac->_trues++;
        }
        else
        {            
            cfpacket_create(pkt, 0, 0);
        }
    }

    if (mclmac->_state == 4)
    {
        if (mclmac_get_current_cf_slot(mclmac) == 0)
        {
            ARROW(mclmac->mac)_cf_message_received = false;
            return false;
        }
        if (mclmac->_trues == 2)
        {
            ARROW(mclmac->mac)_cf_message_received = false;
            return false;
        }
        ARROW(mclmac->mac)_cf_message_received = true;
        CFPacket_t *pkt = &ARROW(mclmac->mac)_cf_messages[1];
        if (ARROW(mclmac->mac)_cf_message_received)
        {
            uint16_t id = rand() % 256;
            id = (id == mclmac->_nodeID ? id + 1 : id);
            uint16_t nodeid = mclmac_get_nodeid(mclmac);
            cfpacket_create(pkt, id, nodeid);
            mclmac->_trues++;
        }
    }

    if (mclmac->_state == 5)
    {
        if (mclmac->_trues5 > 2)
        {
            ARROW(mclmac->mac)_cf_message_received = false;
            return false;
        }
        ARROW(mclmac->mac)_cf_message_received = (rand() % 1024 > 256 ? true : false);
        CFPacket_t *pkt = &ARROW(mclmac->mac)_cf_messages[1];
        if (ARROW(mclmac->mac)_cf_message_received)
        {
            uint16_t id = rand() % 256;
            id = (id == mclmac->_nodeID ? id + 1 : id);
            uint16_t nodeid = mclmac_get_nodeid(mclmac);
            cfpacket_create(pkt, id, nodeid);
            mclmac->_trues5++;
        }
    }

    return ARROW(mclmac->mac)_cf_message_received;
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

/**
 * State 1:
 *  A correct control packet, destined for the current node, is received.
 */
void stub_mclmac_receive_control_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    ARRAY byteString;
#ifdef __LINUX__
    byteString = (uint8_t *)malloc(MAX_MESSAGE_SIZE * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteString, MAX_MESSAGE_SIZE);
#endif
    for (uint i = 0; i < MAX_MESSAGE_SIZE; i++)
        WRITE_ARRAY(REFERENCE byteString, rand(), i);
    uint8_t bytes = 1;
    // Store type
    WRITE_ARRAY(REFERENCE byteString, bytes,    0);
    if (mclmac->_state == 1 || mclmac->_state == 3 || mclmac->_state == 4 || mclmac->_state == 5 || mclmac->_state == 6)
    {
        // Store node id
        bytes = (ARROW(mclmac->mac)transmiterID & 0xff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    1);
        bytes = (ARROW(mclmac->mac)transmiterID & 0x00ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    2);
        // Store current frame
        uint32_t frame = ARROW(ARROW(mclmac->mac)frame)current_frame;
        bytes = (frame & 0xff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    3);
        bytes = (frame & 0x00ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    4);
        bytes = (frame & 0x0000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    5);
        bytes = (frame & 0x000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    6);
        // Store current slot
        uint8_t slot = mclmac_get_current_slot(mclmac);
        WRITE_ARRAY(REFERENCE byteString, slot,     7);
        // Store collision slot, 0
        WRITE_ARRAY(REFERENCE byteString, 0,        8);
        // Store collision frequency, 0
        WRITE_ARRAY(REFERENCE byteString, 0,        9);
        WRITE_ARRAY(REFERENCE byteString, 0,        10);
        WRITE_ARRAY(REFERENCE byteString, 0,        11);
        WRITE_ARRAY(REFERENCE byteString, 0,        12);
        // Store hop count
        uint16_t hops = mclmac->_hopCount;
        bytes = (hops & 0xff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    13);
        bytes = (hops & 0x00ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    14);
        // Store network time
        uint64_t time = mclmac->_networkTime;
        bytes = (time & 0xff00000000000000) >> 56;
        WRITE_ARRAY(REFERENCE byteString, bytes,    15);
        bytes = (time & 0x00ff000000000000) >> 48;
        WRITE_ARRAY(REFERENCE byteString, bytes,    16);
        bytes = (time & 0x0000ff0000000000) >> 40;
        WRITE_ARRAY(REFERENCE byteString, bytes,    17);
        bytes = (time & 0x000000ff00000000) >> 32;
        WRITE_ARRAY(REFERENCE byteString, bytes,    18);
        bytes = (time & 0x00000000ff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    19);
        bytes = (time & 0x0000000000ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    20);
        bytes = (time & 0x000000000000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    21);
        bytes = (time & 0x00000000000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    22);
        // Store init time
        uint32_t init_time = mclmac->_initTime;
        bytes = (init_time & 0xff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    23);
        bytes = (init_time & 0x00ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    24);
        bytes = (init_time & 0x0000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    25);
        bytes = (init_time & 0x000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    26);
        if (mclmac->_state == 3)
        {
            // Store collision slot, 0
            bytes = ARROW(mclmac->mac)_collisionSlot;
            WRITE_ARRAY(REFERENCE byteString, bytes,    8);
            // Store collision frequency, 0
            uint32_t freq = ARROW(mclmac->mac)transmitChannel;
            bytes = (freq & 0xff000000) >> 24;
            WRITE_ARRAY(REFERENCE byteString, bytes,    9);
            bytes = (freq & 0x00ff0000) >> 16;
            WRITE_ARRAY(REFERENCE byteString, bytes,    10);
            bytes = (freq & 0x0000ff00) >> 8;
            WRITE_ARRAY(REFERENCE byteString, bytes,    11);
            bytes = (freq & 0x000000ff);
            WRITE_ARRAY(REFERENCE byteString, bytes,    12);
        }
        else if (mclmac->_state == 4)
        {
            WRITE_ARRAY(REFERENCE byteString, slot + 1, 7);
        }
        else if (mclmac->_state == 5)
        {
            uint32_t frame = ARROW(ARROW(mclmac->mac)frame)current_frame + 1;
            bytes = (frame & 0xff000000) >> 24;
            WRITE_ARRAY(REFERENCE byteString, bytes,    3);
            bytes = (frame & 0x00ff0000) >> 16;
            WRITE_ARRAY(REFERENCE byteString, bytes,    4);
            bytes = (frame & 0x0000ff00) >> 8;
            WRITE_ARRAY(REFERENCE byteString, bytes,    5);
            bytes = (frame & 0x000000ff);
            WRITE_ARRAY(REFERENCE byteString, bytes,    6);
        }
        else if (mclmac->_state == 6)
        {
            uint64_t time = mclmac->_networkTime + 1;
            bytes = (time & 0xff00000000000000) >> 56;
            WRITE_ARRAY(REFERENCE byteString, bytes,    15);
            bytes = (time & 0x00ff000000000000) >> 48;
            WRITE_ARRAY(REFERENCE byteString, bytes,    16);
            bytes = (time & 0x0000ff0000000000) >> 40;
            WRITE_ARRAY(REFERENCE byteString, bytes,    17);
            bytes = (time & 0x000000ff00000000) >> 32;
            WRITE_ARRAY(REFERENCE byteString, bytes,    18);
            bytes = (time & 0x00000000ff000000) >> 24;
            WRITE_ARRAY(REFERENCE byteString, bytes,    19);
            bytes = (time & 0x0000000000ff0000) >> 16;
            WRITE_ARRAY(REFERENCE byteString, bytes,    20);
            bytes = (time & 0x000000000000ff00) >> 8;
            WRITE_ARRAY(REFERENCE byteString, bytes,    21);
            bytes = (time & 0x00000000000000ff);
            WRITE_ARRAY(REFERENCE byteString, bytes,    22);
        }
    }

    else if (mclmac->_state == 2)
    {
        // Store node id
        bytes = (ARROW(mclmac->mac)transmiterID & 0xff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    1);
        bytes = (ARROW(mclmac->mac)transmiterID & 0x00ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    2);
    }

    ControlPacket_t *pkt = REFERENCE ARROW(mclmac->mac)ctrlpkt_recv;
    controlpacket_construct_packet_from_bytestring(pkt, &byteString);
#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
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

void stub_mclmac_receive_data_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    ARRAY byteString;
#ifdef __LINUX__
    byteString = (uint8_t *)malloc(PACKET_SIZE_MAC * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteString, PACKET_SIZE_MAC);
#endif
    WRITE_ARRAY(REFERENCE byteString, 2, 0);
    uint8_t bytes = 0;
    bytes = (mclmac->_nodeID & 0xff00) >> 8;
    WRITE_ARRAY(REFERENCE byteString, bytes, 1);
    bytes = (mclmac->_nodeID & 0x00ff);
    WRITE_ARRAY(REFERENCE byteString, bytes, 2);
    for (int i = 3; i < PACKET_SIZE_MAC; i++)
    {
        WRITE_ARRAY(REFERENCE byteString, rand(), i);
    }
    uint pos = ARROW(mclmac->mac)_last_received;
    DataPacket_t *pkt = &ARROW(mclmac->mac)_packets_received[pos];
    datapacket_construct_from_bytestring(pkt, &byteString);
    ARROW(mclmac->mac)_last_received++;
    ARROW(mclmac->mac)_number_packets_received++;
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
