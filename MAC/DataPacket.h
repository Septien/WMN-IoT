/**
 * @ingroup     MAC
 * @{
 * @file
 * @brief       The data packet implementation required by the selected MAC protocol
 * 
 * @author      José Antonio Septién Hernández.
 * @}
 */
#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    bool     isFragment;
    uint8_t  totalFragments;
    uint8_t  fragmentNumber;
    size_t   packetLength;
    uint8_t   dataLength;
    uint8_t  *data;
    uint16_t crc;
}DataPacket_t;

void initDP(DataPacket_t **pkt);
void destroyPacketDP(DataPacket_t **pk);
void createPacketDP(DataPacket_t *pkt, bool isFragment, uint8_t totalFragments, uint8_t fragmentNumber, void **data, size_t size);
void clearPacketDP(DataPacket_t *pkt);
void setIsFragmentDP(DataPacket_t *pkt, bool isFragment);
bool isFragmentDP(DataPacket_t *pkt);
void setTotalFragmentsDP(DataPacket_t *pkt, uint8_t totalFragments);
uint8_t getTotalFragmentsDP(DataPacket_t *pkt);
void setFragmentNumberDP(DataPacket_t *pkt, uint8_t fragmentNumber);
uint8_t getFragmentNumberDP(DataPacket_t *pkt);
void setPacketDataDP(DataPacket_t *pkt, void **data, uint8_t size);
void getPacketDataDP(DataPacket_t *pkt, void **data, uint8_t *size);
uint8_t getDataLengthDP(DataPacket_t *pkt);
size_t getPacketLengthDP(DataPacket_t *pkt);
void getPacketByteStringDP(DataPacket_t *pkt, uint8_t **byteString, size_t *size);
void constructPktFromByteStringDP(DataPacket_t *pkt, uint8_t *byteString, size_t size);

#endif  // DATAPACKET_H