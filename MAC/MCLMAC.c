#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

void MCLMAC_init(MCLMAC_t DOUBLE_POINTER mclmac, 
#ifdef __LINUX__
    uint8_t *radio,
#endif
#ifdef __RIOT__
    sx127x_t *radio,
#endif
    uint16_t nodeid, size_t dataQSize, uint8_t _nSlots, uint8_t _nChannels
)
{
#ifdef __LINUX__
    (*mclmac) = (MCLMAC_t *)malloc(sizeof(MCLMAC_t));
    if ((*mclmac) == NULL)
    {
        return;
    }
#endif
    memset((SINGLE_POINTER mclmac), 0, sizeof(MCLMAC_t));

    MAC_internals_init(&(SINGLE_POINTER mclmac)->mac, radio);
    (SINGLE_POINTER mclmac)->_nodeID = nodeid;
    (SINGLE_POINTER mclmac)->_dataQSize = dataQSize;
    (SINGLE_POINTER mclmac)->_nChannels = _nChannels;
    (SINGLE_POINTER mclmac)->_nSlots = _nSlots;
    (SINGLE_POINTER mclmac)->_hopCount = 0;

    // Initialize state machines
    mclmac_init_mac_state_machine((SINGLE_POINTER mclmac));
    mclmac_init_powermode_state_machine((SINGLE_POINTER mclmac));

    memset((SINGLE_POINTER mclmac)->_frequencies, 0, 8 * sizeof(uint32_t));
    //int bytes = get_number_bytes(_nChannels * _nSlots);
    memset((SINGLE_POINTER mclmac)->_occupiedSlots, 0, 8 * sizeof(uint16_t));
}
 
void MCLMAC_destroy(MCLMAC_t DOUBLE_POINTER mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(*mclmac != NULL);
#endif

    MAC_internals_destroy(&(SINGLE_POINTER mclmac)->mac);
#ifdef __LINUX__
    free((*mclmac));
    *mclmac = NULL;
#endif

#ifdef __RIOT__
    memset(mclmac, 0, sizeof(MCLMAC_t));
#endif
}

void MCLMAC_clear(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    
    MAC_internals_clear(REFERENCE mclmac->mac);
    mclmac->_dataQSize = 0;
    mclmac->macState.currentState = START;
    mclmac->powerMode.currentState = STARTP;
    mclmac->_networkTime = 0;
    mclmac->_hopCount = 0;
}

void mclmac_set_cf_channel(MCLMAC_t *mclmac, uint32_t cfchannel)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)cfChannel = cfchannel;
}

uint32_t mclmac_get_cf_channel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    return ARROW(mclmac->mac)cfChannel;
}

void mclmac_set_transmit_channel(MCLMAC_t *mclmac, uint32_t channel)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)transmitChannel = channel;
}

uint32_t mclmac_get_transmit_channel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    return ARROW(mclmac->mac)transmitChannel;
}

void mclmac_set_reception_channel(MCLMAC_t *mclmac, uint32_t rChannel)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)receiveChannel = rChannel;
}

uint32_t mclmac_get_reception_channel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    return ARROW(mclmac->mac)receiveChannel;
}

/*void mclmac_set_available_channels(MCLMAC_t *mclmac, ARRAY* channels, uint8_t nChannels)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(channels != NULL);
#endif
#ifdef __RIOT__
    assert(channels->size > 0);
#endif
    assert(nChannels > 0);

    mclmac->_nChannels = nChannels;
#ifdef __LINUX__
    mclmac->mac->channels = (uint8_t *)malloc(mclmac->mac->numberChannels * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&mclmac->mac.channels, nChannels);
#endif
    for (int i = 0; i < ARROW(mclmac)_nChannels; i++)
    {
        uint8_t element = READ_ARRAY((SINGLE_POINTER channels), i);
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)channels, element, i);
    }
}

void mclmac_get_available_channels(MCLMAC_t *mclmac, ARRAY* channels, uint8_t *nChannels)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->channels != NULL);
    assert(channels != NULL);
    assert(*channels != NULL);
#endif
#ifdef __RIOT__
    assert(mclmac->mac.channels.size > 0);
    assert(channels->size > 0);
#endif
    assert(nChannels != NULL);

    *nChannels = ARROW(mclmac->mac)numberChannels;
    for (int i = 0; i < *nChannels; i++)
    {
        uint8_t element = READ_ARRAY(REFERENCE ARROW(mclmac->mac)channels, i);
        WRITE_ARRAY(SINGLE_POINTER channels, element, i);
    }
}*/

void mclmac_set_nodeid(MCLMAC_t *mclmac, uint16_t id)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    mclmac->_nodeID = id;
}

uint16_t mclmac_get_nodeid(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__    
    assert(mclmac->mac != NULL);
#endif

    return mclmac->_nodeID;
}

void mclmac_set_selected_slot(MCLMAC_t *mclmac, uint8_t selectedSlot)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)selectedSlot = selectedSlot;

}

uint8_t mclmac_get_selected_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif
    
    return ARROW(mclmac->mac)selectedSlot;
}

void mclmac_set_number_of_hops(MCLMAC_t *mclmac, uint8_t hops)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    mclmac->_hopCount = hops;
}

uint8_t mclmac_get_number_of_hops(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    return mclmac->_hopCount;
}

void mclmac_set_current_frame(MCLMAC_t *mclmac, uint32_t frame_number)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif

    ARROW(ARROW(mclmac->mac)frame)current_frame = frame_number;
}

void mclmac_increase_frame(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif

    ARROW(ARROW(mclmac->mac)frame)current_frame++;
}

void mclmac_set_current_slot(MCLMAC_t *mclmac, uint8_t current_slot)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif

    ARROW(ARROW(mclmac->mac)frame)current_slot = current_slot;
}

void mclmac_increase_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif

    ARROW(ARROW(mclmac->mac)frame)current_slot++;
}

void mclmac_set_slots_number(MCLMAC_t *mclmac, uint8_t slots_number)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif
    assert(slots_number > 0);

    ARROW(ARROW(mclmac->mac)frame)slots_number = slots_number;
}

void mclmac_set_cf_slots_number(MCLMAC_t *mclmac, uint8_t cf_slots_number)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif
    assert(cf_slots_number > 0);

    ARROW(ARROW(mclmac->mac)frame)cf_slots_number = cf_slots_number;
}

void mclmac_set_current_cf_slot(MCLMAC_t *mclmac, uint8_t current_cf_slot)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif

    ARROW(ARROW(mclmac->mac)frame)current_cf_slot = current_cf_slot;
}

void mclmac_increase_cf_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif

    ARROW(ARROW(mclmac->mac)frame)current_cf_slot++;
}

void mclmac_set_slot_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double slot_duration
#endif
#ifdef __RIOT__
uint32_t slot_duration
#endif
)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif
    assert(slot_duration > 0);

    ARROW(ARROW(mclmac->mac)frame)slot_duration = slot_duration;
}

void mclmac_set_frame_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double frame_duration
#endif
#ifdef __RIOT__
uint32_t frame_duration
#endif
)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif
    assert(frame_duration > 0);

    ARROW(ARROW(mclmac->mac)frame)frame_duration = frame_duration;
}

void mclmac_set_cf_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double cf_duration
#endif
#ifdef __RIOT__
uint32_t cf_duration 
#endif
)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif
    assert(cf_duration > 0);
    ARROW(ARROW(mclmac->mac)frame)cf_duration = cf_duration;
}

void mclmac_record_collision(MCLMAC_t *mclmac, uint8_t collisionSlot, uint32_t collisionFrequency)
{
    assert(mclmac != NULL);
    assert(collisionFrequency > 0);

    ARROW(mclmac->mac)_collisionDetected = true;
    ARROW(mclmac->mac)_collisionSlot = collisionSlot;
    ARROW(mclmac->mac)_collisionFrequency = collisionFrequency;
}

/*void mclmac_set_destination_id(MCLMAC_t *mclmac, uint16_t id)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)destinationID = id;
}

uint16_t mclmac_get_destination_id(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return ARROW(mclmac->mac)destinationID;
}*/

void mclmac_set_network_time(MCLMAC_t *mclmac, uint32_t time)
{
    assert(mclmac != NULL);

    mclmac->_networkTime = time;
}

uint32_t mclmac_get_network_time(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->_networkTime;
}

void mclmac_start_CAD_mode(MCLMAC_t *mclmac)
{
    // TO DO
    (void) mclmac;
    return;
}

int32_t stub_mclmac_read_queue_element(MCLMAC_t *mclmac, uint16_t *bytes, size_t size, uint32_t *read_from)
{
    assert(mclmac != NULL);
    assert(bytes != NULL);
    assert(read_from != NULL);
    assert(size > 0);

    if ((*read_from) == size - 1)
    {
        *bytes = 0;
        return 0;
    }
    uint16_t datasize = rand() % 250;
    if ((size - (*read_from)) < (datasize + 7U))
    {
        *bytes = 0;
        return 0;
    }

    uint8_t element;
    /* Node id */
    element = rand();
    element = (element == 0 ? 1 : element);
    WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_packets, element, *read_from);
    element = rand();
    element = (element == 0 ? 1 : element);
    WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_packets, element, (*read_from) + 1);
    /* Is fragment */
    WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_packets, ((rand() % 256) > 124 ? 1 : 2), (*read_from) + 2);
    /* total fragment */
    element = rand();
    element = (element == 0 ? 1 : element);
    WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_packets, element, (*read_from) + 3);
    /* Fragment number */
    element = rand();
    element = (element == 0 ? 1 : element);
    WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_packets, element, (*read_from) + 4);
    /* data size */
    WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_packets, (uint8_t)datasize, (*read_from) + 5);
    uint8_t i;
    for (i = 0; i < datasize; i++)
    {
        element = rand();
        element = (element == 0 ? 2 : element);
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_packets, element, (*read_from) + i + 6);
    }
    WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_packets, '\0', (*read_from) + datasize + 6);
    *bytes = 7 + datasize;
    *read_from += *bytes;
    
    return 1;
}

int32_t stub_mclmac_write_queue_element(MCLMAC_t *mclmac, size_t size)
{
    assert(mclmac != NULL);

    if (size <= 0)
        return 0;
    
    size_t i;
    /* "Write the packet into de queue* (just pretend to do something.) */
    for (i = 0; i < 255; i++)
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_packets_received, 0, i);
    /* Displace the rest of the elements to the front. */
    for (i = 256; i < size; i++)
    {
        uint8_t element = READ_ARRAY(REFERENCE ARROW(mclmac->mac)_packets_received, i);
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_packets_received, element, i - 255);
    }
    return 1;
}

void stub_mclmac_change_cf_channel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(ARROW(mclmac->mac)radio != NULL);
    assert(ARROW(mclmac->mac)cfChannel >= 902000000 && ARROW(mclmac->mac)cfChannel <= 928000000);

    // Change the radio frequency

    // Change the radio state to standby

}

void stub_mclmac_start_cf_phase(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(ARROW(mclmac->mac)radio != NULL);

    // Change the radio state to rx single
}

void stub_mclmac_send_cf_message(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif
    
//    mclmac_clear_cf_packet(mclmac);
}

bool stub_mclmac_receive_cf_message(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    ARROW(mclmac->mac)_cf_message_received = ((rand() % 256) > 128 ? true : false);
    if (ARROW(mclmac->mac)_cf_message_received)
    {
        uint8_t element = rand() % 256;
        element = (element == 0 ? 1 : element);
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_cf_messages, element, 0);
        element = rand() % 256;
        element = (element == 0 ? 1 : element);
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_cf_messages, element, 1);
        element = rand() % 256;
        element = (element == 0 ? 1 : element);
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_cf_messages, element, 2);
        element = rand() % 256;
        element = (element == 0 ? 1 : element);
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_cf_messages, element, 3);
    }
    else
    {
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_cf_messages, 0, 0);
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_cf_messages, 0, 1);
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_cf_messages, 0, 2);
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)_cf_messages, 0, 3);
    }

    return ARROW(mclmac->mac)_cf_message_received;
}
