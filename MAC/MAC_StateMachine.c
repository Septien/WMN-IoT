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
        if (mclmac->macState.currentState == DISCOVERY)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION)
            return E_MAC_TRANSITION_ERROR;
        mclmac_set_MAC_state(mclmac, INITIALIZATION);
        return E_MAC_TRANSITION_SUCCESS;
        break;

    case SYNCHRONIZATION:
        if (mclmac->macState.currentState == START)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == DISCOVERY)
            return E_MAC_TRANSITION_ERROR;
        mclmac_set_MAC_state(mclmac, SYNCHRONIZATION);
        return E_MAC_TRANSITION_SUCCESS;
        break;

    case DISCOVERY:
        if (mclmac->macState.currentState == START)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == INITIALIZATION)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == TIMESLOT_AND_CHANNEL_SELECTION)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == MEDIUM_ACCESS)
            return E_MAC_TRANSITION_ERROR;
        mclmac_set_MAC_state(mclmac, DISCOVERY);
        return E_MAC_TRANSITION_SUCCESS;
        break;

    case TIMESLOT_AND_CHANNEL_SELECTION:
        if (mclmac->macState.currentState == START)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == INITIALIZATION)
            return E_MAC_TRANSITION_ERROR;
        if (mclmac->macState.currentState == SYNCHRONIZATION)
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
        if (mclmac->macState.currentState == DISCOVERY)
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
        if (mclmac->macState.currentState == DISCOVERY)
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
    
    switch (mclmac->macState.currentState)
    {
    case START:
        /* Store the allowed frequencies. */
        for (int i = 0; i < 8; i++)
            mclmac->_frequencies[i] = 902000000 + (rand() % 1000);
        // Initialize the timeouts API
        timeout_init();
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
    default:
        break;
    }

    return E_MAC_EXECUTION_SUCCESS;
}