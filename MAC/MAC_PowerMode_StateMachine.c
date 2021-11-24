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
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == TRANSMIT)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == RECEIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        mclmac_set_powermode_state(mclmac, ACTIVE);
        break;

    case TRANSMIT:
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == PASSIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == RECEIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        mclmac_set_powermode_state(mclmac, TRANSMIT);
        break;

    case RECEIVE:
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == PASSIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == TRANSMIT)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        mclmac_set_powermode_state(mclmac, RECEIVE);
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
    assert(ARROW(mclmac->mac)_max_number_packets_buffer > 0);

    if (mclmac->powerMode.currentState == NONEP)
        return E_PM_EXECUTION_FAILED;
    
    switch (mclmac->powerMode.currentState)
    {
    case STARTP:
        // Set the number of slots and cfslots
        mclmac_set_slots_number(mclmac, mclmac->_nSlots);
        mclmac_set_cf_slots_number(mclmac, mclmac->_nSlots);

        // Set the value of the timers
        mclmac_set_frame_duration(mclmac, TIME(1000));
        mclmac_set_slot_duration(mclmac, TIME(500));
        mclmac_set_cf_duration(mclmac, TIME(100));

        // Initialize frame, slots, and cfslots counter to zero
        mclmac_set_current_frame(mclmac, 0);
        mclmac_set_current_slot(mclmac, 0);
        mclmac_set_current_cf_slot(mclmac, 0);
        ARROW(mclmac->mac)_packets_read = 0;

        // Create an array of size of at most 5 packet of 256 bytes each
#ifdef __LINUX__
        mclmac->mac->_packets = (uint8_t *)malloc(mclmac->mac->_max_number_packets_buffer * sizeof(uint8_t));
        if (mclmac->mac->_packets == NULL)
        {
            exit(0);
        }
        memset(mclmac->mac->_packets, 0, mclmac->mac->_max_number_packets_buffer);
        mclmac->mac->_packets_received = (uint8_t *)malloc(mclmac->mac->_max_number_packets_buffer * sizeof(uint8_t));
        if (mclmac->mac->_packets_received == NULL)
        {
            exit(0);
        }
        mclmac->mac->_cf_messages = (uint8_t *)malloc(mclmac->mac->_max_cf_messages * sizeof(uint8_t));
        if (mclmac->mac->_cf_messages == NULL)
        {
            exit(0);
        }
        memset(mclmac->mac->_cf_messages, 0, mclmac->mac->_max_cf_messages);
#endif
#ifdef __RIOT__
        int ret = create_array(&mclmac->mac._packets, mclmac->mac._max_number_packets_buffer);
        if (ret == 0)
        {
            exit(0);
        }
        ret = create_array(&mclmac->mac._packets_received, mclmac->mac._max_number_packets_buffer);
        if (ret == 0)
        {
            exit(0);
        }
        ret = create_array(&mclmac->mac._cf_messages, mclmac->mac._max_cf_messages);
        if (ret == 0)
        {
            exit(0);
        }
#endif
        // Pass immediatly to PASSIVE state
        mclmac_set_next_powermode_state(mclmac, PASSIVE);
        // Arm the slot timer for the first time.
        ARROW(ARROW(mclmac->mac)frame)slot_timer = timeout_set(ARROW(ARROW(mclmac->mac)frame)slot_duration);
        break;
    
    case PASSIVE: ;
        mclmac_set_current_cf_slot(mclmac, 0);
        /* Set radio to sleep */
        bool sleep = true;
        uint8_t packets_read = 0;
        uint16_t bytes = 0;
        uint32_t read_from = 0;
        while (sleep)
        {
            if (timeout_passed(ARROW(ARROW(mclmac->mac)frame)slot_timer) == 1)
            {
                /* Terminate the cycle and unarm the timeout */
                timeout_unset(ARROW(ARROW(mclmac->mac)frame)slot_timer);
                sleep = false;
            }

            /* Read at most 5 packets from the queue. */
            if (packets_read < 5)
            {
                uint8_t nelement = stub_mclmac_read_queue_element(mclmac, &bytes, ARROW(mclmac->mac)_max_number_packets_buffer, &read_from);
                if (nelement == 1)
                    packets_read++;
            }

            /* 'Write' packets into queue. */
            if (ARROW(mclmac->mac)_num_packets_received > 0)
            {
                uint8_t nelements = stub_mclmac_write_queue_element(mclmac, ARROW(mclmac->mac)_max_number_packets_buffer);
                if (nelements == 1)
                    ARROW(mclmac->mac)_num_packets_received--;
            }
        }
        ARROW(mclmac->mac)_packets_read += packets_read;

        /* Set once again the slot timer before leaving the state. */
        ARROW(ARROW(mclmac->mac)frame)slot_timer = timeout_set(ARROW(ARROW(mclmac->mac)frame)slot_duration);
        mclmac_set_next_powermode_state(mclmac, ACTIVE);
        break;
    
    case ACTIVE: ;
        // Change to the cf channel
        stub_mclmac_change_cf_channel(mclmac);
        stub_mclmac_start_cf_phase(mclmac);
        // Reset the cf counter to zero
        mclmac_set_current_cf_slot(mclmac, 0);
        bool to_send = false;
        /* Check if there are packets on the queue. */
        if (ARROW(mclmac->mac)_packets_read > 0)
        {
            // Get the id of the destination from the first packet
            //uint8_t destinationID = READ_ARRAY(REFERENCE ARROW(mclmac->mac)_packets, 0);
            //mclmac_set_destination_id(mclmac, destinationID);
//            mclmac_create_cf_packet(mclmac);
            to_send = true;
        }

        // Craete and arm the cf_slot_timer
        ARROW(ARROW(mclmac->mac)frame)cf_timer = timeout_set(ARROW(ARROW(mclmac->mac)frame)cf_duration);
        bool active = true;
        bool transmit = false;
        bool receive = false;
        while (active)
        {
            // Check if timeout passed
            if (timeout_passed(ARROW(ARROW(mclmac->mac)frame)cf_timer))
            {
                timeout_unset(ARROW(ARROW(mclmac->mac)frame)cf_timer);
                mclmac_increase_cf_slot(mclmac);
                ARROW(ARROW(mclmac->mac)frame)cf_timer = timeout_set(ARROW(ARROW(mclmac->mac)frame)cf_duration);
            }
            // Check if cf phase is over
            if (ARROW(ARROW(mclmac->mac)frame)current_cf_slot == ARROW(ARROW(mclmac->mac)frame)cf_slots_number)
            {
                // Remove timer and end cycle
                timeout_unset(ARROW(ARROW(mclmac->mac)frame)cf_timer);
                active = false;
            }
            // check if current cf slot corresponds to the selected slot
            if (ARROW(ARROW(mclmac->mac)frame)current_cf_slot == ARROW(mclmac->mac)selectedSlot)
            {
                if (to_send)
                {
                    stub_mclmac_send_cf_message(mclmac);
                    transmit = true;
                }
            }
            // Check if a message is received
            if (stub_mclmac_receive_cf_message(mclmac))
            {
                receive = true;
                mclmac_set_transmit_channel(mclmac, rand());
            }
        }

        if (transmit)
            mclmac_set_next_powermode_state(mclmac, TRANSMIT);        
        else if (receive)
            mclmac_set_next_powermode_state(mclmac, RECEIVE);
        else
            mclmac_set_next_powermode_state(mclmac, PASSIVE);

#ifdef __LINUX__
        // change radio to common channel
#endif        
        //mclmac_clear_cf_packet(mclmac);
        break;

    default: ;
        break;
    }

    return E_PM_EXECUTION_SUCCESS;
}
