/**
 * @ingroup     MAC
 * @{
 * @file
 * @brief       The CFs packet implementation required by the selected MAC protocol
 * 
 * @author      José Antonio Septién Hernández.
 * @}
 */
#ifndef CFPKT_H
#define CFPKT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct CFPacket
{
    uint8_t nodeID;
    uint8_t destinationID;
}CFPacket_t;

void initCF(CFPacket_t **pkt);
void destroyPacketCF(CFPacket_t **pkt);
void createPacketCF(CFPacket_t *pkt, uint8_t nodeID, uint8_t destinationID);
void clearPacketCF(CFPacket_t *pkt);
void setNodeIDCF(CFPacket_t *pkt, uint8_t nodeID);
uint8_t getNodeIDCF(CFPacket_t *pkt);
void setDestinationIDCF(CFPacket_t *pkt, uint8_t destinationID);
uint8_t getDestinationIDCF(CFPacket_t *pkt);
void getPacketByteStringCF(CFPacket_t *pkt, uint8_t **byteString, size_t *size);
void constructPktFromByteStringCF(CFPacket_t *pkt, uint8_t *byteString, size_t size);

#endif // CFPKT_H