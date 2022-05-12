#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

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


void test_mclmac_init_MAC_state_machine(void)
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

    mclmac_init_mac_state_machine(REFERENCE mclmac);
    assert(ARROW(mclmac)macState.currentState == START);
    assert(ARROW(mclmac)macState.nextState == NONE);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_MAC_state(void)
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

    state_t state = START;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);

    state = INITIALIZATION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);

    state = SYNCHRONIZATION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);

    state = TIMESLOT_AND_CHANNEL_SELECTION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);

    state = MEDIUM_ACCESS;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_next_MAC_state(void)
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

    state_t state = INITIALIZATION;
    mclmac_set_next_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.nextState == state);

    state = SYNCHRONIZATION;
    mclmac_set_next_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.nextState == state);

    state = TIMESLOT_AND_CHANNEL_SELECTION;
    mclmac_set_next_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.nextState == state);

    state = MEDIUM_ACCESS;
    mclmac_set_next_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.nextState == state);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_MAC_state(void)
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

    state_t state = START, stateA;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    stateA = mclmac_get_MAC_state(REFERENCE mclmac);
    assert(stateA == state);

    state = INITIALIZATION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    stateA = mclmac_get_MAC_state(REFERENCE mclmac);
    assert(stateA == state);

    state = SYNCHRONIZATION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    stateA = mclmac_get_MAC_state(REFERENCE mclmac);
    assert(stateA == state);

    state = TIMESLOT_AND_CHANNEL_SELECTION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    stateA = mclmac_get_MAC_state(REFERENCE mclmac);
    assert(stateA == state);
    
    state = MEDIUM_ACCESS;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    stateA = mclmac_get_MAC_state(REFERENCE mclmac);
    assert(stateA == state);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_update_mac_state_machine(void)
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

    mclmac_init_mac_state_machine(REFERENCE mclmac);
    /**
     * We begin at the START state, where initialization occurs.
     * From this state, only the INITIALIZATION state is reachable.
     * If trying to reach any other state from START, return error.
     * This state cannot be reached by any other state, if it tries to 
     * reach this state from any other, return error.
     * If trying to reach the same state, return no transition;
     */
    // If next state is NONE, do nothing, return no transition
    int ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_NO_TRANSITION);
    assert(ARROW(mclmac)macState.nextState == NONE);
    assert(ARROW(mclmac)macState.currentState == START);

    // Try to transit from START to any state other than INITIALIZATION, return error.
    mclmac_set_next_MAC_state(REFERENCE mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == START);
    assert(ARROW(mclmac)macState.nextState == SYNCHRONIZATION);

    mclmac_set_next_MAC_state(REFERENCE mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == START);
    assert(ARROW(mclmac)macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == START);
    assert(ARROW(mclmac)macState.nextState == MEDIUM_ACCESS);

    mclmac_set_next_MAC_state(REFERENCE mclmac, FINISH);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == START);
    assert(ARROW(mclmac)macState.nextState == FINISH);

    // Set next state as INITIALIZATION, return success.
    mclmac_set_next_MAC_state(REFERENCE mclmac, INITIALIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)macState.currentState == INITIALIZATION);
    assert(ARROW(mclmac)macState.nextState == INITIALIZATION);

    /**
     * We are now at the INITIALIZATION state.
     * From this state, only the SYNCHRONIZATION state can be reached.
     * No other state can be reached.
     * This state can be reached only from START and MEDIUM_ACCESS state.
     * If next state is the same as the current one or NONE, return no transition.
     */
    // Try to reach the same state
    mclmac_set_next_MAC_state(REFERENCE mclmac, INITIALIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_NO_TRANSITION);
    assert(ARROW(mclmac)macState.currentState == INITIALIZATION);
    assert(ARROW(mclmac)macState.nextState == INITIALIZATION);

    // Try to reach start, return error.
    ARROW(mclmac)macState.nextState = START;
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_INVALID_STATE);
    assert(ARROW(mclmac)macState.currentState == INITIALIZATION);
    assert(ARROW(mclmac)macState.nextState == START);

    // Try to reach TIMESLOT_AND_CHANNEL_SELECTION, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == INITIALIZATION);
    assert(ARROW(mclmac)macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

    // Try to reach MEDIUM_ACCESS, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == INITIALIZATION);
    assert(ARROW(mclmac)macState.nextState == MEDIUM_ACCESS);

    // Try to reach FINISH, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, FINISH);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == INITIALIZATION);
    assert(ARROW(mclmac)macState.nextState == FINISH);

    // Set next state as SYNCHRONIZATION, return success
    mclmac_set_next_MAC_state(REFERENCE mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)macState.currentState == SYNCHRONIZATION);
    assert(ARROW(mclmac)macState.nextState == SYNCHRONIZATION);


    /**
     * We are now in the SYNCRHONIZATION state.
     * From this state, only the TIMESLOT_AND_CHANNEL_SELECTION state can be reached.
     * No other state can be reached.
     * This state can be reached only from the INITIALIZATION one.
     * 1f next state is the same as the current one or NONE, return no transition.
     */
    // Try to reach the same state
    mclmac_set_next_MAC_state(REFERENCE mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_NO_TRANSITION);
    assert(ARROW(mclmac)macState.currentState == SYNCHRONIZATION);
    assert(ARROW(mclmac)macState.nextState == SYNCHRONIZATION);

    // Try to reach start, return error.
    ARROW(mclmac)macState.nextState = START;
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_INVALID_STATE);
    assert(ARROW(mclmac)macState.currentState == SYNCHRONIZATION);
    assert(ARROW(mclmac)macState.nextState == START);

    // Set next state as INITIALIZATION, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, INITIALIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == SYNCHRONIZATION);
    assert(ARROW(mclmac)macState.nextState == INITIALIZATION);

    // Try to reach MEDIUM_ACCESS, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == SYNCHRONIZATION);
    assert(ARROW(mclmac)macState.nextState == MEDIUM_ACCESS);

    // Try to reach FINISH, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, FINISH);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == SYNCHRONIZATION);
    assert(ARROW(mclmac)macState.nextState == FINISH);

    // Try to reach TIMESLOT_AND_CHANNEL_SELECTION, return success
    mclmac_set_next_MAC_state(REFERENCE mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(ARROW(mclmac)macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

    /**
     * We are now at the TIMESLOT_AND_CHANNEL_SELECTION state.
     * This state can be reached only by the SYNCHRONIZATION channel.
     * This state can reach the SYNCRHONIZATION state, and the 
     * MEIDUM_ACCESS state.
     * No other states can be reached from this one.
     */
    // Try to reach the same state
    mclmac_set_next_MAC_state(REFERENCE mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_NO_TRANSITION);
    assert(ARROW(mclmac)macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(ARROW(mclmac)macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

    // Try to reach the start state, return error.
    ARROW(mclmac)macState.nextState = START;
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_INVALID_STATE);
    assert(ARROW(mclmac)macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(ARROW(mclmac)macState.nextState == START);

    // Try to reach the INITIALIZATION state, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, INITIALIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(ARROW(mclmac)macState.nextState == INITIALIZATION);

    // Try to reach the FINISH state, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, FINISH);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(ARROW(mclmac)macState.nextState == FINISH);

    // Try to reach the SYNCHRONIZATION state, return success
    mclmac_set_next_MAC_state(REFERENCE mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)macState.currentState == SYNCHRONIZATION);
    assert(ARROW(mclmac)macState.nextState == SYNCHRONIZATION);

    // Return to TIMESLOT_AND_CHANNEL_SELECTION
    mclmac_set_next_MAC_state(REFERENCE mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    // Try to reach the MEDIUM_ACCESS state, return success
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)macState.currentState == MEDIUM_ACCESS);
    assert(ARROW(mclmac)macState.nextState == MEDIUM_ACCESS);

    /**
     * We are now at the MEDIUM_ACCESS state.
     * This state can be reached only from the TIMESLOT_AND_CHANNEL_SELECTION state.
     * This state can reach the INITIALIZATION, SYNCHRONIZATION, FINISH states.
     * and itself
     * No other states can be reached from this one.
     */
    // Try to reach the same state
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_NO_TRANSITION);
    assert(ARROW(mclmac)macState.currentState == MEDIUM_ACCESS);
    assert(ARROW(mclmac)macState.nextState == MEDIUM_ACCESS);

    // Try to reach the start state, return error.
    ARROW(mclmac)macState.nextState = START;
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_INVALID_STATE);
    assert(ARROW(mclmac)macState.currentState == MEDIUM_ACCESS);
    assert(ARROW(mclmac)macState.nextState == START);

    // Try to reach the TIMESLOT_AND_CHANNEL_SELECTION state, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == MEDIUM_ACCESS);
    assert(ARROW(mclmac)macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

    // Try to reach the INITIALIZATION state, return success
    mclmac_set_next_MAC_state(REFERENCE mclmac, INITIALIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)macState.currentState == INITIALIZATION);
    assert(ARROW(mclmac)macState.nextState == INITIALIZATION);

    // Return to MEDIUM_ACCESS
    mclmac_set_next_MAC_state(REFERENCE mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    mclmac_set_next_MAC_state(REFERENCE mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    // Try to reach the SYNCRHONIZATION state, return success
    mclmac_set_next_MAC_state(REFERENCE mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)macState.currentState == SYNCHRONIZATION);
    assert(ARROW(mclmac)macState.nextState == SYNCHRONIZATION);

    // Return to MEDIUM_ACCESS
    mclmac_set_next_MAC_state(REFERENCE mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    // Try to reach the FINISH state, return success
    mclmac_set_next_MAC_state(REFERENCE mclmac, FINISH);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)macState.currentState == FINISH);
    assert(ARROW(mclmac)macState.nextState == FINISH);

    MCLMAC_destroy(&mclmac);
}

void test_start_state_mac_stmachine(void)
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

    mclmac_init_mac_state_machine(REFERENCE mclmac);

    int ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /**
     * We are now at the START state and should execute its functionality.
     * The tasks to execute by the state are the following:
     *  -Fill the array with the allowed frequencies.
     *  -Select a random wakeup_frame, for the synchronization and discovery state.
     *  -Once the variables are correctly initialize, set the next state to INITIALIZE.
     *  -Return success.
     */
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    for (int i = 0; i < 8; i++)
        assert(ARROW(mclmac)_frequencies[i] >= 902000000 && ARROW(mclmac)_frequencies[i] <= 928000000);
    assert(ARROW(mclmac)macState.nextState == INITIALIZATION);
    assert(ARROW(mclmac)_wakeup_frame > 0);
    assert(ARROW(mclmac)_is_first_node == false);

    MCLMAC_destroy(&mclmac);
    
}

void test_initialization_state_mac_stmachine(void)
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

    mclmac_init_mac_state_machine(REFERENCE mclmac);
    // Pass to START state and execute
    int ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    // Pass to INITIALIZATION state
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    /**
     * We are now at the INITIALIZATION state.
     * This state hears for cf packets on the network; once such packet is found, 
     * pass directly to SINCHRONIZATION state.
     * The state does the following:
     *      -Set the radio frequency to cf frequency.
     *      -Put the radio on rx_single to hear for incomming packets.
     *      -Verify that the packet is valid (it should contain four bytes, the first two for 
     *      nodeid and the last two for destinationid.)
     *      -If the packet is not valid, set once again the radio to rx_single and continue
     *      listening.
     *      -Once a valid packet is found, pass to the SYNCHRONIZATION state.
     *      -Set a timer to a timer at INITIALIZATION_WAIT seconds, if timer expires, exit and return failure.
     *      -If timer expires, both nextState and currentState should remain unchanged.
     *      -Return success if packet is found.
     */
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    // Assert the frequency is cf.
    // Assert radio is in standby
    // Make it fail by expiring the initialization timeout 
    ARROW(mclmac)_init_state = 0;
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)macState.currentState == INITIALIZATION);
    assert(ARROW(mclmac)macState.nextState == SYNCHRONIZATION);
    // Make it success
    ARROW(mclmac)_init_state = 1;
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)macState.nextState == SYNCHRONIZATION);
    assert(ARROW(mclmac)macState.currentState == INITIALIZATION);

    MCLMAC_destroy(&mclmac);
}

void test_synchronization_state_mac_stmachine(void)
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

    mclmac_init_mac_state_machine(REFERENCE mclmac);

    /*Execute START state */
    int ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /* Execute INITIALIZATION state, the first time will fail. */
    ARROW(mclmac)_init_state = 1;
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    ARROW(mclmac)_initTime = rand() % 100;
    ARROW(mclmac)_networkTime = (rand() % 1000) + ARROW(mclmac)_initTime;
    ARROW(mclmac)_hopCount = rand() % 10;
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame = rand() % 10;
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot = rand() % MAX_NUMBER_SLOTS;
    /**
     * We are now at the SYNCHRONIZATION state.
     * The purpose of this state is to synchronize the node with the network
     * by hearing for incoming control packets. From the packets, the following
     * information will be gathererd: minimum number of hops to the source,
     * the network time, and the frequencies and slots occupied by each node.
     * The number of slots are already known, due to the config file.
     * The node should hear for a random number of frames of at least the 
     * number of available slots, to ensure proper gatherig of data.
     * The random number of frames will be stored in a variable called
     * wakeup_frame. When the current_frame equals wakeup_frame - 1,
     * pass to the next state, TIMESLOT_AND_CHANNEL_SELECTION.
     */
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)_networkTime > 0);
    assert(ARROW(mclmac)_initTime > 0);
    assert(ARROW(mclmac)_hopCount > 0);
    assert(ARROW(mclmac)macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < MAX_NUMBER_FREQS; i++)
    {
        printf("For the frequency %d, we have:\n", ARROW(mclmac)_frequencies[i]);
        for (int j = 0; j < m; j++)
        {
            printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(ARROW(mclmac)_occupied_frequencies_slots[i][j]));
        }
        printf("\n");
    }

    MCLMAC_destroy(&mclmac);
}

void test_timeslot_frequency_state_mac_stmachine(void)
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

    mclmac_init_mac_state_machine(REFERENCE mclmac);

    /*Execute START state */
    int ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /* Execute INITIALIZATION state, the first time will fail. */
    ARROW(mclmac)_init_state = 1;
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    ARROW(mclmac)_initTime = rand() % 100;
    ARROW(mclmac)_networkTime = (rand() % 1000) + ARROW(mclmac)_initTime;
    ARROW(mclmac)_hopCount = rand() % 10;
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame = rand() % 10;
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot = rand() % MAX_NUMBER_SLOTS;
    /* Execute the SYNCHRONIZATION state. The slots and frequency returned data will be random, 
    so we will modify it for testing. */
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /**
     * We are now at the TIMESLOT_AND_CHANNEL_SELECTION state.
     * From the previous state, we have the information regarding the occupied slots per
     * frequency for all the neighbors of the node. We now have to select a pair slot/frequency
     * for subsequent transmission. The are two main cases:
     *  -When there are no available slots/channels.
     *  -when there is at least one available slot/channel.
     * For the first case, the next state should be SYCHRONIZATION.
     * For the second case, the next state should be MEDIUM_ACCESS.
     * Both cases will return E_MAC_EXECUTION_SUCCESS.
     * The execution sequence will be:
     *  -Set a timeout of length FRAME_DURATION.
     *  -Travers the array of frequencies/slots until you find an empty slot on all frequencies.
     *  -If no available slot, set next state to SYNCHRONIZATION, remove the timer and return.
     *  -If a slot is available, randomly select a frequency.
     *  -Store the selected pair slot/frequency.
     *  -Enter a loop, which will end when the timeout ends.
     *  -Set the next state to MEDIUM_ACCESS.
     */
    /* For the case when there is no available slot. */
    int n = MAX_NUMBER_FREQS;
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            ARROW(mclmac)_occupied_frequencies_slots[i][j] = 0xff;
        }
    }
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)macState.nextState == SYNCHRONIZATION);

    /* For the case when there is one available slot. */
    uint8_t pos = rand() % 8;
    uint8_t bit = 1U << (7U - pos);
    uint8_t slot = 0xff;
    slot = slot & (~bit);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            ARROW(mclmac)_occupied_frequencies_slots[i][j] = slot;
        }
    }
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)macState.nextState == MEDIUM_ACCESS);
    assert(ARROW(ARROW(mclmac)mac)selectedSlot == pos);

    MCLMAC_destroy(&mclmac);
}

void test_medium_access_state_stmachine(void)
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

    mclmac_init_mac_state_machine(REFERENCE mclmac);

    /*Execute START state */
    int ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /* Execute INITIALIZATION state, the first time will fail. */
    ARROW(mclmac)_init_state = 1;
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    ARROW(mclmac)_initTime = rand() % 100;
    ARROW(mclmac)_networkTime = (rand() % 1000) + ARROW(mclmac)_initTime;
    ARROW(mclmac)_hopCount = rand() % 10;
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame = rand() % 10;
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot = rand() % MAX_NUMBER_SLOTS;
    /* Execute the SYNCHRONIZATION state. The slots and frequency returned data will be random, 
    so we will modify it for testing. */
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /* Execute the TIMESLOT_AND_CHANNEL_SELECTION state, assuring a free slot */
    uint8_t pos = rand() % 8;
    uint8_t bit = 1U << (7U - pos);
    uint8_t slot = 0xff;
    slot = slot & (~bit);
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
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    /**
     * We are now at the MEDIUM_ACCESS state, which will control the transmission and reception
     * of the packets.If nothing happens, the machine should stay on this state. In case of
     * errors, the following should be done:
     *  -In case of collision, return to the SYNCHRONIZATION state, to get a new slot/channel pair.
     *  -In case of synchronization error (time drifts), it should return to INITIALIZATION state,
     *   to resynchronize the clock.
     * The state will execute the POWERMODE state machine for data transmission and reception,
     * one state at the time. It should handle the transsitions and the errors.
     * The state will do the following:
     *  -Initialize the PowerMode state machine.
     *  -Execute the STARTP state at the beginning, just once.
     *  -Enter a while loop, which will be executed unless an error occurs.
     *  -Whithin the loop, do the following:
     *      1. Update the state of the machine.
     *      3. Execute the state.
     *      2. Check for errors in the execution. Possible errors:
     *          **At the ACTIVE state: Collision at slot. It should return collision error,
     *            set the next state to SYNCHRONIZATION and end the cycle.
     *          **At the RECEIVE state: Synchronization error. It should return synchronization
     *            error; set the next state to INITIALIZATION and end the cycle.
     */
    // Collision error.
    ARROW(mclmac)_trues = 0;
    ARROW(mclmac)_trues5 = 0;
    ARROW(mclmac)_state_cf = 2;
    ARROW(mclmac)_state_ctrl = 3;
    mclmac_set_current_slot(REFERENCE mclmac, 1U);
    mclmac_set_current_cf_slot(REFERENCE mclmac, 1U);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)macState.nextState == SYNCHRONIZATION);
    assert(ARROW(mclmac)macState.currentState == MEDIUM_ACCESS);
    assert(ARROW(mclmac)powerMode.currentState == FINISHP);
    assert(ARROW(mclmac)powerMode.nextState == FINISHP);

    // Synchronization error, slot different
    ARROW(mclmac)_trues = 0;
    ARROW(mclmac)_trues5 = 0;
    ARROW(mclmac)_state_cf = 2;
    ARROW(mclmac)_state_ctrl = 4;
    mclmac_set_current_slot(REFERENCE mclmac, 1U);
    mclmac_set_current_cf_slot(REFERENCE mclmac, 1U);
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)macState.nextState == INITIALIZATION);
    assert(ARROW(mclmac)macState.currentState == MEDIUM_ACCESS);
    assert(ARROW(mclmac)powerMode.currentState == FINISHP);
    assert(ARROW(mclmac)powerMode.nextState == FINISHP);

    // Synchronization error, frame different
    ARROW(mclmac)_trues = 0;
    ARROW(mclmac)_trues5 = 0;
    ARROW(mclmac)_state_cf = 2;
    ARROW(mclmac)_state_ctrl = 5;
    mclmac_set_current_slot(REFERENCE mclmac, 1U);
    mclmac_set_current_cf_slot(REFERENCE mclmac, 1U);
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)macState.nextState == INITIALIZATION);
    assert(ARROW(mclmac)macState.currentState == MEDIUM_ACCESS);
    assert(ARROW(mclmac)powerMode.currentState == FINISHP);
    assert(ARROW(mclmac)powerMode.nextState == FINISHP);
    
    // Synchronization error, network time different
    ARROW(mclmac)_trues = 0;
    ARROW(mclmac)_trues5 = 0;
    ARROW(mclmac)_state_cf = 2;
    ARROW(mclmac)_state_ctrl = 6;
    mclmac_set_current_slot(REFERENCE mclmac, 1U);
    mclmac_set_current_cf_slot(REFERENCE mclmac, 1U);
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)macState.nextState == INITIALIZATION);
    assert(ARROW(mclmac)macState.currentState == MEDIUM_ACCESS);
    assert(ARROW(mclmac)powerMode.currentState == FINISHP);
    assert(ARROW(mclmac)powerMode.nextState == FINISHP);

    // Collision detected, two packets are received
    /*printf("Collision detected.\n");
    ARROW(mclmac)_trues = 0;
    ARROW(mclmac)_trues5 = 0;
    ARROW(mclmac)_state_cf = 5;
    ARROW(mclmac)_state_ctrl = 0;    
    mclmac_set_current_slot(REFERENCE mclmac, 1U);
    mclmac_set_current_cf_slot(REFERENCE mclmac, 1U);
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(ARROW(mclmac)mac)_collisionDetected == true);
    assert(ARROW(ARROW(mclmac)mac)_collisionSlot == ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot);
    assert(ARROW(ARROW(mclmac)mac)_collisionFrequency == ARROW(ARROW(mclmac)mac)receiveChannel);
    assert(ARROW(ARROW(mclmac)mac)_destination_id == 0);*/

    // Collision detected, receive and transmit at the same time
    ARROW(mclmac)_trues = 0;
    ARROW(mclmac)_trues5 = 0;
    ARROW(mclmac)_state_cf = 2;
    ARROW(mclmac)_state_ctrl = 0;    
    mclmac_set_current_slot(REFERENCE mclmac, 0);
    mclmac_set_current_cf_slot(REFERENCE mclmac, 0);
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(ARROW(mclmac)mac)_destination_id == 0);

    MCLMAC_destroy(&mclmac);
}

void test_first_node_case_mac(void)
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

    mclmac_init_mac_state_machine(REFERENCE mclmac);

    // Execute the START state
    int ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);

    // Execute the INITIALIZATION state
    ARROW(mclmac)_init_state = 0;
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)_is_first_node == true);

    // Execute the SYNCHRONIZATION state
    ARROW(mclmac)_hopCount = 1;
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)_initTime > 0);
    printf("Init time = %d\n", ARROW(mclmac)_initTime);
    assert(ARROW(mclmac)_hopCount == 0);
    assert(ARROW(mclmac)_is_first_node == true);

    // Execute the TIMESLOT_AND_CHANNEL_SELECTION state
    ARROW(ARROW(mclmac)mac)selectedSlot = 1; // Just store a value different to zero, to make the test fail
    mclmac_update_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(ARROW(ARROW(mclmac)mac)selectedSlot == 0);
    assert(ARROW(ARROW(mclmac)mac)transmitChannel == ARROW(mclmac)_frequencies[0]);

    /*mclmac_update_mac_state_machine(REFERENCE mclmac);
    ret = mclmac_execute_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);*/

    MCLMAC_destroy(&mclmac);
}

void executetests_mac_statemachine(void)
{
    init_queues();
    timeout_init();

    printf("Testing mclmac_init_MAC_state_machine function.\n");
    test_mclmac_init_MAC_state_machine();
    printf("Test passed.\n");

    printf("Testing mclmac_set_MAC_state function.\n");
    test_mclmac_set_MAC_state();
    printf("Test passed.\n");

    printf("Testing mclmac_set_next_MAC_state function.\n");
    test_mclmac_set_next_MAC_state();
    printf("Test passed.\n");

    printf("Testing mclmac_get_MAC_state function.\n");
    test_mclmac_get_MAC_state();
    printf("Test passed.\n");

    printf("Testing mclmac_udpate_mac_state_machine function.\n");
    test_mclmac_update_mac_state_machine();
    printf("Test passed.\n");

    printf("Testing mclmac_execute_state_machine function.\n");
    printf("Testing the START state.\n");
    test_start_state_mac_stmachine();
    printf("Test passed.\n");

    printf("Testing the INITIALIZATION state.\n");
    test_initialization_state_mac_stmachine();
    printf("Test passed.\n");

    printf("Testing the SYNCHRONIZATION state.\n");
    //test_synchronization_state_mac_stmachine();
    printf("Test passed.\n");

    printf("Testing the TIMESLOT_AND_CHANNEL_SELECTION state.\n");
    test_timeslot_frequency_state_mac_stmachine();
    printf("Test passed.\n");

    printf("Testing the MEDIUM_ACCESS state.\n");
    test_medium_access_state_stmachine();
    printf("Test passed.\n");

    printf("Testing the network's first node case.\n");
    test_first_node_case_mac();
    printf("Test passed.\n");

    timeout_done();
    end_queues();

    return;
}
