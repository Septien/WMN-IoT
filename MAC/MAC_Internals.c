#include "MAC_Internals.h"
#include <assert.h>
#include <string.h>

void MAC_internals_init(MAC_Internals_t DOUBLE_POINTER mac)
{
#ifdef __LINUX__
    *mac = (MAC_Internals_t *)malloc(sizeof(MAC_Internals_t));
#endif
    memset(SINGLE_POINTER mac, 0, sizeof(MAC_Internals_t));
#ifdef __LINUX__
    (*mac)->radio = NULL;
    (*mac)->cfpkt = NULL;
    (*mac)->ctrlpkt = NULL;
    (*mac)->datapkt = NULL;
    (*mac)->channels = NULL;
    (*mac)->slots = NULL;
#endif
    // Set to one so other parts of the code (during testing) can run, if not previously initialize
    (SINGLE_POINTER mac)->nodeID = 1;
}

void MAC_internals_destroy(MAC_Internals_t DOUBLE_POINTER mac)
{
    assert(mac != NULL);
#ifdef __LINUX__
    assert(*mac != NULL);

    /* Destroy packets data structures */
    if ((SINGLE_POINTER mac)->ctrlpkt != NULL)
        controlpacket_destroy(&(*mac)->ctrlpkt);
    if ((SINGLE_POINTER mac)->cfpkt != NULL)
        cfpacket_destroy(&(*mac)->cfpkt);
    if ((SINGLE_POINTER mac)->datapkt != NULL)
        datapacket_destroy(&(*mac)->datapkt);
    // Destroy internal arrays
    if ((*mac)->channels != NULL)
        free((*mac)->channels);
    if ((*mac)->slots != NULL)
        free((*mac)->slots);
    free((*mac));
    *mac = NULL;
#endif
#ifdef __RIOT__
    controlpacket_destroy(&mac->ctrlpkt);
    cfpacket_destroy(&mac->cfpkt);
    datapacket_destroy(&mac->datapkt);
    if (mac->channels.size > 0)
        free_array(&mac->channels);
    if (mac->slots.size > 0)
        free_array(&mac->slots);
#endif
}

void MAC_internals_clear(MAC_Internals_t *mac)
{
    // Keep radio, it is not physically possible to change the radio once deployed.
    assert(mac != NULL);
    //--
#ifdef __LINUX__
    if (mac->channels != NULL)
        free(mac->channels);
    mac->channels = NULL;
    mac->numberChannels = 0;
    if (mac->slots != NULL)
        free(mac->slots);
    mac->slots = NULL;
#endif
#ifdef __RIOT__
    if (mac->channels.size > 0)
        free_array(&mac->channels);
    if (mac->slots.size > 0)
        free_array(&mac->slots);
#endif
    memset(mac, 0, sizeof(MAC_Internals_t));
}
