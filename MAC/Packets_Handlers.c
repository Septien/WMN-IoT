/* Implement all the handles for the different packets. */
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

/*void mclmac_create_cf_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif
    assert(ARROW(mclmac->mac)nodeID > 0);

#ifdef __LINUX__
    if (mclmac->mac->cfpkt != NULL)
        cfpacket_destroy(&mclmac->mac->cfpkt);
#endif
#ifdef __RIOT__
    cfpacket_destroy(&mclmac->mac.cfpkt);
#endif

    cfpacket_init(&ARROW(mclmac->mac)cfpkt);
    cfpacket_create(REFERENCE ARROW(mclmac->mac)cfpkt, ARROW(mclmac->mac)nodeID, ARROW(mclmac->mac)destinationID);
}

void mclmac_create_control_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);

    if (mclmac->mac->ctrlpkt != NULL)
        controlpacket_destroy(&mclmac->mac->ctrlpkt);
#endif
#ifdef __RIOT__
    controlpacket_destroy(&mclmac->mac.ctrlpkt);
#endif

    controlpacket_init(&ARROW(mclmac->mac)ctrlpkt, mclmac->_nSlots, mclmac->_nChannels);
    controlpacket_create(REFERENCE ARROW(mclmac->mac)ctrlpkt, ARROW(mclmac->mac)nodeID, &mclmac->_occupiedSlots, mclmac->_collisionSlot, 
                    mclmac->_collisionFrequency, ARROW(mclmac->mac)hopCount, mclmac->_networkTime, mclmac->_ack);
}

void mclmac_create_data_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);

    if (mclmac->mac->datapkt != NULL)
        datapacket_destroy(&mclmac->mac->datapkt);
#endif
#ifdef __RIOT__
    datapacket_destroy(&mclmac->mac.datapkt);
#endif

    datapacket_init(&ARROW(mclmac->mac)datapkt);
    datapacket_create(REFERENCE ARROW(mclmac->mac)datapkt, mclmac->_isFragment, mclmac->_numberFragments, mclmac->_fragmentNumber, NULL, 0);
}

void mclmac_clear_cf_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->cfpkt != NULL);
#endif

    cfpacket_clear(REFERENCE ARROW(mclmac->mac)cfpkt);
}*/

void mclmac_set_packet_data(MCLMAC_t *mclmac, ARRAY* data, uint8_t size)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->datapkt != NULL);
    assert(mclmac->mac->datapkt->data == NULL);
#endif
#ifdef __RIOT__
    assert(mclmac->mac.datapkt.data.size == 0);
#endif
    assert(size > 0);
    
    datapacket_set_data(REFERENCE ARROW(mclmac->mac)datapkt, data, size);
}

void mclmac_delete_data_from_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->datapkt != NULL);
#endif

    datapacket_delete_data(REFERENCE ARROW(mclmac->mac)datapkt);
}

void mclmac_clear_data_from_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->datapkt != NULL);
#endif

    datapacket_clear_data(REFERENCE ARROW(mclmac->mac)datapkt);
}

/**
 * @brief For the SYNCHRONIZATION state. It will receive all the incoming 
 *      control packets from the network, one at the time, and store the received
 *      data on the ctrlpkt. With that data, the node will fill its information
 *      about the network, so it can join it.
 *      For simulate the reception of packets, this function will create 
 *      an byte string that will contain the associated data, and create 
 *      the packet with it. The following static variables are necessary:
 *          -A variable containing the network time, which is the total number
 *          of slots that has passed.
 *          -The number of hops, which will vary a little in each call.
 *          -The current slot and current frame, which will be random at the beginning.
 *          -We also need to record the slot and frequency at which the packet is received.
 *      The rest of the fields will be randomly generated.
 * @param mclmac 
 * @param ctrlpkt 
 */
void stub_mclmac_receive_ctrlpkt_sync(MCLMAC_t *mclmac, ControlPacket_t *ctrlpkt, uint32_t *frequency)
{
    assert(mclmac != NULL);
    assert(ctrlpkt != NULL);
    assert(frequency != NULL);

    static uint32_t frame = 0;
    static uint8_t slot = 0;
    static uint8_t hopCount = 100;
    static uint32_t network_time = 220;
    
    *frequency = rand() % MAX_NUMBER_FREQS;
    controlpacket_set_current_frame(ctrlpkt, frame);
    controlpacket_set_current_slot(ctrlpkt, slot);
    controlpacket_set_hop_count(ctrlpkt, hopCount);
    controlpacket_set_network_time(ctrlpkt, network_time);

    slot++;
    network_time++;
    if ((slot % MAX_NUMBER_SLOTS) == 0)
    {
        slot = 0;
        frame++;
    }
}
