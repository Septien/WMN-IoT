#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "MCLMAC.h"

void test_mclmac_set_MAC_state(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    state_t state = START;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);
    state = INITIALIZATION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);
    state = SYNCHRONIZATION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);
    state = DISCOVERY_AND_SELECTION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.currentState == state);
    state = MEDIUMACCESS;
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
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    state_t state = INITIALIZATION;
    mclmac_set_next_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.nextState == state);
    state = SYNCHRONIZATION;
    mclmac_set_next_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.nextState == state);
    state = DISCOVERY_AND_SELECTION;
    mclmac_set_next_MAC_state(REFERENCE mclmac, state);
    assert(ARROW(mclmac)macState.nextState == state);
    state = MEDIUMACCESS;
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
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

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

    state = DISCOVERY_AND_SELECTION;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    stateA = mclmac_get_MAC_state(REFERENCE mclmac);
    assert(stateA == state);

    state = MEDIUMACCESS;
    mclmac_set_MAC_state(REFERENCE mclmac, state);
    stateA = mclmac_get_MAC_state(REFERENCE mclmac);
    assert(stateA == state);

    MCLMAC_destroy(&mclmac);
}

void executetests_mac_statemachine(void)
{
    printf("Testing mclmac_set_MAC_state function.\n");
    test_mclmac_set_MAC_state();
    printf("Test passed.\n");

    printf("Testing mclmac_set_next_MAC_state function.\n");
    test_mclmac_set_next_MAC_state();
    printf("Test passed.\n");

    printf("Testing mclmac_get_MAC_state function.\n");
    test_mclmac_get_MAC_state();
    printf("Test passed.\n");

    return;
}
