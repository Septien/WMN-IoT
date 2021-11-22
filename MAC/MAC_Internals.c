#include "MAC_Internals.h"
#include <assert.h>
#include <string.h>

void MAC_internals_init(MAC_Internals_t DOUBLE_POINTER mac,
#ifdef __LINUX__
    uint8_t *radio
#endif
#ifdef __RIOT__
    sx127x_t *radio
#endif
)
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
    (*mac)->frame = (Frame_t *)malloc(sizeof(Frame_t));
    if ((*mac)->frame == NULL)
        return;
#endif
    memset(REFERENCE (SINGLE_POINTER mac)->frame, 0, sizeof(Frame_t));

    (SINGLE_POINTER mac)->radio = radio;
    (SINGLE_POINTER mac)->_max_number_packets_buffer = 5 * 256;      // Maximum 5 packets of siza 256 each
    (SINGLE_POINTER mac)->_max_cf_messages = 5 * 2 * sizeof(uint16_t);
}

void MAC_internals_destroy(MAC_Internals_t DOUBLE_POINTER mac)
{
    assert(mac != NULL);
#ifdef __LINUX__
    assert(*mac != NULL);

    /* Destroy packets data structures */
    if ((*mac)->ctrlpkt != NULL)
        controlpacket_destroy(&(*mac)->ctrlpkt);
    if ((*mac)->cfpkt != NULL)
        cfpacket_destroy(&(*mac)->cfpkt);
    if ((*mac)->datapkt != NULL)
        datapacket_destroy(&(*mac)->datapkt);
    // Destroy Frame
    if ((*mac)->frame != NULL)
        free((*mac)->frame);
    free((*mac));
    *mac = NULL;
#endif
#ifdef __RIOT__
    controlpacket_destroy(&mac->ctrlpkt);
    cfpacket_destroy(&mac->cfpkt);
    datapacket_destroy(&mac->datapkt);
#endif
}

void MAC_internals_clear(MAC_Internals_t *mac)
{
    // Keep radio, it is not physically possible to change the radio once deployed.
    assert(mac != NULL);

    ARROW(mac->frame)current_slot = 0;
    ARROW(mac->frame)current_frame = 0;
    ARROW(mac->frame)current_cf_slot = 0;
    ARROW(mac->frame)cf_slots_number = 0;
    ARROW(mac->frame)slots_number = 0;
    mac->selectedSlot = 0;
    mac->transmitChannel = 0;
    mac->receiveChannel = 0;
    mac->cfChannel = 0;
    mac->_collisionDetected = false;
    mac->_collisionSlot = 0;
    mac->_collisionFrequency = 0;
    mac->_cf_message_received = false;
}
