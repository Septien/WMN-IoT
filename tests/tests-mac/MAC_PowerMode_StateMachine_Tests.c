#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "MAC_PowerMode_StateMachine_Tests.h"
#include "MCLMAC.h"

#include "cUnit.h"

#ifdef __RIOT__
#include "net/netdev.h"
#endif

struct powermode_data
{
        MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    nrf24l01p_ng_t *radio;
    nrf24l01p_ng_t *radio_test;
    netdev_t *netdev;
    netdev_t *netdev_test;
#endif
};

static void setup(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
#ifdef __LINUX__
    MCLMAC_init(&data->mclmac, data->radio);
#endif
#ifdef __RIOT__
#ifndef NATIVE
    netopt_state_t state = NETOPT_STATE_STANDBY;
    data->netdev->driver->set(data->netdev, NETOPT_STATE, (void *)&state, sizeof(state));
#endif
    MCLMAC_init(&data->mclmac, data->netdev);
#endif
}

static void teardown(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
#ifdef __RIOT__
#ifndef NATIVE
    netopt_state_t state = NETOPT_STATE_SLEEP;
    data->netdev->driver->set(data->netdev, NETOPT_STATE, (void *)&state, sizeof(state));
#endif
#endif
    MCLMAC_destroy(&data->mclmac);
}

bool test_mclmac_init_powermode_state_machine(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_powermode_state_machine(mclmac);
    bool passed = true;
    passed = passed && (mclmac->powerMode.currentState == STARTP);
    passed = passed && (mclmac->powerMode.nextState == NONEP);

    return passed;
}

bool test_mclmac_set_powermode_state(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    PowerMode_t mode = PASSIVE;
    mclmac_set_powermode_state(mclmac, mode);
    bool passed = true;
    passed = passed && (mclmac->powerMode.currentState == mode);

    mode = ACTIVE;
    mclmac_set_powermode_state(mclmac, mode);
    passed = passed && (mclmac->powerMode.currentState == mode);

    mode = TRANSMIT;
    mclmac_set_powermode_state(mclmac, mode);
    passed = passed && (mclmac->powerMode.currentState == mode);

    mode = RECEIVE;
    mclmac_set_powermode_state(mclmac, mode);
    passed = passed && (mclmac->powerMode.currentState == mode);

    return passed;
}

bool test_mclmac_set_next_powermode_state(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    PowerMode_t mode = ACTIVE;
    mclmac_set_next_powermode_state(mclmac, mode);
    bool passed = true;
    passed = passed && (mclmac->powerMode.nextState == mode);

    mode = TRANSMIT;
    mclmac_set_next_powermode_state(mclmac, mode);
    passed = passed && (mclmac->powerMode.nextState == mode);

    mode = RECEIVE;
    mclmac_set_next_powermode_state(mclmac, mode);
    passed = passed && (mclmac->powerMode.nextState == mode);

    return passed;
}

bool test_mclmac_update_powermode_state_machine(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_powermode_state_machine(mclmac);

    // Try to transition from STARTP state to ACTIVE, TRANSMIT,  RECEIVE or FINISHP states.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_powermode_state(mclmac, STARTP);
    mclmac_set_next_powermode_state(mclmac, ACTIVE);
    int r = mclmac_update_powermode_state_machine(mclmac);
    bool passed = true;
    passed = passed && (r == E_PM_TRANSITION_ERROR);
    passed = passed && (mclmac->powerMode.currentState == STARTP);
    
    mclmac_set_next_powermode_state(mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);
    passed = passed && (mclmac->powerMode.currentState == STARTP);
    
    mclmac_set_next_powermode_state(mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);
    passed = passed && (mclmac->powerMode.currentState == STARTP);
    
    mclmac_set_next_powermode_state(mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);
    passed = passed && (mclmac->powerMode.currentState == STARTP);

    // Try to transition from PASSIVE state to TRANSMIT,  RECEIVE or FINISHP states.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_powermode_state(mclmac, PASSIVE);
    mclmac_set_next_powermode_state(mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);
    passed = passed && (mclmac->powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);
    passed = passed && (mclmac->powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);
    passed = passed && (mclmac->powerMode.currentState == PASSIVE);

    // Try to transit from FINISHP to any other state
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_powermode_state(mclmac, FINISHP);

    mclmac_set_next_powermode_state(mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);

    mclmac_set_next_powermode_state(mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);

    mclmac_set_next_powermode_state(mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);

    mclmac_set_next_powermode_state(mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);

    // Once in finish state, to use once again the state machine,
    // call mclmac_init_powermode_state_machine, and go to PASSIVE state
    mclmac_init_powermode_state_machine(mclmac);
    mclmac_set_powermode_state(mclmac, STARTP);
    mclmac_set_next_powermode_state(mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);

    // Transit from PASSIVE to ACTIVE
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == ACTIVE);

    // Transit from ACTIVE to PASSIVE
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == PASSIVE);

    // Transit from PASSIVE to TRANSMIT. Remember to pass through ACTIVE,
    // as specified by the state machine.
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == TRANSMIT);

    // Try to get to RECEIVE mode.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);
    passed = passed && (mclmac->powerMode.currentState == TRANSMIT);

    // Try to get to ACTIVE mode.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);

    // You are in TRANSMIT mode, in order to get to RECEIVE mode, 
    // you have to get to PASSIVE again, an travers the path to 
    // RECEIVE state, as indicated by the state machine.
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == RECEIVE);

    // Try to get to TRANSMIT mode.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);

    // Try to get to ACTIVE mode
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_ERROR);

    // Follow the path: PASSIVE -> ACTIVE -> FINISHP
    mclmac_set_next_powermode_state(mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == FINISHP);

    // Travers the path: PASSIVE -> ACTIVE -> TRANSMIT -> FINISHP
    mclmac_init_powermode_state_machine(mclmac);
    mclmac_set_powermode_state(mclmac, STARTP);
    mclmac_set_next_powermode_state(mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == TRANSMIT);

    mclmac_set_next_powermode_state(mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == FINISHP);

    // Travers the path: PASSIVE -> ACTIVE -> RECEIVE -> FINISHP
    mclmac_init_powermode_state_machine(mclmac);
    mclmac_set_powermode_state(mclmac, STARTP);
    mclmac_set_next_powermode_state(mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == TRANSMIT);

    mclmac_set_next_powermode_state(mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_TRANSITION_SUCCESS);
    passed = passed && (mclmac->powerMode.currentState == FINISHP);

    // If nextState is NONEP, return E_PM_NO_TRANSITION
    mclmac_set_next_powermode_state(mclmac, NONEP);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_NO_TRANSITION);

    // If invalid state, return E_PM_INVALID_STATE
    mclmac_set_next_powermode_state(mclmac, 10);
    r = mclmac_update_powermode_state_machine(mclmac);
    passed = passed && (r == E_PM_INVALID_STATE);

    return passed;
}

bool test_startp_state_powermode_stmachine(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * We will begin the execution of the PowerMode state machine for medium access.
     * We just initialized the state machine, and now should execute the STARTP state of the 
     * machine. We will start execution of this machine only at beginning of a frame. 
     * The This state will do the following:
     *  -Set the number of slots.
     *  -Set the number of cf slots.
     *  -Set the duration of the frame.
     *  -Set the duration of the slots.
     *  -Set the duration of the cf slots.
     *  -Set the current slot to 0.
     *  -Set the current frame, obtained from _wakeup_frame + 1.
     *  -Set the current cf slot to 0.
     *  -Set the cf channel.
     *  -Immedialy pass to the PASSIVE state.
     *  -Arm for the first time the slot_timer timeout.
     */
    int ret = mclmac_execute_powermode_state(mclmac);
    bool passed = true;
    passed = passed && (ret == E_PM_EXECUTION_SUCCESS);
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)slots_number == mclmac->_nSlots);
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)cf_slots_number == mclmac->_nChannels);
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)frame_duration == TIME(FRAME_DURATION));
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)slot_duration == TIME(SLOT_DURATION));
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)cf_duration == TIME(CF_SLOT_DURATION));
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_frame == mclmac->_wakeup_frame + 1);
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_slot == 0);
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_cf_slot == 0);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_message == 0);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_control == 0);
    passed = passed && (ARROW(mclmac->mac)_number_packets_received == 0);
    passed = passed && (ARROW(mclmac->mac)_first_send_message == 0);
    passed = passed && (ARROW(mclmac->mac)_last_send_message == 0);
    passed = passed && (ARROW(mclmac->mac)_first_send_control == 0);
    passed = passed && (ARROW(mclmac->mac)_last_send_control == 0);
    passed = passed && (ARROW(mclmac->mac)_first_received == 0);
    passed = passed && (ARROW(mclmac->mac)_last_received == 0);
    passed = passed && (ARROW(mclmac->mac)cfChannel == CF_FREQUENCY);
    passed = passed && (mclmac->powerMode.nextState == PASSIVE);

    timeout_unset(ARROW(ARROW(mclmac->mac)frame)slot_timer);

    return passed;
}

bool test_passive_state_powermode_stmachine(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_powermode_state_machine(mclmac);

    // Execute the STARTP state, and update state to PASSIVE state.
    int ret = mclmac_execute_powermode_state(mclmac);
    ret = mclmac_update_powermode_state_machine(mclmac);

#ifdef __LINUX__
    mclmac->_self_pid = pthread_self();
#endif
#ifdef __RIOT__
    mclmac->_self_pid = thread_getpid();
#endif
    open_queue(mclmac->_mac_queue_id, mclmac->_self_pid);

    uint8_t msg[MAX_MESSAGE_SIZE];
    uint i;
    for (i = 0; i < MAX_MESSAGE_SIZE; i++)
        msg[i] = rand();
    msg[0] = 2;
    send_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, mclmac->_self_pid);
    ARROW(mclmac->mac)_number_packets_received = 0;
    /** 
     * We are now at the PASSIVE state. This state sets the radio on SLEEP mode, and
     * execute other pertinent functions:
     *  -Set the radio to SLEEP mode.
     *  -Start the slot timer.
     *  -On a loop, do the following:
     *      *Check whether the timeout has expired.
     *      *Once the timeout expires, unarm it and end the cycle.
     *      *Read/write packets from/to other layers for sending or further processing.
     *      *With the packets receive from the upper layers, create an array containing
     *       the IDs of the nodes to send the packet.
     *  -It should return:
     *      *E_PM_EXECUTION_SUCCESS.
     */
    // For control packets
    uint32_t time = mclmac->_networkTime;
    ret = mclmac_execute_powermode_state(mclmac);
    bool passed = true;
    passed = passed && (ret == E_PM_EXECUTION_SUCCESS);
    passed = passed && (mclmac->powerMode.nextState == ACTIVE);
    passed = passed && (mclmac->_networkTime == time + 1);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_control > 0);
    passed = passed && (elements_on_queue(mclmac->_mac_queue_id) == 0);

    // For message packets
    msg[0] = 7;
    send_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, mclmac->_self_pid);
    mclmac->powerMode.nextState = PASSIVE;
    time = mclmac->_networkTime;
    ret = mclmac_execute_powermode_state(mclmac);
    passed = passed && (ret == E_PM_EXECUTION_SUCCESS);
    passed = passed && (mclmac->powerMode.nextState == ACTIVE);
    passed = passed && (mclmac->_networkTime == time + 1);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_message > 0);
    passed = passed && (elements_on_queue(mclmac->_mac_queue_id) == 0);
    timeout_unset(ARROW(ARROW(mclmac->mac)frame)slot_timer);
    ARROW(mclmac->mac)_packets_to_send_message = 0;
    ARROW(mclmac->mac)_packets_to_send_control = 0;
    ARROW(mclmac->mac)_last_send_message = 0;
    ARROW(mclmac->mac)_last_send_control = 0;
    DataPacket_t *pkt = &ARROW(mclmac->mac)_message_packets_to_send[0];
    datapacket_clear(pkt);
    pkt = &ARROW(mclmac->mac)_control_packets_to_send[0];
    datapacket_clear(pkt);

    msg[0] = 2;
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++)
        send_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, mclmac->_self_pid);
    ARROW(ARROW(mclmac->mac)frame)slot_timer = timeout_set(TIME(SLOT_DURATION));
    ret = mclmac_execute_powermode_state(mclmac);
    passed = passed && (ret == E_PM_EXECUTION_SUCCESS);
    passed = passed && (elements_on_queue(mclmac->_mac_queue_id) == 0);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_control == MAX_ELEMENTS_ON_QUEUE);
    timeout_unset(ARROW(ARROW(mclmac->mac)frame)slot_timer);

    msg[0] = 7;
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++) {
        ret = send_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, mclmac->_self_pid);
    }
    ARROW(ARROW(mclmac->mac)frame)slot_timer = timeout_set(TIME(SLOT_DURATION));
    ret = mclmac_execute_powermode_state(mclmac);
    passed = passed && (ret == E_PM_EXECUTION_SUCCESS);
    passed = passed && (elements_on_queue(mclmac->_mac_queue_id) == 0);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_message == MAX_ELEMENTS_ON_QUEUE);
    timeout_unset(ARROW(ARROW(mclmac->mac)frame)slot_timer);
    close_queue(mclmac->_mac_queue_id);

    return passed;
}

bool test_active_state_powermode_stmachine(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);
    mclmac_init_powermode_state_machine(mclmac);

    /* Execute the MAC state machine, as we require a time slot selected. */
    /*Execute START state */
    int ret = mclmac_execute_mac_state_machine(mclmac);
    ret = mclmac_update_mac_state_machine(mclmac);

    /* Execute INITIALIZATION state, the first time will fail. */
    mclmac->_init_state = 1;
    ret = mclmac_execute_mac_state_machine(mclmac);
    ret = mclmac_update_mac_state_machine(mclmac);

    mclmac->_initTime = rand() % 100;
    mclmac->_networkTime = (rand() % 1000) + mclmac->_initTime;
    mclmac->_hopCount = rand() % 10;
    ARROW(ARROW(mclmac->mac)frame)current_frame = rand() % 10;
    ARROW(ARROW(mclmac->mac)frame)current_slot = rand() % MAX_NUMBER_SLOTS;
    /* Execute the SYNCHRONIZATION state. The slots and frequency returned data will be random, 
    so we will modify it for testing. */
    ret = mclmac_execute_mac_state_machine(mclmac);
    ret = mclmac_update_mac_state_machine(mclmac);

    /* Execute the TIMESLOT_AND_CHANNEL_SELECTION state, assuring a free slot */
    uint8_t slot = 0x7f;
    int n = MAX_NUMBER_FREQS;
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            mclmac->_occupied_frequencies_slots[i][j] = slot;
        }
    }
    ret = mclmac_execute_mac_state_machine(mclmac);

    /* Execute the PowerMode State machine. */
    // Execute the STARTP state, and update state to PASSIVE state.
    ret = mclmac_execute_powermode_state(mclmac);
    ret = mclmac_update_powermode_state_machine(mclmac);
    // Add packets to the queue
    uint8_t msg[MAX_MESSAGE_SIZE];
    uint i;
    for (i = 0; i < MAX_MESSAGE_SIZE; i++)
        msg[i] = rand();
    msg[0] = 2;
    send_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, mclmac->_self_pid);
    msg[0] = 7;
    send_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, mclmac->_self_pid);
    // Execute the PASSIVE state and update state machine.
    ret = mclmac_execute_powermode_state(mclmac);
    ret = mclmac_update_powermode_state_machine(mclmac);
    /**
     * We are now at the ACTIVE state. This state will determine whether the node should 
     * transmit or receive a packet, or if there is a collision.
     * It should transmit when the following conditions comply:
     *  -A packet is ready to send.
     *  -The current timeslot corresponds to the node's timeslot
     * Reception is done when a packet is received in any of the slots, other than 
     * the slot corresponding to the node's slot.
     * A collision occurs in the following situation:
     *  -Two nodes want to transmit to the node in the same slot.
     *  -A node wants to transmit in the same slot as the current node.
     * 
     * In this state, the CF phase is executed. The CF phase has the following characteristics:
     *  -It last CF_PHASE_PERCENTAGE precentage of the slot time.
     *  -It is divided into MAX_NUMBER_FREQS slots, each of equal duration.
     *  -Each slot corresponds to a frequency, the number of the slot is the index of the frequency.
     *  -If the current slot corresponds to the slot selected by the node, it waits to the index
     *   of its frequency, and sends a packet.
     *  -A timer is necessary to indicate the end of each slot.
     *  -The radio should be set to the CF frequency at the beginning of the phase.
     * 
     * Steps to perform in the state:
     *  -Start the CF phase:
     *      -Change radio to CF frequency and set the radio to rx_single.
     *      -Set to 0 the current cf channel.
     *  -Initiate a timer for the cf slot.
     *  -Get the current slot and the selected slot.
     *  -Get the selected frequency.
     *  -When the current slot corresponds to the selected one.
     *      -Check if there are packets to send. 
     *      -Send a cf packet in the corresponding slot.
     *      -Set the next state to TRANSMIT.
     *  -Wait for any other packets on the other frequencies.
     *      -When a packet is received, check if the destinationID corresponds 
     *       to the current node id.
     *      -Store the frequency and the node id.
     *      -Check the next state to RECEIVE.
     *  -If no packets are received, set the next state to PASSIVE.
     *  -Increase the cf_slots counter.
     * 
     * It should return:
     *  -E_PM_EXECUTION_SUCCESS when there is no error.
     *  -The next state should be PASSIVE, TRANSMIT, or RECEIVE, in case of success.
     *  -In case of collision error, it should be E_PM_SYNCHRONIZATION_ERROR.
     */
    /* Test case 1:
        -The current slot corresponds to the selected slot, and there is a packet to send.
        It should return:
            -E_PM_EXECUTION_SUCCESS.
            -The next state should be TRANSMIT.
            -The current slot should not be increased.
            -The current cf slot should be equal to the number of frequencies.
            -The time passed should be approximately 20% of the total slot time.
    */
    mclmac->_trues = 0;
    mclmac->_trues5 = 0;
    // set state to 1, to indicate first test case.
    mclmac->_state_cf = 1;
    mclmac_set_current_slot(mclmac, 0);
    ret = mclmac_execute_powermode_state(mclmac);
    bool passed1 = true;
    passed1 = passed1 && (ret == E_PM_EXECUTION_SUCCESS);
    passed1 = passed1 && (mclmac->powerMode.nextState == TRANSMIT);
    passed1 = passed1 && (mclmac->powerMode.currentState == ACTIVE);
    passed1 = passed1 && (ARROW(ARROW(mclmac->mac)frame)current_cf_slot == MAX_NUMBER_FREQS);
    passed1 = passed1 && (ARROW(mclmac->mac)_is_internal_collision == false);
    if (!passed1) {
        printf("Test case 1 \033[1;31mfailed\033[0m.\n");
    }

    /* Test case 2:
        -There are no packets to transmit, but a packet is received.
        It should return:
            -E_PM_EXECUTION_SUCCESS.
            -The next state should be RECEIVE.
            -The current cf slot should be equal to the total number of frequencies.
    */
    mclmac->_state_cf = 2;
    mclmac_set_current_slot(mclmac, 1U);
    mclmac_set_current_cf_slot(mclmac, 1U);
    ret = mclmac_execute_powermode_state(mclmac);
    char str[1000];
    bool passed2 = true;
    passed2 = check_condition(passed2, ret == E_PM_EXECUTION_SUCCESS, "ret == E_PM_EXECUTION_SUCCESS", str);
    passed2 = check_condition(passed2, mclmac->powerMode.nextState == RECEIVE, "mclmac->powerMode.nextState == RECEIVE", str);
    passed2 = check_condition(passed2, mclmac->powerMode.currentState == ACTIVE, "mclmac->powerMode.currentState == ACTIVE", str);
    passed2 = check_condition(passed2, ARROW(ARROW(mclmac->mac)frame)current_cf_slot == MAX_NUMBER_FREQS, "mclmac->mac->frame->current_cf_slot == MAX_NUMBER_FREQS", str);
    passed2 = check_condition(passed2, ARROW(mclmac->mac)_is_internal_collision == false, "mclmac->mac->_is_internal_collision == false", str);
    if (!passed2) {
        printf("Test case 2 \033[1;31mfailed\033[0m.\n");
        printf("%s\n", str);
    }
    // Clear str
    memset(str, 0, 1000);

    /* Test case 3:
            -There are no packets to receive or send.
            It should return:
                -E_PM_EXECUTION_SUCCESS.
                -The next state should be PASSIVE.
                -The current cf slot should be equal to the total number of frequencies.
    */
    mclmac->_state_cf = 3;
    ARROW(mclmac->mac)_packets_to_send_message = 0;
    ARROW(mclmac->mac)_packets_to_send_control = 0;
    ret = mclmac_execute_powermode_state(mclmac);
    bool passed3 = true;
    passed3 = passed3 && (ret == E_PM_EXECUTION_SUCCESS);
    passed3 = passed3 && (mclmac->powerMode.nextState == PASSIVE);
    passed3 = passed3 && (mclmac->powerMode.currentState == ACTIVE);
    passed3 = passed3 && (ARROW(ARROW(mclmac->mac)frame)current_cf_slot == MAX_NUMBER_FREQS);
    passed3 = passed3 && (ARROW(mclmac->mac)_is_internal_collision == false);
    if (!passed3) {
        printf("Test case 3 \033[1;31mfailed\033[0m.\n");
    }

    /*  Test case 4:
            -A packet was received and it should send a packet.
            It should return:
                -E_PM_COLLISION_DETECTED.
                -The next state should be PASSIVE.
                -The current cf slot should be equal to the total number of frequencies.
    */
    mclmac->_state_cf = 4;
    ARROW(mclmac->mac)_packets_to_send_message = 1;
    mclmac_set_current_slot(mclmac, 0);
    ret = mclmac_execute_powermode_state(mclmac);
    bool passed4 = true;
    passed4 = check_condition(passed4, ret == E_PM_COLLISION_DETECTED, "ret == E_PM_COLLISION_DETECTED", str);
    passed4 = check_condition(passed4, mclmac->powerMode.nextState == PASSIVE, "mclmac->powerMode.nextState == PASSIVE", str);
    passed4 = check_condition(passed4, mclmac->powerMode.currentState == ACTIVE, "mclmac->powerMode.currentState == ACTIVE", str);
    passed4 = check_condition(passed4, ARROW(ARROW(mclmac->mac)frame)current_cf_slot == MAX_NUMBER_FREQS, "mclmac->mac->frame->current_cf_slot == MAX_NUMBER_FREQS", str);
    passed4 = check_condition(passed4, ARROW(mclmac->mac)_is_internal_collision == true, "mclmac->mac->_is_internal_collision == true", str);
    if (!passed4) {
        printf("Test case 4 \033[1;31mfailed\033[0m.\n");
        printf("%s\n", str);
    }
    memset(str, 0, 1000);

    /*  Test case 5:
            -Two packets should be received, and no packets to sent.
            It should return:
                -E_PM_COLLISION_DETECTED.
                -The next state should be PASSIVE.
                -The current cf slot should be equal to the total number of frequencies.
    */
    ARROW(mclmac->mac)_is_internal_collision = false;
    mclmac->_state_cf = 5;
    mclmac_set_current_slot(mclmac, 1U);
    ret = mclmac_execute_powermode_state(mclmac);
    bool passed5 = true;
    passed5 = check_condition(passed5, ret == E_PM_COLLISION_DETECTED, "ret == E_PM_COLLISION_DETECTED", str);
    passed5 = check_condition(passed5, mclmac->powerMode.nextState == PASSIVE, "mclmac->powerMode.nextState == PASSIVE", str);
    passed5 = check_condition(passed5, mclmac->powerMode.currentState == ACTIVE, "mclmac->powerMode.currentState == ACTIVE", str);
    passed5 = check_condition(passed5, ARROW(ARROW(mclmac->mac)frame)current_cf_slot == MAX_NUMBER_FREQS, "mclmac->mac->frame->current_cf_slot == MAX_NUMBER_FREQS", str);
    passed5 = check_condition(passed5, ARROW(mclmac->mac)_is_internal_collision == false, "mclmac->mac->_is_internal_collision == false", str);
    if (!passed5) {
        printf("Test case 5 \033[1;31mfailed\033[0m.\n");
        printf("%s", str);
    }
    memset(str, 0, 1000);

    /* Test case 6:
            -A collision was detected previously, that is, _collisionDetedted = true.
            It should as well be _destination_id = 0.
            It should return:
                -E_PM_EXECUTION_SUCCESS.
                -The next state should be TRANSMIT.
    */
    mclmac->_state_cf = 1;
    ARROW(mclmac->mac)_collisionDetected = true;
    ARROW(mclmac->mac)_destination_id[0] = 0;
    ARROW(mclmac->mac)_destination_id[1] = 0;
    mclmac_set_current_slot(mclmac, 0);
    mclmac_set_current_cf_slot(mclmac, 0);
    ret = mclmac_execute_powermode_state(mclmac);
    bool passed6 = true;
    passed6 = check_condition(passed6, ret == E_PM_EXECUTION_SUCCESS, "ret == E_PM_EXECUTION_SUCCESS", str);
    passed6 = check_condition(passed6, mclmac->powerMode.nextState == TRANSMIT, "mclmac->powerMode.nextState == TRANSMIT", str);
    passed6 = check_condition(passed6, mclmac->powerMode.currentState == ACTIVE, "mclmac->powerMode.currentState == ACTIVE", str);
    passed6 = check_condition(passed6, !ARROW(mclmac->mac)_is_internal_collision, "!mclmac->mac->_is_internal_collision", str);
    if (!passed6) {
        printf("Test case 6 \033[1;31mfailed\033[0m.\n");
        printf("%s", str);
    }

    return passed1 && passed2 && passed3 && passed4 && passed5 & passed6;
}

bool test_transmit_powermode_stmachine(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);
    mclmac_init_powermode_state_machine(mclmac);

    /* Execute the MAC state machine, as we require a time slot selected. */
    /*Execute START state */
    int ret = mclmac_execute_mac_state_machine(mclmac);
    ret = mclmac_update_mac_state_machine(mclmac);

    /* Execute INITIALIZATION state, the first time will fail. */
    mclmac->_init_state = 1;
    ret = mclmac_execute_mac_state_machine(mclmac);
    ret = mclmac_update_mac_state_machine(mclmac);

    mclmac->_initTime = rand() % 100;
    mclmac->_networkTime = (rand() % 1000) + mclmac->_initTime;
    mclmac->_hopCount = rand() % 10;
    ARROW(ARROW(mclmac->mac)frame)current_frame = rand() % 10;
    ARROW(ARROW(mclmac->mac)frame)current_slot = rand() % MAX_NUMBER_SLOTS;
    /* Execute the SYNCHRONIZATION state. The slots and frequency returned data will be random, 
    so we will modify it for testing. */
    ret = mclmac_execute_mac_state_machine(mclmac);
    ret = mclmac_update_mac_state_machine(mclmac);

    /* Execute the TIMESLOT_AND_CHANNEL_SELECTION state, assuring a free slot */
    uint8_t slot = 0x7f;
    int n = MAX_NUMBER_FREQS;
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            mclmac->_occupied_frequencies_slots[i][j] = slot;
        }
    }
    ret = mclmac_execute_mac_state_machine(mclmac);
    /* Execute the PowerMode State machine. */
    // Execute the STARTP state, and update state to PASSIVE state.
    ret = mclmac_execute_powermode_state(mclmac);
    ret = mclmac_update_powermode_state_machine(mclmac);
    // Add packets to the queue
    uint8_t msg[MAX_MESSAGE_SIZE];
    uint i;
    for (i = 0; i < MAX_MESSAGE_SIZE; i++)
        msg[i] = rand();
    msg[0] = 2;
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE / 2; i++)
        send_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, mclmac->_self_pid);
    msg[0] = 7;
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE / 2; i++)
        send_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, mclmac->_self_pid);
    // Execute the PASSIVE state and update state machine.
    ret = mclmac_execute_powermode_state(mclmac);
    ret = mclmac_update_powermode_state_machine(mclmac);
    bool passed = true;
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_message == MAX_ELEMENTS_ON_QUEUE / 2);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_control == MAX_ELEMENTS_ON_QUEUE / 2);
    /* Transit to state ACTIVE, and simulate that a packet should be sent. */
    mclmac->_state_cf = 1;
    mclmac_set_current_slot(mclmac, 0);
    ret = mclmac_execute_powermode_state(mclmac);
    ret = mclmac_update_powermode_state_machine(mclmac);

    /**  We are now at the TRANSMIT state, where we are at our own slot, and a packet should
     * sent. A CF packet was already sent to the corresponding channel, now we should execute the
     * TRANSMIT state. On the TRANSMIT state, the split phase is executed, which consists of
     * the following actions:
     *      -Set the radio to TX mode.
     *      -Create and send a control packet, including if any collisions were found, at which slot and frequency.
     *          -If a previous packet was created, remove its content.
     *          -Create a new packet with the updated data.
     *          -Get the node id, network time, init time, and hop count from the MCLMAC data structure.
     *          -Get the current frame and current slot from the Frame data structure.
     *          -Get the collision slot and collsision frequency from the MAC_Internals data structure. 
     *           If there is no collision detected, set to 0 both values.
     *      -Send the data packet or packets.
     *          -Read the most recent packet from the queue.
     *          -Create a data packet from it.
     *          -Copy to the radio and send.
     *      -Set the radio to RX mode.
     *      -Wait for another control packet which contains the ACK for the sent packets.
     *      -Get the ACK fields and verify which packets were received.
     *  For the ACKs, we will take a similar approach to that of the transport layer, report only a window
     * of received packets. This layer won't retransmit any packet in case of packet loss, it will
     * report the loss to upper layers, which will decide what to do about it.
    */
    ret = mclmac_execute_powermode_state(mclmac);
    passed = passed && (ret == E_PM_EXECUTION_SUCCESS);
    // All packets were send
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_message == 0);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_control == 0);
    passed = passed && (ARROW(mclmac->mac)_first_send_control == ARROW(mclmac->mac)_last_send_control);
    passed = passed && (ARROW(mclmac->mac)_first_send_message == ARROW(mclmac->mac)_last_send_message);
    passed = passed && (mclmac->powerMode.currentState == TRANSMIT);
    passed = passed && (mclmac->powerMode.nextState == PASSIVE);

    // Simulate an external collision detection
    ARROW(mclmac->mac)_collisionDetected = true;
    ARROW(mclmac->mac)_is_internal_collision = false;
    ARROW(mclmac->mac)_collisionSlot = rand() % MAX_NUMBER_SLOTS;
    ARROW(mclmac->mac)_collisionFrequency = mclmac->_frequencies[2];
    mclmac_set_next_powermode_state(mclmac, TRANSMIT);
    ret = mclmac_execute_powermode_state(mclmac);
    passed = passed && (ret == E_PM_EXECUTION_SUCCESS);
    passed = passed && (ARROW(mclmac->mac)_collisionDetected == false);
    passed = passed && (ARROW(mclmac->mac)_is_internal_collision == false);
    passed = passed && (ARROW(mclmac->mac)_collisionSlot == 0);
    passed = passed && (ARROW(mclmac->mac)_collisionFrequency == 0);
    passed = passed && (mclmac->powerMode.nextState == PASSIVE);

    // Simulate an internal collision detection
    ARROW(mclmac->mac)_collisionDetected = true;
    ARROW(mclmac->mac)_is_internal_collision = true;
    ARROW(mclmac->mac)_collisionSlot = rand() % MAX_NUMBER_SLOTS;
    ARROW(mclmac->mac)_collisionFrequency = mclmac->_frequencies[2];
    mclmac_set_next_powermode_state(mclmac, TRANSMIT);
    ret = mclmac_execute_powermode_state(mclmac);
    passed = passed && (ret == E_PM_COLLISION_ERROR);
    passed = passed && (ARROW(mclmac->mac)_collisionDetected == false);
    passed = passed && (ARROW(mclmac->mac)_is_internal_collision == false);
    passed = passed && (ARROW(mclmac->mac)_collisionSlot == 0);
    passed = passed && (ARROW(mclmac->mac)_collisionFrequency == 0);
    passed = passed && (mclmac->powerMode.nextState == FINISHP);

    return passed;
}

bool test_receive_state_powermode_stmachine(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);
    mclmac_init_powermode_state_machine(mclmac);

    /* Execute the MAC state machine, as we require a time slot selected. */
    /*Execute START state */
    int ret = mclmac_execute_mac_state_machine(mclmac);
    ret = mclmac_update_mac_state_machine(mclmac);

    /* Execute INITIALIZATION state, the first time will fail. */
    mclmac->_init_state = 1;
    ret = mclmac_execute_mac_state_machine(mclmac);
    ret = mclmac_update_mac_state_machine(mclmac);

    mclmac->_initTime = rand();
    mclmac->_networkTime = (rand() % 1000) + mclmac->_initTime;
    mclmac->_hopCount = rand() % 1000;
    ARROW(ARROW(mclmac->mac)frame)current_frame = rand() % 10;
    ARROW(ARROW(mclmac->mac)frame)current_slot = rand() % MAX_NUMBER_SLOTS;
    /* Execute the SYNCHRONIZATION state. The slots and frequency returned data will be random, 
    so we will modify it for testing. */
    ret = mclmac_execute_mac_state_machine(mclmac);
    ret = mclmac_update_mac_state_machine(mclmac);

    /* Execute the TIMESLOT_AND_CHANNEL_SELECTION state, assuring a free slot */
    uint8_t slot = 0x7f;
    int n = MAX_NUMBER_FREQS;
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            mclmac->_occupied_frequencies_slots[i][j] = slot;
        }
    }
    ret = mclmac_execute_mac_state_machine(mclmac);
    /* Execute the PowerMode State machine. */
    // Execute the STARTP state, and update state to PASSIVE state.
    ret = mclmac_execute_powermode_state(mclmac);
    ret = mclmac_update_powermode_state_machine(mclmac);

    // Execute the PASSIVE state and update state machine.
    ret = mclmac_execute_powermode_state(mclmac);
    ret = mclmac_update_powermode_state_machine(mclmac);

    /* Transit to state ACTIVE, and simulate that we should receive a packet. */
    mclmac->_trues = 0;
    mclmac->_trues5 = 0;
    mclmac->_state_cf = 2;
    mclmac_set_current_slot(mclmac, 1U);
    mclmac_set_current_cf_slot(mclmac, 1U);
    ret = mclmac_execute_powermode_state(mclmac);
    ret = mclmac_update_powermode_state_machine(mclmac);
    
    /** We are now at the RECEIVE state, where we should receive incoming packets from 
     * the medium and store it on the array _packets_received. We shall check only for the packet
     * types 0 or 1 (MAC control packets), that we should immediatly discard. Any other packets
     * shall be stored on the array for subsequent retransmission to upper layers. This state also
     * handle errors such as synchronization and collision notifications received from other nodes.
     * Receive 2 + MAX_NUMBER_DATA_PACKETS.
    */
    char str[1000] = {0};
    /* Test first the case no control packet arrives. */
    mclmac->_state_ctrl = 7;
    bool passed1 = true;
    passed1 = check_condition(passed1, mclmac->powerMode.nextState == RECEIVE, "mclmac->powerMode.nextState == RECEIVE", str);
    passed1 = check_condition(passed1, mclmac->powerMode.currentState == RECEIVE, "mclmac->powerMode.currentState == RECEIVE", str);
    ret = mclmac_execute_powermode_state(mclmac);
    passed1 = check_condition(passed1, ret == E_PM_EXECUTION_SUCCESS, "ret == E_PM_EXECUTION_SUCCESS", str);
    passed1 = check_condition(passed1, mclmac->powerMode.nextState == PASSIVE, "mclmac->powerMode.nextState == PASSIVE", str);
    if (!passed1) {
        printf("Test case 0 failed.\n%s", str);
        memset(str, 0, 1000);
    }

    mclmac->powerMode.nextState = RECEIVE;
    mclmac->_state_ctrl = 1;
    bool passed2 = true;
    passed2 = check_condition(passed2, mclmac->powerMode.nextState == RECEIVE, "mclmac->powerMode.nextState == RECEIVE", str);
    passed2 = check_condition(passed2, mclmac->powerMode.currentState == RECEIVE, "mclmac->powerMode.currentState == RECEIVE", str);
    ret = mclmac_execute_powermode_state(mclmac);
    ControlPacket_t *ctrlpkt = REFERENCE ARROW(mclmac->mac)ctrlpkt_recv;
    passed2 = check_condition(passed2, ret == E_PM_EXECUTION_SUCCESS, "ret == E_PM_EXECUTION_SUCCESS", str);
    passed2 = check_condition(passed2, ctrlpkt->node_id[0] == ARROW(mclmac->mac)transmitter_id[0], "ctrlpkt->node_id[0] == mclmac->mac->transmitter_id[0]", str);
    passed2 = check_condition(passed2, ctrlpkt->node_id[1] == ARROW(mclmac->mac)transmitter_id[1], "ctrlpkt->node_id[1] == mclmac->mac->transmitter_id[1]", str);
    passed2 = check_condition(passed2, ctrlpkt->currentFrame == ARROW(ARROW(mclmac->mac)frame)current_frame, "ctrlpkt->currentFrame == mclmac->mac->frame->current_frame", str);
    passed2 = check_condition(passed2, ctrlpkt->currentSlot == ARROW(ARROW(mclmac->mac)frame)current_slot, "ctrlpkt->currentSlot == mclmac->mac->frame->current_slot", str);
    passed2 = check_condition(passed2, ctrlpkt->collisionSlot == 0, "ctrlpkt->collisionSlot == 0", str);
    passed2 = check_condition(passed2, ctrlpkt->collisionFrequency == 0, "ctrlpkt->collisionFrequency == 0", str);
    passed2 = check_condition(passed2, ctrlpkt->hopCount == mclmac->_hopCount, "ctrlpkt->hopCount == mclmac->_hopCount", str);
    passed2 = check_condition(passed2, ctrlpkt->networkTime == mclmac->_networkTime, "ctrlpkt->networkTime == mclmac->_networkTime", str);
    passed2 = check_condition(passed2, ctrlpkt->initTime == mclmac->_initTime, "ctrlpkt->initTime == mclmac->_initTime", str);
    passed2 = check_condition(passed2, mclmac->powerMode.nextState == PASSIVE, "mclmac->powerMode.nextState == PASSIVE", str);
    passed2 = check_condition(passed2, mclmac->powerMode.currentState == RECEIVE, "mclmac->powerMode.currentState == RECEIVE", str);
    passed2 = check_condition(passed2, ARROW(mclmac->mac)_number_packets_received == 2 * MAX_NUMBER_DATA_PACKETS, "mclmac->mac->_number_packets_received == 2 * MAX_NUMBER_DATA_PACKETS", str);
    strcat(str, "Within for\n");
    for (uint i = 0; i < 2 * MAX_NUMBER_DATA_PACKETS; i++)
    {
        DataPacket_t *pkt = &ARROW(mclmac->mac)_packets_received[i];
        passed2 = check_condition(passed2, pkt->destination_id[0] == mclmac->_node_id[0], "pkt->destination_id[0] == mclmac->_node_id[0]", str);
        passed2 = check_condition(passed2, pkt->destination_id[1] == mclmac->_node_id[1], "pkt->destination_id[1] == mclmac->_node_id[1]", str);
        passed2 = check_condition(passed2, pkt->type > 1, "pkt->type > 1", str);
        passed2 = check_condition(passed2, pkt->size > 0, "pkt->size > 0", str);
#ifdef __LINUX__
        passed2 = check_condition(passed2, pkt->data != NULL, "pkt->data != NULL", str);
#endif
#ifdef __RIOT__
        passed2 = check_condition(passed2, pkt->data.size > 0, "pkt->data.size > 0", str);
#endif
    }
    if (!passed2) {
        printf("Test case 2 failed.\n%s", str);
    }
    memset(str, 0, 1000);

    /**
     * Check the case when a collision is indicated in the node's selected slot and 
     * frequency. This happens when the received collisionSlot and collisionFrequency
     * correspond to the node's frequency and slot.
     * The control packet should hold such information, and the function should
     * return E_PM_COLLISION_ERROR. The next state should be FNISHP.
     */
    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 3;
    ret = mclmac_execute_powermode_state(mclmac);
    bool passed3 = true;
    passed3 = check_condition(passed3, ret == E_PM_COLLISION_ERROR, "ret == E_PM_COLLISION_ERROR", str);
    passed3 = check_condition(passed3, mclmac->powerMode.currentState == RECEIVE, "mclmac->powerMode.currentState == RECEIVE", str);
    passed3 = check_condition(passed3, mclmac->powerMode.nextState == FINISHP, "mclmac->powerMode.nextState == FINISHP", str);
    passed3 = check_condition(passed3, ctrlpkt->collisionSlot == ARROW(mclmac->mac)selectedSlot, "ctrlpkt->collisionSlot == mclmac->mac->selectedSlot", str);
    passed3 = check_condition(passed3, ctrlpkt->collisionFrequency == ARROW(mclmac->mac)transmitChannel, "ctrlpkt->collisionFrequency == mclmac->mac->transmitChannel", str);
    if (!passed3) {
        printf("Test case 3.\n%s", str);
        memset(str, 0, 1000);
    }

    /* Current slot is different from packet's current slot.
    * Return E_PM_SYNCHRONIZATION_ERROR. Next state is FINISHP.
    */
    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 4;
    mclmac->powerMode.nextState = RECEIVE;
    ret = mclmac_execute_powermode_state(mclmac);
    bool passed4 = true;
    passed4 = check_condition(passed4, ret == E_PM_SYNCHRONIZATION_ERROR, "ret == E_PM_SYNCHRONIZATION_ERROR", str);
    passed4 = check_condition(passed4, mclmac->powerMode.currentState == RECEIVE, "mclmac->powerMode.currentState == RECEIVE", str);
    passed4 = check_condition(passed4, mclmac->powerMode.nextState == FINISHP, "mclmac->powerMode.nextState == FINISHP", str);
    passed4 = check_condition(passed4, ctrlpkt->currentSlot != ARROW(ARROW(mclmac->mac)frame)current_slot, "ctrlpkt->currentSlot != mclmac->mac->frame->current_slot", str);
    if (!passed4) {
        printf("Test case 4 failed.\n%s", str);
        memset(str, 0, 1000);
    }

    /* Current frame is different from the node's current frame.
    * Return E_PM_SYNCHRONIZATION_ERROR. Next state is FINISHP.
    */
    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 5;
    mclmac->powerMode.nextState = RECEIVE;
    ret = mclmac_execute_powermode_state(mclmac);
    bool passed5 = true;
    passed5 = check_condition(passed5, ret == E_PM_SYNCHRONIZATION_ERROR, "ret == E_PM_SYNCHRONIZATION_ERROR", str);
    passed5 = check_condition(passed5, mclmac->powerMode.currentState == RECEIVE, "mclmac->powerMode.currentState == RECEIVE", str);
    passed5 = check_condition(passed5, mclmac->powerMode.nextState == FINISHP, "mclmac->powerMode.nextState == FINISHP", str);
    passed5 = check_condition(passed5, ctrlpkt->currentFrame != ARROW(ARROW(mclmac->mac)frame)current_frame, "ctrlpkt->currentFrame != mclmac->mac->frame->current_frame", str);
    if (!passed5) {
        printf("Test case 5 failed.\n%s", str);
        memset(str, 0, 1000);
    }

    /* Network time is different from node's network time.
    * Return E_PM_SYNCHRONIZATION_ERROR. Next state is FINISHP.
    */
    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 6;
    mclmac->powerMode.nextState = RECEIVE;
    ret = mclmac_execute_powermode_state(mclmac);
    bool passed6 = true;
    passed6 = check_condition(passed6, ret == E_PM_SYNCHRONIZATION_ERROR, "ret == E_PM_SYNCHRONIZATION_ERROR", str);
    passed6 = check_condition(passed6, mclmac->powerMode.currentState == RECEIVE, "mclmac->powerMode.currentState == RECEIVE", str);
    passed6 = check_condition(passed6, mclmac->powerMode.nextState == FINISHP, "mclmac->powerMode.nextState == FINISHP", str);
    passed6 = check_condition(passed6, ctrlpkt->networkTime != mclmac->_networkTime, "ctrlpkt->networkTime != mclmac->_networkTime", str);
    if (!passed6) {
        printf("Test case 6 failed.\n%s", str);
        memset(str, 0, 1000);
    }

    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 1;
    mclmac->powerMode.nextState = RECEIVE;
    ret = mclmac_execute_powermode_state(mclmac);
    bool passed = true;
    passed = check_condition(passed, ret == E_PM_EXECUTION_SUCCESS, "ret == E_PM_EXECUTION_SUCCESS", str);
    if (!passed) {
        printf("Test case 7 failed.\n%s", str);
    }

    return passed && passed1 && passed2 && passed3 && passed4 && passed5 && passed6;
}

bool test_finishp_state_powermode_stmachine(void *arg)
{
    struct powermode_data *data = (struct powermode_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);
    mclmac_init_powermode_state_machine(mclmac);

    ControlPacket_t *ctrlpkt = REFERENCE ARROW(mclmac->mac)ctrlpkt;
    uint64_t node_id[2] = {0};
    node_id[0] = rand();
    node_id[1] = rand();
    int freqs = MAX_NUMBER_FREQS;
    int slots = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    uint8_t occupied_slots[freqs][slots];
    for (int i = 0; i < freqs; i++) {
        for (int j = 0; j < slots; j++) {
            occupied_slots[i][j] = rand();
        }
    }

    controlpacket_create(ctrlpkt, node_id, rand(), rand(), rand(), rand(), rand(), rand(), rand(), (uint8_t *)occupied_slots);
    ctrlpkt = REFERENCE ARROW(mclmac->mac)ctrlpkt_recv;
    controlpacket_create(ctrlpkt, node_id, rand(), rand(), rand(), rand(), rand(), rand(), rand(), (uint8_t *)occupied_slots);

    CFPacket_t *cfpkt = &ARROW(mclmac->mac)_cf_messages[0];
    uint64_t node_id2[2] = {0};
    node_id2[0] = rand();
    node_id2[1] = rand();
    cfpacket_create(cfpkt, node_id, node_id2);
    cfpkt = &ARROW(mclmac->mac)_cf_messages[1];
    cfpacket_create(cfpkt, node_id, node_id2);
    ARROW(mclmac->mac)_destination_id[0] = rand();
    ARROW(mclmac->mac)_destination_id[1] = rand();
    ARROW(mclmac->mac)_packets_to_send_message = 0;
    ARROW(mclmac->mac)_first_send_message = 0;
    ARROW(mclmac->mac)_last_send_message = MAX_NUMBER_DATA_PACKETS - 1;
    ARROW(mclmac->mac)_packets_to_send_control = 0;
    ARROW(mclmac->mac)_first_send_control = 0;
    ARROW(mclmac->mac)_last_send_control = MAX_NUMBER_DATA_PACKETS - 1;
    ARROW(mclmac->mac)_number_packets_received = 0;
    ARROW(mclmac->mac)_first_received = 0;
    ARROW(mclmac->mac)_last_received = MAX_NUMBER_DATA_PACKETS - 1;
    ARRAY data_array;
#ifdef __LINUX__
    data_array = (uint8_t *)malloc((PACKET_SIZE_MAC - 3)* sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data_array, PACKET_SIZE_MAC - 3);
#endif
    uint i;
    for (i = 0; i < PACKET_SIZE_MAC - 3; i++)
        WRITE_ARRAY(REFERENCE data_array, rand(), i);
    DataPacket_t *pkt;
    for (i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
    {
        pkt = &ARROW(mclmac->mac)_message_packets_to_send[i];
        datapacket_create(pkt, rand() % 10, node_id2, &data_array, PACKET_SIZE_MAC - 3);
        ARROW(mclmac->mac)_packets_to_send_message++;
        pkt = &ARROW(mclmac->mac)_control_packets_to_send[i];
        datapacket_create(pkt, rand() % 10, node_id2, &data_array, PACKET_SIZE_MAC - 3);
        ARROW(mclmac->mac)_packets_to_send_control++;
        pkt = &ARROW(mclmac->mac)_packets_received[i];
        datapacket_create(pkt, rand() % 10, node_id2, &data_array, PACKET_SIZE_MAC - 3);
        ARROW(mclmac->mac)_number_packets_received++;
    }

    mclmac->powerMode.currentState = FINISHP;
    mclmac->powerMode.nextState = FINISHP;
    int ret = mclmac_execute_powermode_state(mclmac);
    bool passed = true;
    passed = passed && (ret == E_PM_EXECUTION_SUCCESS);
    passed = passed && (ARROW(mclmac->mac)_destination_id[0] == 0);
    passed = passed && (ARROW(mclmac->mac)_destination_id[1] == 0);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_message == 0);
    passed = passed && (ARROW(mclmac->mac)_first_send_message == 0);
    passed = passed && (ARROW(mclmac->mac)_last_send_message == 0);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_control == 0);
    passed = passed && (ARROW(mclmac->mac)_first_send_control == 0);
    passed = passed && (ARROW(mclmac->mac)_last_send_control == 0);
    passed = passed && (ARROW(mclmac->mac)_number_packets_received == 0);
    passed = passed && (ARROW(mclmac->mac)_first_received == 0);
    passed = passed && (ARROW(mclmac->mac)_last_received == 0);
    for (i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
    {
        pkt = &ARROW(mclmac->mac)_message_packets_to_send[i];
        passed = passed && (pkt->type == -1);
        passed = passed && (pkt->destination_id[0] == 0);
        passed = passed && (pkt->destination_id[1] == 0);
        passed = passed && (pkt->size == 0);
#ifdef __LINUX__
        passed = passed && (pkt->data == NULL);
#endif
#ifdef __RIOT__
        passed = passed && (pkt->data.size == 0);
#endif
        pkt = &ARROW(mclmac->mac)_control_packets_to_send[i];
        passed = passed && (pkt->type == -1);
        passed = passed && (pkt->destination_id[0] == 0);
        passed = passed && (pkt->destination_id[1] == 0);
        passed = passed && (pkt->size == 0);
#ifdef __LINUX__
        passed = passed && (pkt->data == NULL);
#endif
#ifdef __RIOT__
        passed = passed && (pkt->data.size == 0);
#endif
        pkt = &ARROW(mclmac->mac)_packets_received[i];
        passed = passed && (pkt->type == -1);
        passed = passed && (pkt->destination_id[0] == 0);
        passed = passed && (pkt->destination_id[1] == 0);
        passed = passed && (pkt->size == 0);
#ifdef __LINUX__
        passed = passed && (pkt->data == NULL);
#endif
#ifdef __RIOT__
        passed = passed && (pkt->data.size == 0);
#endif
    }

#ifdef __LINUX__
    free(data_array);
#endif
#ifdef __RIOT__
    free_array(&data_array);
#endif

    return passed;
}

void executetests_mac_powermode_statemachine(void)
{
    timeout_init();
    init_queues();

    cUnit_t *tests;
    struct powermode_data data;
#if defined __RIOT__ && !defined NATIVE
    nrf24l01p_ng_t radio, radio_test;
    nrf24l01p_ng_params_t params = {
        .spi = SPI_DEV(0),
        .spi_clk = NRF24L01P_NG_PARAM_SPI_CLK,
        .pin_cs = GPIO5,
        .pin_ce = GPIO17,
        .pin_irq = GPIO21,
        .config = {
            .cfg_crc = NRF24L01P_NG_PARAM_CRC_LEN,
            .cfg_tx_power = NRF24L01P_NG_PARAM_TX_POWER,
            .cfg_data_rate = NRF24L01P_NG_PARAM_DATA_RATE,
            .cfg_channel = NRF24L01P_NG_PARAM_CHANNEL,
            .cfg_max_retr = NRF24L01P_NG_PARAM_MAX_RETRANSM,
            .cfg_retr_delay = NRF24L01P_NG_PARAM_RETRANSM_DELAY,
        }
    };
    nrf24l01p_ng_params_t params_test = {
        .spi = SPI_DEV(1),
        .spi_clk = NRF24L01P_NG_PARAM_SPI_CLK,
        .pin_cs = GPIO15,
        .pin_ce = GPIO2,
        .pin_irq = GPIO4,
        .config = {
            .cfg_crc = NRF24L01P_NG_PARAM_CRC_LEN,
            .cfg_tx_power = NRF24L01P_NG_PARAM_TX_POWER,
            .cfg_data_rate = NRF24L01P_NG_PARAM_DATA_RATE,
            .cfg_channel = NRF24L01P_NG_PARAM_CHANNEL,
            .cfg_max_retr = NRF24L01P_NG_PARAM_MAX_RETRANSM,
            .cfg_retr_delay = NRF24L01P_NG_PARAM_RETRANSM_DELAY,
        }
    };
    data.radio = &radio;
    data.radio_test = &radio_test;
    nrf24l01p_ng_setup(data.radio, &params, 2);
    nrf24l01p_ng_setup(data.radio_test, &params_test, 2);
    // Setup netdev
    data.netdev = &data.radio->netdev;
    data.netdev_test = &data.radio_test->netdev;
    data.radio->netdev.driver->init(data.netdev);
    data.radio_test->netdev.driver->init(data.netdev_test);
#elif defined _RIOT__ && defined NATIVE
    data.netdev = NULL;
    data.netdev_test = NULL;
    data.radio = NULL;
    data.radio_test = NULL;
#elif defined __LINUX__
    data.radio = NULL;
#endif

    cunit_init(&tests, &setup, &teardown, (void *)&data);

    cunit_add_test(tests, &test_mclmac_init_powermode_state_machine, "mclmac_init_powermode_state_machine\0");
    cunit_add_test(tests, &test_mclmac_set_powermode_state, "mclmac_set_powermode_state\0");
    cunit_add_test(tests, &test_mclmac_set_next_powermode_state, "mclmac_set_next_powermode_state\0");
    cunit_add_test(tests, &test_mclmac_update_powermode_state_machine, "mclmac_update_powermode_state_machine\0");
    cunit_add_test(tests, &test_startp_state_powermode_stmachine, "PowerMode's STARTP state\0");
    cunit_add_test(tests, &test_passive_state_powermode_stmachine, "PowerMode's PASSIVE state\0");
    cunit_add_test(tests, &test_active_state_powermode_stmachine, "PowerMode's ACTIVE state\0");
    cunit_add_test(tests, &test_transmit_powermode_stmachine, "PowerMode's TRANSMIT state\0");
    cunit_add_test(tests, &test_receive_state_powermode_stmachine, "PowerMode's RECEIVE state\0");
    cunit_add_test(tests, &test_finishp_state_powermode_stmachine, "PowerMode's FINISHP state\0");

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
    end_queues();
    timeout_done();
    return;
}
