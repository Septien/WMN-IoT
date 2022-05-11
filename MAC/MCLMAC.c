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
    uint16_t nodeid
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
    (SINGLE_POINTER mclmac)->_nChannels = MAX_NUMBER_FREQS;
    (SINGLE_POINTER mclmac)->_nSlots = MAX_NUMBER_SLOTS;
    (SINGLE_POINTER mclmac)->_hopCount = 0;
    (SINGLE_POINTER mclmac)->_networkTime = 0;
    (SINGLE_POINTER mclmac)->_mac_queue_id = 0;
    (SINGLE_POINTER mclmac)->_routing_queue_id = 0;
    (SINGLE_POINTER mclmac)->_transport_queue_id = 0;
    (SINGLE_POINTER mclmac)->_app_queue_id = 0;

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

    (SINGLE_POINTER mclmac)->_mac_queue_id = create_queue(QUEUE_SIZE, MAX_MESSAGE_SIZE, MAX_ELEMENTS_ON_QUEUE, &(SINGLE_POINTER mclmac)->stack);
}
 
void MCLMAC_destroy(MCLMAC_t DOUBLE_POINTER mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(*mclmac != NULL);
#endif

    MAC_internals_destroy(&(SINGLE_POINTER mclmac)->mac);
    if ((SINGLE_POINTER mclmac)->_mac_queue_id != 0)
        close_queue((SINGLE_POINTER mclmac)->_mac_queue_id);
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
    mclmac->macState.currentState = START;
    mclmac->powerMode.currentState = STARTP;
    mclmac->_networkTime = 0;
    mclmac->_hopCount = 0;
    mclmac->stack = NULL;
    if (mclmac->_mac_queue_id != 0)
        close_queue(mclmac->_mac_queue_id);
    mclmac->_mac_queue_id = 0;
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

uint32_t mclmac_get_frequency(MCLMAC_t *mclmac, uint8_t index)
{
    assert(mclmac != NULL);
    assert(index < MAX_NUMBER_FREQS);

    return mclmac->_frequencies[index];
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

/**
 * @brief Set the id of the node that will transmit a message to the 
 * current node.
 * 
 * @param mclmac 
 * @param id 
 */
void mclmac_set_transmiterid(MCLMAC_t *mclmac, uint16_t id)
{
    assert(mclmac != NULL);
    assert(id != mclmac->_nodeID);

    ARROW(mclmac->mac)transmiterID = id;
}

/**
 * @brief Get the id of the node that will transmit a message to the
 * current node.
 * 
 * @param mclmac 
 * @return uint16_t 
 */
uint16_t mclmac_get_transmiterid(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return ARROW(mclmac->mac)transmiterID;
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

uint8_t mclmac_get_current_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return ARROW(ARROW(mclmac->mac)frame)current_slot;
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

uint8_t mclmac_get_current_cf_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return ARROW(ARROW(mclmac->mac)frame)current_cf_slot;
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

uint16_t mclmac_available_data_packets(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    uint8_t message_packets = ARROW(mclmac->mac)_packets_to_send_message;
    uint8_t control_packets = ARROW(mclmac->mac)_packets_to_send_control;

    return (uint16_t)(message_packets + control_packets);
}

void mclmac_start_CAD_mode(MCLMAC_t *mclmac)
{
    // TO DO
    (void) mclmac;
    return;
}

int32_t mclmac_read_queue_element(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(ARROW(mclmac->mac)_packets_to_send_message <= MAX_NUMBER_DATA_PACKETS);
    assert(ARROW(mclmac->mac)_packets_to_send_control <= MAX_NUMBER_DATA_PACKETS);

    if (ARROW(mclmac->mac)_packets_to_send_message == MAX_NUMBER_DATA_PACKETS)
        return 0;
    if (ARROW(mclmac->mac)_packets_to_send_control == MAX_NUMBER_DATA_PACKETS)
        return 0;

    if (elements_on_queue(mclmac->_mac_queue_id) == 0)
        return 0;

    if (elements_on_queue(mclmac->_mac_queue_id) > 0)
    {
        size_t size = MAX_MESSAGE_SIZE;
        uint8_t *msg;
#ifdef __LINUX__
        msg = (uint8_t *)malloc(size * sizeof(uint8_t));
        pthread_t pid;
#endif
#ifdef __RIOT__
        uint8_t _msg[size];
        msg = _msg;
        kernel_pid_t pid;
#endif
        recv_message(mclmac->_mac_queue_id, msg, size, &pid);
        ARRAY byteString;
#ifdef __LINUX__
        byteString = (uint8_t *)malloc((size) * sizeof(uint8_t));
#endif
#ifdef __RIOT__
        create_array(&byteString, size);
#endif
        // Copy the message to the byte string
        // Store the type
        WRITE_ARRAY(REFERENCE byteString, msg[0], 0);
        // Store the  destination id
        WRITE_ARRAY(REFERENCE byteString, msg[1], 1);
        WRITE_ARRAY(REFERENCE byteString, msg[2], 2);
        for (uint i = 3; i < size; i++)
        {
            uint8_t e = msg[i];
            WRITE_ARRAY(REFERENCE byteString, e, i);
        }
        if (msg[0] >= 7 && msg[0] < 10)
        {
            uint8_t last = ARROW(mclmac->mac)_last_send_message;
            DataPacket_t *pkt = &ARROW(mclmac->mac)_message_packets_to_send[last];
            datapacket_construct_from_bytestring(pkt, &byteString);
            ARROW(mclmac->mac)_last_send_message++;
            ARROW(mclmac->mac)_packets_to_send_message++;
        }
        else if (msg[0] >= 2 && msg[0] < 7)
        {
            uint8_t last = ARROW(mclmac->mac)_last_send_control;
            DataPacket_t *pkt = &ARROW(mclmac->mac)_control_packets_to_send[last];
            datapacket_construct_from_bytestring(pkt, &byteString);
            ARROW(mclmac->mac)_last_send_control++;
            ARROW(mclmac->mac)_packets_to_send_control++;
        }
        // Invalid type, return 0
        else
            return 0;
#ifdef __LINUX__
        free(byteString);
#endif
#ifdef __RIOT__
        free_array(&byteString);
#endif
    }

    return 1;
}

int32_t mclmac_write_queue_element(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    if (ARROW(mclmac->mac)_number_packets_received == 0)
        return 0;
    // When queue is empty, no sure how useful this check is.
    if (ARROW(mclmac->mac)_first_received == ARROW(mclmac->mac)_last_received && ARROW(mclmac->mac)_number_packets_received == 0)
        return 0;
    if (elements_on_queue(mclmac->_routing_queue_id) == MAX_ELEMENTS_ON_QUEUE)
        return 0;
    if (mclmac->_routing_queue_id == 0)
        return 0;

    uint8_t first = ARROW(mclmac->mac)_first_received;
    // Get the first packet of the queue
    DataPacket_t *pkt = &ARROW(mclmac->mac)_packets_received[first];
    ARRAY _byteString;
    datapacket_get_packet_bytestring(pkt, &_byteString);
#ifdef __LINUX__
    uint8_t *byteString = _byteString;
#endif
#ifdef __RIOT__
    uint8_t bs[MAX_MESSAGE_SIZE];
    for (uint i = 0; i < MAX_MESSAGE_SIZE; i++)
        bs[i] = READ_ARRAY(&_byteString, i);
    uint8_t *byteString = bs;
#endif

    // Send to routing layer
    int ret = send_message(mclmac->_routing_queue_id, byteString, PACKET_SIZE_MAC, mclmac->_self_pid);
    if (ret == 0)
        return 0;

    // Push the packet on the inter-layer queue (just clear it for now)
    datapacket_clear(pkt);
    first = (first + 1) % MAX_NUMBER_DATA_PACKETS;
    ARROW(mclmac->mac)_first_received = first;
    ARROW(mclmac->mac)_number_packets_received--;

#ifdef __LINUX__
    free(_byteString);
#endif
#ifdef __RIOT__
    free_array(&_byteString);
#endif

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

    // Change the channel
    stub_mclmac_change_cf_channel(mclmac);
    // Change the radio state to rx single
    // Reset the cf counter to zero
    mclmac_set_current_cf_slot(mclmac, 0);
}

int32_t stub_mclmac_start_split_phase(MCLMAC_t *mclmac, PowerMode_t state)
{
    assert(mclmac != NULL);
    
    if (state != TRANSMIT && state != RECEIVE)
        return -1;
    
    return 1;

    // Set the frequency on the radio.
    // Set the radio on stand by.
}
