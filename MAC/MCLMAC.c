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
    uint16_t nodeid, size_t dataQSize
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
    (SINGLE_POINTER mclmac)->_nChannels = MAX_NUMBER_FREQS;
    (SINGLE_POINTER mclmac)->_nSlots = MAX_NUMBER_SLOTS;
    (SINGLE_POINTER mclmac)->_hopCount = 0;
    (SINGLE_POINTER mclmac)->_networkTime = 0;

    // Initialize state machines
    mclmac_init_mac_state_machine((SINGLE_POINTER mclmac));
    mclmac_init_powermode_state_machine((SINGLE_POINTER mclmac));

    int n = MAX_NUMBER_FREQS;
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            (SINGLE_POINTER mclmac)->_occupied_frequencies_slots[i][j] = 0;
    (SINGLE_POINTER mclmac)->_frequencies[0] = FREQ1;
    (SINGLE_POINTER mclmac)->_frequencies[1] = FREQ2;
    (SINGLE_POINTER mclmac)->_frequencies[2] = FREQ3;
    (SINGLE_POINTER mclmac)->_frequencies[3] = FREQ4;
    (SINGLE_POINTER mclmac)->_frequencies[4] = FREQ5;
    (SINGLE_POINTER mclmac)->_frequencies[5] = FREQ6;
    (SINGLE_POINTER mclmac)->_frequencies[6] = FREQ7;
    (SINGLE_POINTER mclmac)->_frequencies[7] = FREQ8;
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

int32_t stub_mclmac_read_queue_element(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(ARROW(mclmac->mac)_packets_read <= MAX_NUMBER_DATA_PACKETS);

    if (ARROW(mclmac->mac)_packets_read == MAX_NUMBER_DATA_PACKETS)
        return 0;

    uint8_t position = ARROW(mclmac->mac)_last_send;
    DataPacket_t *pkt = &ARROW(mclmac->mac)_packet_to_send[position];
    uint8_t element;
    /* Node id */
    element = rand();
    ARROW(mclmac->mac)_destination_ids[position] = element;
    /* Is fragment */
    element = ((rand() % 256) > 128 ? 1 : 0);
    datapacket_set_isFragment(pkt, element);
    /* total fragment */
    element = rand();
    element = (element == 0 ? 1 : element);
    datapacket_set_total_fragments(pkt, element);
    /* Fragment number */
    element--;
    datapacket_set_fragment_number(pkt, element);
    /* data size */
    uint16_t datasize = rand() % 250;
    datasize = (datasize == 0 ? 1 : datasize);
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(datasize * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, datasize);
#endif
    uint8_t i;
    for (i = 0; i < datasize; i++)
    {
        element = rand();
        WRITE_ARRAY(REFERENCE data, element, i);
    }
    datapacket_set_data(pkt, &data, datasize);

    // Increase the number of packets read
    ARROW(mclmac->mac)_packets_read++;
    // Increase the last element
    position = (position + 1) % MAX_NUMBER_DATA_PACKETS;
    ARROW(mclmac->mac)_last_send = position;

    return 1;
}

int32_t stub_mclmac_write_queue_element(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    if (ARROW(mclmac->mac)_number_packets_received == 0)
        return 0;

    uint8_t first = ARROW(mclmac->mac)_first_received;
    // Get the first packet of the queue
    DataPacket_t *pkt = &ARROW(mclmac->mac)_packets_received [first];
    // Push the packet on the inter-layer queue (just clear it for now)
    datapacket_clear(pkt);
    first = (first + 1) % MAX_NUMBER_DATA_PACKETS;
    ARROW(mclmac->mac)_first_received = first;
    ARROW(mclmac->mac)_number_packets_received--;

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

/**
 * @brief Check the presence of a valid cf packet. Use for the INITIALIZATION state.
 * 
 * @param mclmac 
 */
bool stub_mclmac_cf_packet_detected(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    static int state = 0;
#ifdef __LINUX__
    int timeout = 0;
#endif
#ifdef __RIOT__
    uint32_t timeout = 0;
#endif
    /* For making the function fail. */
    if (state == 0)
    {
        /* Sleep for 1.1 second or more and pretend a valid packet is found. */
        timeout = timeout_set(TIME(1100000));
        while(timeout_passed(timeout) != 1) ;
        timeout_unset(timeout);
        state = 1;
        return false;
    }
    else if (state == 1)
    {
        /* Sleep for less than 1 second, and pretend a valid packet is found. */
        timeout = timeout_set(TIME(900000));
        while(timeout_passed(timeout) != 1) ;
        timeout_unset(timeout);
        state = 0;
        return true;
    }
    return false;
}

void stub_mclmac_send_cf_message(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif
    
//    mclmac_clear_cf_packet(mclmac);
}
/*
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
*/