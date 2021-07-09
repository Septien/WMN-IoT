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

void initCP(ControlPacket_t **pkt, uint8_t nSlots, uint8_t nChannels);
void destroyPacketCP(ControlPacket_t **pkt);
void createPacketCP(ControlPacket_t *pkt, uint8_t nodeID, uint8_t *occupiedSlots, uint8_t collisionSlots, uint32_t collisionFrequency, uint8_t hopCount, uint8_t ack);
void clearPacktCP(ControlPacket_t *pkt);
void setNodeIDCP(ControlPacket_t *pkt, uint8_t nodeID);
uint8_t getNodeIDCP(ControlPacket_t *pkt);
void setCollisionSlotCP(ControlPacket_t *pkt, uint8_t slot);
uint8_t getCollisionSlotCP(ControlPacket_t *pkt);
void setOccupiedSlotsCP(ControlPacket_t *pkt, uint8_t *occupiedSlots);
void getOccupiedSlotsCP(ControlPacket_t *pkt, uint8_t *occupiedSlots);
void setCollisionFrequencyCP(ControlPacket_t *pkt, uint32_t frequency);
uint32_t getColissionFrequencyCP(ControlPacket_t *pkt);
void setHopCountCP(ControlPacket_t *pkt, uint8_t count);
uint8_t getHopCountCP(ControlPacket_t *pkt);
void setACKCP(ControlPacket_t *pkt, uint8_t ack);
uint8_t getACKCP(ControlPacket_t *pkt);
void getPacketByteStringCP(ControlPacket_t *pkt, uint8_t **byteStr, size_t *size);
void constructPacketFromByteStringCP(ControlPacket_t *pkt, uint8_t *byteString, size_t size);

uint8_t getNumberBytes(uint8_t n);
#endif // CTRLPKT_H