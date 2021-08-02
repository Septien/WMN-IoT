/**
 * @ingroup     MAC
 * @{
 * @file
 * @brief       API provided for using the MAC protocol along with all its provided services.
 * 
 * @author      José Antonio Septién Hernández.
 * @}
 */
#ifndef MCLMAC_H
#define MCLMAC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "../Frame.h"
#include "../MAC_Internals.h"

#ifdef __LINUX__
#include <unistd.h>
#include <signal.h>
#include <time.h>
#endif

#ifdef __RIOT__
#include "config.h"
#include "periodic.h"
#include "ztimer.h"
#include "sx127x.h"
#include "sx127x_internals.h"
#include "sx127x_netdev.h"
#include "sx127x_registers.h"
#endif

typedef enum STATE {START, INITIALIZATION, SYNCHRONIZATION, DISCOVERY_AND_SELECTION, MEDIUMACCESS} state_t;

typedef enum POWERMODE {PASSIVE, ACTIVE, TRANSMIT, RECEIVE} PowerMode_t;

typedef struct MCLMAC
{
    // Internals to the MAC
    MAC_Internals_t *mac;
    // For communicating with other layers
    uint8_t *dataQueue;
    Frame_t *frame;
    state_t state;
    PowerMode_t powerMode;

    // Private members
    bool _collisionDetected;
    uint32_t _networkTime;
    size_t _dataQSize;
}MCLMAC_t;

void macInit(MCLMAC_t **mclmac, 
#ifdef __LINUX__
    uint8_t *radio,
#endif
#ifdef __RIOT__
    sx127x_t *radio,
#endif
    size_t dataQSize, uint8_t _nSlots, uint8_t _nChannels
);
void destroyMCLMAC(MCLMAC_t **mclmac);
void clearMCLMAC(MCLMAC_t *mclmac);

// State machines
void setMACState(MCLMAC_t *mclmac, state_t state);
state_t getMACState(MCLMAC_t *mclmac);
void setPowerModeState(MCLMAC_t *mclmac, PowerMode_t mode);
PowerMode_t getPowerModeState(MCLMAC_t *mclmac);

// Channels
void setCFChannel(MCLMAC_t *mclmac, uint32_t cfChannel);
uint32_t getCFChannel(MCLMAC_t *mclmac);
void setTransmitChannel(MCLMAC_t *mclmac, uint32_t channel);
uint32_t getTransmitChannel(MCLMAC_t *mclmac);
void setReceptionChannel(MCLMAC_t *mclmac, uint32_t rChannel);
uint32_t getReceptionChannel(MCLMAC_t *mclmac);
void setAvailableChannels(MCLMAC_t *mclmac, uint32_t *channels, uint8_t nChannels);
void getAvailableChannels(MCLMAC_t *mclmac, uint32_t **channels, uint8_t *nChannels);

// Channel selection
void changeToTransmitChannel(MCLMAC_t *mclmac);
void changeToReceiveChannel(MCLMAC_t *mclmac);
void chanteToCFChannel(MCLMAC_t *mclmac);

// Radio modes
void startCADMode(MCLMAC_t *mclmac);
void startSplitPhase(MCLMAC_t *mclmac);
void startCFPhase(MCLMAC_t *mclmac);
bool CADDetected(MCLMAC_t *mclmac);

// Properties and configuration
void setNodeID(MCLMAC_t *mclmac, uint8_t id);
uint8_t getNodeID(MCLMAC_t *mclmac);
void setSelectedSlot(MCLMAC_t *mclmac, uint8_t selectedSlot);
uint8_t getSelectedSlot(MCLMAC_t *mclmac);
void setNumberOfHops(MCLMAC_t *mclmac, uint8_t hops);
uint8_t getNumberOfHops(MCLMAC_t *mclmac);
void computeFreeSlotsAndChannels(MCLMAC_t *mclmac);
void setCurrentFrame(MCLMAC_t *mclmac, uint8_t frame);
void increaseFrame(MCLMAC_t *mclmac);
void setCurrentSlot(MCLMAC_t *mclmac, uint8_t slot);
void increaseSlot(MCLMAC_t *mclmac);
void setSlotNumbers(MCLMAC_t *mclmac, uint8_t nSlots);
void setCurrentCFSlot(MCLMAC_t *mclmac, uint8_t nCFSlot);
void increaseCFSlot(MCLMAC_t *mclmac);
void setSlotDuration(MCLMAC_t *mclmac,
#ifdef __LINUX__
struct itimerval slotDuration
#endif
#ifdef __RIOT__
uint32_t slotDuration
#endif
);
void setFrameDuration(MCLMAC_t *mclmac,
#ifdef __LINUX__
struct itimerval frameDuration
#endif
#ifdef __RIOT__
uint32_t frameDuration
#endif
);
void setCFDuration(MCLMAC_t *mclmac,
#ifdef __LINUX__
struct itimerval cfDuration
#endif
#ifdef __RIOT__
uint32_t setCFDuration 
#endif
);
void recordCollision(MCLMAC_t *mclmac, uint8_t collisionSlot, uint32_t collisionFrequency);
void setDestinationID(MCLMAC_t *mclmac, uint8_t id);
uint8_t getDestinationID(MCLMAC_t *mclmac);
void setNetworkTime(MCLMAC_t *mclmac, uint32_t time);
uint32_t getNetworkTime(MCLMAC_t *mclmac);

// Packet functions
void createControlPacket(MCLMAC_t *mclmac);
void createDataPacket(MCLMAC_t *mclmac);
void setPacketData(MCLMAC_t *mclmac, uint8_t *data, uint8_t size);
void sendControlPacket(MCLMAC_t *mclmac);
void sendCFPacket(MCLMAC_t *mclmac);
void sendDataPacket(MCLMAC_t *mclmac);
void getControlMessage(MCLMAC_t *mclmac);
void getCFMessage(MCLMAC_t *mclmac);
void getDataMessage(MCLMAC_t *mclmac);


/* Private functions */
void _selectSlotAndChannel(MCLMAC_t *mclmac);


#endif  // MCLMAC_H