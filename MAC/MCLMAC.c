#include "include/MCLMAC.h"
#include <assert.h>
#include <string.h>

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

    initMACIn(&mclmacA->mac, _nSlots, _nChannels);
    mclmacA->_dataQSize = dataQSize;
    mclmacA->dataQueue = (uint8_t *)malloc(mclmacA->_dataQSize * sizeof(uint8_t));
    mclmacA->_collisionDetected = 0; // No collision
    frame->currentSlot = 0;
    frame->currentFrame = 0;
    frame->currentCFSlot = 0;
    frame->cfSlotsNumber = 0;
    frame->slotsNumber = 0;
    mclmacA->frame = frame;
    mclmacA->state = START;
    mclmacA->powerMode = PASSIVE;
    mclmacA->_networkTime = 0;
    *mclmac = mclmacA;
}

void destroyMCLMAC(MCLMAC_t **mclmac)
{
    assert(mclmac != NULL);
    assert(*mclmac != NULL);

    destroyMAC(&(*mclmac)->mac);
    free((*mclmac)->dataQueue);
    free((*mclmac)->frame);
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
    mclmac->state = START;
    mclmac->powerMode    = PASSIVE;
    mclmac->_collisionDetected = 0;
    mclmac->_networkTime = 0;
}

void setMACState(MCLMAC_t *mclmac, state_t state)
{
    assert(mclmac != NULL);
    mclmac->state = state;
}

state_t getMACState(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->state;
}

void setPowerModeState(MCLMAC_t *mclmac, PowerMode_t mode)
{
    assert(mclmac != NULL);
    mclmac->powerMode = mode;
}

PowerMode_t getPowerModeState(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    return mclmac->powerMode;
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

void recordCollision(MCLMAC_t *mclmac, uint8_t collisionSlot, uint32_t collisionFrequency)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->ctrlpkt != NULL);
    assert(collisionFrequency > 0);
    assert(collisionSlot >= 0);

    mclmac->_collisionDetected = true;
    setCollisionSlotCP(mclmac->mac->ctrlpkt, collisionSlot);
    setCollisionFrequencyCP(mclmac->mac->ctrlpkt, collisionFrequency);
}

void setDestinationID(MCLMAC_t *mclmac, uint8_t destinationID)
{
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->cfpkt != NULL);

    setDestinationIDCF(mclmac->mac->cfpkt, destinationID);
}

void startCADMode(MCLMAC_t *mclmac)
{
    // TO DO
    return;
}
