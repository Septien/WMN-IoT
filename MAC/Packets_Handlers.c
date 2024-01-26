/* Implement all the handles for the different packets. */
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

/* Stub functions for testing with native. */
bool _cf_packet_detected_native(MCLMAC_t *mclmac);
void _receive_ctrlpkt_sync_native(MCLMAC_t *mclmac, ControlPacket_t *ctrlpkt);
bool _receive_cf_message_native(MCLMAC_t *mclmac);
void _receive_control_packet_native(MCLMAC_t *mclmac);
void _receive_data_packet_native(MCLMAC_t *mclmac, ARRAY *byteString);

void mclmac_start_packet_detection(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    (void) mclmac;
    return;
}

/**
 * @brief Check the presence of a valid cf packet. Use for the INITIALIZATION state.
 * 
 * @param mclmac 
 */
bool mclmac_cf_packet_detected(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return _cf_packet_detected_native(mclmac);
}

void mclmac_send_cf_message(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif
    
    // Change radio to standby
    mclmac_set_radio_standby(mclmac);
    CFPacket_t *pkt = &mclmac->mac._cf_messages[0];
    ARRAY byte_string;
    cfpacket_get_packet_bytestring(pkt, &byte_string);
    // Return radio to rx
    mclmac_set_radio_standby(mclmac);
    // Sleep for a while before returning
#ifdef __LINUX__
    usleep(1);
    free(byte_string);
#endif
#ifdef __RIOT__
    ztimer_sleep(ZTIMER_USEC, 1);
    free_array(&byte_string);
#endif
}

bool mclmac_receive_ctrlpkt_sync(MCLMAC_t *mclmac, ControlPacket_t *ctrlpkt)
{
    assert(mclmac != NULL);
    assert(ctrlpkt != NULL);

    if (mclmac->_state_ctrl == 1) {
        return false;
    }
    _receive_ctrlpkt_sync_native(mclmac, ctrlpkt);

    return true;
}

bool mclmac_receive_cf_message(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return _receive_cf_message_native(mclmac);
}

void mclmac_create_control_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    ControlPacket_t *pkt = &mclmac->mac.ctrlpkt;
    controlpacket_clear(pkt);

    // Get the relevant data.
    uint64_t node_id[2] = {0};
    node_id[0] = mclmac->_node_id[0];
    node_id[1] = mclmac->_node_id[1];
    uint32_t currentFrame = mclmac->mac.frame.current_frame;
    uint8_t currentSlot = mclmac->mac.frame.current_slot;
    uint8_t collisionSlot = mclmac->mac._collisionSlot;
    uint32_t collisionFrequency = mclmac->mac._collisionFrequency;
    uint16_t hopCount = mclmac->_hopCount;
    uint64_t networkTime = mclmac->_networkTime;
    uint32_t initTime = mclmac->_initTime;

    controlpacket_create(pkt, node_id, currentFrame, currentSlot, collisionSlot, collisionFrequency,
                        hopCount, networkTime, initTime, (uint8_t *)mclmac->_occupied_frequencies_slots);
}

void mclmac_send_control_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac.ctrlpkt != NULL);
#endif

    ControlPacket_t *pkt = &mclmac->mac.ctrlpkt;
    ARRAY byteStr;

    controlpacket_get_packet_bytestring(pkt, &byteStr);

    controlpacket_clear(pkt);

#ifdef __LINUX__
    free(byteStr);
#endif
#ifdef __RIOT__
    free_array(&byteStr);
#endif
}

bool mclmac_receive_control_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    _receive_control_packet_native(mclmac);

    return true;
}

void mclmac_send_data_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    if (mclmac->mac._packets_to_send_message == 0 && mclmac->mac._packets_to_send_control == 0)
        return;
    if (mclmac->mac._first_send_message > MAX_NUMBER_DATA_PACKETS)
        return;
    if (mclmac->mac._first_send_control > MAX_NUMBER_DATA_PACKETS)
        return;

    // Iterate over all packets on both queues,sending first the control packets
    if (mclmac->mac._packets_to_send_control > 0)
    {
        // Get the packet pointed to by _first_send_control
        uint8_t first_send = mclmac->mac._first_send_control;
        DataPacket_t *pkt = &mclmac->mac._control_packets_to_send[first_send];
        // Get the byte string
        ARRAY byteString;
        datapacket_get_packet_bytestring(pkt, &byteString);

        datapacket_clear(pkt);
        first_send = (first_send + 1) % MAX_NUMBER_DATA_PACKETS;
        mclmac->mac._first_send_control = first_send;
        mclmac->mac._packets_to_send_control--;
#ifdef __LINUX__
        free(byteString);
#endif
#ifdef __RIOT__
        free_array(&byteString);
#endif
    }

    else if (mclmac->mac._packets_to_send_message > 0)
    {
        // Get the packet pointed to by _first_send_message
        uint8_t first_send = mclmac->mac._first_send_message;
        DataPacket_t *pkt = &mclmac->mac._message_packets_to_send[first_send];
        // Get the byte string
        ARRAY byteString;
        datapacket_get_packet_bytestring(pkt, &byteString);
        datapacket_clear(pkt);
        first_send = (first_send + 1) % MAX_NUMBER_DATA_PACKETS;
        mclmac->mac._first_send_message = first_send;
        mclmac->mac._packets_to_send_message--;
#ifdef __LINUX__
        free(byteString);
#endif
#ifdef __RIOT__
        free_array(&byteString);
#endif
    }
}

bool mclmac_receive_data_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    ARRAY byteString;
#ifdef __LINUX__
    byteString = (uint8_t *)malloc(PACKET_SIZE_MAC * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteString, PACKET_SIZE_MAC);
#endif
    int pos = 0;
    _receive_data_packet_native(mclmac, &byteString);
    pos = mclmac->mac._last_received;
    DataPacket_t *pkt = &mclmac->mac._packets_received[pos];
    datapacket_construct_from_bytestring(pkt, &byteString);
    pos = (pos + 1) % (2 * MAX_NUMBER_DATA_PACKETS);
    mclmac->mac._last_received = pos;
    mclmac->mac._number_packets_received++;
#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
#endif
    return true;
}

bool _cf_packet_detected_native(MCLMAC_t *mclmac)
{
#ifdef __LINUX__
    int timeout = 0;
#endif
#ifdef __RIOT__
    uint32_t timeout = 0;
#endif
#ifdef TESTING
    /* For making the function fail. */
    if (mclmac->_init_state == 0)
    {
        /* Sleep for 1.1 second or more and pretend a valid packet is found. */
        timeout = timeout_set(TIME(1100000));
        while(timeout_passed(timeout) != 1) ;
        timeout_unset(timeout);
        return false;
    }
    else if (mclmac->_init_state == 1)
    {
        /* Sleep for less than 1 second, and pretend a valid packet is found. */
        timeout = timeout_set(TIME(900000));
        while(timeout_passed(timeout) != 1) ;
        timeout_unset(timeout);
        return true;
    }
#endif
    return true;
}

void _receive_ctrlpkt_sync_native(MCLMAC_t *mclmac, ControlPacket_t *ctrlpkt)
{
    uint32_t frame = mclmac->mac.frame.current_frame;
    uint8_t slot = mclmac->mac.frame.current_slot;
    uint8_t hopCount = mclmac->_hopCount;
    uint32_t network_time = mclmac->_networkTime;
    uint32_t init_time = mclmac->_initTime;
    uint64_t node_id[2] = {0};

    int freq = rand() % MAX_NUMBER_FREQS;
    uint8_t bit = 0;
    bit |= 1 << (slot % 8);
    mclmac->_occupied_frequencies_slots[freq][slot / 8] |= bit;
    controlpacket_create(ctrlpkt, node_id, frame, slot, NO_COLLISION_SLOT, NO_COLLISION_FREQ,
                        hopCount, network_time, init_time, (uint8_t *)mclmac->_occupied_frequencies_slots);
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
bool _receive_cf_message_native(MCLMAC_t *mclmac)
{
#ifdef TESTING
    if (mclmac->_state_cf == 1 || mclmac->_state_cf == 3)
    {
        mclmac->mac._cf_message_received = false;
        return false;
    }

    uint64_t node_id[2] = {0};
    uint64_t id[2] = {0};
    if (mclmac->_state_cf == 2)
    {
        if (mclmac->_trues == 1)
        {
            mclmac->mac._cf_message_received = false;
            return false;
        }

        mclmac->mac._cf_message_received = true;
        CFPacket_t *pkt = &mclmac->mac._cf_messages[1];
        if (mclmac->mac._cf_message_received)
        {
            id[0] = rand();
            id[1] = rand();
            mclmac_get_nodeid(mclmac, node_id);
            cfpacket_create(pkt, id, node_id);
            mclmac->_trues++;
        }
        else
        {
            cfpacket_create(pkt, id, node_id);
        }
    }

    if (mclmac->_state_cf == 4)
    {
        if (mclmac_get_current_cf_slot(mclmac) == 0)
        {
            mclmac->mac._cf_message_received = false;
            return false;
        }
        if (mclmac->_trues == 2)
        {
            mclmac->mac._cf_message_received = false;
            return false;
        }
        mclmac->mac._cf_message_received = true;
        CFPacket_t *pkt = &mclmac->mac._cf_messages[1];
        if (mclmac->mac._cf_message_received)
        {
            id[0] = rand();
            id[1] = rand();
            mclmac_get_nodeid(mclmac, node_id);
            cfpacket_create(pkt, id, node_id);
            mclmac->_trues++;
        }
    }

    if (mclmac->_state_cf == 5)
    {
        if (mclmac->_trues5 >= 2)
        {
            mclmac->mac._cf_message_received = false;
            return false;
        }
        mclmac->mac._cf_message_received = (rand() % 1024 > 256 ? true : false);
        CFPacket_t *pkt = &mclmac->mac._cf_messages[1];
        if (mclmac->mac._cf_message_received)
        {
            id[0] = rand();
            id[1] = rand();
            mclmac_get_nodeid(mclmac, node_id);
            cfpacket_create(pkt, id, node_id);
            mclmac->_trues5++;
        }
    }
#endif
    return true;
}

void _receive_control_packet_native(MCLMAC_t *mclmac)
{
    ARRAY byteString;
#ifdef __LINUX__
    byteString = (uint8_t *)malloc(MAX_MESSAGE_SIZE * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteString, MAX_MESSAGE_SIZE);
#endif

    for (uint i = 0; i < MAX_MESSAGE_SIZE; i++)
        WRITE_ARRAY(REFERENCE byteString, rand() % 250, i);
    uint8_t bytes = 1;
    // Store type
    WRITE_ARRAY(REFERENCE byteString, bytes,    0);
#ifdef TESTING
    if (mclmac->_state_ctrl == 1 || mclmac->_state_ctrl == 3 || mclmac->_state_ctrl == 4 || mclmac->_state_ctrl == 5 || mclmac->_state_ctrl == 6)
    {
        // Store node id
        bytes = (mclmac->mac.transmitter_id[0] & 0xff00000000000000) >> 56;
        WRITE_ARRAY(REFERENCE byteString, bytes,    1);
        bytes = (mclmac->mac.transmitter_id[0] & 0x00ff000000000000) >> 48;
        WRITE_ARRAY(REFERENCE byteString, bytes,    2);
        bytes = (mclmac->mac.transmitter_id[0] & 0x0000ff0000000000) >> 40;
        WRITE_ARRAY(REFERENCE byteString, bytes,    3);
        bytes = (mclmac->mac.transmitter_id[0] & 0x000000ff00000000) >> 32;
        WRITE_ARRAY(REFERENCE byteString, bytes,    4);
        bytes = (mclmac->mac.transmitter_id[0] & 0x00000000ff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    5);
        bytes = (mclmac->mac.transmitter_id[0] & 0x0000000000ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    6);
        bytes = (mclmac->mac.transmitter_id[0] & 0x000000000000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    7);
        bytes = (mclmac->mac.transmitter_id[0] & 0x00000000000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    8);
        bytes = (mclmac->mac.transmitter_id[1] & 0xff00000000000000) >> 56;
        WRITE_ARRAY(REFERENCE byteString, bytes,    9);
        bytes = (mclmac->mac.transmitter_id[1] & 0x00ff000000000000) >> 48;
        WRITE_ARRAY(REFERENCE byteString, bytes,    10);
        bytes = (mclmac->mac.transmitter_id[1] & 0x0000ff0000000000) >> 40;
        WRITE_ARRAY(REFERENCE byteString, bytes,    11);
        bytes = (mclmac->mac.transmitter_id[1] & 0x000000ff00000000) >> 32;
        WRITE_ARRAY(REFERENCE byteString, bytes,    12);
        bytes = (mclmac->mac.transmitter_id[1] & 0x00000000ff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    13);
        bytes = (mclmac->mac.transmitter_id[1] & 0x0000000000ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    14);
        bytes = (mclmac->mac.transmitter_id[1] & 0x000000000000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    15);
        bytes = (mclmac->mac.transmitter_id[1] & 0x00000000000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    16);
        // Store current frame
        uint32_t frame = mclmac->mac.frame.current_frame;
        bytes = (frame & 0xff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    17);
        bytes = (frame & 0x00ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    18);
        bytes = (frame & 0x0000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    19);
        bytes = (frame & 0x000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    20);
        // Store current slot
        uint8_t slot = mclmac_get_current_slot(mclmac);
        WRITE_ARRAY(REFERENCE byteString, slot,     21);
        // Store collision slot, 0
        WRITE_ARRAY(REFERENCE byteString, 0,        22);
        // Store collision frequency, 0
        WRITE_ARRAY(REFERENCE byteString, 0,        23);
        WRITE_ARRAY(REFERENCE byteString, 0,        24);
        WRITE_ARRAY(REFERENCE byteString, 0,        25);
        WRITE_ARRAY(REFERENCE byteString, 0,        26);
        // Store hop count
        uint16_t hops = mclmac->_hopCount;
        bytes = (hops & 0xff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    27);
        bytes = (hops & 0x00ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    28);
        // Store network time
        uint64_t time = mclmac->_networkTime;
        bytes = (time & 0xff00000000000000) >> 56;
        WRITE_ARRAY(REFERENCE byteString, bytes,    29);
        bytes = (time & 0x00ff000000000000) >> 48;
        WRITE_ARRAY(REFERENCE byteString, bytes,    30);
        bytes = (time & 0x0000ff0000000000) >> 40;
        WRITE_ARRAY(REFERENCE byteString, bytes,    31);
        bytes = (time & 0x000000ff00000000) >> 32;
        WRITE_ARRAY(REFERENCE byteString, bytes,    32);
        bytes = (time & 0x00000000ff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    33);
        bytes = (time & 0x0000000000ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    34);
        bytes = (time & 0x000000000000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    35);
        bytes = (time & 0x00000000000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    36);
        // Store init time
        uint32_t init_time = mclmac->_initTime;
        bytes = (init_time & 0xff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    37);
        bytes = (init_time & 0x00ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    38);
        bytes = (init_time & 0x0000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    39);
        bytes = (init_time & 0x000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    40);
        if (mclmac->_state_ctrl == 3)
        {
            // Store collision slot
            bytes = mclmac->mac.selectedSlot;
            WRITE_ARRAY(REFERENCE byteString, bytes,    22);
            // Store collision frequency
            uint32_t freq = mclmac->mac.transmitChannel;
            bytes = (freq & 0xff000000) >> 24;
            WRITE_ARRAY(REFERENCE byteString, bytes,    23);
            bytes = (freq & 0x00ff0000) >> 16;
            WRITE_ARRAY(REFERENCE byteString, bytes,    24);
            bytes = (freq & 0x0000ff00) >> 8;
            WRITE_ARRAY(REFERENCE byteString, bytes,    25);
            bytes = (freq & 0x000000ff);
            WRITE_ARRAY(REFERENCE byteString, bytes,    26);
        }
        else if (mclmac->_state_ctrl == 4)
        {
            WRITE_ARRAY(REFERENCE byteString, slot + 1, 21);
        }
        else if (mclmac->_state_ctrl == 5)
        {
            uint32_t frame = mclmac->mac.frame.current_frame + 1;
            bytes = (frame & 0xff000000) >> 24;
            WRITE_ARRAY(REFERENCE byteString, bytes,    17);
            bytes = (frame & 0x00ff0000) >> 16;
            WRITE_ARRAY(REFERENCE byteString, bytes,    18);
            bytes = (frame & 0x0000ff00) >> 8;
            WRITE_ARRAY(REFERENCE byteString, bytes,    19);
            bytes = (frame & 0x000000ff);
            WRITE_ARRAY(REFERENCE byteString, bytes,    20);
        }
        else if (mclmac->_state_ctrl == 6)
        {
            uint64_t time = mclmac->_networkTime + NETWORK_TIME_EPSILON + 1;
            bytes = (time & 0xff00000000000000) >> 56;
            WRITE_ARRAY(REFERENCE byteString, bytes,    29);
            bytes = (time & 0x00ff000000000000) >> 48;
            WRITE_ARRAY(REFERENCE byteString, bytes,    30);
            bytes = (time & 0x0000ff0000000000) >> 40;
            WRITE_ARRAY(REFERENCE byteString, bytes,    31);
            bytes = (time & 0x000000ff00000000) >> 32;
            WRITE_ARRAY(REFERENCE byteString, bytes,    32);
            bytes = (time & 0x00000000ff000000) >> 24;
            WRITE_ARRAY(REFERENCE byteString, bytes,    33);
            bytes = (time & 0x0000000000ff0000) >> 16;
            WRITE_ARRAY(REFERENCE byteString, bytes,    34);
            bytes = (time & 0x000000000000ff00) >> 8;
            WRITE_ARRAY(REFERENCE byteString, bytes,    35);
            bytes = (time & 0x00000000000000ff);
            WRITE_ARRAY(REFERENCE byteString, bytes,    36);
        }
    }

    else if (mclmac->_state_ctrl == 2)
    {
        // Store node id
        bytes = (mclmac->mac.transmitter_id[0] & 0xff00000000000000) >> 56;
        WRITE_ARRAY(REFERENCE byteString, bytes,    1);
        bytes = (mclmac->mac.transmitter_id[0] & 0x00ff000000000000) >> 48;
        WRITE_ARRAY(REFERENCE byteString, bytes,    2);
        bytes = (mclmac->mac.transmitter_id[0] & 0x0000ff0000000000) >> 40;
        WRITE_ARRAY(REFERENCE byteString, bytes,    3);
        bytes = (mclmac->mac.transmitter_id[0] & 0x000000ff00000000) >> 32;
        WRITE_ARRAY(REFERENCE byteString, bytes,    4);
        bytes = (mclmac->mac.transmitter_id[0] & 0x00000000ff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    5);
        bytes = (mclmac->mac.transmitter_id[0] & 0x0000000000ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    6);
        bytes = (mclmac->mac.transmitter_id[0] & 0x000000000000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    7);
        bytes = (mclmac->mac.transmitter_id[0] & 0x00000000000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    8);
        bytes = (mclmac->mac.transmitter_id[1] & 0xff00000000000000) >> 56;
        WRITE_ARRAY(REFERENCE byteString, bytes,    9);
        bytes = (mclmac->mac.transmitter_id[1] & 0x00ff000000000000) >> 48;
        WRITE_ARRAY(REFERENCE byteString, bytes,    10);
        bytes = (mclmac->mac.transmitter_id[1] & 0x0000ff0000000000) >> 40;
        WRITE_ARRAY(REFERENCE byteString, bytes,    11);
        bytes = (mclmac->mac.transmitter_id[1] & 0x000000ff00000000) >> 32;
        WRITE_ARRAY(REFERENCE byteString, bytes,    12);
        bytes = (mclmac->mac.transmitter_id[1] & 0x00000000ff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    13);
        bytes = (mclmac->mac.transmitter_id[1] & 0x0000000000ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    14);
        bytes = (mclmac->mac.transmitter_id[1] & 0x000000000000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    15);
        bytes = (mclmac->mac.transmitter_id[1] & 0x00000000000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    16);
    }
#endif
    ControlPacket_t *pkt = &mclmac->mac.ctrlpkt_recv;
    controlpacket_construct_packet_from_bytestring(pkt, &byteString);
}

void _receive_data_packet_native(MCLMAC_t *mclmac, ARRAY *byteString)
{
    WRITE_ARRAY(SINGLE_POINTER byteString, 2, 0);
    uint8_t bytes = 0;
    bytes = (mclmac->_node_id[0] & 0xff00000000000000) >> 56;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    1);
    bytes = (mclmac->_node_id[0] & 0x00ff000000000000) >> 48;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    2);
    bytes = (mclmac->_node_id[0] & 0x0000ff0000000000) >> 40;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    3);
    bytes = (mclmac->_node_id[0] & 0x000000ff00000000) >> 32;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    4);
    bytes = (mclmac->_node_id[0] & 0x00000000ff000000) >> 24;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    5);
    bytes = (mclmac->_node_id[0] & 0x0000000000ff0000) >> 16;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    6);
    bytes = (mclmac->_node_id[0] & 0x000000000000ff00) >> 8;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    7);
    bytes = (mclmac->_node_id[0] & 0x00000000000000ff);
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    8);
    bytes = (mclmac->_node_id[1] & 0xff00000000000000) >> 56;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    9);
    bytes = (mclmac->_node_id[1] & 0x00ff000000000000) >> 48;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    10);
    bytes = (mclmac->_node_id[1] & 0x0000ff0000000000) >> 40;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    11);
    bytes = (mclmac->_node_id[1] & 0x000000ff00000000) >> 32;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    12);
    bytes = (mclmac->_node_id[1] & 0x00000000ff000000) >> 24;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    13);
    bytes = (mclmac->_node_id[1] & 0x0000000000ff0000) >> 16;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    14);
    bytes = (mclmac->_node_id[1] & 0x000000000000ff00) >> 8;
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    15);
    bytes = (mclmac->_node_id[1] & 0x00000000000000ff);
    WRITE_ARRAY(SINGLE_POINTER byteString, bytes,    16);
    uint8_t size = PACKET_SIZE_MAC - 18;
    WRITE_ARRAY(SINGLE_POINTER byteString, size, 17);
    for (int i = 18; i < PACKET_SIZE_MAC; i++)
    {
        WRITE_ARRAY(SINGLE_POINTER byteString, rand(), i);
    }
}
