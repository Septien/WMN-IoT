#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

void mclmac_init_mac_state_machine(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    mclmac->macState.currentState = START;
    mclmac->macState.nextState = NONE;
}

void mclmac_set_MAC_state(MCLMAC_t *mclmac, state_t state)
{
    assert(mclmac != NULL);
    assert(state != NONE);
    mclmac->macState.currentState = state;
}

void mclmac_set_next_MAC_state(MCLMAC_t *mclmac, state_t next)
{
    assert(mclmac != NULL);
    assert(next != NONE);
    assert(next != START);

    mclmac->macState.nextState = next;
}

state_t mclmac_get_MAC_state(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->macState.currentState;
}

int mclmac_update_mac_state_machine(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    if (mclmac->macState.currentState == mclmac->macState.nextState)
        return E_MAC_NO_TRANSITION;

    switch (mclmac->macState.nextState)
    {
    case NONE:
        return E_MAC_NO_TRANSITION;

    case INITIALIZATION:
        if (mclmac->macState.currentState == SYNCHRONIZATION)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION)
            return E_MAC_TRANSITION_ERROR;
        mclmac_set_MAC_state(mclmac, INITIALIZATION);
        return E_MAC_TRANSITION_SUCCESS;
        break;

    case SYNCHRONIZATION:
        if (mclmac->macState.currentState == START)
            return E_MAC_TRANSITION_ERROR;
        mclmac_set_MAC_state(mclmac, SYNCHRONIZATION);
        return E_MAC_TRANSITION_SUCCESS;
        break;

    case TIMESLOT_AND_CHANNEL_SELECTION:
        if (mclmac->macState.currentState == START)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == INITIALIZATION)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == MEDIUM_ACCESS)
            return E_MAC_TRANSITION_ERROR;
        mclmac_set_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
        return E_MAC_TRANSITION_SUCCESS;
        break;

    case MEDIUM_ACCESS:
        if (mclmac->macState.currentState == START)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == INITIALIZATION)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == SYNCHRONIZATION)
            return E_MAC_TRANSITION_ERROR;
        mclmac_set_MAC_state(mclmac, MEDIUM_ACCESS);
        return E_MAC_TRANSITION_SUCCESS;
        break;

    case FINISH:
        if (mclmac->macState.currentState == START)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == INITIALIZATION)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == SYNCHRONIZATION)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION)
            return E_MAC_TRANSITION_ERROR;
        mclmac_set_MAC_state(mclmac, FINISH);
        return E_MAC_TRANSITION_SUCCESS;
        break;

    default:
        return E_MAC_INVALID_STATE;
        break;
    }

    return E_MAC_NO_UPDATE;
}

int mclmac_execute_mac_state_machine(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    // The timers for the slots and frames
#ifdef __LINUX__
    int slot_timer;
    int frame_timer;
#endif
#ifdef __RIOT__
    uint32_t slot_timer;
    uint32_t frame_timer;
#endif
    
    switch (mclmac->macState.currentState)
    {
    case START: ;
        // Initialize the timeouts API
        timeout_init();
        mclmac->_wakeup_frame = MAX_NUMBER_FREQS + (rand() % MAX_NUMBER_SLOTS);
        mclmac_set_next_MAC_state(mclmac, INITIALIZATION);
        break;
    
    case INITIALIZATION: ;  // <-- weird C syntax: https://stackoverflow.com/questions/18496282/why-do-i-get-a-label-can-only-be-part-of-a-statement-and-a-declaration-is-not-a
        bool finish = false;
        bool found = false;
        // Set cf frequency.
        // Set rx_single
        // Set timer
#ifdef __LINUX__
        int timer = 0;
#endif
#ifdef __RIOT__
        uint32_t timer = 0;
#endif
        timer = timeout_set(TIME(1000000));
        while (!finish)
        {
            bool packet = stub_mclmac_cf_packet_detected(mclmac);
            bool passed = timeout_passed(timer);
            if (passed == 1)
            {
                finish = true;
                found = false;
                timeout_unset(timer);
            }
            else if (packet)
            {
                finish = true;
                found = true;
                timeout_unset(timer);
            }
        }
        if (!found)
            return E_MAC_EXECUTION_FAILED;
        mclmac_set_next_MAC_state(mclmac, SYNCHRONIZATION);
        break;

    case SYNCHRONIZATION:   ;
        /* Set hopCount to inf, and network time to zero. */
        mclmac->_hopCount = 0xffff;         // The greatest possible number for uint16_t
        mclmac->_networkTime = 0;
        /* Create control packet for filling information of the network. */
        ControlPacket_t SINGLE_POINTER ctrlpkt;
        controlpacket_init(&ctrlpkt);
        uint32_t current_frame = 0;
        uint32_t frequency = 0;
        uint8_t current_slot = 0;

        // Set the current frequency
        /* Synchornize */
        stub_mclmac_receive_ctrlpkt_sync(mclmac, REFERENCE ctrlpkt);
        /* Get the frame. */
        current_frame = controlpacket_get_current_frame(REFERENCE ctrlpkt);
        /*Get the current slot. */
        current_slot = controlpacket_get_current_slot(REFERENCE ctrlpkt);
        // Get the latest network time
        mclmac->_networkTime = controlpacket_get_network_time(REFERENCE ctrlpkt);
        // Get the starting time of the network
        mclmac->_initTime = controlpacket_get_init_time(REFERENCE ctrlpkt);
        // Update wakeup_frame
        mclmac->_wakeup_frame += current_frame;

        /* Compute the time difference */
        uint32_t time_passed_frames = current_frame * FRAME_DURATION;
        uint32_t time_passed_slots = (current_slot == 0 ? 0 : (current_slot - 1) * SLOT_DURATION);
        uint32_t total_time_passed = time_passed_frames + time_passed_slots;
        uint32_t current_slot_time_passed = mclmac->_networkTime - total_time_passed;
        uint32_t time_remaining = SLOT_DURATION - current_slot_time_passed;
        /* Arm the timers. */
        slot_timer = timeout_set(TIME(time_remaining));
        frame_timer = timeout_set(TIME(time_remaining));
        if (slot_timer == 2 || frame_timer == 2)
        {
            perror("Unable to create timers. Returning\n");
            return E_MAC_EXECUTION_FAILED;
        }
        /* Begin to receive the packets. */
        while (current_frame != mclmac->_wakeup_frame - 1U)
        {
            /* Listen for the first incoming control packet and get the data. */
            // If a packet is received, update values
            if (stub_mclmac_receive_ctrlpkt_sync(mclmac, REFERENCE ctrlpkt) == true)
            {
                /* Get the corresponding variables. */
                // Get the minimum number of hops
                uint8_t hops = controlpacket_get_hop_count(REFERENCE ctrlpkt);
                if (hops < mclmac->_hopCount)
                    mclmac->_hopCount = hops;

                // Get the information about the occupied slots per frequency
                uint8_t bit = 0;
                /* For knowing the bit to set, get the remainder of current_slot module 8 (number 
                of bits per byte, uint8_t) */
                bit = 1U << (current_slot % 8);
                /* For knowing on which byte to store the bit, divide current_slot by 8. */
                mclmac->_occupied_frequencies_slots[frequency][(uint)(current_slot / 8)] |= bit;
            }

            // Sleep for a while
#ifdef __LINUX__
            usleep(SLOT_DURATION / 2);
#endif
#ifdef __RIOT__
            ztimer_sleep(CLOCK, SLOT_DURATION / 2);
#endif
            /* Check if slot timer expired, if so, increase slot number */
            if (timeout_passed(slot_timer) == 1)
            {
                current_slot++;
                timeout_unset(slot_timer);
                slot_timer = timeout_set(TIME(SLOT_DURATION));
            }

            if (timeout_passed(frame_timer) == 1)
            {
                current_frame++;
                current_slot = 0;
                timeout_unset(frame_timer);
                frame_timer = timeout_set(TIME(FRAME_DURATION));
                frequency = (frequency + 1) % MAX_NUMBER_FREQS;
            }
        }
        timeout_unset(frame_timer);
        timeout_unset(slot_timer);
        controlpacket_destroy(&ctrlpkt);
        mclmac_set_next_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
        return E_MAC_EXECUTION_SUCCESS;
        break;
    
    case TIMESLOT_AND_CHANNEL_SELECTION: ;
        /* Set the frame timer. */
        frame_timer = timeout_set(TIME(FRAME_DURATION));
        if (frame_timer == 2)
        {
            perror("Unable to create frame timer.\n");
            return E_MAC_EXECUTION_FAILED;
        }

        int n = MAX_NUMBER_FREQS;
        int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
        bool available = true;
        /* Check the slots for each frequency, trying to find an empty one on all frequencies. */
        // Travers all the slots.
        for (int i = 0; i < m; i++)
        {
            // For each bit of the slot.
            for (int j = 1; j <= 8; j++)
            {
                uint8_t bit = 1U << (8 - j);
                available = true;
                // For each frequency, check if the corresponding bit is set or not.
                int k;
                for (k = 0; k < n; k++)
                {
                    if (mclmac->_occupied_frequencies_slots[k][i] & bit)
                        available = available && false;
                }
                // Check if available.
                if (available)
                {
                    // Select a random frequency
                    int freq = rand() % MAX_NUMBER_FREQS;
                    mclmac_set_transmit_channel(mclmac, mclmac->_frequencies[freq]);
                    mclmac_set_selected_slot(mclmac, (m * i) + (j - 1));
                    break;
                }
            }
        }
        if (!available)
            mclmac_set_next_MAC_state(mclmac, SYNCHRONIZATION);
        else
            mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);

        /* Wait until the current frame ends. */
        while (timeout_passed(frame_timer) != 1)
#ifdef __LINUX__
            usleep(SLOT_DURATION / 2);
#endif
#ifdef __RIOT__
            ztimer_sleep(CLOCK, SLOT_DURATION / 2);
#endif

        timeout_unset(frame_timer);

        return E_MAC_EXECUTION_SUCCESS;
    break;

    default:
        break;
    }

    return E_MAC_EXECUTION_SUCCESS;
}
