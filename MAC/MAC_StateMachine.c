#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
        (((byte) & 0x80) ? '1' : '0'), \
        (((byte) & 0x40) ? '1' : '0'), \
        (((byte) & 0x20) ? '1' : '0'), \
        (((byte) & 0x10) ? '1' : '0'), \
        (((byte) & 0x08) ? '1' : '0'), \
        (((byte) & 0x04) ? '1' : '0'), \
        (((byte) & 0x02) ? '1' : '0'), \
        (((byte) & 0x01) ? '1' : '0')

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
        if (mclmac->macState.currentState == START || mclmac->macState.currentState == MEDIUM_ACCESS)
            mclmac_set_MAC_state(mclmac, INITIALIZATION);
        else
            return E_MAC_TRANSITION_ERROR;
        break;

    case SYNCHRONIZATION:
        if (mclmac->macState.currentState == START)
            return E_MAC_TRANSITION_ERROR;
        mclmac_set_MAC_state(mclmac, SYNCHRONIZATION);
        break;

    case TIMESLOT_AND_CHANNEL_SELECTION:
        if (mclmac->macState.currentState == SYNCHRONIZATION)
            mclmac_set_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
        else
            return E_MAC_TRANSITION_ERROR;
        
        break;

    case MEDIUM_ACCESS:
        if (mclmac->macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION)
            mclmac_set_MAC_state(mclmac, MEDIUM_ACCESS);
        else
            return E_MAC_TRANSITION_ERROR;

        break;

    case FINISH:
        if (mclmac->macState.currentState == MEDIUM_ACCESS)
            mclmac_set_MAC_state(mclmac, FINISH);
        else
            return E_MAC_TRANSITION_ERROR;

        break;

    default:
        return E_MAC_INVALID_STATE;
        break;
    }

    return E_MAC_TRANSITION_SUCCESS;
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
        uint32_t wakeup_frame = rand() % MAX_NUMBER_SLOTS;
        mclmac->_wakeup_frame = (wakeup_frame == 0 ? 1 : wakeup_frame);
        // Get the current thread's pid
#ifdef __LINUX__
        mclmac->_self_pid = pthread_self();
#endif
#ifdef __RIOT__
        mclmac->_self_pid = thread_getpid();
#endif
        // Open a new queue using the default values (defined on config_ipc.h)
        open_queue(mclmac->_mac_queue_id, mclmac->_self_pid);
        mclmac_set_next_MAC_state(mclmac, INITIALIZATION);
        mclmac->_is_first_node = false;
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
        timer = timeout_set(TIME(DETECTION_TIME));
        while (!finish)
        {
            bool packet = mclmac_cf_packet_detected(mclmac);
            bool passed = timeout_passed(timer);
            if (passed == 1)
            {
                finish = true;
                found = false;
            }
            else if (packet)
            {
                finish = true;
                found = true;
            }
        }
        if (!found)
        {
            mclmac->_is_first_node = true;
        }
        timeout_unset(timer);
        // Wait until the cf slot terminates
#ifdef __LINUX__
        usleep(0.95 * CF_SLOT_DURATION);
#endif
#ifdef __RIOT__
        ztimer_sleep(ZTIMER_USEC, (uint32_t)(0.95 * CF_SLOT_DURATION));
#endif
        mclmac_set_radio_standby(mclmac);
        mclmac_set_next_MAC_state(mclmac, SYNCHRONIZATION);
        break;

    case SYNCHRONIZATION:   ;
        // Handle the network's first node
        if (mclmac->_is_first_node)
        {
#ifdef __LINUX__
            mclmac->_initTime =  (uint32_t) time(NULL);
#endif
#ifdef __RIOT__
            mclmac->_initTime = ztimer_now(CLOCK);
#endif
            mclmac->_hopCount = 0;
            mclmac_set_next_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
            return E_MAC_EXECUTION_SUCCESS;
        }
        mclmac_set_radio_rx(mclmac);
        /* Set hopCount to inf, and network time to zero. */
        mclmac->_hopCount = 0xffff;         // The greatest possible number for uint16_t
        mclmac->_networkTime = 0;
        /* Create control packet for filling information of the network. */
        ControlPacket_t SINGLE_POINTER ctrlpkt;
        controlpacket_init(&ctrlpkt);
        uint32_t current_frame = 0;
        uint32_t frequency = 0;
        uint8_t current_slot = 0;

        /* Synchornize */
        int count = 0;
        while (count <= 10) {
            // Try at most 10 times to listen for a control packet on the medium.
            bool ret = mclmac_receive_ctrlpkt_sync(mclmac, REFERENCE ctrlpkt);
            if (!ret) {
                count++;
                continue;
            }
            break;
        }
        if (count == 11) {
            return E_MAC_EXECUTION_FAILED;
        }
        /* Get the frame. */
        current_frame = controlpacket_get_current_frame(REFERENCE ctrlpkt);
        /* Get the current slot. */
        current_slot = controlpacket_get_current_slot(REFERENCE ctrlpkt);
        // Get the latest network time
        mclmac->_networkTime = controlpacket_get_network_time(REFERENCE ctrlpkt);
        // Get the starting time of the network
        mclmac->_initTime = controlpacket_get_init_time(REFERENCE ctrlpkt);
        uint8_t occupied_slots[MAX_NUMBER_FREQS][(MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0)];
        controlpacket_get_occupied_slots(REFERENCE ctrlpkt, (uint8_t *)occupied_slots);
        // Copy the received vector to node's one
        memcpy(mclmac->_occupied_frequencies_slots, occupied_slots, sizeof(occupied_slots));
        // Set to occupied by neighbor the corresponding bit on _selected_slots_neighbors array
        uint8_t bit = 0;
        bit |= 1U << (current_slot % 8);
        uint8_t pos = (uint8_t)(current_slot / 8U);
        mclmac->_selected_slots_neighbors[pos] |= bit;
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
        if (slot_timer == TIMEOUT_SET_ERROR || frame_timer == TIMEOUT_SET_ERROR)
        {
            perror("Unable to create timers. Returning.\n");
            return E_MAC_EXECUTION_FAILED;
        }
        /* Begin to receive the packets. */
        while (current_frame < mclmac->_wakeup_frame - 1U)
        {
            /* Listen for the first incoming control packet and get the data. */
            // If a packet is received, update values
            if (mclmac_receive_ctrlpkt_sync(mclmac, REFERENCE ctrlpkt))
            {
                /* Get the corresponding variables. */
                // Get the minimum number of hops
                uint8_t hops = controlpacket_get_hop_count(REFERENCE ctrlpkt);
                if (hops < mclmac->_hopCount)
                    mclmac->_hopCount = hops;

                controlpacket_get_occupied_slots(REFERENCE ctrlpkt, (uint8_t *)occupied_slots);
                // For each frequency, OR all the available slots
                for (int i = 0; i < MAX_NUMBER_FREQS; i++) {
                    for (int j = 0; j < (MAX_NUMBER_SLOTS / 8) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0); j++) {
                        mclmac->_occupied_frequencies_slots[i][j] |= occupied_slots[i][j];
                    }
                }
                // Store the corresponding bit
                bit = 0x00;
                bit |= 1U << (current_slot % 8);
                pos = (uint8_t)(current_slot / 8);
                mclmac->_selected_slots_neighbors[pos] |= bit;
            }

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
        mclmac_set_radio_standby(mclmac);
        mclmac_set_next_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);

        return E_MAC_EXECUTION_SUCCESS;
        break;
    
    case TIMESLOT_AND_CHANNEL_SELECTION: ;
        mclmac_set_radio_sleep(mclmac);
        if (mclmac->_is_first_node == true)
        {
            // Set the first slot and the first frequency
            mclmac_set_transmit_channel(mclmac, mclmac->_frequencies[0]);
            mclmac_set_selected_slot(mclmac, 0);
            mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
            // Set the first bit on the frequency zero to 1, to indicate is occupied the slot
            mclmac->_occupied_frequencies_slots[0][0] = 0x80;
            return E_MAC_EXECUTION_SUCCESS;
        }
        /* Set the frame timer. */
        frame_timer = timeout_set(TIME(FRAME_DURATION));
        if (frame_timer == TIMEOUT_SET_ERROR)
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
            if (available) {
                break;
            }
        }
        if (!available)
            mclmac_set_next_MAC_state(mclmac, SYNCHRONIZATION);
        else
            mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);

        /* Wait until the current frame ends. */
        while (timeout_passed(frame_timer) != 1) ;

        timeout_unset(frame_timer);

        return E_MAC_EXECUTION_SUCCESS;
    break;

    case MEDIUM_ACCESS: ;
        mclmac_init_powermode_state_machine(mclmac);

        // Execute the STARTP state
        while (1)
        {
            int ret = mclmac_execute_powermode_state(mclmac);
            if (ret == E_PM_EXECUTION_FAILED)
            {
                mclmac->powerMode.currentState = STARTP;
                continue;
            }
            else
                break;
        }
        // Update powermode state
        mclmac_update_powermode_state_machine(mclmac);

        // Execute the PowerMode state machine
        bool execute = true;
        while (execute)
        {
            int ret = mclmac_execute_powermode_state(mclmac);
            switch (ret)
            {
            case E_PM_EXECUTION_SUCCESS:
                mclmac_update_powermode_state_machine(mclmac);
                break;
            case E_PM_COLLISION_ERROR:
                mclmac_set_next_powermode_state(mclmac, FINISHP);
                mclmac_set_next_MAC_state(mclmac, SYNCHRONIZATION);
                execute = false;
                break;
            case E_PM_SYNCHRONIZATION_ERROR:
                mclmac_set_next_powermode_state(mclmac, FINISHP);
                mclmac_set_next_MAC_state(mclmac, INITIALIZATION);
                execute = false;
                break;
            case E_PM_COLLISION_DETECTED:
                ARROW(mclmac->mac)_collisionDetected = true;
                ARROW(mclmac->mac)_collisionSlot = mclmac_get_current_slot(mclmac);
                ARROW(mclmac->mac)_collisionFrequency = mclmac_get_reception_channel(mclmac);
                ARROW(mclmac->mac)_destination_id[0] = 0;      // Set to broadcast
                ARROW(mclmac->mac)_destination_id[1] = 0;
                break;
            default:
                break;
            }
        }

        // Always execute the FINISHP state at the end of the MEDIUM_ACCESS state
        mclmac_update_powermode_state_machine(mclmac);
        mclmac_execute_powermode_state(mclmac);

        break;

    default:
        break;
    }

    return E_MAC_EXECUTION_SUCCESS;
}
