/* Implement all the handles for the different packets. */
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

void mclmac_start_packet_detection(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

#ifdef __RIOT__
    uint8_t broadcast_addr[NRF24L01P_NG_ADDR_WIDTH] = NRF24L01P_NG_BROADCAST_ADDR;
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_ADDRESS,
                                    (void *)broadcast_addr, NRF24L01P_NG_ADDR_WIDTH);
    uint16_t channel = (uint16_t)mclmac->mac.cfChannel;
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_CHANNEL,
                                    (void *)&channel, sizeof(uint16_t));
    netopt_state_t state = NETOPT_STATE_STANDBY;
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&state, sizeof(netopt_state_t));
    state = NETOPT_STATE_RX;
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&state, sizeof(netopt_state_t));
#endif
}

/**
 * @brief Check the presence of a valid cf packet. Use for the INITIALIZATION state.
 * 
 * @param mclmac 
 */
bool mclmac_cf_packet_detected(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

#ifdef __LINUX__
    int timeout = 0;
#endif
#ifdef __RIOT__
    uint32_t timeout = 0;
#endif
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
#ifdef __RIOT__
    int size = mclmac->mac.netdev->driver->recv(mclmac->mac.netdev, NULL, 
                                                NRF24L01P_NG_MAX_PAYLOAD_WIDTH, NULL);
    if (size == 0)
    {
        return false;
    }
    else if (size > 0 && size < 32)
    {
        return false;
    }
    else if (size == 32)
    {
        return true;
    }
#endif
    return false;
}

void mclmac_send_cf_message(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif
    
    // Change radio to standby
    mclmac_set_radio_standby(mclmac);
    CFPacket_t *pkt = REFERENCE ARROW(mclmac->mac)_cf_messages[0];
    ARRAY byte_string;
    cfpacket_get_packet_bytestring(pkt, &byte_string);
    uint8_t packet[NRF24L01P_NG_MAX_PAYLOAD_WIDTH] = {0};
    for (int i = 0; i < NRF24L01P_NG_MAX_PAYLOAD_WIDTH; i++)
    {
        packet[i] = READ_ARRAY(REFERENCE byte_string, i);
    }
    uint64_t addr[NRF24L01P_NG_ADDR_WIDTH] = NRF24L01P_NG_BROADCAST_ADDR;
#ifdef __RIOT__
    iolist_t data = {
        .iol_next = NULL,
        .iol_base = packet,
        .iol_len = sizeof(packet)
    };
    iolist_t list = {
        .iol_next = &data,
        .iol_base = (void *)addr,
        .iol_len = sizeof(addr)
    };
    int res;
    /*while ((res = mclmac->mac.netdev->driver->send(mclmac->mac.netdev, &list)) < 0)
    {
        if (res == -EAGAIN) {
            continue;
        }
        else if (res == -EBUSY) {
            continue;
        }
        break;
    }*/
#endif
    mclmac_set_radio_standby(mclmac);
    // Sleep for a while before returnint
#ifdef __LINUX__
    usleep(1);
    free(byte_string);
#endif
#ifdef __RIOT__
    ztimer_sleep(ZTIMER_USEC, 1);
    free_array(&byte_string);
#endif
}

bool stub_mclmac_receive_ctrlpkt_sync(MCLMAC_t *mclmac, ControlPacket_t *ctrlpkt)
{
    assert(mclmac != NULL);
    assert(ctrlpkt != NULL);

    uint32_t frame = ARROW(ARROW(mclmac->mac)frame)current_frame;
    uint8_t slot = ARROW(ARROW(mclmac->mac)frame)current_slot;
    uint8_t hopCount = mclmac->_hopCount;
    uint32_t network_time = mclmac->_networkTime;
    uint32_t init_time = mclmac->_initTime;

    controlpacket_set_current_frame(ctrlpkt, frame);
    controlpacket_set_current_slot(ctrlpkt, slot);
    controlpacket_set_hop_count(ctrlpkt, hopCount);
    controlpacket_set_network_time(ctrlpkt, network_time);
    controlpacket_set_init_time(ctrlpkt, init_time);

    if (rand() > 128)
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
bool mclmac_receive_cf_message(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    if (mclmac->_state_cf == 1 || mclmac->_state_cf == 3)
    {
        ARROW(mclmac->mac)_cf_message_received = false;
        return false;
    }

    uint64_t node_id[2] = {0};
    uint64_t id[2] = {0};
    if (mclmac->_state_cf == 2)
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
            ARROW(mclmac->mac)_cf_message_received = false;
            return false;
        }
        ARROW(mclmac->mac)_cf_message_received = (rand() % 1024 > 256 ? true : false);
        CFPacket_t *pkt = &ARROW(mclmac->mac)_cf_messages[1];
        if (ARROW(mclmac->mac)_cf_message_received)
        {
            id[0] = rand();
            id[1] = rand();
            mclmac_get_nodeid(mclmac, node_id);
            cfpacket_create(pkt, id, node_id);
            mclmac->_trues5++;
        }
    }
#ifdef __RIOT__
    /*int frame_len = mclmac->mac.netdev->driver->recv(mclmac->mac.netdev, NULL, 1, NULL);
    mclmac->mac._cf_message_receive = false;
    if (frame_len == 0) {
        return false;
    }
    else if (frame_len > 0 && frame_len < NRF24L01P_NG_MAX_PAYLOAD_WIDTH) {
        return false;
    }
    else if (frame_len == NRF24L01P_NG_MAX_PAYLOAD_WIDTH)
    {
        uint8_t frame[NRF24L01P_NG_MAX_PAYLOAD_WIDTH] = {0};
        frame_len = mclmac->mac.netdev->driver->recv(mclmac->mac.netdev, (void *) frame,
                                                        frame_len, NULL);
        if (frame_len < NRF24L01P_NG_MAX_PAYLOAD_WIDTH)
            return false;
        mclmac->mac._cf_message_received = true;
        array_t byte_string;
        create_array(&byte_string, frame_len);
        for (int i = 0; i < frame_len; i++)
        {
            write_element(&byte_string, frame[i], i);
        }
        cfpacket_construct_from_bytestring(pkt, byte_string);
        free_array(&byte_string);
    }*/
#endif

    return ARROW(mclmac->mac)_cf_message_received;
}

void mclmac_create_control_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    ControlPacket_t *pkt = REFERENCE ARROW(mclmac->mac)ctrlpkt;
    controlpacket_clear(pkt);

    // Get the relevant data.
    uint64_t node_id[2] = {0};
    node_id[0] = mclmac->_node_id[0];
    node_id[1] = mclmac->_node_id[1];
    uint32_t currentFrame = ARROW(ARROW(mclmac->mac)frame)current_frame;
    uint8_t currentSlot = ARROW(ARROW(mclmac->mac)frame)current_slot;
    uint8_t collisionSlot = ARROW(mclmac->mac)_collisionSlot;
    uint32_t collisionFrequency = ARROW(mclmac->mac)_collisionFrequency;
    uint16_t hopCount = mclmac->_hopCount;
    uint64_t networkTime = mclmac->_networkTime;
    uint32_t initTime = mclmac->_initTime;

    controlpacket_create(pkt, node_id, currentFrame, currentSlot, collisionSlot, collisionFrequency,
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
        WRITE_ARRAY(REFERENCE byteString, rand() % 250, i);
    uint8_t bytes = 1;
    // Store type
    WRITE_ARRAY(REFERENCE byteString, bytes,    0);
    if (mclmac->_state_ctrl == 1 || mclmac->_state_ctrl == 3 || mclmac->_state_ctrl == 4 || mclmac->_state_ctrl == 5 || mclmac->_state_ctrl == 6)
    {
        // Store node id
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0xff00000000000000) >> 56;
        WRITE_ARRAY(REFERENCE byteString, bytes,    1);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x00ff000000000000) >> 48;
        WRITE_ARRAY(REFERENCE byteString, bytes,    2);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x0000ff0000000000) >> 40;
        WRITE_ARRAY(REFERENCE byteString, bytes,    3);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x000000ff00000000) >> 32;
        WRITE_ARRAY(REFERENCE byteString, bytes,    4);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x00000000ff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    5);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x0000000000ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    6);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x000000000000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    7);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x00000000000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    8);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0xff00000000000000) >> 56;
        WRITE_ARRAY(REFERENCE byteString, bytes,    9);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x00ff000000000000) >> 48;
        WRITE_ARRAY(REFERENCE byteString, bytes,    10);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x0000ff0000000000) >> 40;
        WRITE_ARRAY(REFERENCE byteString, bytes,    11);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x000000ff00000000) >> 32;
        WRITE_ARRAY(REFERENCE byteString, bytes,    12);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x00000000ff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    13);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x0000000000ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    14);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x000000000000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    15);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x00000000000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    16);
        // Store current frame
        uint32_t frame = ARROW(ARROW(mclmac->mac)frame)current_frame;
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
            bytes = ARROW(mclmac->mac)selectedSlot;
            WRITE_ARRAY(REFERENCE byteString, bytes,    22);
            // Store collision frequency
            uint32_t freq = ARROW(mclmac->mac)transmitChannel;
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
            uint32_t frame = ARROW(ARROW(mclmac->mac)frame)current_frame + 1;
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
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0xff00000000000000) >> 56;
        WRITE_ARRAY(REFERENCE byteString, bytes,    1);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x00ff000000000000) >> 48;
        WRITE_ARRAY(REFERENCE byteString, bytes,    2);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x0000ff0000000000) >> 40;
        WRITE_ARRAY(REFERENCE byteString, bytes,    3);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x000000ff00000000) >> 32;
        WRITE_ARRAY(REFERENCE byteString, bytes,    4);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x00000000ff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    5);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x0000000000ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    6);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x000000000000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    7);
        bytes = (ARROW(mclmac->mac)transmitter_id[0] & 0x00000000000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    8);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0xff00000000000000) >> 56;
        WRITE_ARRAY(REFERENCE byteString, bytes,    9);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x00ff000000000000) >> 48;
        WRITE_ARRAY(REFERENCE byteString, bytes,    10);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x0000ff0000000000) >> 40;
        WRITE_ARRAY(REFERENCE byteString, bytes,    11);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x000000ff00000000) >> 32;
        WRITE_ARRAY(REFERENCE byteString, bytes,    12);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x00000000ff000000) >> 24;
        WRITE_ARRAY(REFERENCE byteString, bytes,    13);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x0000000000ff0000) >> 16;
        WRITE_ARRAY(REFERENCE byteString, bytes,    14);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x000000000000ff00) >> 8;
        WRITE_ARRAY(REFERENCE byteString, bytes,    15);
        bytes = (ARROW(mclmac->mac)transmitter_id[1] & 0x00000000000000ff);
        WRITE_ARRAY(REFERENCE byteString, bytes,    16);
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
    bytes = (mclmac->_node_id[0] & 0xff00000000000000) >> 56;
    WRITE_ARRAY(REFERENCE byteString, bytes,    1);
    bytes = (mclmac->_node_id[0] & 0x00ff000000000000) >> 48;
    WRITE_ARRAY(REFERENCE byteString, bytes,    2);
    bytes = (mclmac->_node_id[0] & 0x0000ff0000000000) >> 40;
    WRITE_ARRAY(REFERENCE byteString, bytes,    3);
    bytes = (mclmac->_node_id[0] & 0x000000ff00000000) >> 32;
    WRITE_ARRAY(REFERENCE byteString, bytes,    4);
    bytes = (mclmac->_node_id[0] & 0x00000000ff000000) >> 24;
    WRITE_ARRAY(REFERENCE byteString, bytes,    5);
    bytes = (mclmac->_node_id[0] & 0x0000000000ff0000) >> 16;
    WRITE_ARRAY(REFERENCE byteString, bytes,    6);
    bytes = (mclmac->_node_id[0] & 0x000000000000ff00) >> 8;
    WRITE_ARRAY(REFERENCE byteString, bytes,    7);
    bytes = (mclmac->_node_id[0] & 0x00000000000000ff);
    WRITE_ARRAY(REFERENCE byteString, bytes,    8);
    bytes = (mclmac->_node_id[1] & 0xff00000000000000) >> 56;
    WRITE_ARRAY(REFERENCE byteString, bytes,    9);
    bytes = (mclmac->_node_id[1] & 0x00ff000000000000) >> 48;
    WRITE_ARRAY(REFERENCE byteString, bytes,    10);
    bytes = (mclmac->_node_id[1] & 0x0000ff0000000000) >> 40;
    WRITE_ARRAY(REFERENCE byteString, bytes,    11);
    bytes = (mclmac->_node_id[1] & 0x000000ff00000000) >> 32;
    WRITE_ARRAY(REFERENCE byteString, bytes,    12);
    bytes = (mclmac->_node_id[1] & 0x00000000ff000000) >> 24;
    WRITE_ARRAY(REFERENCE byteString, bytes,    13);
    bytes = (mclmac->_node_id[1] & 0x0000000000ff0000) >> 16;
    WRITE_ARRAY(REFERENCE byteString, bytes,    14);
    bytes = (mclmac->_node_id[1] & 0x000000000000ff00) >> 8;
    WRITE_ARRAY(REFERENCE byteString, bytes,    15);
    bytes = (mclmac->_node_id[1] & 0x00000000000000ff);
    WRITE_ARRAY(REFERENCE byteString, bytes,    16);
    uint8_t size = rand() % (PACKET_SIZE_MAC - 17);
    WRITE_ARRAY(REFERENCE byteString, size, 17);
    for (int i = 18; i < PACKET_SIZE_MAC; i++)
    {
        WRITE_ARRAY(REFERENCE byteString, rand(), i);
    }
    uint pos = ARROW(mclmac->mac)_last_received;
    DataPacket_t *pkt = &ARROW(mclmac->mac)_packets_received[pos];
    datapacket_construct_from_bytestring(pkt, &byteString);
    ARROW(mclmac->mac)_last_received++;
    ARROW(mclmac->mac)_number_packets_received++;
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
