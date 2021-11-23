#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "MCLMAC.h"

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
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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
    size_t  dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    state_t state = START;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);
    state = INITIALIZATION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);
    state = SYNCHRONIZATION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);
    state = DISCOVERY;
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
    size_t  dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    state_t state = INITIALIZATION;
    mclmac_set_next_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.nextState == state);
    state = SYNCHRONIZATION;
    mclmac_set_next_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.nextState == state);
    state = DISCOVERY;
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
    size_t  dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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

    state = DISCOVERY;
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
    size_t  dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    mclmac_init_mac_state_machine(REFERENCE mclmac);
    /**
     * We begin at the START state, where initialization occurs.
     * From this state, only the INITIALIZATION state is reachable.
     * If trying to reach any other state from START, return error.
     * This state cannot be reached by any other state, if it tries to 
     * reach this state from any other, return error.
     * If trying to reach the same state, return no transition;
     */
    // If next state is NONE, do nathing, return no transition
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

    mclmac_set_next_MAC_state(REFERENCE mclmac, DISCOVERY);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == START);
    assert(ARROW(mclmac)macState.nextState == DISCOVERY);

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

    // Try to reach DISCOVERY, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, DISCOVERY);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == INITIALIZATION);
    assert(ARROW(mclmac)macState.nextState == DISCOVERY);

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
     * From this state, only the DISCOVERY state can be reached.
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

    // Try to reach TIMESLOT_AND_CHANNEL_SELECTION, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == SYNCHRONIZATION);
    assert(ARROW(mclmac)macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

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

    // Try to reach DISCOVERY, return success
    mclmac_set_next_MAC_state(REFERENCE mclmac, DISCOVERY);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)macState.currentState == DISCOVERY);
    assert(ARROW(mclmac)macState.nextState == DISCOVERY);

    /** We are now in the DISCOVERY state.
     * We can reach only the TIMESLOT_AND_CHANNEL_SELECTION state from this one.
     * No other state can be reached from this.
     * This state can be reached only from the SYNCHRONIZATION one.
    */
     // Try to reach the same state
    mclmac_set_next_MAC_state(REFERENCE mclmac, DISCOVERY);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_NO_TRANSITION);
    assert(ARROW(mclmac)macState.currentState == DISCOVERY);
    assert(ARROW(mclmac)macState.nextState == DISCOVERY);

    // Try to reach the start state, return error.
    ARROW(mclmac)macState.nextState = START;
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_INVALID_STATE);
    assert(ARROW(mclmac)macState.currentState == DISCOVERY);
    assert(ARROW(mclmac)macState.nextState == START);

    // Try to reach initialization, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, INITIALIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == DISCOVERY);
    assert(ARROW(mclmac)macState.nextState == INITIALIZATION);

    // Try to reach the SYNCHRONIZATION state
    mclmac_set_next_MAC_state(REFERENCE mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == DISCOVERY);
    assert(ARROW(mclmac)macState.nextState == SYNCHRONIZATION);

    // Try to reach the MEDIUM_ACCESS state, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == DISCOVERY);
    assert(ARROW(mclmac)macState.nextState == MEDIUM_ACCESS);

    // Try to reach the FINISH state, return error.
    mclmac_set_next_MAC_state(REFERENCE mclmac, FINISH);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == DISCOVERY);
    assert(ARROW(mclmac)macState.nextState == FINISH);

    // Try to reach the TIMESLOT_AND_CHANNEL_SELECTION state, return success
    mclmac_set_next_MAC_state(REFERENCE mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(ARROW(mclmac)macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

    /**
     * We are now at the TIMESLOT_AND_CHANNEL_SELECTION state.
     * This state can be reached only by the DISCOVERY channel.
     * This state can reach th SYNCRHONIZATION state, and the 
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

    // Try to reach the DISCOVERY state, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, DISCOVERY);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(ARROW(mclmac)macState.nextState == DISCOVERY);

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
    mclmac_set_next_MAC_state(REFERENCE mclmac, DISCOVERY);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
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

    // Try to reach the DISCOVERY state, return error
    mclmac_set_next_MAC_state(REFERENCE mclmac, DISCOVERY);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(ARROW(mclmac)macState.currentState == MEDIUM_ACCESS);
    assert(ARROW(mclmac)macState.nextState == DISCOVERY);

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
    mclmac_set_next_MAC_state(REFERENCE mclmac, DISCOVERY);
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
    mclmac_set_next_MAC_state(REFERENCE mclmac, DISCOVERY);
    ret = mclmac_update_mac_state_machine(REFERENCE mclmac);
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
}

void executetests_mac_statemachine(void)
{
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

    printf("Testing mclmac_udpate_mac_state_machine.\n");
    test_mclmac_update_mac_state_machine();
    printf("Test passed.\n");

    return;
}
