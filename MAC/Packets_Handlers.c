/* Implement all the handles for the different packets. */
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

void mclmac_create_cf_packet(MCLMAC_t *mclmac)
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
    return;
}

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
