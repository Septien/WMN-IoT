/**
 * @ingroup     MAC
 * @{
 * @file
 * @brief       This data structure stores all the packets received by upper layers,
 *              for sending to the medium. It contains only the destinatio id, the data
 *              to send (treated as a string of uint8_t) and the data's size.
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

#include "memory_macros.h"
#ifdef __RIOT__
#include "memory.h"
#endif

#include "config_mac.h"

typedef struct {
    int8_t      type;
    uint8_t     size;
    uint16_t    destination_id;
    ARRAY       data;
}DataPacket_t;

/**
 * @brief Initializes the data packet structure. setting the data array to NULL, 
 * the destination id to 0, the size to 0 and the type to -1.
 * 
 * @param pkt 
 */
void datapacket_init(DataPacket_t DOUBLE_POINTER pkt);

/**
 * @brief Destroy the packet en free any used memory.
 * 
 * @param pkt 
 */
void datapacket_destroy(DataPacket_t DOUBLE_POINTER pkt);

/**
 * @brief Creates the data packet with the given input parameters. Initialize the 
 * data array to the give *size* and copies the data to the structure; stores the 
 * type and destination id.
 * 
 * @param pkt 
 * @param type 
 * @param destination_id 
 * @param data 
 * @param size 
 */
void datapacket_create(DataPacket_t *pkt, int8_t type, uint16_t destination_id, ARRAY* data, uint8_t size);

/**
 * @brief Clear all the fields on the packet, and free any used memory. Set type to -1.
 * 
 * @param pkt 
 */
void datapacket_clear(DataPacket_t *pkt);

/**
 * @brief Get the representation of the packet in a string of bytes *byteString* (uint8_t.)
 * 
 * @param pkt 
 * @param byteString 
 */
void datapacket_get_packet_bytestring(DataPacket_t *pkt, ARRAY* byteString);

/**
 * @brief Given a string of bytes *byteString*, construct the packet getting the data
 * from such string. The data is stored on *byteString* as follows: <destination_id, type, data size, data>.
 * 
 * @param pkt 
 * @param byteString 
 */
void datapacket_construct_from_bytestring(DataPacket_t *pkt, ARRAY* byteString);

#endif  // DATAPACKET_H
