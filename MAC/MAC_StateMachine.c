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