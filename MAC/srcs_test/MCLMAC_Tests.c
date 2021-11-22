#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include "string.h"

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
    uint16_t nodeid = 0;
    size_t  dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);
#ifdef __LINUX__
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    //assert(mclmac->frame != NULL);
#endif
    /*assert(ARROW(ARROW(mclmac)frame)current_slot == 0);
    assert(ARROW(ARROW(mclmac)frame)current_frame == 0);
    assert(ARROW(ARROW(mclmac)frame)slots_number == 0);
    assert(ARROW(ARROW(mclmac)frame)current_cf_slot == 0);
    assert(ARROW(ARROW(mclmac)frame)cf_slots_number == 0);*/
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    assert(ARROW(mclmac)macState.currentState == START);
    assert(ARROW(mclmac)_nodeID == nodeid);
    assert(ARROW(mclmac)_dataQSize == dataQsize);
    assert(ARROW(mclmac)_networkTime == 0);
    assert(ARROW(mclmac)_nSlots == _nSlots);
    assert(ARROW(mclmac)_nChannels == _nChannels);
#ifdef __LINUX__
    /*assert(mclmac->_packets == NULL);
    assert(mclmac->_packets_received == NULL);*/
#endif
#ifdef __RIOT__
    /*assert(mclmac._packets.size == 0);
    assert(mclmac._packets.size == 0);*/
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    MCLMAC_destroy(&mclmac);
#ifdef __LINUX__
    assert(mclmac == NULL);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    MCLMAC_clear(REFERENCE mclmac);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame == 0);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot == 0);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == 0);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)cf_slots_number == 0);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)slots_number == 0);
    assert(ARROW(mclmac)_dataQSize == 0);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    assert(ARROW(mclmac)macState.currentState == START);
    assert(ARROW(mclmac)_hopCount == 0);
    assert(ARROW(mclmac)_networkTime == 0);

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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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

/*void test_mclmac_set_available_channels(void)
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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
}*/

void test_mclmac_set_nodeid(void)
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

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint16_t nodeid = (uint16_t)rand();
        mclmac_set_nodeid(REFERENCE mclmac, nodeid);
        assert(ARROW(mclmac)_nodeID == nodeid);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    uint8_t hops;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        hops = (uint8_t)rand() % 256;
        mclmac_set_number_of_hops(REFERENCE mclmac, hops);
        assert(ARROW(mclmac)_hopCount == hops);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t frame_number = (uint32_t)rand();
        mclmac_set_current_frame(REFERENCE mclmac, frame_number);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame == frame_number);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    uint32_t frame_number = 0;
    mclmac_set_current_frame(REFERENCE mclmac, frame_number);

    for (int i = 0; i < ITERATIONS; i++)
    {
        mclmac_increase_frame(REFERENCE mclmac);
        frame_number++;
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame == frame_number);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = (uint8_t)rand() % 256;
        mclmac_set_current_slot(REFERENCE mclmac, slot);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot == slot);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    uint8_t slot = 0;
    mclmac_set_current_slot(REFERENCE mclmac, slot);
    int n = rand() % 512;
    for (int i = 0; i < n; i++)
    {
        mclmac_increase_slot(REFERENCE mclmac);
        slot++;
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot == slot);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slots_number = (uint8_t) rand();
        slots_number = (slots_number == 0 ? 1 : slots_number);
        mclmac_set_slots_number(REFERENCE mclmac, slots_number);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)slots_number == slots_number);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t cf_slots_number = 1 + ((uint8_t) rand());
        cf_slots_number = (cf_slots_number == 0 ? 1 : cf_slots_number);
        mclmac_set_cf_slots_number(REFERENCE mclmac, cf_slots_number);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)cf_slots_number == cf_slots_number);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t current_cf_slot = ((uint8_t) rand() % 256);
        current_cf_slot = (current_cf_slot == 0 ? 1 : current_cf_slot);
        mclmac_set_current_cf_slot(REFERENCE mclmac, current_cf_slot);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == current_cf_slot);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);
    
    uint8_t nCFSlot = rand();
    nCFSlot = (nCFSlot == 0 ? 1 : nCFSlot);
    mclmac_set_current_cf_slot(REFERENCE mclmac, nCFSlot);
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        mclmac_increase_cf_slot(REFERENCE mclmac);
        nCFSlot++;
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == nCFSlot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_slot_duration(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
    double slot_duration = 0;
#endif
#ifdef __RIOT__
    sx127x_t radio;
    uint32_t slot_duration = 0;
#endif
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        slot_duration = rand();
        mclmac_set_slot_duration(REFERENCE mclmac, slot_duration);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)slot_duration == slot_duration);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        frame_duration = rand();
        mclmac_set_frame_duration(REFERENCE mclmac, frame_duration);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)frame_duration == frame_duration);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        cf_duration = rand();
        mclmac_set_cf_duration(REFERENCE mclmac, cf_duration);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)cf_duration == cf_duration);
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t freq = (uint32_t) rand();
        uint8_t slot = (uint8_t) rand();
        mclmac_record_collision(REFERENCE mclmac, slot, freq);
        assert(ARROW(ARROW(mclmac)mac)_collisionSlot == slot);
        assert(ARROW(ARROW(mclmac)mac)_collisionFrequency == freq);
        assert(ARROW(ARROW(mclmac)mac)_collisionDetected == true);
    }

    MCLMAC_destroy(&mclmac);
}

/*void test_mclmac_set_destination_id(void)
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint16_t destinationid = (uint16_t) rand();
        mclmac_set_destination_id(REFERENCE mclmac, destinationid);
        uint16_t destinationidR = mclmac_get_destination_id(REFERENCE mclmac);
        assert(destinationid == destinationidR);
    }

    MCLMAC_destroy(&mclmac);
}*/

void test_mclmac_set_network_time(void)
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
    uint16_t nodeid = 0;
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

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

void test_stub_mclmac_read_queue_element(void)
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

    size_t size = ARROW(ARROW(mclmac)mac)_max_number_packets_buffer;

#ifdef __LINUX__
    mclmac->mac->_packets = (uint8_t *)malloc(size * sizeof(uint8_t));
    memset(mclmac->mac->_packets, 0, size);
#endif
#ifdef __RIOT__
    int ret = create_array(&mclmac.mac._packets, size);
    if (ret == 0)
    {
        printf("Unable to create array.\n");
        return;
    }
    for (uint j = 0; j < size; j++)
        write_element(&mclmac.mac._packets, 0, j);
#endif

    uint16_t bytes;      // store how many bytes read from queue
    uint32_t read_from, old_read_from;
    /* Test case 1: 
    *   When last_pos is size - 1
    *       -nelements should be 0.
    *       -bytes should be 0.
    *       -last_pos should not change.
    * */
    read_from = size - 1;
    int nelements = stub_mclmac_read_queue_element(REFERENCE mclmac, &bytes, size, &read_from);
    assert(nelements == 0);
    assert(bytes == 0);
    assert(read_from == size - 1);

    /* Test case 2: 
    *   -The array is emtpy. The function should generate the elements randomly.
    *   It should return:
    *       -nelements = 1.
    *       -last_pos should be equal to bytes.
    *       -bytes should be greater than 0.
    *       -The last element, at bytes - 1, should contain '\0'.
    *  */
    read_from = 0;
    old_read_from = read_from;
    nelements = stub_mclmac_read_queue_element(REFERENCE mclmac, &bytes, size, &read_from);
    assert(nelements == 1);
    assert(bytes > 0);
    assert(read_from == bytes);
    uint8_t end = READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_packets, bytes - 1);
    assert(end == '\0');
    int i;
    for (i = old_read_from; i < bytes - 1; i++)
    {
        assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_packets, i) != 0);
    }

    /**
     * Test case 3:
     * -The array alreaady contains elements, it should add the data at the first empty place.
     *  It should return:
     *      -nelements = 1.
     *      -read_from should increase by bytes sise.
     *      -bytes should be greater than zero.
     *      -The last element, at red_from - 1, should be '\0'.
     * */
    old_read_from = read_from;
    bytes = 0;
    nelements = stub_mclmac_read_queue_element(REFERENCE mclmac, &bytes, size, &read_from);
    assert(nelements == 1);
    assert(bytes > 0);
    assert(read_from == old_read_from + bytes);
    end = READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_packets, read_from - 1);
    assert(end == '\0');
    end = READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_packets, read_from - 2);
    assert(end != 0);

    /**
     * Test case 4:
     *  -If the number of elements to add to the array is greater than the available 
     *  memory on the array, return 0.
    */
    read_from = size - 2;
    nelements = stub_mclmac_read_queue_element(REFERENCE mclmac, &bytes, size, &read_from);
    assert(nelements == 0);
    assert(bytes == 0);
    assert(read_from == size - 2);

#ifdef __LINUX__
    free(mclmac->mac->_packets);
#endif
#ifdef __RIOT__
    free_array(&mclmac.mac._packets);
#endif

    MCLMAC_destroy(&mclmac);
}

void test_stub_mclmac_write_queue_element(void)
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
    size_t size = 5 * 256;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int ret;
#ifdef __LINUX__
    mclmac->mac->_packets_received = (uint8_t *)malloc(size * sizeof(uint8_t));
    if (mclmac->mac->_packets_received == NULL)
    {
        printf("LINUX: Unable to create array _packets_received. LINUX.\n");
        return;
    }
#endif
#ifdef __RIOT__
    ret = create_array(&mclmac.mac._packets_received, size);
    if (ret == 0)
    {
        printf("Unable to create array _packets_received, RIOT.\n");
        return;
    }
#endif
    ARROW(ARROW(mclmac)mac)_num_packets_received = 5;
    // Fill the array with random data
    for (size_t i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_packets_received, rand(), i);

        /**
         * Test case 1:
         *  -If size = 0, return 0
        */
       ret = stub_mclmac_write_queue_element(REFERENCE mclmac, 0);
       assert(ret == 0);

       /**
        * Test case 2:
        *   -Read one element of the array. It should changer:
        *       -Return 1;
       */
      ret = stub_mclmac_write_queue_element(REFERENCE mclmac, size);
      assert(ret == 1);

#ifdef __LINUX__
    free(mclmac->mac->_packets_received);
#endif
#ifdef __RIOT__ 
    free_array(&mclmac.mac._packets_received);
#endif

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_change_cf_channel(void)
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

    mclmac_set_cf_channel(REFERENCE mclmac, 915000000);

    stub_mclmac_change_cf_channel(REFERENCE mclmac);
    
    // Check the state is standby
    // Check the channel is cf

    MCLMAC_destroy(&mclmac);
}
void test_mclmac_start_cf_phase(void)
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

    mclmac_set_cf_channel(REFERENCE mclmac, 915000000);

    stub_mclmac_change_cf_channel(REFERENCE mclmac);

    stub_mclmac_start_cf_phase(REFERENCE mclmac);

    // Check for the frequency

    // Check the state of the radio is rx-single

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_send_cf_message(void)
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

    mclmac_set_nodeid(REFERENCE mclmac, 1);
//    mclmac_set_destination_id(REFERENCE mclmac, 2);
//    mclmac_create_cf_packet(REFERENCE mclmac);

    mclmac_set_cf_channel(REFERENCE mclmac, 915000000);

    stub_mclmac_change_cf_channel(REFERENCE mclmac);

    stub_mclmac_start_cf_phase(REFERENCE mclmac);

    /* Send the packet the destination id is already known, and the 
       cf phase is already started. */
    stub_mclmac_send_cf_message(REFERENCE mclmac);

    MCLMAC_destroy(&mclmac);
}

void test_stub_mclmac_receive_cf_message(void)
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

    mclmac_set_nodeid(REFERENCE mclmac, 1);
//    mclmac_set_destination_id(REFERENCE mclmac, 2);

    mclmac_set_cf_channel(REFERENCE mclmac, 915000000);

    stub_mclmac_change_cf_channel(REFERENCE mclmac);

    stub_mclmac_start_cf_phase(REFERENCE mclmac);

    /* Receive the packet the destination id is already known, and the 
       cf phase is already started. */
    stub_mclmac_receive_cf_message(REFERENCE mclmac);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_receive_cf_message(void)
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
    size_t size = 5 * sizeof(uint16_t);

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize, _nSlots, _nChannels);

    int ret;
#ifdef __LINUX__
    mclmac->mac->_cf_messages = (uint8_t *)malloc(size * sizeof(uint8_t));
    if (mclmac->mac->_cf_messages == NULL)
    {
        printf("Unable to create _cf_messages queue. Linux.\n");
        exit(0);
    }
    memset(mclmac->mac->_cf_messages, 0, size);
#endif
#ifdef __RIOT__
    ret = create_array(&mclmac.mac._cf_messages, size);
    if (ret == 0)
    {
        printf("Unable to craeate _cf_messages queue. RIOT.\n");
        exit(0);
    }
#endif
//    mclmac_create_cf_packet(REFERENCE mclmac);

    for (int i = 0; i < ITERATIONS; i++)
    {
        ret = stub_mclmac_receive_cf_message(REFERENCE mclmac);
        if (ret)
        {
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 0) > 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 1) > 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 2) > 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 3) > 0);
        }
        else
        {
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 0) == 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 1) == 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 2) == 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 3) == 0);
        }
    }

#ifdef __LINUX__
    free(mclmac->mac->_cf_messages);
#endif
#ifdef __RIOT__
    free_array(&mclmac.mac._cf_messages);
#endif

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

    /*printf("Testing mclmac_set_available_channels function.\n");
    test_mclmac_set_available_channels();
    printf("Test passed.\n");

    printf("Testing mclmac_get_available_channels function.\n");
    test_mclmac_get_available_channels();
    printf("Test passed.\n");*/

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

    /*printf("Testing mclmac_set_destination_id function.\n");
    test_mclmac_set_destination_id();
    printf("Test passed.\n");

    printf("Testing mclmac_get_destination_id function.\n");
    test_mclmac_get_destination_id();
    printf("Test passed.\n");*/

    printf("Testing mclmac_set_network_time function.\n");
    test_mclmac_set_network_time();
    printf("Test passed.\n");

    printf("Testing mclmac_get_network_time function.\n");
    test_mclmac_get_network_time();
    printf("Test passed.\n");

    printf("Testing stub_mclmac_read_queue_element function.\n");
    test_stub_mclmac_read_queue_element();
    printf("Test passed.\n");

    printf("Testing stub_mclmac_write_queue_element function.\n");
    test_stub_mclmac_write_queue_element();
    printf("Test passed.\n");

    printf("Testing mclmac_change_cf_channel function.\n");
    test_mclmac_change_cf_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_start_cf_phase function.\n");
    test_mclmac_start_cf_phase();
    printf("Test passed.\n");

    printf("Testing mclmac_send_cf_message function.\n");
    test_mclmac_send_cf_message();
    printf("Test passed.\n");

    printf("Testing mclmac_receive_cf_message function.\n");
    test_mclmac_receive_cf_message();
    printf("Test passed.\n");
    return;
}
