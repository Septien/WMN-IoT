#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "MCLMAC.h"

#define ITERATIONS 1000

void test_MCLMAC_init(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t  dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);
#ifdef __LINUX__
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(mclmac->frame != NULL);
#endif
    assert(ARROW(ARROW(mclmac)frame)current_slot == 0);
    assert(ARROW(ARROW(mclmac)frame)current_frame == 0);
    assert(ARROW(ARROW(mclmac)frame)slots_number == 0);
    assert(ARROW(ARROW(mclmac)frame)current_cf_slot == 0);
    assert(ARROW(ARROW(mclmac)frame)cf_slots_number == 0);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    assert(ARROW(mclmac)macState.currentState == START);
    assert(ARROW(mclmac)_dataQSize == dataQsize);
    assert(ARROW(mclmac)_collisionDetected == 0);
    assert(ARROW(mclmac)_networkTime == 0);
    assert(ARROW(mclmac)_collisionFrequency == 0);
    assert(ARROW(mclmac)_collisionSlot == 0);
    assert(ARROW(mclmac)_nSlots == _nSlots);
    assert(ARROW(mclmac)_nChannels == _nChannels);
#ifdef __LINUX__
    assert(mclmac->_occupiedSlots != NULL);
#endif
#ifdef __RIOT__
    assert(mclmac._occupiedSlots.size > 0);
#endif

    MCLMAC_destroy(&mclmac);
}

void test_MCLMAC_destroy(void)
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

    MCLMAC_destroy(&mclmac);
#ifdef __LINUX__
    assert(mclmac == NULL);
#endif
#ifdef __RIOT__
    assert(mclmac._occupiedSlots.size == 0);
#endif
}

void test_MCLMAC_clear(void)
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

    MCLMAC_clear(REFERENCE mclmac);
    assert(ARROW(ARROW(mclmac)frame)current_frame == 0);
    assert(ARROW(ARROW(mclmac)frame)current_slot == 0);
    assert(ARROW(ARROW(mclmac)frame)current_cf_slot == 0);
    assert(ARROW(ARROW(mclmac)frame)cf_slots_number == 0);
    assert(ARROW(ARROW(mclmac)frame)slots_number == 0);
    assert(ARROW(mclmac)_dataQSize == 0);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);
    assert(ARROW(mclmac)macState.currentState == START);
    assert(ARROW(mclmac)_collisionDetected == 0);
    assert(ARROW(mclmac)_networkTime == 0);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_init_powermode_state_machine(void)
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

    mclmac_init_powermode_state_machine(REFERENCE mclmac);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    assert(ARROW(mclmac)powerMode.nextState == NONEP);

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

void test_mclmac_set_powermode_state(void)
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
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

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
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

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

void test_mclmac_set_cf_channel(void)
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

    uint32_t cfchannel;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        cfchannel = (uint32_t)rand();
        mclmac_set_cf_channel(REFERENCE mclmac, cfchannel);
        assert(ARROW(ARROW(mclmac)mac)cfChannel == cfchannel);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_cf_channel(void)
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

    uint32_t cfchannel, cfchannelS;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        cfchannel = (uint32_t)rand();
        mclmac_set_cf_channel(REFERENCE mclmac, cfchannel);
        cfchannelS = mclmac_get_cf_channel(REFERENCE mclmac);
        assert(cfchannelS == cfchannel);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_transmit_channel(void)
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

    uint32_t channel;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        channel = (uint32_t)rand();
        mclmac_set_transmit_channel(REFERENCE mclmac, channel);
        assert(ARROW(ARROW(mclmac)mac)transmitChannel == channel);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_transmit_channel(void)
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

    uint32_t channel, channelS;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        channel = (uint32_t)rand();
        mclmac_set_transmit_channel(REFERENCE mclmac, channel);
        channelS = mclmac_get_transmit_channel(REFERENCE mclmac);
        assert(channelS == channel);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_reception_channel(void)
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

    uint32_t channel;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        channel = (uint32_t)rand();
        mclmac_set_reception_channel(REFERENCE mclmac, channel);
        assert(ARROW(ARROW(mclmac)mac)receiveChannel == channel);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_reception_channel(void)
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

    uint32_t channel, channelS;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        channel = (uint32_t)rand();
        mclmac_set_reception_channel(REFERENCE mclmac, channel);
        channelS = mclmac_get_reception_channel(REFERENCE mclmac);
        assert(channelS == channel);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_available_channels(void)
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

    uint8_t nChannels = (uint8_t) rand();
    nChannels = (nChannels == 0 ? 1 : nChannels);
    ARRAY channels;
#ifdef __LINUX__
     channels = (uint8_t *)malloc(4 * nChannels * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&channels, nChannels * 4);
#endif
    int i;
    for (i = 0; i < nChannels; i++)
        WRITE_ARRAY(REFERENCE channels, (uint8_t)rand(), i);
    
    mclmac_set_available_channels(REFERENCE mclmac, &channels, nChannels);
    assert(ARROW(ARROW(mclmac)mac)numberChannels == nChannels);
    for (i = 0; i < nChannels; i++)
    {
        uint8_t element = READ_ARRAY(REFERENCE channels, i);
        uint8_t element2 = READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)channels, i);
        assert(element == element2);
    }

#ifdef __LINUX__
    free(channels);
#endif
#ifdef __RIOT__
    free_array(&channels);
#endif

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_available_channels(void)
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

    uint8_t nChannels = (uint8_t) rand();
    ARRAY channels;
#ifdef __LINUX__
     channels = (uint8_t *)malloc(nChannels * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&channels, nChannels);
#endif
    int i;
    for (i = 0; i < nChannels; i++)
        WRITE_ARRAY(REFERENCE channels, (uint8_t)rand(), i);
    
    mclmac_set_available_channels(REFERENCE mclmac, &channels, nChannels);

    uint8_t nChannelsS;
    ARRAY channelS;
#ifdef __LINUX__
    channelS = (uint8_t *)malloc(nChannels * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&channelS, nChannels);
#endif
    mclmac_get_available_channels(REFERENCE mclmac, &channelS, &nChannelsS);

    assert(nChannelsS == nChannels);
    for (i = 0; i < nChannelsS; i++)
        assert(READ_ARRAY(REFERENCE channelS, i) == READ_ARRAY(REFERENCE channels, i));    

#ifdef __LINUX__
    free(channels);
    free(channelS);
#endif
#ifdef __RIOT__
    free_array(&channels);
    free_array(&channelS);
#endif

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_nodeid(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint16_t nodeid = (uint16_t)rand();
        mclmac_set_nodeid(REFERENCE mclmac, nodeid);
        assert(ARROW(ARROW(mclmac)mac)nodeID == nodeid);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_nodeid(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint16_t nodeid = (uint16_t)rand();
        mclmac_set_nodeid(REFERENCE mclmac, nodeid);
        uint16_t nodeidR = mclmac_get_nodeid(REFERENCE mclmac);
        assert(nodeidR == nodeid);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_selected_slot(void)
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

    uint8_t slot;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        slot = (uint8_t) rand() % 256;
        mclmac_set_selected_slot(REFERENCE mclmac, slot);
        assert(ARROW(ARROW(mclmac)mac)selectedSlot == slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_selected_slot(void)
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

    uint8_t slot, slotR;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        slot = (uint8_t) rand() % 256;
        mclmac_set_selected_slot(REFERENCE mclmac, slot);
        slotR = mclmac_get_selected_slot(REFERENCE mclmac);
        assert(slotR == slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_number_of_hops(void)
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

    uint8_t hops;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        hops = (uint8_t)rand() % 256;
        mclmac_set_number_of_hops(REFERENCE mclmac, hops);
        assert(ARROW(ARROW(mclmac)mac)hopCount == hops);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_number_of_hops(void)
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

    uint8_t hops, hopsR;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        hops = (uint8_t)rand();
        mclmac_set_number_of_hops(REFERENCE mclmac, hops);
        hopsR = mclmac_get_number_of_hops(REFERENCE mclmac);
        assert(hopsR == hops);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_current_frame(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t frame_number = (uint32_t)rand();
        mclmac_set_current_frame(REFERENCE mclmac, frame_number);
        assert(ARROW(ARROW(mclmac)frame)current_frame);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_increase_frame(void)
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

    uint32_t frame_number = rand() % ITERATIONS;
    mclmac_set_current_frame(REFERENCE mclmac, frame_number);

    for (int i = 0; i < 1e6; i++)
    {
        mclmac_increase_frame(REFERENCE mclmac);
        frame_number++;
        assert(ARROW(ARROW(mclmac)frame)current_frame == frame_number);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_current_slot(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = (uint8_t)rand() % 256;
        mclmac_set_current_slot(REFERENCE mclmac, slot);
        assert(ARROW(ARROW(mclmac)frame)current_slot == slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_increase_slot(void)
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

    uint8_t slot = 0;
    mclmac_set_current_slot(REFERENCE mclmac, slot);
    int n = rand() % 512;
    for (int i = 0; i < n; i++)
    {
        mclmac_increase_slot(REFERENCE mclmac);
        slot++;
        assert(ARROW(ARROW(mclmac)frame)current_slot == slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_slots_number(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slots_number = (uint8_t) rand();
        slots_number = (slots_number == 0 ? 1 : slots_number);
        mclmac_set_slots_number(REFERENCE mclmac, slots_number);
        assert(ARROW(ARROW(mclmac)frame)slots_number == slots_number);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_cf_slots_number(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t cf_slots_number = 1 + ((uint8_t) rand());
        cf_slots_number = (cf_slots_number == 0 ? 1 : cf_slots_number);
        mclmac_set_cf_slots_number(REFERENCE mclmac, cf_slots_number);
        assert(ARROW(ARROW(mclmac)frame)cf_slots_number == cf_slots_number);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_current_cf_slot(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t current_cf_slot = ((uint8_t) rand() % 256);
        current_cf_slot = (current_cf_slot == 0 ? 1 : current_cf_slot);
        mclmac_set_current_cf_slot(REFERENCE mclmac, current_cf_slot);
        assert(ARROW(ARROW(mclmac)frame)current_cf_slot == current_cf_slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_increase_cf_slot(void)
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
    
    uint8_t nCFSlot = rand();
    nCFSlot = (nCFSlot == 0 ? 1 : nCFSlot);
    mclmac_set_current_cf_slot(REFERENCE mclmac, nCFSlot);
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        mclmac_increase_cf_slot(REFERENCE mclmac);
        nCFSlot++;
        assert(ARROW(ARROW(mclmac)frame)current_cf_slot == nCFSlot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_slot_duration(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
    double slot_duration;
#endif
#ifdef __RIOT__
    sx127x_t radio;
    uint32_t slot_duration;
#endif
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        slot_duration = rand();
        mclmac_set_slot_duration(REFERENCE mclmac, slot_duration);
        assert(ARROW(ARROW(mclmac)frame)slot_duration == slot_duration);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_frame_duration(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
    double frame_duration;
#endif
#ifdef __RIOT__
    sx127x_t radio;
    uint32_t frame_duration;
#endif
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        frame_duration = rand();
        mclmac_set_frame_duration(REFERENCE mclmac, frame_duration);
        assert(ARROW(ARROW(mclmac)frame)frame_duration == frame_duration);
    }
    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_cf_duration(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
    double cf_duration;
#endif
#ifdef __RIOT__
    sx127x_t radio;
    uint32_t cf_duration;
#endif
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        cf_duration = rand();
        mclmac_set_cf_duration(REFERENCE mclmac, cf_duration);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_record_collision(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t freq = (uint32_t) rand();
        uint8_t slot = (uint8_t) rand();
        mclmac_record_collision(REFERENCE mclmac, slot, freq);
        assert(ARROW(mclmac)_collisionSlot == slot);
        assert(ARROW(mclmac)_collisionFrequency == freq);
        assert(ARROW(mclmac)_collisionDetected == true);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_destination_id(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint16_t destinationID = (uint16_t) rand();
        mclmac_set_destination_id(REFERENCE mclmac, destinationID);
        assert(ARROW(ARROW(mclmac)mac)destinationID == destinationID);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_destination_id(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint16_t destinationid = (uint16_t) rand();
        mclmac_set_destination_id(REFERENCE mclmac, destinationid);
        uint16_t destinationidR = mclmac_get_destination_id(REFERENCE mclmac);
        assert(destinationid == destinationidR);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_network_time(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t netTime = (uint32_t)rand();
        mclmac_set_network_time(REFERENCE mclmac, netTime);
        assert(ARROW(mclmac)_networkTime == netTime);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_network_time(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t netTime = rand();
        mclmac_set_network_time(REFERENCE mclmac, netTime);
        uint32_t netTimeA = mclmac_get_network_time(REFERENCE mclmac);
        assert(netTimeA == netTime);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_create_cf_packet(void)
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

    mclmac_create_cf_packet(REFERENCE mclmac);
#ifdef __LINUX__
    assert(ARROW(ARROW(mclmac)mac)cfpkt != NULL);
#endif
    assert(ARROW(ARROW(ARROW(mclmac)mac)cfpkt)nodeID > 0);
    assert(ARROW(ARROW(ARROW(mclmac)mac)cfpkt)destinationID >= 0);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_create_control_packet(void)
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

    uint8_t bytes = get_number_bytes(_nSlots * _nChannels);
    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);
    ARROW(ARROW(mclmac)mac)nodeID = rand();
    ARROW(mclmac)_collisionFrequency = rand();
    ARROW(mclmac)_collisionSlot = rand();
    ARROW(ARROW(mclmac)mac)hopCount = rand();
    ARROW(mclmac)_ack = rand();
    ARROW(mclmac)_networkTime = rand();
    int i;
    for (i = 0; i < bytes; i++)
        WRITE_ARRAY(REFERENCE ARROW(mclmac)_occupiedSlots, rand(), i);

    mclmac_create_control_packet(REFERENCE mclmac);
#ifdef __LINUX__
    assert(mclmac->mac->ctrlpkt != NULL);
#endif
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)nodeID == ARROW(ARROW(mclmac)mac)nodeID);
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)collisionSlot ==ARROW(mclmac)_collisionSlot);
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)collisionFrequency == ARROW(mclmac)_collisionFrequency);
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)hopCount == ARROW(ARROW(mclmac)mac)hopCount);
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)ack == ARROW(mclmac)_ack);
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)networkTime == ARROW(mclmac)_networkTime);
    for (i = 0; i < bytes; i++)
        assert(READ_ARRAY(REFERENCE ARROW(mclmac)_occupiedSlots, i) == READ_ARRAY(REFERENCE ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)occupiedSlots, i));

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_create_data_packet(void)
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
    ARROW(mclmac)_isFragment = rand();
    ARROW(mclmac)_numberFragments = rand();
    ARROW(mclmac)_fragmentNumber = rand();

    mclmac_create_data_packet(REFERENCE mclmac);
#ifdef __LINUX__
    assert(mclmac->mac->datapkt != NULL);
#endif
    assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)isFragment == ARROW(mclmac)_isFragment);
    assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)totalFragments == ARROW(mclmac)_numberFragments);
    assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)fragmentNumber == ARROW(mclmac)_fragmentNumber);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_packet_data(void)
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

    mclmac_create_data_packet(REFERENCE mclmac);

    // The total length of the data packet, including the fields, should not exceed 256 bytes. This as the
    // maximum buffer size for the SX1276 modem is 256 bytes.
    uint8_t size;
    int n = 1;//rand() % ITERATIONS;
    int j;
    for (int i = 0; i < n; i++)
    {
        size = 1 + rand() % 201;
        ARRAY data;
#ifdef __LINUX__
        data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
        create_array(&data, size);
#endif
        for (j = 0; j < size; j++)
            WRITE_ARRAY(REFERENCE data, rand(), j);

        mclmac_set_packet_data(REFERENCE mclmac, &data, size);

        assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)dataLength == size);
#ifdef __LINUX__
        assert(mclmac->mac->datapkt->data != NULL);
#endif
#ifdef __RIOT__
        assert(mclmac.mac.datapkt.data.size > 0);
#endif
        for (j = 0; j < size; j++)
        {
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(ARROW(mclmac)mac)datapkt)data, j) == READ_ARRAY(REFERENCE data, j));
        }

        //mclmac_delete_data_from_packet(mclmac);
#ifdef __LINUX__
        free(data);
        data = NULL;
#endif
#ifdef __RIOT__
        free_array(&data);
#endif
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_delete_data_from_packet(void)
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

    mclmac_create_data_packet(REFERENCE mclmac);
    uint8_t size = 249;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (int i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    mclmac_set_packet_data(REFERENCE mclmac, &data, size);

    mclmac_delete_data_from_packet(REFERENCE mclmac);
    assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)dataLength == 0);
#ifdef __LINUX__
    assert(mclmac->mac->datapkt->data == NULL);
#endif
#ifdef __RIOT__
    assert(mclmac.mac.datapkt.data.size == 0);
#endif

#ifdef __LINUX__
    free(data);
#endif
#ifdef __RIOT__
    free_array(&data);
#endif

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_clear_data_from_packet(void)
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

    mclmac_create_data_packet(REFERENCE mclmac);
    uint8_t size = 249;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (int i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    mclmac_set_packet_data(REFERENCE mclmac, &data, size);
    mclmac_clear_data_from_packet(REFERENCE mclmac);
    assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)dataLength == size);
    for (int i = 0; i < size; i++)
        assert(READ_ARRAY(REFERENCE ARROW(ARROW(ARROW(mclmac)mac)datapkt)data, i) == 0);

#ifdef __LINUX__
    free(data);
#endif
#ifdef __RIOT__
    free_array(&data);
#endif

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
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;
    int r;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    mclmac_init_powermode_state_machine(REFERENCE mclmac);

    // Try to transition from STARTP state to ACTIVE, TRANSMIT,  RECEIVE or FINISHP states.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_powermode_state(REFERENCE mclmac, STARTP);
    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
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

void test_mclmac_execute_powermode_state(void)
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
    int r;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    mclmac_init_powermode_state_machine(REFERENCE mclmac);

    ARROW(mclmac)_frameDuration = 100;
    ARROW(mclmac)_slotDuration = 100;
    ARROW(mclmac)_cfDuration = 100;
    mclmac_set_powermode_state(REFERENCE mclmac, NONEP);
    r = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(r == E_PM_EXECUTION_FAILED);

    /* State STARTP. Execute the following tasks:
    *   -Set the number of slots and cfslots.
    *   -Set the timers value.
    *   -Initialize the values of frame, slot, and cfslot to zero.
    *   -Immediately pass to PASSIVE state.
    *  Return E_PM_EXECUTION_SUCESS when successfully executed.
    */
    mclmac_set_powermode_state(REFERENCE mclmac, STARTP);
    r = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(r == E_PM_EXECUTION_SUCCESS);
#ifdef __LINUX__
   assert(mclmac->mac->cfpkt == NULL);
   assert(mclmac->mac->datapkt == NULL);
   assert(mclmac->mac->ctrlpkt == NULL);
#endif
    assert(ARROW(ARROW(mclmac)frame)slots_number == ARROW(mclmac)_nSlots);
    assert(ARROW(ARROW(mclmac)frame)cf_slots_number == ARROW(mclmac)_nSlots);
    assert(ARROW(ARROW(mclmac)frame)frame_duration == ARROW(mclmac)_frameDuration);
    assert(ARROW(ARROW(mclmac)frame)slot_duration == ARROW(mclmac)_slotDuration);
    assert(ARROW(ARROW(mclmac)frame)cf_duration == ARROW(mclmac)_cfDuration);
    assert(ARROW(ARROW(mclmac)frame)current_frame == 0);
    assert(ARROW(ARROW(mclmac)frame)current_slot == 0);
    assert(ARROW(ARROW(mclmac)frame)current_cf_slot == 0);
    assert(ARROW(mclmac)powerMode.nextState == PASSIVE);

    /* State PASSIVE. Execute the following tasks:
    *   -Set the radio to SLEEP mode.
    *   -Start the slot timer.
    *   -Set next state to NONEP.
    */
    mclmac_set_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(r == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)powerMode.nextState == NONEP);
    assert(ARROW(ARROW(mclmac)frame)current_cf_slot == 0);
#ifdef __LINUX__
    // Assert mode is sleep
#endif
#ifdef __RIOT__
    /*uint8_t mode = sx127x_get_op_mode(mclmac->mac->radio);
    assert(mode == sx127x_RF_OPMODE_SLEEP);
    uint8_t state = sx127x_get_state(mclmac->mac->radio);
    assert(state == SX127X_RF_IDLE);*/
#endif

    /** State ACTIVE. Executes the following tasks:
     *      - Initiates the slot for synchornization and data transmission.
     *      - Initiates the slot timer.
     *      - Initialize the cf packet.
     *      - Initiates the cf phase: in which:
     *          - Changes the radio to the common channel.
     *          - Sets the cf counter to zero.
     *          - Initiates the cf timer.
     *          - It checks wether the current cf slot is equal to the 
     *            selected slot, in which case it transmits a cf packet with destination ID.
     *            Set the next state to TRANSMIT.
     *          - Otherwise, it puts the radio in listen mode, and if packet arrives in which the 
     *            destinationID == nodeID or destinationID == 0, save the frequency and set 
     *            the next state to RECEIVE.
     *          - When the CF timer expires, increment the cf counter.
     *          - When the CF counter equals the number of slots, check whether we should transmit,
     *            receive, or none. If none required, set next state to PASSIVE, otherwise, change 
     *            to the required frequency.
     *          - In case a collision is detected: save frequency and slot of collision, and wait
     *            for the next time we send a control packet to tell the network. Save the number
     *            cf packets received.
     */
    mclmac_set_current_cf_slot(REFERENCE mclmac, 1);
    mclmac_set_cf_channel(REFERENCE mclmac, 915000000);
    mclmac_set_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(r == E_PM_EXECUTION_SUCCESS);
#ifdef __LINUX__
    assert(mclmac->mac->cfpkt != NULL);
    // Check channel is changed
#endif
#ifdef __RIOT__
    // Wait 30 us for radio to change frequency.
    /*uint32_t channel = sx127x_get_channel(mclmac->mac->radio);
    assert(channel == mclmac->mac->cfChannel);*/
#endif
    mclmac_set_current_cf_slot(REFERENCE mclmac, ARROW(ARROW(mclmac)mac)selectedSlot);
    r = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(ARROW(ARROW(ARROW(mclmac)mac)cfpkt)destinationID == ARROW(ARROW(mclmac)mac)destinationID);
    assert(ARROW(mclmac)powerMode.nextState == TRANSMIT);

    MCLMAC_destroy(&mclmac);
}

void executeTestsMCLMAC(void)
{
    srand(time(NULL));

    printf("Testing MAC_init function.\n");
    test_MCLMAC_init();
    printf("Test passed.\n");

    printf("Testing MCLMAC_destroy function.\n");
    test_MCLMAC_destroy();
    printf("Test passed.\n");

    printf("Test MCLMAC_clear function.\n");
    test_MCLMAC_clear();
    printf("Test passed.\n");

    printf("Testing _mclmac_init_powermode_state_machine function.\n");
    test_mclmac_init_powermode_state_machine();
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

    printf("Testing mclmac_set_powermode_state function.\n");
    test_mclmac_set_powermode_state();
    printf("Test passed.\n");

    printf("Testing mclmac_set_next_powermode_state function.\n");
    test_mclmac_set_next_powermode_state();
    printf("Test passed.\n");

    printf("Testing mclmac_get_powermode_state function.\n");
    test_mclmac_get_powermode_state();
    printf("Test passed.\n");

    printf("Testing mclmac_set_cf_channel function.\n");
    test_mclmac_set_cf_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_get_cf_channel function.\n");
    test_mclmac_get_cf_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_set_transmit_channel function.\n");
    test_mclmac_set_transmit_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_get_transmite_channel function.\n");
    test_mclmac_get_transmit_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_set_reception_channel function.\n");
    test_mclmac_set_reception_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_get_reception_channel function.\n");
    test_mclmac_get_reception_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_set_available_channels function.\n");
    test_mclmac_set_available_channels();
    printf("Test passed.\n");

    printf("Testing mclmac_get_available_channels function.\n");
    test_mclmac_get_available_channels();
    printf("Test passed.\n");

    printf("Testing mclmac_set_nodeid function.\n");
    test_mclmac_set_nodeid();
    printf("Test passed.\n");

    printf("Testing mclmac_get_nodeid function.\n");
    test_mclmac_get_nodeid();
    printf("Test passed.\n");

    printf("Testing mclmac_set_selected_slot function.\n");
    test_mclmac_set_selected_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_get_selected_slot function.\n");
    test_mclmac_get_selected_slot();
    printf("Test passed.\n");


    printf("Testing mclmac_set_number_of_hops function.\n");
    test_mclmac_set_number_of_hops();
    printf("Test passed.\n");

    printf("Testing mclmac_get_number_of_hops function.\n");
    test_mclmac_get_number_of_hops();
    printf("Test passed.\n");

    printf("Testing mclmac_set_current_frame function.\n");
    test_mclmac_set_current_frame();
    printf("Test passed.\n");

    printf("Test mclmac_increase_frame function.\n");
    test_mclmac_increase_frame();
    printf("Test passed.\n");

    printf("Testing mclmac_set_current_slot function.\n");
    test_mclmac_set_current_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_increase_slot function.\n");
    test_mclmac_increase_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_set_slots_number function.\n");
    test_mclmac_set_slots_number();
    printf("Test passed.\n");

    printf("Testing mclmac_set_cf_slots_number function.\n");
    test_mclmac_set_cf_slots_number();
    printf("Test passed.\n");

    printf("Testing mclmac_set_current_cf_slot function.\n");
    test_mclmac_set_current_cf_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_increase_cf_slot function.\n");
    test_mclmac_increase_cf_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_set_slot_duration function.\n");
    test_mclmac_set_slot_duration();  
    printf("Test passed.\n");

    printf("Testing mclmac_set_frame_duration function.\n");
    test_mclmac_set_frame_duration();
    printf("Test passed.\n");

    printf("Testing mclmac_set_cf_duration function.\n");
    test_mclmac_set_cf_duration();
    printf("Test passed.\n");

    printf("Testing mclmac_record_collision function.\n");
    test_mclmac_record_collision();
    printf("Test passed.\n");

    printf("Testing mclmac_set_destination_id function.\n");
    test_mclmac_set_destination_id();
    printf("Test passed.\n");

    printf("Testing mclmac_get_destination_id function.\n");
    test_mclmac_get_destination_id();
    printf("Test passed.\n");

    printf("Testing mclmac_set_network_time function.\n");
    test_mclmac_set_network_time();
    printf("Test passed.\n");

    printf("Testing mclmac_get_network_time function.\n");
    test_mclmac_get_network_time();
    printf("Test passed.\n");

    printf("Testing mclmac_create_cf_packet function.\n");
    test_mclmac_create_cf_packet();
    printf("Test passed.\n");

    printf("Testing mclmac_create_control_packet function.\n");
    test_mclmac_create_control_packet();
    printf("Test passed.\n");

    printf("Testing mclmac_create_data_packet function.\n");
    test_mclmac_create_data_packet();
    printf("Test passed.\n");

    printf("Testing mclmac_set_packet_data function.\n");
    test_mclmac_set_packet_data();
    printf("Test passed.\n");

    printf("Testing mclmac_delete_data_from_packet function.\n");
    test_mclmac_delete_data_from_packet();
    printf("Test passed.\n");

    printf("Testing mclmac_clear_data_from_packet function.\n");
    test_mclmac_clear_data_from_packet();
    printf("Test passed.\n");

    printf("Test mclmac_update_powermode_state_machine function.\n");
    test_mclmac_update_powermode_state_machine();
    printf("Test passed.\n");

    printf("Testing mclmac_execute_powermode_state function.\n");
    test_mclmac_execute_powermode_state();
    printf("Test passed.\n");

    return;
}
