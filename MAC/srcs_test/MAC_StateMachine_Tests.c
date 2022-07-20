#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "MCLMAC.h"

#include "cUnit.h"

#ifdef __RIOT__
#include "net/netdev.h"
#endif

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

struct statemachine_data {
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    nrf24l01p_ng_t *radio;
    nrf24l01p_ng_t *radio_test;
    netdev_t *netdev;
    netdev_t *netdev_test;
#endif
};

void setup_statemachine(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
#ifdef __LINUX__
    MCLMAC_init(&data->mclmac, data->radio);
#endif
#ifdef __RIOT__
    netopt_state_t state = NETOPT_STATE_STANDBY;
    data->netdev->driver->set(data->netdev, NETOPT_STATE, (void *)&state, sizeof(state));
    MCLMAC_init(&data->mclmac, data->netdev);
#endif
}

void teardown_statemachine(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
#ifdef __RIOT__
    netopt_state_t state = NETOPT_STATE_SLEEP;
    data->netdev->driver->set(data->netdev, NETOPT_STATE, (void *)&state, sizeof(state));
#endif
    MCLMAC_destroy(&data->mclmac);
}

void test_mclmac_init_MAC_state_machine(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);
    assert(mclmac->macState.currentState == START);
    assert(mclmac->macState.nextState == NONE);
}

void test_mclmac_set_MAC_state(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    state_t state = START;
    mclmac_set_MAC_state(mclmac, state);
    assert(mclmac->macState.currentState == state);

    state = INITIALIZATION;
    mclmac_set_MAC_state(mclmac, state);
    assert(mclmac->macState.currentState == state);

    state = SYNCHRONIZATION;
    mclmac_set_MAC_state(mclmac, state);
    assert(mclmac->macState.currentState == state);

    state = TIMESLOT_AND_CHANNEL_SELECTION;
    mclmac_set_MAC_state(mclmac, state);
    assert(mclmac->macState.currentState == state);

    state = MEDIUM_ACCESS;
    mclmac_set_MAC_state(mclmac, state);
    assert(mclmac->macState.currentState == state);
}

void test_mclmac_set_next_MAC_state(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;


    state_t state = INITIALIZATION;
    mclmac_set_next_MAC_state(mclmac, state);
    assert(mclmac->macState.nextState == state);

    state = SYNCHRONIZATION;
    mclmac_set_next_MAC_state(mclmac, state);
    assert(mclmac->macState.nextState == state);

    state = TIMESLOT_AND_CHANNEL_SELECTION;
    mclmac_set_next_MAC_state(mclmac, state);
    assert(mclmac->macState.nextState == state);

    state = MEDIUM_ACCESS;
    mclmac_set_next_MAC_state(mclmac, state);
    assert(mclmac->macState.nextState == state);
}

void test_mclmac_update_mac_state_machine(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);
    /**
     * We begin at the START state, where initialization occurs.
     * From this state, only the INITIALIZATION state is reachable.
     * If trying to reach any other state from START, return error.
     * This state cannot be reached by any other state, if it tries to 
     * reach this state from any other, return error.
     * If trying to reach the same state, return no transition;
     */
    // If next state is NONE, do nothing, return no transition
    int ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_NO_TRANSITION);
    assert(mclmac->macState.nextState == NONE);
    assert(mclmac->macState.currentState == START);

    // Try to transit from START to any state other than INITIALIZATION, return error.
    mclmac_set_next_MAC_state(mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == START);
    assert(mclmac->macState.nextState == SYNCHRONIZATION);

    mclmac_set_next_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == START);
    assert(mclmac->macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == START);
    assert(mclmac->macState.nextState == MEDIUM_ACCESS);

    mclmac_set_next_MAC_state(mclmac, FINISH);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == START);
    assert(mclmac->macState.nextState == FINISH);

    // Set next state as INITIALIZATION, return success.
    mclmac_set_next_MAC_state(mclmac, INITIALIZATION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(mclmac->macState.currentState == INITIALIZATION);
    assert(mclmac->macState.nextState == INITIALIZATION);

    /**
     * We are now at the INITIALIZATION state.
     * From this state, only the SYNCHRONIZATION state can be reached.
     * No other state can be reached.
     * This state can be reached only from START and MEDIUM_ACCESS state.
     * If next state is the same as the current one or NONE, return no transition.
     */
    // Try to reach the same state
    mclmac_set_next_MAC_state(mclmac, INITIALIZATION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_NO_TRANSITION);
    assert(mclmac->macState.currentState == INITIALIZATION);
    assert(mclmac->macState.nextState == INITIALIZATION);

    // Try to reach start, return error.
    mclmac->macState.nextState = START;
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_INVALID_STATE);
    assert(mclmac->macState.currentState == INITIALIZATION);
    assert(mclmac->macState.nextState == START);

    // Try to reach TIMESLOT_AND_CHANNEL_SELECTION, return error
    mclmac_set_next_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == INITIALIZATION);
    assert(mclmac->macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

    // Try to reach MEDIUM_ACCESS, return error
    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == INITIALIZATION);
    assert(mclmac->macState.nextState == MEDIUM_ACCESS);

    // Try to reach FINISH, return error
    mclmac_set_next_MAC_state(mclmac, FINISH);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == INITIALIZATION);
    assert(mclmac->macState.nextState == FINISH);

    // Set next state as SYNCHRONIZATION, return success
    mclmac_set_next_MAC_state(mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(mclmac->macState.currentState == SYNCHRONIZATION);
    assert(mclmac->macState.nextState == SYNCHRONIZATION);


    /**
     * We are now in the SYNCRHONIZATION state.
     * From this state, only the TIMESLOT_AND_CHANNEL_SELECTION state can be reached.
     * No other state can be reached.
     * This state can be reached only from the INITIALIZATION one.
     * 1f next state is the same as the current one or NONE, return no transition.
     */
    // Try to reach the same state
    mclmac_set_next_MAC_state(mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_NO_TRANSITION);
    assert(mclmac->macState.currentState == SYNCHRONIZATION);
    assert(mclmac->macState.nextState == SYNCHRONIZATION);

    // Try to reach start, return error.
    mclmac->macState.nextState = START;
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_INVALID_STATE);
    assert(mclmac->macState.currentState == SYNCHRONIZATION);
    assert(mclmac->macState.nextState == START);

    // Set next state as INITIALIZATION, return error
    mclmac_set_next_MAC_state(mclmac, INITIALIZATION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == SYNCHRONIZATION);
    assert(mclmac->macState.nextState == INITIALIZATION);

    // Try to reach MEDIUM_ACCESS, return error
    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == SYNCHRONIZATION);
    assert(mclmac->macState.nextState == MEDIUM_ACCESS);

    // Try to reach FINISH, return error
    mclmac_set_next_MAC_state(mclmac, FINISH);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == SYNCHRONIZATION);
    assert(mclmac->macState.nextState == FINISH);

    // Try to reach TIMESLOT_AND_CHANNEL_SELECTION, return success
    mclmac_set_next_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(mclmac->macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(mclmac->macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

    /**
     * We are now at the TIMESLOT_AND_CHANNEL_SELECTION state.
     * This state can be reached only by the SYNCHRONIZATION channel.
     * This state can reach the SYNCRHONIZATION state, and the 
     * MEIDUM_ACCESS state.
     * No other states can be reached from this one.
     */
    // Try to reach the same state
    mclmac_set_next_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_NO_TRANSITION);
    assert(mclmac->macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(mclmac->macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

    // Try to reach the start state, return error.
    mclmac->macState.nextState = START;
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_INVALID_STATE);
    assert(mclmac->macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(mclmac->macState.nextState == START);

    // Try to reach the INITIALIZATION state, return error
    mclmac_set_next_MAC_state(mclmac, INITIALIZATION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(mclmac->macState.nextState == INITIALIZATION);

    // Try to reach the FINISH state, return error
    mclmac_set_next_MAC_state(mclmac, FINISH);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION);
    assert(mclmac->macState.nextState == FINISH);

    // Try to reach the SYNCHRONIZATION state, return success
    mclmac_set_next_MAC_state(mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(mclmac->macState.currentState == SYNCHRONIZATION);
    assert(mclmac->macState.nextState == SYNCHRONIZATION);

    // Return to TIMESLOT_AND_CHANNEL_SELECTION
    mclmac_set_next_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(mclmac);

    // Try to reach the MEDIUM_ACCESS state, return success
    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(mclmac->macState.currentState == MEDIUM_ACCESS);
    assert(mclmac->macState.nextState == MEDIUM_ACCESS);

    /**
     * We are now at the MEDIUM_ACCESS state.
     * This state can be reached only from the TIMESLOT_AND_CHANNEL_SELECTION state.
     * This state can reach the INITIALIZATION, SYNCHRONIZATION, FINISH states.
     * and itself
     * No other states can be reached from this one.
     */
    // Try to reach the same state
    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_NO_TRANSITION);
    assert(mclmac->macState.currentState == MEDIUM_ACCESS);
    assert(mclmac->macState.nextState == MEDIUM_ACCESS);

    // Try to reach the start state, return error.
    mclmac->macState.nextState = START;
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_INVALID_STATE);
    assert(mclmac->macState.currentState == MEDIUM_ACCESS);
    assert(mclmac->macState.nextState == START);

    // Try to reach the TIMESLOT_AND_CHANNEL_SELECTION state, return error
    mclmac_set_next_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_ERROR);
    assert(mclmac->macState.currentState == MEDIUM_ACCESS);
    assert(mclmac->macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);

    // Try to reach the INITIALIZATION state, return success
    mclmac_set_next_MAC_state(mclmac, INITIALIZATION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(mclmac->macState.currentState == INITIALIZATION);
    assert(mclmac->macState.nextState == INITIALIZATION);

    // Return to MEDIUM_ACCESS
    mclmac_set_next_MAC_state(mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(mclmac);
    mclmac_set_next_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(mclmac);
    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(mclmac);

    // Try to reach the SYNCRHONIZATION state, return success
    mclmac_set_next_MAC_state(mclmac, SYNCHRONIZATION);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(mclmac->macState.currentState == SYNCHRONIZATION);
    assert(mclmac->macState.nextState == SYNCHRONIZATION);

    // Return to MEDIUM_ACCESS
    mclmac_set_next_MAC_state(mclmac, TIMESLOT_AND_CHANNEL_SELECTION);
    ret = mclmac_update_mac_state_machine(mclmac);
    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_update_mac_state_machine(mclmac);

    // Try to reach the FINISH state, return success
    mclmac_set_next_MAC_state(mclmac, FINISH);
    ret = mclmac_update_mac_state_machine(mclmac);
    assert(ret == E_MAC_TRANSITION_SUCCESS);
    assert(mclmac->macState.currentState == FINISH);
    assert(mclmac->macState.nextState == FINISH);
}

void test_start_state_mac_stmachine(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);

    int ret = mclmac_update_mac_state_machine(mclmac);

    /**
     * We are now at the START state and should execute its functionality.
     * The tasks to execute by the state are the following:
     *  -Fill the array with the allowed frequencies.
     *  -Select a random wakeup_frame, for the synchronization and discovery state.
     *  -Once the variables are correctly initialize, set the next state to INITIALIZE.
     *  -Return success.
     */
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    for (int i = 0; i < 8; i++)
        assert(mclmac->_frequencies[i] >= 902000000 && mclmac->_frequencies[i] <= 928000000);
    assert(mclmac->macState.nextState == INITIALIZATION);
    assert(mclmac->_wakeup_frame > 0);
    assert(mclmac->_is_first_node == false);    
}

void test_initialization_state_mac_stmachine(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);
    // Pass to START state and execute
    int ret = mclmac_update_mac_state_machine(mclmac);
    ret = mclmac_execute_mac_state_machine(mclmac);
    // Pass to INITIALIZATION state
    ret = mclmac_update_mac_state_machine(mclmac);
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
    ret = mclmac_execute_mac_state_machine(mclmac);
    // Assert the frequency is cf.
    // Assert radio is in standby
    // Make it fail by expiring the initialization timeout 
    mclmac->_init_state = 0;
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(mclmac->macState.currentState == INITIALIZATION);
    assert(mclmac->macState.nextState == SYNCHRONIZATION);
    // Make it success
    mclmac->_init_state = 1;
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(mclmac->macState.nextState == SYNCHRONIZATION);
    assert(mclmac->macState.currentState == INITIALIZATION);
}

void test_synchronization_state_mac_stmachine(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);

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
    /**
     * We are now at the SYNCHRONIZATION state.
     * The purpose of this state is to synchronize the node with the network
     * by hearing for incoming control packets. From the packets, the following
     * information will be gathererd: minimum number of hops to the source,
     * the network time, and the frequencies and slots occupied by each node.
     * The number of slots are already known, due to the config file.
     * The node should hear for a random number of frames of at least the 
     * number of available frequencies, to ensure proper gatherig of data.
     * The random number of frames will be stored in a variable called
     * wakeup_frame. When the current_frame equals wakeup_frame - 1,
     * pass to the next state, TIMESLOT_AND_CHANNEL_SELECTION.
     */
    // No control packets were received
    mclmac->_state_ctrl = 1;
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_MAC_EXECUTION_FAILED);

    mclmac->_state_ctrl = 0;
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(mclmac->_initTime > 0);
    assert(mclmac->_hopCount > 0);
    assert(mclmac->macState.nextState == TIMESLOT_AND_CHANNEL_SELECTION);
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < MAX_NUMBER_FREQS; i++)
    {
        printf("For the frequency %d, we have:", mclmac->_frequencies[i]);
        for (int j = 0; j < m; j++)
        {
            printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(mclmac->_occupied_frequencies_slots[i][j]));
        }
        printf("\n");
    }
    for (int i = 0; i < MAX_NUMBER_SLOTS / 8; i++) {
        printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(mclmac->_selected_slots_neighbors[i]));
    }
    printf("\n");
}

void test_timeslot_frequency_state_mac_stmachine(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);

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
            mclmac->_occupied_frequencies_slots[i][j] = 0xff;
        }
    }
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(mclmac->macState.nextState == SYNCHRONIZATION);

    /* For the case when there is one available slot. */
    uint8_t pos = rand() % 8;
    uint8_t bit = 1U << (7U - pos);
    uint8_t slot = 0xff;
    slot = slot & (~bit);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            mclmac->_occupied_frequencies_slots[i][j] = slot;
        }
    }
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(mclmac->macState.nextState == MEDIUM_ACCESS);
    assert(ARROW(mclmac->mac)selectedSlot == pos);
}

void test_medium_access_state_stmachine(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);

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
            mclmac->_occupied_frequencies_slots[i][j] = slot;
        }
    }
    ret = mclmac_execute_mac_state_machine(mclmac);
    ret = mclmac_update_mac_state_machine(mclmac);

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
    mclmac->_trues = 0;
    mclmac->_trues5 = 0;
    mclmac->_state_cf = 2;
    mclmac->_state_ctrl = 3;
    mclmac_set_current_slot(mclmac, 1U);
    mclmac_set_current_cf_slot(mclmac, 1U);
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(mclmac->macState.nextState == SYNCHRONIZATION);
    assert(mclmac->macState.currentState == MEDIUM_ACCESS);
    assert(mclmac->powerMode.currentState == FINISHP);
    assert(mclmac->powerMode.nextState == FINISHP);

    // Synchronization error, slot different
    mclmac->_trues = 0;
    mclmac->_trues5 = 0;
    mclmac->_state_cf = 2;
    mclmac->_state_ctrl = 4;
    mclmac_set_current_slot(mclmac, 1U);
    mclmac_set_current_cf_slot(mclmac, 1U);
    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(mclmac->macState.nextState == INITIALIZATION);
    assert(mclmac->macState.currentState == MEDIUM_ACCESS);
    assert(mclmac->powerMode.currentState == FINISHP);
    assert(mclmac->powerMode.nextState == FINISHP);

    // Synchronization error, frame different
    mclmac->_trues = 0;
    mclmac->_trues5 = 0;
    mclmac->_state_cf = 2;
    mclmac->_state_ctrl = 5;
    mclmac_set_current_slot(mclmac, 1U);
    mclmac_set_current_cf_slot(mclmac, 1U);
    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(mclmac->macState.nextState == INITIALIZATION);
    assert(mclmac->macState.currentState == MEDIUM_ACCESS);
    assert(mclmac->powerMode.currentState == FINISHP);
    assert(mclmac->powerMode.nextState == FINISHP);
    
    // Synchronization error, network time different
    mclmac->_trues = 0;
    mclmac->_trues5 = 0;
    mclmac->_state_cf = 2;
    mclmac->_state_ctrl = 6;
    mclmac_set_current_slot(mclmac, 1U);
    mclmac_set_current_cf_slot(mclmac, 1U);
    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(mclmac->macState.nextState == INITIALIZATION);
    assert(mclmac->macState.currentState == MEDIUM_ACCESS);
    assert(mclmac->powerMode.currentState == FINISHP);
    assert(mclmac->powerMode.nextState == FINISHP);

    // Collision detected, two packets are received
    /*printf("Collision detected.\n");
    mclmac->_trues = 0;
    mclmac->_trues5 = 0;
    mclmac->_state_cf = 5;
    mclmac->_state_ctrl = 0;    
    mclmac_set_current_slot(mclmac, 1U);
    mclmac_set_current_cf_slot(mclmac, 1U);
    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac->mac)_collisionDetected == true);
    assert(ARROW(mclmac->mac)_collisionSlot == ARROW(ARROW(mclmac->mac)frame)current_slot);
    assert(ARROW(mclmac->mac)_collisionFrequency == ARROW(mclmac->mac)receiveChannel);
    assert(ARROW(mclmac->mac)_destination_id == 0);*/

    // Collision detected, receive and transmit at the same time
    mclmac->_trues = 0;
    mclmac->_trues5 = 0;
    mclmac->_state_cf = 2;
    mclmac->_state_ctrl = 0;
    mclmac_set_current_slot(mclmac, 0);
    mclmac_set_current_cf_slot(mclmac, 0);
    mclmac_set_next_MAC_state(mclmac, MEDIUM_ACCESS);
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac->mac)_destination_id[0] == 0);
    assert(ARROW(mclmac->mac)_destination_id[1] == 0);
}

void test_first_node_case_mac(void *arg)
{
    struct statemachine_data *data = (struct statemachine_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_init_mac_state_machine(mclmac);

    // Execute the START state
    int ret = mclmac_execute_mac_state_machine(mclmac);
    ret = mclmac_update_mac_state_machine(mclmac);

    // Execute the INITIALIZATION state
    mclmac->_init_state = 0;
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(mclmac->_is_first_node == true);

    // Execute the SYNCHRONIZATION state
    mclmac->_hopCount = 1;
    ret = mclmac_update_mac_state_machine(mclmac);
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(mclmac->_initTime > 0);
    printf("Init time = %d\n", mclmac->_initTime);
    assert(mclmac->_hopCount == 0);
    assert(mclmac->_is_first_node == true);

    // Execute the TIMESLOT_AND_CHANNEL_SELECTION state
    ARROW(mclmac->mac)selectedSlot = 1; // Just store a value different to zero, to make the test fail
    mclmac_update_mac_state_machine(mclmac);
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);
    assert(ARROW(mclmac->mac)selectedSlot == 0);
    assert(ARROW(mclmac->mac)transmitChannel == mclmac->_frequencies[0]);
    assert(mclmac->_occupied_frequencies_slots[0][0] == 0x80);

    /*mclmac_update_mac_state_machine(mclmac);
    ret = mclmac_execute_mac_state_machine(mclmac);
    assert(ret == E_MAC_EXECUTION_SUCCESS);*/
}

void executetests_mac_statemachine(void)
{
    init_queues();
    timeout_init();

    cUnit_t *tests;
    struct statemachine_data data;
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

    cunit_init(&tests, &setup_statemachine, &teardown_statemachine, (void *)&data);

    cunit_add_test(tests, &test_mclmac_init_MAC_state_machine, "mclmac_init_MAC_state_machine\0");
    cunit_add_test(tests, &test_mclmac_set_MAC_state, "mclmac_set_MAC_state\0");
    cunit_add_test(tests, &test_mclmac_set_next_MAC_state, "mclmac_set_next_MAC_state\0");
    cunit_add_test(tests, &test_mclmac_update_mac_state_machine, "mclmac_update_mac_state_machine\0");
    cunit_add_test(tests, &test_start_state_mac_stmachine, "START state.\0");
    cunit_add_test(tests, &test_initialization_state_mac_stmachine, "INITIALIZATION state.\0");
    cunit_add_test(tests, &test_synchronization_state_mac_stmachine, "SYNCHRONIZATION state.\0");
    cunit_add_test(tests, &test_timeslot_frequency_state_mac_stmachine, "TIMESLOT_AND_CHANNEL_SELECTION state.\0");
    cunit_add_test(tests, &test_medium_access_state_stmachine, "MEDIUM_ACCESS state.\0");
    cunit_add_test(tests, &test_first_node_case_mac, "network's first node case\0");

    cunit_execute_tests(tests);

    cunit_terminate(&tests);

    timeout_done();
    end_queues();
}
