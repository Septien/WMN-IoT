#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "../include/MCLMAC.h"

#define ITERATIONS 1000

void testmacInit()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(mclmac->frame != NULL);
    assert(mclmac->frame->currentSlot == 0);
    assert(mclmac->frame->currentFrame == 0);
    assert(mclmac->frame->currentCFSlot == 0);
    assert(mclmac->frame->cfSlotsNumber == 0);
    assert(mclmac->frame->slotsNumber == 0);
    assert(mclmac->dataQueue != NULL);
    assert(mclmac->powerMode.currentState == PASSIVE);
    assert(mclmac->macState.currentState == START);
    assert(mclmac->_dataQSize == dataQsize);
    assert(mclmac->_collisionDetected == 0);
    assert(mclmac->_networkTime == 0);
    assert(mclmac->_collisionFrequency == 0);
    assert(mclmac->_collisionSlot == 0);
    assert(mclmac->_nSlots == _nSlots);
    assert(mclmac->_nChannels == _nChannels);

    destroyMCLMAC(&mclmac);
}

void testdestroyMCLMAC()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    destroyMCLMAC(&mclmac);
    assert(mclmac == NULL);
}

void testclearMCLMAC()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    clearMCLMAC(mclmac);
    assert(mclmac->frame->currentFrame == 0);
    assert(mclmac->frame->currentSlot == 0);
    assert(mclmac->frame->currentCFSlot == 0);
    assert(mclmac->frame->cfSlotsNumber == 0);
    assert(mclmac->frame->slotsNumber == 0);
    assert(mclmac->dataQueue == NULL);
    assert(mclmac->_dataQSize == 0);
    assert(mclmac->powerMode.currentState == PASSIVE);
    assert(mclmac->macState.currentState == START);
    assert(mclmac->_collisionDetected == 0);
    assert(mclmac->_networkTime == 0);

    destroyMCLMAC(&mclmac);
}

void testsetMACState()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    state_t state = START;
    setMACState(mclmac, state);
    assert(mclmac->macState.currentState == state);
    state = INITIALIZATION;
    setMACState(mclmac, state);
    assert(mclmac->macState.currentState == state);
    state = SYNCHRONIZATION;
    setMACState(mclmac, state);
    assert(mclmac->macState.currentState == state);
    state = DISCOVERY_AND_SELECTION;
    setMACState(mclmac, state);
    assert(mclmac->macState.currentState == state);
    state = MEDIUMACCESS;
    setMACState(mclmac, state);
    assert(mclmac->macState.currentState == state);

    destroyMCLMAC(&mclmac);
}

void testsetNextMACState()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    state_t state = INITIALIZATION;
    setNextMACState(mclmac, state);
    assert(mclmac->macState.nextState == state);
    state = SYNCHRONIZATION;
    setNextMACState(mclmac, state);
    assert(mclmac->macState.nextState == state);
    state = DISCOVERY_AND_SELECTION;
    setNextMACState(mclmac, state);
    assert(mclmac->macState.nextState == state);
    state = MEDIUMACCESS;
    setNextMACState(mclmac, state);
    assert(mclmac->macState.nextState == state);

    destroyMCLMAC(&mclmac);
}

void testgetMACState()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    state_t state = START, stateA;
    setMACState(mclmac, state);
    stateA = getMACState(mclmac);
    assert(stateA == state);

    state = INITIALIZATION;
    setMACState(mclmac, state);
    stateA = getMACState(mclmac);
    assert(stateA == state);

    state = SYNCHRONIZATION;
    setMACState(mclmac, state);
    stateA = getMACState(mclmac);
    assert(stateA == state);

    state = DISCOVERY_AND_SELECTION;
    setMACState(mclmac, state);
    stateA = getMACState(mclmac);
    assert(stateA == state);

    state = MEDIUMACCESS;
    setMACState(mclmac, state);
    stateA = getMACState(mclmac);
    assert(stateA == state);

    destroyMCLMAC(&mclmac);
}

void testsetPowerModeState()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    PowerMode_t mode = PASSIVE;
    setPowerModeState(mclmac, mode);
    assert(mclmac->powerMode.currentState == mode);

    mode = ACTIVE;
    setPowerModeState(mclmac, mode);
    assert(mclmac->powerMode.currentState == mode);

    mode = TRANSMIT;
    setPowerModeState(mclmac, mode);
    assert(mclmac->powerMode.currentState == mode);

    mode = RECEIVE;
    setPowerModeState(mclmac, mode);
    assert(mclmac->powerMode.currentState == mode);

    destroyMCLMAC(&mclmac);
}

void testsetNextPowerModeState()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    PowerMode_t mode = ACTIVE;
    setNextPowerModeState(mclmac, mode);
    assert(mclmac->powerMode.nextState == mode);

    mode = TRANSMIT;
    setNextPowerModeState(mclmac, mode);
    assert(mclmac->powerMode.nextState == mode);

    mode = RECEIVE;
    setNextPowerModeState(mclmac, mode);
    assert(mclmac->powerMode.nextState == mode);

    destroyMCLMAC(&mclmac);
}

void testgetPowerModeState()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    PowerMode_t mode = PASSIVE, modeR;
    setPowerModeState(mclmac, mode);
    modeR = getPowerModeState(mclmac);
    assert(modeR == mode);

    mode = ACTIVE;
    setPowerModeState(mclmac, mode);
    modeR = getPowerModeState(mclmac);
    assert(modeR == mode);

    mode = TRANSMIT;
    setPowerModeState(mclmac, mode);
    modeR = getPowerModeState(mclmac);
    assert(modeR == mode);

    mode = RECEIVE;
    setPowerModeState(mclmac, mode);
    modeR = getPowerModeState(mclmac);
    assert(modeR == mode);
    
    destroyMCLMAC(&mclmac);
}

void testsetCFChannel()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint32_t cfchannel;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        cfchannel = (uint32_t)rand();
        setCFChannel(mclmac, cfchannel);
        assert(mclmac->mac->cfChannel == cfchannel);
    }

    destroyMCLMAC(&mclmac);
}

void testgetCFChannel()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint32_t cfchannel, cfchannelS;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        cfchannel = (uint32_t)rand();
        setCFChannel(mclmac, cfchannel);
        cfchannelS = getCFChannel(mclmac);
        assert(cfchannelS == cfchannel);
    }

    destroyMCLMAC(&mclmac);
}

void testsetSelectedChannel()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint32_t channel;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        channel = (uint32_t)rand();
        setTransmitChannel(mclmac, channel);
        assert(mclmac->mac->transmitChannel == channel);
    }

    destroyMCLMAC(&mclmac);
}

void testgetSelectedChannel()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint32_t channel, channelS;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        channel = (uint32_t)rand();
        setTransmitChannel(mclmac, channel);
        channelS = getTransmitChannel(mclmac);
        assert(channelS == channel);
    }

    destroyMCLMAC(&mclmac);
}

void testsetReceptionChannel()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint32_t channel;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        channel = (uint32_t)rand();
        setReceptionChannel(mclmac, channel);
        assert(mclmac->mac->receiveChannel == channel);
    }

    destroyMCLMAC(&mclmac);
}

void testgetReceptionChannel()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint32_t channel, channelS;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        channel = (uint32_t)rand();
        setReceptionChannel(mclmac, channel);
        channelS = getReceptionChannel(mclmac);
        assert(channelS == channel);
    }

    destroyMCLMAC(&mclmac);
}

void testsetAvailableChannels()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t nChannels = (uint8_t) rand();
    uint32_t *channels = (uint32_t *)malloc(nChannels * sizeof(uint32_t));
    int i;
    for (i = 0; i < nChannels; i++)
        channels[i] = (uint32_t)rand();
    
    setAvailableChannels(mclmac, channels, nChannels);
    assert(mclmac->mac->numberChannels == nChannels);
    for (i = 0; i < nChannels; i++)
        assert(mclmac->mac->channels[i] == channels[i]);

    free(channels);

    destroyMCLMAC(&mclmac);
}

void testgetAvailableChannels()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t nChannels = (uint8_t) rand();
    uint32_t *channels = (uint32_t *)malloc(nChannels * sizeof(uint32_t));
    int i;
    for (i = 0; i < nChannels; i++)
        channels[i] = (uint32_t)rand();
    
    setAvailableChannels(mclmac, channels, nChannels);

    uint8_t nChannelsS;
    uint32_t *channelS = (uint32_t *)malloc(nChannels * sizeof(uint32_t));
    getAvailableChannels(mclmac, &channelS, &nChannelsS);

    assert(nChannelsS == nChannels);
    for (i = 0; i < nChannelsS; i++)
        assert(channelS[i] == channels[i]);

    free(channels);
    free(channelS);
    destroyMCLMAC(&mclmac);
}

void testsetNodeIDMCL()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t nodeid;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        nodeid = (uint8_t)rand();
        setNodeIDMCL(mclmac, nodeid);
        assert(mclmac->mac->nodeID == nodeid);
    }

    destroyMCLMAC(&mclmac);
}

void testgetNodeIDMCL()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t nodeid, nodeidR;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        nodeid = (uint8_t)rand();
        setNodeIDMCL(mclmac, nodeid);
        nodeidR = getNodeIDMCL(mclmac);
        assert(nodeidR == nodeid);
    }

    destroyMCLMAC(&mclmac);
}

void testsetSelectedSlot()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t slot;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        slot = (uint8_t) rand() % 256;
        setSelectedSlot(mclmac, slot);
        assert(mclmac->mac->selectedSlot == slot);
    }

    destroyMCLMAC(&mclmac);
}

void testgetSelectedSlot()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t slot, slotR;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        slot = (uint8_t) rand() % 256;
        setSelectedSlot(mclmac, slot);
        slotR = getSelectedSlot(mclmac);
        assert(slotR == slot);
    }

    destroyMCLMAC(&mclmac);
}

void testsetNumberOfHops()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t hops;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        hops = (uint8_t)rand() % 256;
        setNumberOfHops(mclmac, hops);
        assert(mclmac->mac->hopCount == hops);
    }

    destroyMCLMAC(&mclmac);
}

void testgetNumberOfHops()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t hops, hopsR;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        hops = (uint8_t)rand();
        setNumberOfHops(mclmac, hops);
        hopsR = getNumberOfHops(mclmac);
        assert(hopsR == hops);
    }

    destroyMCLMAC(&mclmac);
}

void testsetCurrentFrame()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint32_t frame;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        frame = (uint32_t)rand();
        setCurrentFrame(mclmac, frame);
        assert(mclmac->frame->currentFrame);
    }

    destroyMCLMAC(&mclmac);
}

void testincreaseFrame()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint32_t frame = rand() % ITERATIONS;
    setCurrentFrame(mclmac, frame);

    for (int i = 0; i < 1e6; i++)
    {
        increaseFrame(mclmac);
        frame++;
        assert(mclmac->frame->currentFrame == frame);
    }

    destroyMCLMAC(&mclmac);
}

void testsetCurrentSlot()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t slot;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        slot = (uint8_t)rand() % 256;
        setCurrentSlot(mclmac, slot);
        assert(mclmac->frame->currentSlot == slot);
    }

    destroyMCLMAC(&mclmac);
}

void testincreaseSlot()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t slot = 0;
    setCurrentSlot(mclmac, slot);
    int n = rand() % 512;
    for (int i = 0; i < n; i++)
    {
        increaseSlot(mclmac);
        slot++;
        assert(mclmac->frame->currentSlot == slot);
    }

    destroyMCLMAC(&mclmac);
}

void testsetSlotsNumber()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t nSlots;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        nSlots = (uint8_t) rand();
        setSlotsNumber(mclmac, nSlots);
        assert(mclmac->frame->slotsNumber == nSlots);
    }

    destroyMCLMAC(&mclmac);
}

void testsetCurrentCFSlot()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint8_t nCFslot;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        nCFslot = (uint8_t) rand() % 256;
        setCurrentCFSlot(mclmac, nCFslot);
        assert(mclmac->frame->currentCFSlot == nCFslot);
    }

    destroyMCLMAC(&mclmac);
}

void testincreaseCFSlot()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);
    
    uint8_t nCFSlot = 0;
    setCurrentCFSlot(mclmac, nCFSlot);
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        increaseCFSlot(mclmac);
        nCFSlot++;
        assert(mclmac->frame->currentCFSlot == nCFSlot);
    }

    destroyMCLMAC(&mclmac);
}

void testsetSlotDuration()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
    struct itimerval slotDuration;
    // Use only it_value field
    slotDuration.it_value.tv_usec = rand() % 1000000;
    slotDuration.it_value.tv_sec = 0;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
    uint32_t slotDuration = random();
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        setSlotDuration(mclmac, &slotDuration);
#ifdef __LINUX__
        assert(mclmac->frame->slotDuration.it_value.tv_usec == slotDuration.it_value.tv_usec);
        assert(mclmac->frame->slotDuration.it_value.tv_sec == slotDuration.it_value.tv_sec);
#endif
#ifdef __RIOT__
        assert(mclmac->frame->slotDuration == slotDuration);
#endif
    }

    destroyMCLMAC(&mclmac);
}

void testsetFrameDuration()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
    struct itimerval frameDuration;
    // Use only it_value field
    frameDuration.it_value.tv_usec = rand() % 1000000;
    frameDuration.it_value.tv_sec = rand() % 10;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
    uint32_t frameDuration = random();
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        setFrameDuration(mclmac, &frameDuration);
#ifdef __LINUX__
        assert(mclmac->frame->frameDuration.it_value.tv_usec == frameDuration.it_value.tv_usec);
        assert(mclmac->frame->frameDuration.it_value.tv_sec == frameDuration.it_value.tv_sec);
        frameDuration.it_value.tv_usec = rand() % 1000000;
        frameDuration.it_value.tv_sec = rand() % 10;
#endif
#ifdef __RIOT__
        assert(mclmac->frame->frameDuration == frameDuration);
        frameDuration = random();
#endif
    }
    destroyMCLMAC(&mclmac);
}

void testsetCFDuration()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
    struct itimerval cfDuration;
    // Use only it_value field
    cfDuration.it_value.tv_usec = rand() % 1000000;
    cfDuration.it_value.tv_sec = rand() % 10;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
    uint32_t cfDuration = random();
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        setCFDuration(mclmac, &cfDuration);
#ifdef __LINUX__
        assert(mclmac->frame->cfDuration.it_value.tv_usec == cfDuration.it_value.tv_usec);
        assert(mclmac->frame->cfDuration.it_value.tv_sec == cfDuration.it_value.tv_sec);
        cfDuration.it_value.tv_usec = rand() % 1000000;
        cfDuration.it_value.tv_sec = rand() % 10;
#endif
#ifdef __RIOT__
        assert(mclmac->frame->cfDuration == cfDuration);
        cfDuration = random();
#endif
    }

    destroyMCLMAC(&mclmac);
}

void testrecordCollision()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint32_t freq;
    uint8_t slot;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        freq = (uint32_t)rand();
        slot = (uint8_t)rand();
        recordCollision(mclmac, slot, freq);
        assert(mclmac->_collisionSlot == slot);
        assert(mclmac->_collisionFrequency == freq);
        assert(mclmac->_collisionDetected == true);
    }

    destroyMCLMAC(&mclmac);
}

void testsetDestinationID()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    /* TODO: Change the type of detinationID in CF packet: uint8_t -> uint16_t*/
    uint8_t destinationID;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        destinationID = (uint8_t) rand();
        setDestinationID(mclmac, destinationID);
        assert(mclmac->mac->destinationID == destinationID);
    }

    destroyMCLMAC(&mclmac);
}

void testsetNetworkTime()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint32_t netTime;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        netTime = (uint32_t)rand();
        setNetworkTime(mclmac, netTime);
        assert(mclmac->_networkTime == netTime);
    }

    destroyMCLMAC(&mclmac);
}

void testgetNetworkTime()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    uint32_t netTime, netTimeA;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        netTime = rand();
        setNetworkTime(mclmac, netTime);
        netTimeA = getNetworkTime(mclmac);
        assert(netTimeA == netTime);
    }

    destroyMCLMAC(&mclmac);
}

void testcreateCFPacket()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    createCFPacket(mclmac);
    assert(mclmac->mac->cfpkt != NULL);
    assert(mclmac->mac->cfpkt->nodeID > 0);
    assert(mclmac->mac->cfpkt->destinationID >= 0);

    destroyMCLMAC(&mclmac);
}

void testcreateControlPacket()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    createControlPacket(mclmac);
    assert(mclmac->mac->ctrlpkt != NULL);
    assert(mclmac->mac->ctrlpkt->nodeID == mclmac->mac->nodeID);
    assert(mclmac->mac->ctrlpkt->collisionSlot == mclmac->_collisionSlot);
    assert(mclmac->mac->ctrlpkt->collisionFrequency == mclmac->_collisionFrequency);
    assert(mclmac->mac->ctrlpkt->hopCount == mclmac->mac->hopCount);
    assert(mclmac->mac->ctrlpkt->ack == mclmac->_ack);
    assert(mclmac->mac->ctrlpkt->networkTime == mclmac->_networkTime);

    destroyMCLMAC(&mclmac);
}

void testcreateDataPacket()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    createDataPacket(mclmac);
    assert(mclmac->mac->datapkt != NULL);
    assert(mclmac->mac->datapkt->isFragment == mclmac->_isFragment);
    assert(mclmac->mac->datapkt->totalFragments == mclmac->_numberFragments);
    assert(mclmac->mac->datapkt->fragmentNumber == mclmac->_fragmentNumber);

    destroyMCLMAC(&mclmac);
}

void testsetPacketData()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    createDataPacket(mclmac);

    // The total length of the data packet, including the fields, should not exceed 256 bytes. This as the
    // maximum buffer size for the SX1276 modem is 256 bytes.
    uint8_t size;
   int n = rand() % 1000;
    int j;
    for (int i = 0; i < n; i++)
    {
        size = 1 + rand() % 201;
        uint8_t *data = (uint8_t *)malloc(size * sizeof(uint8_t));
        for (j = 0; j < size; j++)
            data[j] = rand();
        setPacketData(mclmac, data, size);

        assert(mclmac->mac->datapkt->dataLength == size);
        assert(mclmac->mac->datapkt->data != NULL);
        for (j = 0; j < size; j++)
        {
            assert(mclmac->mac->datapkt->data[j] == data[j]);
        }

        deleteDataFromPacket(mclmac);
        free(data);
        data = NULL;
    }

    destroyMCLMAC(&mclmac);
}

void testdeleteDataFromPacket()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    createDataPacket(mclmac);
    uint8_t size = 1 + rand() % 201;
    uint8_t *data = (uint8_t *)malloc(size * sizeof(uint8_t));
    for (int i = 0; i < size; i++)
        data[i] = rand();
    setPacketData(mclmac, data, size);

    deleteDataFromPacket(mclmac);
    assert(mclmac->mac->datapkt->dataLength == 0);
    assert(mclmac->mac->datapkt->data == NULL);

    free(data);
    destroyMCLMAC(&mclmac);
}

void testclearDataFromPacket()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    createDataPacket(mclmac);
    uint8_t size = 1 + rand() % 201;
    uint8_t *data = (uint8_t *)malloc(size * sizeof(uint8_t));
    for (int i = 0; i < size; i++)
        data[i] = rand();
    setPacketData(mclmac, data, size);
    clearDataFromPacket(mclmac);
    assert(mclmac->mac->datapkt->dataLength == size);
    for (int i = 0; i < size; i++)
        assert(mclmac->mac->datapkt->data[i] == 0);

    free(data);
    destroyMCLMAC(&mclmac);
}

void testinitPMStateMachine()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    initPMStateMachine(mclmac);
    assert(mclmac->powerMode.currentState == STARTP);
    assert(mclmac->powerMode.nextState == NONEP);

    destroyMCLMAC(&mclmac);
}

void testupdateStateMachine()
{
    MCLMAC_t *mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    sx127x_t *radio;
#endif
    int dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;
    int r;

    macInit(&mclmac, radio, dataQsize, _nSlots, _nChannels);

    initPMStateMachine(mclmac);

    // Try to transition from STARTP state to ACTIVE, TRANSMIT,  RECEIVE or FINISHP states.
    // Return E_PM_TRANSITION_ERROR
    setPowerModeState(mclmac, STARTP);
    setNextPowerModeState(mclmac, ACTIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(mclmac->powerMode.currentState == STARTP);
    
    setNextPowerModeState(mclmac, TRANSMIT);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(mclmac->powerMode.currentState == STARTP);
    
    setNextPowerModeState(mclmac, RECEIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(mclmac->powerMode.currentState == STARTP);
    
    setNextPowerModeState(mclmac, FINISHP);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(mclmac->powerMode.currentState == STARTP);

    // Try to transition from PASSIVE state to TRANSMIT,  RECEIVE or FINISHP states.
    // Return E_PM_TRANSITION_ERROR
    setPowerModeState(mclmac, PASSIVE);
    setNextPowerModeState(mclmac, TRANSMIT);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(mclmac->powerMode.currentState == PASSIVE);

    setNextPowerModeState(mclmac, RECEIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(mclmac->powerMode.currentState == PASSIVE);

    setNextPowerModeState(mclmac, FINISHP);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(mclmac->powerMode.currentState == PASSIVE);

    // Try to transit from FINISHP to any other state
    // Return E_PM_TRANSITION_ERROR
    setPowerModeState(mclmac, FINISHP);

    setNextPowerModeState(mclmac, PASSIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    setNextPowerModeState(mclmac, ACTIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    setNextPowerModeState(mclmac, TRANSMIT);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    setNextPowerModeState(mclmac, RECEIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // Once in finish state, to use once again the state machine,
    // call initPMStateMachine, and go to PASSIVE state
    initPMStateMachine(mclmac);
    setPowerModeState(mclmac, STARTP);
    setNextPowerModeState(mclmac, PASSIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);

    // Transit from PASSIVE to ACTIVE
    // Return E_PM_TRANSITION_SUCCESS
    setNextPowerModeState(mclmac, ACTIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == ACTIVE);

    // Transit from ACTIVE to PASSIVE
    // Return E_PM_TRANSITION_SUCCESS
    setNextPowerModeState(mclmac, PASSIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == PASSIVE);

    // Transit from PASSIVE to TRANSMIT. Remember to pass through ACTIVE,
    // as specified by the state machine.
    // Return E_PM_TRANSITION_SUCCESS
    setNextPowerModeState(mclmac, ACTIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == ACTIVE);

    setNextPowerModeState(mclmac, TRANSMIT);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == TRANSMIT);

    // Try to get to RECEIVE mode.
    // Return E_PM_TRANSITION_ERROR
    setNextPowerModeState(mclmac, RECEIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(mclmac->powerMode.currentState == TRANSMIT);

    // Try to get to ACTIVE mode.
    // Return E_PM_TRANSITION_ERROR
    setNextPowerModeState(mclmac, ACTIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // You are in TRANSMIT mode, in order to get to RECEIVE mode, 
    // you have to get to PASSIVE again, an travers the path to 
    // RECEIVE state, as indicated by the state machine.
    // Return E_PM_TRANSITION_SUCCESS
    setNextPowerModeState(mclmac, PASSIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == PASSIVE);

    setNextPowerModeState(mclmac, ACTIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == ACTIVE);

    setNextPowerModeState(mclmac, RECEIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == RECEIVE);

    // Try to get to TRANSMIT mode.
    // Return E_PM_TRANSITION_ERROR
    setNextPowerModeState(mclmac, TRANSMIT);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // Try to get to ACTIVE mode
    // Return E_PM_TRANSITION_ERROR
    setNextPowerModeState(mclmac, ACTIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // Follow the path: PASSIVE -> ACTIVE -> FINISHP
    setNextPowerModeState(mclmac, PASSIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == PASSIVE);

    setNextPowerModeState(mclmac, ACTIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == ACTIVE);

    setNextPowerModeState(mclmac, FINISHP);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == FINISHP);

    // Travers the path: PASSIVE -> ACTIVE -> TRANSMIT -> FINISHP
    initPMStateMachine(mclmac);
    setPowerModeState(mclmac, STARTP);
    setNextPowerModeState(mclmac, PASSIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == PASSIVE);

    setNextPowerModeState(mclmac, ACTIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == ACTIVE);

    setNextPowerModeState(mclmac, TRANSMIT);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == TRANSMIT);

    setNextPowerModeState(mclmac, FINISHP);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == FINISHP);

    // Travers the path: PASSIVE -> ACTIVE -> RECEIVE -> FINISHP
    initPMStateMachine(mclmac);
    setPowerModeState(mclmac, STARTP);
    setNextPowerModeState(mclmac, PASSIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == PASSIVE);

    setNextPowerModeState(mclmac, ACTIVE);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == ACTIVE);

    setNextPowerModeState(mclmac, TRANSMIT);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == TRANSMIT);

    setNextPowerModeState(mclmac, FINISHP);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(mclmac->powerMode.currentState == FINISHP);

    // If nextState is NONEP, return E_PM_NO_TRANSITION
    setNextPowerModeState(mclmac, NONEP);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_NO_TRANSITION);

    // If invalid state, return E_PM_INVALID_STATE
    setNextPowerModeState(mclmac, 10);
    r = updatePMStateMachine(mclmac);
    assert(r == E_PM_INVALID_STATE);

    destroyMCLMAC(&mclmac);
}

void executeTestsMCLMAC()
{
    srand(time(NULL));
    printf("Testing macInit function.\n");
    testmacInit();
    printf("Test passed.\n");

    printf("Testing destroyMCLMAC function.\n");
    testdestroyMCLMAC();
    printf("Test passed.\n");

    printf("Test clearMCLMAC function.\n");
    testclearMCLMAC();
    printf("Test passed.\n");

    printf("Testing setMACState function.\n");
    testsetMACState();
    printf("Test passed.\n");

    printf("Test getMACState function.\n");
    testgetMACState();
    printf("Test passed.\n");

    printf("Test setPowerModeState function.\n");
    testsetPowerModeState();
    printf("Test passed.\n");

    printf("Test getPowerModeState function.\n");
    testgetPowerModeState();
    printf("Test passed.\n");

    printf("Test setCFChannel function.\n");
    testsetCFChannel();
    printf("Test passed.\n");

    printf("Test getCFChannel function.\n");
    testgetCFChannel();
    printf("Test passed.\n");

    printf("Test setSelectedChannel function.\n");
    testsetSelectedChannel();
    printf("Test passed.\n");

    printf("Test getSelectedChannel function.\n");
    testgetSelectedChannel();
    printf("Test passed.\n");

    printf("Test setReceptionChannel function.\n");
    testsetReceptionChannel();
    printf("Test passed.\n");

    printf("Test getReceptionChannel function.\n");
    testgetReceptionChannel();
    printf("Test passed.\n");

    printf("Test setAvailableChannels function.\n");
    testsetAvailableChannels();
    printf("Test passed.\n");

    printf("Test getAvailableChannels function.\n");
    testgetAvailableChannels();
    printf("Test passed.\n");

    printf("Testing setNodeIDMCL function.\n");
    testsetNodeIDMCL();
    printf("Test passed.\n");

    printf("Testing getNodeIDMCL function.\n");
    testgetNodeIDMCL();
    printf("Test passed.\n");

    printf("Test setSelectedSlot function.\n");
    testsetSelectedSlot();
    printf("Test passed.\n");

    printf("Testing getSelectedSlot function.\n");
    testgetSelectedSlot();
    printf("Test passed.\n");

    printf("Testing setNumberOfHops function.\n");
    testsetNumberOfHops();
    printf("Test passed.\n");

    printf("Testing getNumberOfHops function.\n");
    testgetNumberOfHops();
    printf("Test passed.\n");

    printf("Testing setCurrentFrame function.\n");
    testsetCurrentFrame();
    printf("Test passed.\n");

    printf("Test increaseFrame function.\n");
    testincreaseFrame();
    printf("Test passed.\n");

    printf("Testing setCurrentSlot function.\n");
    testsetCurrentSlot();
    printf("Test passed.\n");

    printf("Testing increaseSlot function.\n");
    testincreaseSlot();
    printf("Test passed.\n");

    printf("Testing setSlotDuration function.\n");
    testsetSlotDuration();  
    printf("Test passed.\n");

    printf("Testing setFrameDuration function.\n");
    testsetFrameDuration();
    printf("Test passed.\n");

    printf("Testing setCFDuration function.\n");
    testsetCFDuration();
    printf("Test passed.\n");

    printf("Test setSlotsNumber function.\n");
    testsetSlotsNumber();
    printf("Test passed.\n");

    printf("Testing setCurrentCFSlot function.\n");
    testsetCurrentCFSlot();
    printf("Test passed.\n");

    printf("Testing increaseCFSlot function.\n");
    testincreaseCFSlot();
    printf("Test passed.\n");

    printf("Testing recordCollision function.\n");
    testrecordCollision();
    printf("Test passed.\n");

    printf("Testing setDestinationID function.\n");
    testsetDestinationID();
    printf("Test passed.\n");

    printf("Testing setNetworkTime function.\n");
    testsetNetworkTime();
    printf("Test passed.\n");

    printf("Testing getNetworkTime function.\n");
    testgetNetworkTime();
    printf("Test passed.\n");

    printf("Testing createCFPacket function.\n");
    testcreateCFPacket();
    printf("Test passed.\n");

    printf("Testing createControlPacket function.\n");
    testcreateControlPacket();
    printf("Test passed.\n");

    printf("Testing createDataPacket function.\n");
    testcreateDataPacket();
    printf("Test passed.\n");

    printf("Testing setPacketData function.\n");
    testsetPacketData();
    printf("Test passed.\n");

    printf("Testing deleteDataFromPacket function.\n");
    testdeleteDataFromPacket();
    printf("Test passed.\n");

    printf("Testing clearDataFromPacket function.\n");
    testclearDataFromPacket();
    printf("Test passed.\n");

    printf("Testing setNextMACState funtion.\n");
    testsetNextMACState();
    printf("Test passed.\n");

    printf("Testing setNextPowerModeState function.\n");
    testsetNextPowerModeState();
    printf("Test passed.\n");

    printf("Testing initPMStateMachine function.\n");
    testinitPMStateMachine();
    printf("Test passed.\n");

    printf("Test updateStateMachine function.\n");
    testupdateStateMachine();
    printf("Test passed.\n");

    return;
}