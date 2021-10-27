#include "MAC_Internals.h"
#include <assert.h>
#include <string.h>

void MAC_internals_init(MAC_Internals_t DOUBLE_POINTER mac)
{
    *mac = NULL;
    MAC_Internals_t *macA;
    macA = (MAC_Internals_t *)malloc(sizeof(MAC_Internals_t));
    memset(macA, 0, sizeof(MAC_Internals_t));
    macA->radio = NULL;
    macA->cfpkt = NULL;
    macA->ctrlpkt = NULL;
    macA->datapkt = NULL;
    macA->channels = NULL;
    macA->slots = NULL;
    // Set to one so other parts of the code (during testing) can run, if not previously initialize
    macA->nodeID = 1;
    *mac = macA;
}

void MAC_internals_destroy(MAC_Internals_t DOUBLE_POINTER mac)
{
    assert(mac != NULL);
    assert(*mac != NULL);

    /* Destroy packets data structures */
    if ((*mac)->ctrlpkt != NULL)
        controlpacket_destroy(&(*mac)->ctrlpkt);
    if ((*mac)->cfpkt != NULL)
        cfpacket_destroy(&(*mac)->cfpkt);
    if ((*mac)->datapkt != NULL)
        datapacket_destroy(&(*mac)->datapkt);
    
    // Destroy internal arrays
    if ((*mac)->channels != NULL)
        free((*mac)->channels);
    if ((*mac)->slots != NULL)
        free((*mac)->slots);
    free((*mac));
    *mac = NULL;
}

void MAC_internals_clear(MAC_Internals_t *mac)
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
