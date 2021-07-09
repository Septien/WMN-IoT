#ifndef CTRLPKT_H
#define CTRLPKT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    uint8_t nodeID;
    uint8_t *occupiedSlots;
    uint8_t collisionSlot;
    uint32_t collisionFrequency;
    uint8_t hopCount;
    uint8_t ack;
    uint8_t _nSlots;
    uint8_t _nChannels;
} ControlPacket_t;

void init(ControlPacket_t **pkt, uint8_t nSlots, uint8_t nChannels);
void destroyPacket(ControlPacket_t **pkt);
void createPacket(ControlPacket_t *pkt, uint8_t nodeID, uint8_t *occupiedSlots, uint8_t collisionSlots, uint32_t collisionFrequency, uint8_t hopCount, uint8_t ack);
void clearPackt(ControlPacket_t *pkt);
void setNodeID(ControlPacket_t *pkt, uint8_t nodeID);
uint8_t getNodeID(ControlPacket_t *pkt);
void setCollisionSlot(ControlPacket_t *pkt, uint8_t slot);
uint8_t getCollisionSlot(ControlPacket_t *pkt);
void setOccupiedSlots(ControlPacket_t *pkt, uint8_t *occupiedSlots);
void getOccupiedSlots(ControlPacket_t *pkt, uint8_t *occupiedSlots);
void setCollisionFrequency(ControlPacket_t *pkt, uint32_t frequency);
uint32_t getColissionFrequency(ControlPacket_t *pkt);
void setHopCount(ControlPacket_t *pkt, uint8_t count);
uint8_t getHopCount(ControlPacket_t *pkt);
void setACK(ControlPacket_t *pkt, uint8_t ack);
uint8_t getACK(ControlPacket_t *pkt);
void getPacketByteString(ControlPacket_t *pkt, uint8_t **byteStr);
void consturctPacketFromByteString(ControlPacket_t *pkt, uint8_t *byteString, size_t size);

uint8_t getNumberBytes(uint8_t n);
#endif // CTRLPKT_H