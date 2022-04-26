#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "MAC_PowerMode_StateMachine_Tests.h"
#include "MCLMAC.h"

void test_mclmac_init_powermode_state_machine(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    mclmac_init_powermode_state_machine(REFERENCE mclmac);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    assert(ARROW(mclmac)powerMode.nextState == NONEP);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_powermode_state(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    PowerMode_t mode = PASSIVE;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.currentState == mode);

    mode = ACTIVE;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.currentState == mode);

    mode = TRANSMIT;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.currentState == mode);

    mode = RECEIVE;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.currentState == mode);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_next_powermode_state(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    PowerMode_t mode = ACTIVE;
    mclmac_set_next_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.nextState == mode);

    mode = TRANSMIT;
    mclmac_set_next_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.nextState == mode);

    mode = RECEIVE;
    mclmac_set_next_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.nextState == mode);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_powermode_state(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    PowerMode_t mode = PASSIVE, modeR;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    modeR = mclmac_get_powermode_state(REFERENCE mclmac);
    assert(modeR == mode);

    mode = ACTIVE;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    modeR = mclmac_get_powermode_state(REFERENCE mclmac);
    assert(modeR == mode);

    mode = TRANSMIT;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    modeR = mclmac_get_powermode_state(REFERENCE mclmac);
    assert(modeR == mode);

    mode = RECEIVE;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    modeR = mclmac_get_powermode_state(REFERENCE mclmac);
    assert(modeR == mode);
    
    MCLMAC_destroy(&mclmac);
}

void test_mclmac_update_powermode_state_machine(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    mclmac_init_powermode_state_machine(REFERENCE mclmac);

    // Try to transition from STARTP state to ACTIVE, TRANSMIT,  RECEIVE or FINISHP states.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_powermode_state(REFERENCE mclmac, STARTP);
    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    int r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    
    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    
    mclmac_set_next_powermode_state(REFERENCE mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    
    mclmac_set_next_powermode_state(REFERENCE mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);

    // Try to transition from PASSIVE state to TRANSMIT,  RECEIVE or FINISHP states.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_powermode_state(REFERENCE mclmac, PASSIVE);
    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    // Try to transit from FINISHP to any other state
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_powermode_state(REFERENCE mclmac, FINISHP);

    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    mclmac_set_next_powermode_state(REFERENCE mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // Once in finish state, to use once again the state machine,
    // call mclmac_init_powermode_state_machine, and go to PASSIVE state
    mclmac_init_powermode_state_machine(REFERENCE mclmac);
    mclmac_set_powermode_state(REFERENCE mclmac, STARTP);
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);

    // Transit from PASSIVE to ACTIVE
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    // Transit from ACTIVE to PASSIVE
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    // Transit from PASSIVE to TRANSMIT. Remember to pass through ACTIVE,
    // as specified by the state machine.
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == TRANSMIT);

    // Try to get to RECEIVE mode.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(REFERENCE mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == TRANSMIT);

    // Try to get to ACTIVE mode.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // You are in TRANSMIT mode, in order to get to RECEIVE mode, 
    // you have to get to PASSIVE again, an travers the path to 
    // RECEIVE state, as indicated by the state machine.
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == RECEIVE);

    // Try to get to TRANSMIT mode.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // Try to get to ACTIVE mode
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // Follow the path: PASSIVE -> ACTIVE -> FINISHP
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == FINISHP);

    // Travers the path: PASSIVE -> ACTIVE -> TRANSMIT -> FINISHP
    mclmac_init_powermode_state_machine(REFERENCE mclmac);
    mclmac_set_powermode_state(REFERENCE mclmac, STARTP);
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == TRANSMIT);

    mclmac_set_next_powermode_state(REFERENCE mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == FINISHP);

    // Travers the path: PASSIVE -> ACTIVE -> RECEIVE -> FINISHP
    mclmac_init_powermode_state_machine(REFERENCE mclmac);
    mclmac_set_powermode_state(REFERENCE mclmac, STARTP);
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == TRANSMIT);

    mclmac_set_next_powermode_state(REFERENCE mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == FINISHP);

    // If nextState is NONEP, return E_PM_NO_TRANSITION
    mclmac_set_next_powermode_state(REFERENCE mclmac, NONEP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_NO_TRANSITION);

    // If invalid state, return E_PM_INVALID_STATE
    mclmac_set_next_powermode_state(REFERENCE mclmac, 10);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_INVALID_STATE);

    MCLMAC_destroy(&mclmac);
}

void test_startp_state_powermode_stmachine(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    mclmac_init_powermode_state_machine(REFERENCE mclmac);

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
    int ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)slots_number == ARROW(mclmac)_nSlots);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)cf_slots_number == ARROW(mclmac)_nChannels);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)frame_duration == TIME(FRAME_DURATION));
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)slot_duration == TIME(SLOT_DURATION));
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)cf_duration == TIME(CF_SLOT_DURATION));
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame == ARROW(mclmac)_wakeup_frame + 1);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot == 0);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == 0);
    assert(ARROW(ARROW(mclmac)mac)_packets_to_send_message == 0);
    assert(ARROW(ARROW(mclmac)mac)_packets_to_send_control == 0);
    assert(ARROW(ARROW(mclmac)mac)_number_packets_received == 0);
    assert(ARROW(ARROW(mclmac)mac)cfChannel == CF_FREQUENCY);
    assert(ARROW(mclmac)powerMode.nextState == PASSIVE);

    timeout_unset(ARROW(ARROW(ARROW(mclmac)mac)frame)slot_timer);

    MCLMAC_destroy(&mclmac);
}

void test_passive_state_powermode_stmachine(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    mclmac_init_powermode_state_machine(REFERENCE mclmac);

    // Execute the STARTP state, and update state to PASSIVE state.
    int ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    ret = mclmac_update_powermode_state_machine(REFERENCE mclmac);

#ifdef __LINUX__
    mclmac->_self_pid = pthread_self();
#endif
#ifdef __RIOT__
    mclmac._self_pid = thread_getpid();
#endif
    open_queue(ARROW(mclmac)_mac_queue_id, ARROW(mclmac)_self_pid);

    uint8_t msg[MAX_MESSAGE_SIZE];
    uint i;
    for (i = 0; i < MAX_MESSAGE_SIZE; i++)
        msg[i] = rand();
    msg[0] = 2;
    send_message(ARROW(mclmac)_mac_queue_id, msg, MAX_MESSAGE_SIZE, ARROW(mclmac)_self_pid);
    ARROW(ARROW(mclmac)mac)_number_packets_received = 0;
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
    uint32_t time = ARROW(mclmac)_networkTime;
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)powerMode.nextState == ACTIVE);
    assert(ARROW(mclmac)_networkTime == time + 1);
    assert(ARROW(ARROW(mclmac)mac)_packets_to_send_control > 0);
    assert(elements_on_queue(ARROW(mclmac)_mac_queue_id) == 0);

    // For message packets
    msg[0] = 7;
    send_message(ARROW(mclmac)_mac_queue_id, msg, MAX_MESSAGE_SIZE, ARROW(mclmac)_self_pid);
    ARROW(mclmac)powerMode.nextState = PASSIVE;
    time = ARROW(mclmac)_networkTime;
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)powerMode.nextState == ACTIVE);
    assert(ARROW(mclmac)_networkTime == time + 1);
    assert(ARROW(ARROW(mclmac)mac)_packets_to_send_message > 0);
    assert(elements_on_queue(ARROW(mclmac)_mac_queue_id) == 0);
    timeout_unset(ARROW(ARROW(ARROW(mclmac)mac)frame)slot_timer);
    ARROW(ARROW(mclmac)mac)_packets_to_send_message = 0;
    ARROW(ARROW(mclmac)mac)_packets_to_send_control = 0;
    ARROW(ARROW(mclmac)mac)_last_send_message = 0;
    ARROW(ARROW(mclmac)mac)_last_send_control = 0;
    DataPacket_t *pkt = &ARROW(ARROW(mclmac)mac)_message_packets_to_send[0];
    datapacket_clear(pkt);
    pkt = &ARROW(ARROW(mclmac)mac)_control_packets_to_send[0];
    datapacket_clear(pkt);

    msg[0] = 2;
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++)
        send_message(ARROW(mclmac)_mac_queue_id, msg, MAX_MESSAGE_SIZE, ARROW(mclmac)_self_pid);
    ARROW(ARROW(ARROW(mclmac)mac)frame)slot_timer = timeout_set(TIME(SLOT_DURATION));
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(elements_on_queue(ARROW(mclmac)_mac_queue_id) == 0);
    assert(ARROW(ARROW(mclmac)mac)_packets_to_send_control == MAX_ELEMENTS_ON_QUEUE);
    timeout_unset(ARROW(ARROW(ARROW(mclmac)mac)frame)slot_timer);

    msg[0] = 7;
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++)
        send_message(ARROW(mclmac)_mac_queue_id, msg, MAX_MESSAGE_SIZE, ARROW(mclmac)_self_pid);
    ARROW(ARROW(ARROW(mclmac)mac)frame)slot_timer = timeout_set(TIME(SLOT_DURATION));
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(elements_on_queue(ARROW(mclmac)_mac_queue_id) == 0);
    assert(ARROW(ARROW(mclmac)mac)_packets_to_send_message == MAX_ELEMENTS_ON_QUEUE);
    timeout_unset(ARROW(ARROW(ARROW(mclmac)mac)frame)slot_timer);
    close_queue(ARROW(mclmac)_mac_queue_id);
    MCLMAC_destroy(&mclmac);
}

void test_active_state_powermode_stmachine(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 10;

    MCLMAC_init(&mclmac, &radio, nodeid);

    mclmac_init_mac_state_machine(REFERENCE mclmac);
    mclmac_init_powermode_state_machine(REFERENCE mclmac);

    /* Execute the MAC state machine, as we require a time slot selected. */
    /*Execute START state */
    int ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /* Execute INITIALIZATION state, the first time will fail. */
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /* Execute the SYNCHRONIZATION state. The slots and frequency returned data will be random, 
    so we will modify it for testing. */
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /* Execute the TIMESLOT_AND_CHANNEL_SELECTION state, assuring a free slot */
    uint8_t slot = 0x7f;
    int n = MAX_NUMBER_FREQS;
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            ARROW(mclmac)_occupied_frequencies_slots[i][j] = slot;
        }
    }
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);

    /* Execute the PowerMode State machine. */
    // Execute the STARTP state, and update state to PASSIVE state.
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    ret = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    // Add packets to the queue
    uint8_t msg[MAX_MESSAGE_SIZE];
    uint i;
    for (i = 0; i < MAX_MESSAGE_SIZE; i++)
        msg[i] = rand();
    msg[0] = 2;
    send_message(ARROW(mclmac)_mac_queue_id, msg, MAX_MESSAGE_SIZE, ARROW(mclmac)_self_pid);
    msg[0] = 7;
    send_message(ARROW(mclmac)_mac_queue_id, msg, MAX_MESSAGE_SIZE, ARROW(mclmac)_self_pid);
    // Execute the PASSIVE state and update state machine.
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    ret = mclmac_update_powermode_state_machine(REFERENCE mclmac);

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
    // set state to 1, to indicate first test case.
    ARROW(mclmac)_state = 1;
    mclmac_set_current_slot(REFERENCE mclmac, 0);
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)powerMode.nextState == TRANSMIT);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == MAX_NUMBER_FREQS);

    /* Test case 2:
        -There are no packets to transmit, but a packet is received.
        It should return:
            -E_PM_EXECUTION_SUCCESS.
            -The next state should be RECEIVE.
            -The current cf slot should be equal to the total number of frequencies.
    */
    ARROW(mclmac)_state = 2;
    mclmac_set_current_slot(REFERENCE mclmac, 1U);
    mclmac_set_current_cf_slot(REFERENCE mclmac, 1U);
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)powerMode.nextState == RECEIVE);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == MAX_NUMBER_FREQS);

    /* Test case 3:
            -There are no packets to receive or send.
            It should return:
                -E_PM_EXECUTION_SUCCESS.
                -The next state should be PASSIVE.
                -The current cf slot should be equal to the total number of frequencies.
    */
    ARROW(mclmac)_state = 3;
    ARROW(ARROW(mclmac)mac)_packets_to_send_message = 0;
    ARROW(ARROW(mclmac)mac)_packets_to_send_control = 0;
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)powerMode.nextState == PASSIVE);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == MAX_NUMBER_FREQS);

    /*  Test case 4:
            -A packet was received and it should send a packet.
            It should return:
                -E_PM_SYNCHRONIZATION_ERROR.
                -The next state should be FINISHP.
                -The current cf slot should be equal to the total number of frequencies.
    */
    ARROW(mclmac)_state = 4;
    ARROW(ARROW(mclmac)mac)_packets_to_send_message = 1;
    mclmac_set_current_slot(REFERENCE mclmac, 0);
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(ret == E_PM_SYNCHRONIZATION_ERROR);
    assert(ARROW(mclmac)powerMode.nextState == FINISHP);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == MAX_NUMBER_FREQS);

    /*  Test case 5:
            -Two packets should be received, and no packets to sent.
            It should return:
                -E_PM_SYNCHRONIZATION_ERROR.
                -The next state should be FINISHP.
                -The current cf slot should be equal to the total number of frequencies.
    */
    ARROW(mclmac)_state = 5;
    mclmac_set_current_slot(REFERENCE mclmac, 1U);
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(ret == E_PM_SYNCHRONIZATION_ERROR);
    assert(ARROW(mclmac)powerMode.nextState == FINISHP);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == MAX_NUMBER_FREQS);

    MCLMAC_destroy(&mclmac);

}

void test_transmit_powermode_stmachine(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 10;

    MCLMAC_init(&mclmac, &radio, nodeid);

    mclmac_init_mac_state_machine(REFERENCE mclmac);
    mclmac_init_powermode_state_machine(REFERENCE mclmac);

    /* Execute the MAC state machine, as we require a time slot selected. */
    /*Execute START state */
    int ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /* Execute INITIALIZATION state, the first time will fail. */
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /* Execute the SYNCHRONIZATION state. The slots and frequency returned data will be random, 
    so we will modify it for testing. */
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /* Execute the TIMESLOT_AND_CHANNEL_SELECTION state, assuring a free slot */
    uint8_t slot = 0x7f;
    int n = MAX_NUMBER_FREQS;
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            ARROW(mclmac)_occupied_frequencies_slots[i][j] = slot;
        }
    }
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    /* Execute the PowerMode State machine. */
    // Execute the STARTP state, and update state to PASSIVE state.
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    ret = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    // Add packets to the queue
    uint8_t msg[MAX_MESSAGE_SIZE];
    uint i;
    for (i = 0; i < MAX_MESSAGE_SIZE; i++)
        msg[i] = rand();
    /*msg[0] = 2;
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE / 2; i++)
        send_message(ARROW(mclmac)_mac_queue_id, msg, MAX_MESSAGE_SIZE, ARROW(mclmac)_self_pid);*/
    msg[0] = 7;
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++)
        send_message(ARROW(mclmac)_mac_queue_id, msg, MAX_MESSAGE_SIZE, ARROW(mclmac)_self_pid);
    // Execute the PASSIVE state and update state machine.
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    ret = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(ARROW(ARROW(mclmac)mac)_packets_to_send_message == MAX_ELEMENTS_ON_QUEUE);
    //assert(ARROW(ARROW(mclmac)mac)_packets_to_send_control == MAX_ELEMENTS_ON_QUEUE / 2);
    /* Transit to state ACTIVE, and simulate that a packet should be sent. */
    ARROW(mclmac)_state = 1;
    mclmac_set_current_slot(REFERENCE mclmac, 0);
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    ret = mclmac_update_powermode_state_machine(REFERENCE mclmac);

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
    uint8_t current_slot = ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot;
    ret = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    // All packets were send
    assert(ARROW(ARROW(mclmac)mac)_packets_to_send_message == 0);
    assert(ARROW(ARROW(mclmac)mac)_packets_to_send_control == 0);
    assert(ARROW(ARROW(mclmac)mac)_first_send_control == ARROW(ARROW(mclmac)mac)_last_send_control);
    assert(ARROW(ARROW(mclmac)mac)_first_send_message == ARROW(ARROW(mclmac)mac)_last_send_message);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot == current_slot + 1U);
    assert(ARROW(mclmac)powerMode.currentState == TRANSMIT);
    assert(ARROW(mclmac)powerMode.nextState == PASSIVE);

    MCLMAC_destroy(&mclmac);
}

void executetests_mac_powermode_statemachine(void)
{
    timeout_init();
    init_queues();

    printf("Testing _mclmac_init_powermode_state_machine function.\n");
    test_mclmac_init_powermode_state_machine();
    printf("Test passed.\n");

    printf("Testing mclmac_set_powermode_state function.\n");
    test_mclmac_set_powermode_state();
    printf("Test passed.\n");

    printf("Testing mclmac_set_next_powermode_state function.\n");
    test_mclmac_set_next_powermode_state();
    printf("Test passed.\n");

    printf("Testing mclmac_get_powermode_state function.\n");
    test_mclmac_get_powermode_state();
    printf("Test passed.\n");

    printf("Test mclmac_update_powermode_state_machine function.\n");
    test_mclmac_update_powermode_state_machine();
    printf("Test passed.\n");

    printf("Testing the PowerMode's STARTP state.\n");
    test_startp_state_powermode_stmachine();
    printf("Test passed.\n");

    printf("Testing the PowerMode's PASSIVE state.\n");
    test_passive_state_powermode_stmachine();
    printf("Test passed.\n");

    printf("Testing the PowerMode's ACTIVE state.\n");
    test_active_state_powermode_stmachine();
    printf("Test passed.\n");

    printf("Testing the PowerMode's TRANSMIT state.\n");
    test_transmit_powermode_stmachine();
    printf("Test passed.\n");

    end_queues();
    timeout_done();
    return;
}
