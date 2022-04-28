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

PowerMode_t mclmac_get_powermode_state(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->powerMode.currentState;
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
        
        ARROW(mclmac->mac)cfChannel = CF_FREQUENCY;

        // Pass immediatly to PASSIVE state
        mclmac_set_next_powermode_state(mclmac, PASSIVE);
        // Arm the slot timer for the first time.
        ARROW(ARROW(mclmac->mac)frame)slot_timer = timeout_set(ARROW(ARROW(mclmac->mac)frame)slot_duration);
        break;

    case PASSIVE: ;
        mclmac_set_current_cf_slot(mclmac, 0);
        /* Set radio to sleep */
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
            if (elements_on_queue(mclmac->_mac_queue_id) == 0 &&  packets_received == 0)
                continue;   // Jump right back to check timer.

            /* Read packets from upper layers. */
            mclmac_read_queue_element(mclmac);
            /* Write packets to upper layers. */
            mclmac_write_queue_element(mclmac);
        }
        /* Increase by one the network time. */
        mclmac->_networkTime++;
        /* Set once again the slot timer before leaving the state. */
        ARROW(ARROW(mclmac->mac)frame)slot_timer = timeout_set(ARROW(ARROW(mclmac->mac)frame)slot_duration);
        mclmac_set_next_powermode_state(mclmac, ACTIVE);
        break;

    case ACTIVE: ;
        stub_mclmac_start_cf_phase(mclmac);
        uint8_t current_slot = mclmac_get_current_slot(mclmac);
        uint8_t selected_slot = mclmac_get_selected_slot(mclmac);
        uint32_t selected_freq = mclmac_get_transmit_channel(mclmac);
        bool ended = false;
        bool is_current = false;
        bool send = false;
        bool receive = false;
        uint8_t packets = 0;
        if (current_slot == selected_slot)
            is_current = true;

        /* Arm the cf timer. */
        ARROW(ARROW(mclmac->mac)frame)cf_timer = timeout_set(ARROW(ARROW(mclmac->mac)frame)cf_duration);
        /* Iterate over all cf slots. */
        while (!ended)
        {
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
            }
            /* If the current slot is the selected slot, and the current cf slot corresponds to the 
            selected frequency, check if a packet is ready to send. */
            uint8_t current_cf_slot = mclmac_get_current_cf_slot(mclmac);
            if (is_current && selected_freq == mclmac_get_frequency(mclmac, current_cf_slot))
            {
                uint8_t packets_to_send_message = ARROW(mclmac->mac)_packets_to_send_message;
                uint8_t packets_to_send_control = ARROW(mclmac->mac)_packets_to_send_control;
                if ( packets_to_send_message > 0 || packets_to_send_control > 0)
                {
                    /* Create the cf packet and send it. */
                    CFPacket_t *cfpkt = &ARROW(mclmac->mac)_cf_messages[0];
                    uint8_t nodeid = mclmac_get_nodeid(mclmac);
                    cfpacket_create(cfpkt, nodeid, ARROW(mclmac->mac)_destination_id);
                    stub_mclmac_send_cf_message(mclmac);
                    cfpacket_clear(cfpkt);
                    send = true;
                    is_current = false; // Indicate to send no more cf packets
                }
            }
            /* Hear for any incoming CF packets on the medium. */
            else if (stub_mclmac_receive_cf_message(mclmac))
            {
                CFPacket_t *pkt = &ARROW(mclmac->mac)_cf_messages[1];
                uint16_t destinationid = cfpacket_get_destinationid(pkt);
                if (destinationid == mclmac->_nodeID || destinationid == 0) // destinationID = 0 means a broadcast.
                {
                    uint16_t transmiterid = cfpacket_get_nodeid(pkt);
                    mclmac_set_transmiterid(mclmac, transmiterid);
                    uint32_t channel = mclmac_get_frequency(mclmac, current_cf_slot);
                    mclmac_set_transmit_channel(mclmac, channel);
                    packets++;
                    receive = true;
                }
            }
#ifdef __LINUX__
            usleep(CF_SLOT_DURATION / 2);
#endif
#ifdef __RIOT__
            ztimer_sleep(CLOCK, CF_SLOT_DURATION / 2);
#endif
        }

        // Collision detected
        if ((send && receive) || (packets > 1))
        {
            mclmac_set_next_powermode_state(mclmac, FINISHP);
            return E_PM_COLLISION_DETECTED;
        }
        
        // Packets to send
        if (send)
            mclmac_set_next_powermode_state(mclmac, TRANSMIT);
        
        // Packets to receive
        if (receive)
            mclmac_set_next_powermode_state(mclmac, RECEIVE);
        
        // If there are no packets to send
        if (!send && !receive)
            mclmac_set_next_powermode_state(mclmac, PASSIVE);

        break;

    case TRANSMIT:  ;
        stub_mclmac_start_split_phase(mclmac, TRANSMIT);

        /* Create and send control packet. */
        mclmac_create_control_packet(mclmac);
        stub_mclmac_send_control_packet(mclmac);

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
            // Send control packets first
            if (ARROW(mclmac->mac)_packets_to_send_control > 0)
                stub_mclmac_send_layers_control_packet(mclmac);

            // Send data packets only after the control ones were sent
            if (ARROW(mclmac->mac)_packets_to_send_message > 0 && ARROW(mclmac->mac)_packets_to_send_control == 0)
                stub_mclmac_send_data_packet(mclmac);

            if (mclmac_available_data_packets(mclmac) == 0)
                end = true;
        }

         // Increase current slot
        mclmac_increase_slot(mclmac);
        if (mclmac_get_current_slot(mclmac) == ARROW(ARROW(mclmac->mac)frame)slots_number)
        {
            // Increase frame number and set current slot to 0, in case the frame is completed.
            mclmac_increase_frame(mclmac);
            mclmac_set_slots_number(mclmac, 0);
        }

        mclmac_set_next_powermode_state(mclmac, PASSIVE);
        break;

    case RECEIVE: ;
        stub_mclmac_start_split_phase(mclmac, RECEIVE);

        stub_mclmac_receive_control_packet(mclmac);

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
        if (c_slot != own_slot  || frame != own_frame || time != mclmac->_networkTime)
        {
            mclmac_set_next_powermode_state(mclmac, FINISHP);
            return E_PM_SYNCHRONIZATION_ERROR;
        }

        uint packets_received = ARROW(mclmac->mac)_number_packets_received;
        while (packets_received < MAX_ELEMENTS_ON_QUEUE)
        {
            /* Terminate the cycle if the timer expired, but do not remove the timer, so the PASSIVE 
            state can transit to the ACTIVE state. */
            if (timeout_passed(ARROW(ARROW(mclmac->mac)frame)slot_timer) == 1)
                break;

            stub_mclmac_receive_data_packet(mclmac);
            packets_received = ARROW(mclmac->mac)_number_packets_received;
        }

        mclmac_set_next_powermode_state(mclmac, PASSIVE);

        break;

    default: ;
        break;
    }

    return E_PM_EXECUTION_SUCCESS;
}
