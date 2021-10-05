#include "include/MCLMAC.h"
#include <assert.h>
#include <string.h>

#if 0
void macInit(MCLMAC_t **mclmac, 
#ifdef __LINUX__
    uint8_t *radio,
#endif
#ifdef __RIOT__
    sx127x_t *radio,
#endif
    size_t dataQSize, uint8_t _nSlots, uint8_t _nChannels
)
{
    MCLMAC_t *mclmacA = (MCLMAC_t *)malloc(sizeof(MCLMAC_t));
    Frame_t *frame = (Frame_t *)malloc(sizeof(Frame_t));
    if (mclmacA == NULL)
    {
        return;
    }
    if (frame == NULL)
        return;

    memset(mclmacA, 0, sizeof(MCLMAC_t));
    memset(frame, 0, sizeof(frame));
    initMACIn(&mclmacA->mac);
    mclmacA->_dataQSize = dataQSize;
    mclmacA->dataQueue = (uint8_t *)malloc(mclmacA->_dataQSize * sizeof(uint8_t));
    mclmacA->frame = frame;
    mclmacA->macState.currentState = START;
    mclmacA->powerMode.currentState = STARTP;
    mclmacA->_nChannels = _nChannels;
    mclmacA->_nSlots = _nSlots;

    int bytes = getNumberBytes(_nChannels * _nSlots);
    mclmacA->_occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    memset(mclmacA->_occupiedSlots, 0, bytes);
    *mclmac = mclmacA;
}

void destroyMCLMAC(MCLMAC_t **mclmac)
{
    assert(mclmac != NULL);
    assert(*mclmac != NULL);

    destroyMAC(&(*mclmac)->mac);
    free((*mclmac)->dataQueue);
    free((*mclmac)->frame);
    free((*mclmac)->_occupiedSlots);
    free((*mclmac));
    *mclmac = NULL;
}

void clearMCLMAC(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    
    clearMAC(mclmac->mac);
    mclmac->frame->currentSlot = 0;
    mclmac->frame->currentFrame = 0;
    mclmac->frame->currentCFSlot = 0;
    mclmac->frame->cfSlotsNumber = 0;
    mclmac->frame->slotsNumber = 0;
    mclmac->_dataQSize = 0;
    free(mclmac->dataQueue);
    mclmac->dataQueue = NULL;
    mclmac->macState.currentState = START;
    mclmac->powerMode.currentState = PASSIVE;
    mclmac->_collisionDetected = 0;
    mclmac->_networkTime = 0;
}

void initPMStateMachine(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    mclmac->powerMode.currentState = STARTP;
    mclmac->powerMode.nextState = NONEP;
}

int updatePMStateMachine(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    if (mclmac->powerMode.currentState == mclmac->powerMode.nextState)
        return E_PM_NO_TRANSITION;
 
    switch (mclmac->powerMode.nextState)
    {
    case NONEP:
        setPowerModeState(mclmac, NONEP);
        return E_PM_NO_TRANSITION;
    case PASSIVE:
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        setPowerModeState(mclmac, PASSIVE);
        break;
    
    case ACTIVE:
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == TRANSMIT)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == RECEIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        setPowerModeState(mclmac, ACTIVE);
        break;

    case TRANSMIT:
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == PASSIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == RECEIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        setPowerModeState(mclmac, TRANSMIT);
        break;

    case RECEIVE:
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == PASSIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == TRANSMIT)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        setPowerModeState(mclmac, RECEIVE);
        break;

    case FINISHP:
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == PASSIVE)
            return E_PM_TRANSITION_ERROR;
        setPowerModeState(mclmac, FINISHP);
        break;

    default:
        return E_PM_INVALID_STATE;
        break;
    }
    return E_PM_TRANSITION_SUCCESS;
}

int executePMState(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(mclmac->frame != NULL);
    assert(mclmac->_nSlots > 0);
    #ifdef __LINUX__
    assert(mclmac->frame->frameDuration.it_value.tv_usec > 0);
    assert(mclmac->frame->frameDuration.it_value.tv_sec >= 0);
    assert(mclmac->frame->slotDuration.it_value.tv_usec > 0);
    assert(mclmac->frame->slotDuration.it_value.tv_sec >= 0);
    assert(mclmac->frame->cfDuration.it_value.tv_usec > 0);
    assert(mclmac->frame->cfDuration.it_value.tv_sec >= 0);
#endif
#ifdef __RIOT__
    assert(mclmac->frame->frameDuration > 0);
    assert(mclmac->frame->slotDuration > 0);
    assert(mclmac->frame->cfDuration > 0);
#endif

    if (mclmac->powerMode.currentState == NONEP)
        return E_PM_EXECUTION_FAILED;
    
    switch (mclmac->powerMode.currentState)
    {
    case STARTP:
        // Set the number of slots and cfslots
        setSlotsNumber(mclmac, mclmac->_nSlots);
        setCFSlotsNumber(mclmac, mclmac->_nSlots);

        // Set the value of the timers
        setFrameDuration(mclmac, &mclmac->_frameDuration);
        setSlotDuration(mclmac, &mclmac->_slotDuration);
        setCFDuration(mclmac, &mclmac->_cfDuration);

        // Initialize frame, slots, and cfslots counter to zero
        mclmac->frame->currentFrame = 0;
        mclmac->frame->currentSlot = 0;
        mclmac->frame->currentCFSlot = 0;

        // Pass immediatly to PASSIVE state
        setNextPowerModeState(mclmac, PASSIVE);
#ifdef __LINUX__
        // Set timer for active state
        signal(SIGALRM, _slotCallback);
        setitimer(ITIMER_REAL, &mclmac->frame->slotDuration, NULL);
#endif
#ifdef __RIOT__
        mclmac->frame->slotTimer = { .callback=_slotCallback, .arg=(void *)mclmac};
        ztimer_set(ZTIMER_MSEC, &mclmac->frame->slotTimer, mclmac->frame->slotDuration);
#endif
        break;
    
    case PASSIVE:
        // Reset the cf counter to zero
        setCurrentCFSlot(mclmac, 0);
#ifdef __LINUX__
        /* Set to sleep */
#endif
#ifdef __RIOT__
        sx127x_set_sleep(mclmac->mac->modem);
#endif
        setNextPowerModeState(mclmac, NONEP);
        break;
    
    case ACTIVE:
        createCFPacket(mclmac);
#ifdef __LINUX__
        // change radio to common channel
#endif
#ifdef __RIOT__
        sx127x_set_channel(mclmac->mac->radio, mclmac->mac->cfChannel);
        // sleep for 20 us while the radio changes channel (according to datasheet)
        ztimer_sleep(ZTIME_USEC, 20);
#endif
        
        break;
    default:
        break;
    }

    return E_PM_EXECUTION_SUCCESS;
}

void setMACState(MCLMAC_t *mclmac, state_t state)
{
    assert(mclmac != NULL);
    assert(state != NONE);
    mclmac->macState.currentState = state;
}

void setNextMACState(MCLMAC_t *mclmac, state_t next)
{
    assert(mclmac != NULL);
    assert(next != NONE);
    assert(next != START);

    mclmac->macState.nextState = next;
}

state_t getMACState(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->macState.currentState;
}

void setPowerModeState(MCLMAC_t *mclmac, PowerMode_t mode)
{
    assert(mclmac != NULL);
 
    mclmac->powerMode.currentState = mode;
}

void setNextPowerModeState(MCLMAC_t *mclmac, PowerMode_t next)
{
    assert(mclmac != NULL);
    assert(next != STARTP);

    mclmac->powerMode.nextState = next;
}

PowerMode_t getPowerModeState(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    return mclmac->powerMode.currentState;
}

void setCFChannel(MCLMAC_t *mclmac, uint32_t cfchannel)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);

    mclmac->mac->cfChannel = cfchannel;
}

uint32_t getCFChannel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);

    return mclmac->mac->cfChannel;
}

void setTransmitChannel(MCLMAC_t *mclmac, uint32_t channel)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);

    mclmac->mac->transmitChannel = channel;
}

uint32_t getTransmitChannel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);

    return mclmac->mac->transmitChannel;
}

void setReceptionChannel(MCLMAC_t *mclmac, uint32_t rChannel)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);

    mclmac->mac->receiveChannel = rChannel;
}

uint32_t getReceptionChannel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);

    return mclmac->mac->receiveChannel;
}

void setAvailableChannels(MCLMAC_t *mclmac, uint32_t *channels, uint8_t nChannels)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(channels != NULL);
    assert(nChannels > 0);

    mclmac->mac->numberChannels = nChannels;
    mclmac->mac->channels = (uint32_t *)malloc(mclmac->mac->numberChannels * sizeof(uint32_t));
    for (int i = 0; i < mclmac->mac->numberChannels; i++)
    {
        mclmac->mac->channels[i] = channels[i];
    }
}

void getAvailableChannels(MCLMAC_t *mclmac, uint32_t **channels, uint8_t *nChannels)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->channels != NULL);
    assert(channels != NULL);
    assert(*channels != NULL);
    assert(nChannels != NULL);

    *nChannels = mclmac->mac->numberChannels;
    for (int i = 0; i < *nChannels; i++)
        (*channels)[i] = mclmac->mac->channels[i];
}

void setNodeIDMCL(MCLMAC_t *mclmac, uint8_t id)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(id >= 0);

    mclmac->mac->nodeID = id;
}

uint8_t getNodeIDMCL(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);

    return mclmac->mac->nodeID;
}

void setSelectedSlot(MCLMAC_t *mclmac, uint8_t selectedSlot)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(selectedSlot >= 0);

    mclmac->mac->selectedSlot = selectedSlot;

}

uint8_t getSelectedSlot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    
    return mclmac->mac->selectedSlot;
}

void setNumberOfHops(MCLMAC_t *mclmac, uint8_t hops)
{
    assert(mclmac != NULL),
    assert(mclmac->mac != NULL);
    assert(hops >= 0);

    mclmac->mac->hopCount = hops;
}

uint8_t getNumberOfHops(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);

    return mclmac->mac->hopCount;
}

void setCurrentFrame(MCLMAC_t *mclmac, uint32_t frame)
{
    assert(mclmac != NULL);
    assert(mclmac->frame != NULL);

    mclmac->frame->currentFrame = frame;
}

void increaseFrame(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->frame != NULL);
    assert(mclmac->frame->currentFrame >= 0);

    mclmac->frame->currentFrame++;
}

void setCurrentSlot(MCLMAC_t *mclmac, uint8_t slot)
{
    assert(mclmac != NULL);
    assert(mclmac->frame != NULL);

    mclmac->frame->currentSlot = slot;
}

void increaseSlot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->frame != NULL);

    mclmac->frame->currentSlot++;
}

void setSlotsNumber(MCLMAC_t *mclmac, uint8_t nSlots)
{
    assert(mclmac != NULL);
    assert(mclmac->frame != NULL);

    mclmac->frame->slotsNumber = nSlots;
}

void setCFSlotsNumber(MCLMAC_t *mclmac, uint8_t nSlots)
{
    assert(mclmac != NULL);
    assert(mclmac->frame != NULL);
    assert(nSlots > 0);

    mclmac->frame->cfSlotsNumber = nSlots;
}

void setCurrentCFSlot(MCLMAC_t *mclmac, uint8_t nCFSlot)
{
    assert(mclmac != NULL);
    assert(mclmac->frame != NULL);

    mclmac->frame->currentCFSlot = nCFSlot;
}

void increaseCFSlot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->frame != NULL);
    assert(mclmac->frame->currentCFSlot >= 0);

    mclmac->frame->currentCFSlot++;
}

void setSlotDuration(MCLMAC_t *mclmac,
#ifdef __LINUX__
struct itimerval *slotDuration
#endif
#ifdef __RIOT__
uint32_t *slotDuration
#endif
)
{
    assert(mclmac != NULL);
    assert(mclmac->frame != NULL);
    assert(slotDuration != NULL);
#ifdef __LINUX__
    assert((*slotDuration).it_value.tv_usec > 0);
    assert((*slotDuration).it_value.tv_sec >= 0);
    mclmac->frame->slotDuration.it_value.tv_usec = (*slotDuration).it_value.tv_usec;
    mclmac->frame->slotDuration.it_value.tv_sec = (*slotDuration).it_value.tv_sec;
#endif
#ifdef __RIOT__
    assert(slotDuration > 0);
    mclmac->frame->slotDuration = *slotDuration:
#endif
}

void setFrameDuration(MCLMAC_t *mclmac,
#ifdef __LINUX__
struct itimerval *frameDuration
#endif
#ifdef __RIOT__
uint32_t *frameDuration
#endif
)
{
    assert(mclmac != NULL);
    assert(mclmac->frame != NULL);
    assert(frameDuration != NULL);
#ifdef __LINUX__
    assert((*frameDuration).it_value.tv_usec > 0);
    assert((*frameDuration).it_value.tv_usec >= 0);
    mclmac->frame->frameDuration.it_value.tv_usec = (*frameDuration).it_value.tv_usec;
    mclmac->frame->frameDuration.it_value.tv_sec = (*frameDuration).it_value.tv_sec;
#endif
#ifdef __RIOT__
    assert(frameDuration > 0);
    mclmac->frame->frameDuration = *frameDuration;
#endif
}

void setCFDuration(MCLMAC_t *mclmac,
#ifdef __LINUX__
struct itimerval *cfDuration
#endif
#ifdef __RIOT__
uint32_t *cFDuration 
#endif
)
{
    assert(mclmac != NULL);
    assert(mclmac->frame != NULL);
    assert(cfDuration != NULL);
#ifdef __LINUX__
    assert((*cfDuration).it_value.tv_usec > 0);
    assert((*cfDuration).it_value.tv_sec >= 0);
    mclmac->frame->cfDuration.it_value.tv_usec = (*cfDuration).it_value.tv_usec;
    mclmac->frame->cfDuration.it_value.tv_sec = (*cfDuration).it_value.tv_sec;
#endif
#ifdef __RIOT__
    assert((*cfDuration) > 0);
    mclmac->frame->cfDuration = *cfDuration;
#endif
}

void recordCollision(MCLMAC_t *mclmac, uint8_t collisionSlot, uint32_t collisionFrequency)
{
    assert(mclmac != NULL);
    assert(collisionFrequency > 0);
    assert(collisionSlot >= 0);

    mclmac->_collisionDetected = true;
    mclmac->_collisionSlot = collisionSlot;
    mclmac->_collisionFrequency = collisionFrequency;
}

void setDestinationID(MCLMAC_t *mclmac, uint8_t destinationID)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);

    mclmac->mac->destinationID = destinationID;
}

void setNetworkTime(MCLMAC_t *mclmac, uint32_t time)
{
    assert(mclmac != NULL);
    assert(time >= 0);

    mclmac->_networkTime = time;
}

uint32_t getNetworkTime(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->_networkTime;
}

void createCFPacket(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->nodeID > 0);
    assert(mclmac->mac->destinationID >= 0);

    if (mclmac->mac->cfpkt != NULL)
    {
        free(mclmac->mac->cfpkt);
        mclmac->mac->cfpkt = NULL;
    }

    CFPacket_t *cfA;
    initCF(&cfA);
    createPacketCF(cfA, mclmac->mac->nodeID, mclmac->mac->destinationID, mclmac->_networkTime);
    mclmac->mac->cfpkt = cfA;
}

void createControlPacket(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);

    if (mclmac->mac->ctrlpkt != NULL)
    {
        free(mclmac->mac->cfpkt);
        mclmac->mac->ctrlpkt = NULL;
    }


    ControlPacket_t *ctrlpkt;
    initCP(&ctrlpkt, mclmac->_nSlots, mclmac->_nChannels);
    createPacketCP(ctrlpkt, mclmac->mac->nodeID, mclmac->_occupiedSlots, mclmac->_collisionSlot, 
                    mclmac->_collisionFrequency, mclmac->mac->hopCount, mclmac->_networkTime, mclmac->_ack);
    mclmac->mac->ctrlpkt = ctrlpkt;
}

void createDataPacket(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);

    if (mclmac->mac->datapkt != NULL)
    {
        free(mclmac->mac->datapkt);
        mclmac->mac->datapkt = NULL;
    }


    DataPacket_t *datapkt;
    initDP(&datapkt);
    createPacketDP(datapkt, mclmac->_isFragment, mclmac->_numberFragments, mclmac->_fragmentNumber, NULL, 0);
    mclmac->mac->datapkt = datapkt;
}

void setPacketData(MCLMAC_t *mclmac, const uint8_t *data, uint8_t size)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->datapkt != NULL);

    // If size is zero and data array is already initialized, set to zero
    if (size == 0 && mclmac->mac->datapkt->data != NULL)
    {
        memset(mclmac->mac->datapkt->data, 0, mclmac->mac->datapkt->dataLength);
        return;
    }

    mclmac->mac->datapkt->dataLength = size;
    mclmac->mac->datapkt->data = (uint8_t *)malloc(size * sizeof(uint8_t));
    for (int i = 0; i < size; i++)
    {
        mclmac->mac->datapkt->data[i] = data[i];
    }
}

void deleteDataFromPacket(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->datapkt != NULL);

    deleteDataDP(mclmac->mac->datapkt);
}

void clearDataFromPacket(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->datapkt != NULL);

    clearDataDP(mclmac->mac->datapkt);
}

void startCADMode(MCLMAC_t *mclmac)
{
    // TO DO
    return;
}

void _slotCallback(
#ifdef __LINUX__
    int signum
#endif
#ifdef __RIOT__
    void *args
#endif
)
{
#ifdef __LINUX__
    if (signum == SIGALRM)
    {
        slotalarm = 1;
    }
#endif
#ifdef __RIOT__
    MCLMAC_t *mclmac = (MCLMAC_t *)args;
    setNextPowerModeState(mclmac, ACTIVE);
#endif
}
#endif
