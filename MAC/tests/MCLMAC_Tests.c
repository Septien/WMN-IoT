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

    return;
}