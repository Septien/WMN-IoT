#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "../include/MCLMAC.h"

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
    assert(mclmac->powerMode == PASSIVE);
    assert(mclmac->state == START);
    assert(mclmac->_dataQSize == dataQsize);
    assert(mclmac->_collisionDetected == 0);
    assert(mclmac->_networkTime == 0);

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
    assert(mclmac->powerMode == PASSIVE);
    assert(mclmac->state == START);
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
    assert(mclmac->state == state);
    state = INITIALIZATION;
    setMACState(mclmac, state);
    assert(mclmac->state == state);
    state = SYNCHRONIZATION;
    setMACState(mclmac, state);
    assert(mclmac->state == state);
    state = DISCOVERY_AND_SELECTION;
    setMACState(mclmac, state);
    assert(mclmac->state == state);
    state = MEDIUMACCESS;
    setMACState(mclmac, state);
    assert(mclmac->state == state);

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
    assert(mclmac->powerMode == mode);

    mode = ACTIVE;
    setPowerModeState(mclmac, mode);
    assert(mclmac->powerMode == mode);

    mode = TRANSMIT;
    setPowerModeState(mclmac, mode);
    assert(mclmac->powerMode == mode);

    mode = RECEIVE;
    setPowerModeState(mclmac, mode);
    assert(mclmac->powerMode == mode);

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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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

    uint32_t frame = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
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
    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        increaseCFSlot(mclmac);
        nCFSlot++;
        assert(mclmac->frame->currentCFSlot == nCFSlot);
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
    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        freq = (uint32_t)rand();
        slot = (uint8_t)rand();
        recordCollision(mclmac, slot, freq);
        assert(mclmac->mac->ctrlpkt->collisionSlot == slot);
        assert(mclmac->mac->ctrlpkt->collisionFrequency == freq);
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
    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        destinationID = (uint8_t) rand();
        setDestinationID(mclmac, destinationID);
        assert(mclmac->mac->cfpkt->destinationID == destinationID);
    }

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

    return;
}