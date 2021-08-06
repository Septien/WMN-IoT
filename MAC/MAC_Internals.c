#include "MAC_Internals.h"
#include <assert.h>
#include <string.h>

void initMACIn(MAC_Internals_t **mac)
{
    *mac = NULL;
    MAC_Internals_t *macA;
    macA = (MAC_Internals_t *)malloc(sizeof(MAC_Internals_t));
    macA->radio = NULL;
    macA->cfpkt = NULL;
    macA->ctrlpkt = NULL;
    macA->datapkt = NULL;
    macA->channels = NULL;
    macA->slots = NULL;
    *mac = macA;
}

void destroyMAC(MAC_Internals_t **mac)
{
    assert(mac != NULL);
    assert(*mac != NULL);

    /* Destroy packets data structures */
    if ((*mac)->ctrlpkt != NULL)
        destroyPacketCP(&(*mac)->ctrlpkt);
    if ((*mac)->cfpkt != NULL)
        destroyPacketCF(&(*mac)->cfpkt);
    if ((*mac)->datapkt != NULL)
        destroyPacketDP(&(*mac)->datapkt);
    
    // Destroy internal arrays
    if ((*mac)->channels != NULL)
        free((*mac)->channels);
    if ((*mac)->slots != NULL)
        free((*mac)->slots);
    free((*mac));
    *mac = NULL;
}

void clearMAC(MAC_Internals_t *mac)
{
    // Keep radio, it is not physically possible to change the radio once deployed.
    assert(mac != NULL);
    //--
    if (mac->channels != NULL)
        free(mac->channels);
    mac->channels = NULL;
    mac->numberChannels = 0;
    if (mac->slots != NULL)
        free(mac->slots);
    mac->slots = NULL;
    mac->numberSlots = 0;
    mac->nodeID = 0;
    mac->transmitChannel = 0;
    mac->cfChannel = 0;
    mac->receiveChannel = 0;
    mac->selectedSlot = 0;
    mac->hopCount = 0;
}
