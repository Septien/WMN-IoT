/* The implementation of the powermode state machine and directly related functions. */
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

void mclmac_init_powermode_state_machine(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    mclmac->powerMode.currentState = STARTP;
    mclmac->powerMode.nextState = NONEP;
}

void mclmac_set_powermode_state(MCLMAC_t *mclmac, PowerMode_t mode)
{
    assert(mclmac != NULL);
 
    mclmac->powerMode.currentState = mode;
}

void mclmac_set_next_powermode_state(MCLMAC_t *mclmac, PowerMode_t next)
{
    assert(mclmac != NULL);
    assert(next != STARTP);

    mclmac->powerMode.nextState = next;
}

int mclmac_update_powermode_state_machine(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    if (mclmac->powerMode.currentState == mclmac->powerMode.nextState)
        return E_PM_NO_TRANSITION;
 
    switch (mclmac->powerMode.nextState)
    {
    case NONEP:
        mclmac_set_powermode_state(mclmac, NONEP);
        return E_PM_NO_TRANSITION;
    case PASSIVE:
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        mclmac_set_powermode_state(mclmac, PASSIVE);
        break;
    
    case ACTIVE:
        if (mclmac->powerMode.currentState == PASSIVE)
            mclmac_set_powermode_state(mclmac, ACTIVE);
        else
            return E_PM_TRANSITION_ERROR;
        
        break;

    case TRANSMIT:
        if (mclmac->powerMode.currentState == ACTIVE)
            mclmac_set_powermode_state(mclmac, TRANSMIT);
        else
            return E_PM_TRANSITION_ERROR;
        
        break;

    case RECEIVE:
        if (mclmac->powerMode.currentState == ACTIVE)
            mclmac_set_powermode_state(mclmac, RECEIVE);
        else
            return E_PM_TRANSITION_ERROR;
        
        break;

    case FINISHP:
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == PASSIVE)
            return E_PM_TRANSITION_ERROR;
        mclmac_set_powermode_state(mclmac, FINISHP);
        break;

    default:
        return E_PM_INVALID_STATE;
        break;
    }

    return E_PM_TRANSITION_SUCCESS;
}


int mclmac_execute_powermode_state(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif
    assert(mclmac->_nSlots > 0);

    if (mclmac->powerMode.currentState == NONEP)
        return E_PM_EXECUTION_FAILED;
    
    switch (mclmac->powerMode.currentState)
    {
    case STARTP:
        // Set the number of slots and cfslots
        mclmac_set_slots_number(mclmac, mclmac->_nSlots);
        mclmac_set_cf_slots_number(mclmac, mclmac->_nChannels);

        // Set the value of the timers
        mclmac_set_frame_duration(mclmac, TIME(FRAME_DURATION));
        mclmac_set_slot_duration(mclmac, TIME(SLOT_DURATION));
        mclmac_set_cf_duration(mclmac, TIME(CF_SLOT_DURATION));

        // Initialize frame, slots, and cfslots counter to zero
        mclmac_set_current_frame(mclmac, mclmac->_wakeup_frame + 1);
        mclmac_set_current_slot(mclmac, 0);
        mclmac_set_current_cf_slot(mclmac, 0);
        ARROW(mclmac->mac)_packets_to_send_message = 0;
        ARROW(mclmac->mac)_packets_to_send_control = 0;
        ARROW(mclmac->mac)_number_packets_received = 0;
        ARROW(mclmac->mac)_first_send_message = 0;
        ARROW(mclmac->mac)_last_send_message = 0;
        ARROW(mclmac->mac)_first_send_control = 0;
        ARROW(mclmac->mac)_last_send_control = 0;
        ARROW(mclmac->mac)_first_received = 0;
        ARROW(mclmac->mac)_last_received = 0;

        // Pass immediatly to PASSIVE state
        mclmac_set_next_powermode_state(mclmac, PASSIVE);
        // Arm the slot timer for the first time.
        ARROW(ARROW(mclmac->mac)frame)slot_timer = timeout_set(ARROW(ARROW(mclmac->mac)frame)slot_duration);
        break;

    case PASSIVE: ;
        mclmac_set_current_cf_slot(mclmac, 0);
        mclmac_set_radio_sleep(mclmac);
        bool sleep = true;
        while (sleep)
        {
            if (timeout_passed(ARROW(ARROW(mclmac->mac)frame)slot_timer) == 1)
            {
                /* Terminate the cycle and unarm the timeout */
                timeout_unset(ARROW(ARROW(mclmac->mac)frame)slot_timer);
                sleep = false;
            }
            uint16_t packets_received = ARROW(mclmac->mac)_number_packets_received;
            uint32_t packets_on_queue = elements_on_queue(mclmac->_mac_queue_id);
            if ((packets_on_queue == 0) && (packets_received == 0)) {
                continue;   // Jump right back to check timer.
            }

            /* Read packets from upper layers. */
            mclmac_read_queue_element(mclmac);
            /* Write packets to upper layers. */
            mclmac_write_queue_element(mclmac);
        }
        // Increase current slot
        mclmac_increase_slot(mclmac);
        if (mclmac_get_current_slot(mclmac) == ARROW(ARROW(mclmac->mac)frame)slots_number)
        {
            // Increase frame number and set current slot to 0.
            mclmac_increase_frame(mclmac);
            mclmac_set_current_slot(mclmac, 0);
        }
        /* Increase by one the network time. */
        mclmac->_networkTime++;
        /* Set once again the slot timer before leaving the state. */
        ARROW(ARROW(mclmac->mac)frame)slot_timer = timeout_set(ARROW(ARROW(mclmac->mac)frame)slot_duration);
        mclmac_set_next_powermode_state(mclmac, ACTIVE);
        break;

    case ACTIVE: ;
        mclmac_start_cf_phase(mclmac);
        uint8_t current_slot = mclmac_get_current_slot(mclmac);
        uint8_t selected_slot = mclmac_get_selected_slot(mclmac);
        uint32_t selected_freq = mclmac_get_transmit_channel(mclmac);
        bool ended = false;
        bool is_current = false;
        bool send = false;
        bool receive = false;
        uint8_t packets = 0;
        if (current_slot == selected_slot) {
            is_current = true;
        }

        /* Arm the cf timer. */
        ARROW(ARROW(mclmac->mac)frame)cf_timer = timeout_set(ARROW(ARROW(mclmac->mac)frame)cf_duration);
        /* Iterate over all cf slots. */
        while (!ended)
        {
            uint8_t current_cf_slot = mclmac_get_current_cf_slot(mclmac);
            bool current_frequency = (selected_freq == mclmac_get_frequency(mclmac, current_cf_slot));
            int state = (is_current && current_frequency ? 0 : 1);;  // state = 0, receive cf message; state = 1 send cf message.
            /* Check whether the current cf slot finished. */
            if (timeout_passed(ARROW(ARROW(mclmac->mac)frame)cf_timer) == 1)
            {
                timeout_unset(ARROW(ARROW(mclmac->mac)frame)cf_timer);
                mclmac_increase_cf_slot(mclmac);
                // Check if all the cf slots are already traversed, if so, end the cycle
                if (mclmac_get_current_cf_slot(mclmac) == MAX_NUMBER_FREQS)
                {
                    ended = true;
                    continue;
                }
                // Otherwise, re-arm the timeout
                else
                {
                    ARROW(ARROW(mclmac->mac)frame)cf_timer = timeout_set(ARROW(ARROW(mclmac->mac)frame)cf_duration);
                }
                // Update state
                state = (is_current && current_frequency ? 0 : 1);
            }
            /* If the current slot is the selected slot, and the current cf slot corresponds to the 
            selected frequency, check if a packet is ready to send. */
            if (state == 0)
            {
                uint8_t packets_to_send_message = ARROW(mclmac->mac)_packets_to_send_message;
                uint8_t packets_to_send_control = ARROW(mclmac->mac)_packets_to_send_control;
                bool collision_detected = ARROW(mclmac->mac)_collisionDetected;
                if (packets_to_send_message > 0 || packets_to_send_control > 0 || collision_detected == true)
                {
                    /* Create the cf packet and send it. */
                    CFPacket_t *cfpkt = &ARROW(mclmac->mac)_cf_messages[0];
                    uint64_t nodeid[2] = {0};
                    mclmac_get_nodeid(mclmac, nodeid);
                    cfpacket_create(cfpkt, nodeid, ARROW(mclmac->mac)_destination_id);
                    mclmac_send_cf_message(mclmac);
                    cfpacket_clear(cfpkt);
                    send = true;
                    is_current = false; // Indicate to send no more cf packets
                }
                if (mclmac->_is_first_node == true)
                {
                    /* Create the cf packet and send it. */
                    CFPacket_t *cfpkt = &ARROW(mclmac->mac)_cf_messages[0];
                    uint64_t nodeid[2] = {0};
                    uint64_t nodeid2[2] = {0};
                    mclmac_get_nodeid(mclmac, nodeid);
                    cfpacket_create(cfpkt, nodeid, nodeid2);
                    mclmac_send_cf_message(mclmac);
                    cfpacket_clear(cfpkt);
                    send = true;
                    is_current = false;
                }
            }
            else if (state == 1)
            {
                /* Hear for any incoming CF packets on the medium. */
                bool cf_message_received = mclmac_receive_cf_message(mclmac);
                if (!cf_message_received) {
                    continue;
                }
                CFPacket_t *pkt = &ARROW(mclmac->mac)_cf_messages[1];
                uint64_t destinationid[2] = {0};
                cfpacket_get_destinationid(pkt, destinationid);
                bool need_broadcast = (destinationid[0] == 0) && (destinationid[1] == 0); // destination_id = 0 => broadcast
                bool destination_is_current = (destinationid[0] == mclmac->_node_id[0]) && (destinationid[1] == mclmac->_node_id[1]);
                if (destination_is_current || need_broadcast)
                {
                    uint64_t transmitter_id[2] = {0};
                    cfpacket_get_nodeid(pkt, transmitter_id);
                    mclmac_set_transmiterid(mclmac, transmitter_id);
                    uint32_t channel = mclmac_get_frequency(mclmac, current_cf_slot);
                    mclmac_set_reception_channel(mclmac, channel);
                    packets++;
                    receive = true;
                }
                // Tell it no longer is the only one
                if (mclmac->_is_first_node == true) {
                    mclmac->_is_first_node = false;
                }
            }
        }

        // Collision detected
        if ((send && receive) || (packets > 1))
        {
            mclmac_set_next_powermode_state(mclmac, PASSIVE);
            if (send && receive) {
                ARROW(mclmac->mac)_is_internal_collision = true;
            }
            return E_PM_COLLISION_DETECTED;
        }
        
        // Packets to send
        if (send) {
            mclmac_set_next_powermode_state(mclmac, TRANSMIT);
        }

        // Packets to receive
        if (receive) {
            mclmac_set_next_powermode_state(mclmac, RECEIVE);
        }
 
        // If there are no packets to send
        if (!send && !receive) {
            mclmac_set_next_powermode_state(mclmac, PASSIVE);
        }

        break;

    case TRANSMIT:  ;
        mclmac_start_split_phase(mclmac, TRANSMIT);

        /* Create and send control packet. */
        mclmac_create_control_packet(mclmac);
        mclmac_send_control_packet(mclmac);

        // Nodes already notify, unset flag and break
        if (ARROW(mclmac->mac)_collisionDetected == true)
        {
            ARROW(mclmac->mac)_collisionDetected = false;
            ARROW(mclmac->mac)_collisionSlot = 0;
            ARROW(mclmac->mac)_collisionFrequency = 0;
            if (ARROW(mclmac->mac)_is_internal_collision == true)
            {
                ARROW(mclmac->mac)_is_internal_collision = false;
                mclmac_set_next_powermode_state(mclmac, FINISHP);
                return E_PM_COLLISION_ERROR;
            }
            mclmac_set_next_powermode_state(mclmac, PASSIVE);
            break;
        }

        /* Send packets.*/
        bool end = false;
        while (!end)
        {
            // Check if timeout passed
            if (timeout_passed(ARROW(ARROW(mclmac->mac)frame)slot_timer) == 1)
            {
                timeout_unset(ARROW(ARROW(mclmac->mac)frame)slot_timer);
                end = true;
                continue;       // Do not send any more packets
            }
            mclmac_send_data_packet(mclmac);

            if (mclmac_available_data_packets(mclmac) == 0) {
                end = true;
            }
        }

        mclmac_set_next_powermode_state(mclmac, PASSIVE);
        break;

    case RECEIVE:   ;
        mclmac_start_split_phase(mclmac, RECEIVE);

        // Listen to the ctrl pkt up to 2 times.
        int count = 1;
        while (count <= 2) {
            bool ctrlpkt_recv = mclmac_receive_control_packet(mclmac);
            if (!ctrlpkt_recv) {
                count++;
                continue;
            }
            break;
        }
        // Exit the state
        if (count >= 2) {
            mclmac_set_next_powermode_state(mclmac, PASSIVE);
            break;
        }

        ControlPacket_t *pkt = REFERENCE ARROW(mclmac->mac)ctrlpkt_recv;
        uint32_t frequency = controlpacket_get_collision_frequency(pkt);
        uint8_t slot = controlpacket_get_collision_slot(pkt);
        uint8_t c_slot = controlpacket_get_current_slot(pkt);
        uint32_t frame = controlpacket_get_current_frame(pkt);
        uint64_t time = controlpacket_get_network_time(pkt);
        if (frequency == ARROW(mclmac->mac)transmitChannel && slot == ARROW(mclmac->mac)selectedSlot)
        {
            mclmac_set_next_powermode_state(mclmac, FINISHP);
            return E_PM_COLLISION_ERROR;
        }
        uint8_t own_slot = mclmac_get_current_slot(mclmac);
        uint32_t own_frame = ARROW(ARROW(mclmac->mac)frame)current_frame;
        int difference = mclmac->_networkTime - NETWORK_TIME_EPSILON;
        uint64_t lower_value_time = (difference < 0 ? 0 : difference);
        uint64_t upper_value_time = mclmac->_networkTime + NETWORK_TIME_EPSILON;
        bool not_on_time = (time < lower_value_time) || (time > upper_value_time);
        if (c_slot != own_slot  || frame != own_frame || not_on_time)
        {
            mclmac_set_next_powermode_state(mclmac, FINISHP);
            return E_PM_SYNCHRONIZATION_ERROR;
        }

        uint packets_received = ARROW(mclmac->mac)_number_packets_received;
        while (packets_received < 2 * MAX_NUMBER_DATA_PACKETS)
        {
            /* Terminate the cycle if the timer expired, but do not remove the timer, so the PASSIVE 
            state can transit to the ACTIVE state. */
            if (timeout_passed(ARROW(ARROW(mclmac->mac)frame)slot_timer) == 1) {
                break;
            }
            bool ret = mclmac_receive_data_packet(mclmac);
            if (!ret) {
                continue;
            }
            packets_received = ARROW(mclmac->mac)_number_packets_received;
        }

        mclmac_set_next_powermode_state(mclmac, PASSIVE);

        break;

    case FINISHP:   ;
        ARROW(mclmac->mac)_destination_id[0] = 0;
        ARROW(mclmac->mac)_destination_id[1] = 0;
        ARROW(mclmac->mac)_packets_to_send_message = 0;
        ARROW(mclmac->mac)_first_send_message = 0;
        ARROW(mclmac->mac)_last_send_message = 0;
        ARROW(mclmac->mac)_packets_to_send_control = 0;
        ARROW(mclmac->mac)_first_send_control = 0;
        ARROW(mclmac->mac)_last_send_control = 0;
        ARROW(mclmac->mac)_number_packets_received = 0;
        ARROW(mclmac->mac)_first_received = 0;
        ARROW(mclmac->mac)_last_received = 0;
        for (uint i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
        {
            DataPacket_t *pkt = &ARROW(mclmac->mac)_message_packets_to_send[i];
            datapacket_clear(pkt);
            pkt = &ARROW(mclmac->mac)_control_packets_to_send[i];
            datapacket_clear(pkt);
            pkt = &ARROW(mclmac->mac)_packets_received[i];
            datapacket_clear(pkt);
            pkt = &ARROW(mclmac->mac)_packets_received[i + MAX_NUMBER_DATA_PACKETS];
            datapacket_clear(pkt);
        }
        break;

    default: ;
        break;
    }

    return E_PM_EXECUTION_SUCCESS;
}
