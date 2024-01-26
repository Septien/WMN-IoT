#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

void MCLMAC_init(MCLMAC_t *mclmac)
{
    memset(mclmac, 0, sizeof(MCLMAC_t));

    MAC_Internals_t *mac = &mclmac->mac;
    MAC_internals_init(mac);

    uint64_t id[2] = UUID;
    mclmac->_node_id[0] = id[0];
    mclmac->_node_id[1] = id[1];
    mclmac->_nChannels = MAX_NUMBER_FREQS;
    mclmac->_nSlots = MAX_NUMBER_SLOTS;
    mclmac->_hopCount = 0;
    mclmac->_networkTime = 0;
    mclmac->_mac_queue_id = 0;
    mclmac->_routing_queue_id = 0;
    mclmac->_transport_queue_id = 0;
    mclmac->_app_queue_id = 0;

    // Initialize state machines
    mclmac_init_mac_state_machine(mclmac);
    mclmac_init_powermode_state_machine(mclmac);

    int n = MAX_NUMBER_FREQS;
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            mclmac->_occupied_frequencies_slots[i][j] = 0;
        }
    }
    uint32_t frequencies[MAX_NUMBER_FREQS] = FREQUENCIES;
    memcpy(&mclmac->_frequencies, frequencies, sizeof(frequencies));

    mclmac->_mac_queue_id = create_queue(QUEUE_SIZE, MAX_MESSAGE_SIZE, MAX_ELEMENTS_ON_QUEUE, &mclmac->stack);
}
 
void MCLMAC_destroy(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    MAC_internals_destroy(&mclmac->mac);
    if (mclmac->_mac_queue_id != 0) {
        close_queue(mclmac->_mac_queue_id);
    }
    memset(mclmac, 0, sizeof(MCLMAC_t));
}

void MCLMAC_clear(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    
    MAC_internals_clear(&mclmac->mac);
    mclmac->macState.currentState = START;
    mclmac->powerMode.currentState = STARTP;
    mclmac->_networkTime = 0;
    mclmac->_hopCount = 0;
    mclmac->stack = NULL;
    if (mclmac->_mac_queue_id != 0)
        close_queue(mclmac->_mac_queue_id);
    mclmac->_mac_queue_id = 0;
}

void mclmac_set_transmit_channel(MCLMAC_t *mclmac, uint32_t channel)
{
    assert(mclmac != NULL);

    mclmac->mac.transmitChannel = channel;
}

uint32_t mclmac_get_transmit_channel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->mac.transmitChannel;
}

void mclmac_set_reception_channel(MCLMAC_t *mclmac, uint32_t rChannel)
{
    assert(mclmac != NULL);

    mclmac->mac.receiveChannel = rChannel;
}

uint32_t mclmac_get_reception_channel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->mac.receiveChannel;
}

uint32_t mclmac_get_frequency(MCLMAC_t *mclmac, uint8_t index)
{
    assert(mclmac != NULL);
    assert(index < MAX_NUMBER_FREQS);

    return mclmac->_frequencies[index];
}

void mclmac_get_nodeid(MCLMAC_t *mclmac, uint64_t *node_id)
{
    assert(mclmac != NULL);

    node_id[0] = mclmac->_node_id[0];
    node_id[1] = mclmac->_node_id[1];
}

void mclmac_set_transmiterid(MCLMAC_t *mclmac, uint64_t *transmitter_id)
{
    assert(mclmac != NULL);

    mclmac->mac.transmitter_id[0] = transmitter_id[0];
    mclmac->mac.transmitter_id[1] = transmitter_id[1];
}

void mclmac_set_selected_slot(MCLMAC_t *mclmac, uint8_t slot)
{
    assert(mclmac != NULL);

    mclmac->mac.selectedSlot = slot;

}

uint8_t mclmac_get_selected_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    
    return mclmac->mac.selectedSlot;
}

void mclmac_set_current_frame(MCLMAC_t *mclmac, uint32_t frame_number)
{
    assert(mclmac != NULL);

    mclmac->mac.frame.current_frame = frame_number;
}

void mclmac_increase_frame(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    mclmac->mac.frame.current_frame++;
}

void mclmac_set_current_slot(MCLMAC_t *mclmac, uint8_t slot)
{
    assert(mclmac != NULL);

    mclmac->mac.frame.current_slot = slot;
}

uint8_t mclmac_get_current_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->mac.frame.current_slot;
}

void mclmac_increase_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    mclmac->mac.frame.current_slot++;
}

void mclmac_set_slots_number(MCLMAC_t *mclmac, uint8_t n_slots)
{
    assert(mclmac != NULL);
    assert(n_slots > 0);

    mclmac->mac.frame.slots_number = n_slots;
}

void mclmac_set_cf_slots_number(MCLMAC_t *mclmac, uint8_t n_cf_slots)
{
    assert(mclmac != NULL);
    assert(n_cf_slots > 0);

    mclmac->mac.frame.cf_slots_number = n_cf_slots;
}

void mclmac_set_current_cf_slot(MCLMAC_t *mclmac, uint8_t cur_cf_slot)
{
    assert(mclmac != NULL);

    mclmac->mac.frame.current_cf_slot = cur_cf_slot;
}

uint8_t mclmac_get_current_cf_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->mac.frame.current_cf_slot;
}

void mclmac_increase_cf_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    mclmac->mac.frame.current_cf_slot++;
}

void mclmac_set_slot_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double slot_dur
#endif
#ifdef __RIOT__
uint32_t slot_dur
#endif
)
{
    assert(mclmac != NULL);
    assert(slot_dur > 0);

    mclmac->mac.frame.slot_duration = slot_dur;
}

void mclmac_set_frame_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double frame_dur
#endif
#ifdef __RIOT__
uint32_t frame_dur
#endif
)
{
    assert(mclmac != NULL);
    assert(frame_dur > 0);

    mclmac->mac.frame.frame_duration = frame_dur;
}

void mclmac_set_cf_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double cf_dur
#endif
#ifdef __RIOT__
uint32_t cf_dur
#endif
)
{
    assert(mclmac != NULL);
    assert(cf_dur > 0);

    mclmac->mac.frame.cf_duration = cf_dur;
}

uint16_t mclmac_available_data_packets(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    uint8_t message_packets = mclmac->mac._packets_to_send_message;
    uint8_t control_packets = mclmac->mac._packets_to_send_control;

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
    assert(mclmac->mac._packets_to_send_message <= MAX_NUMBER_DATA_PACKETS);
    assert(mclmac->mac._packets_to_send_control <= MAX_NUMBER_DATA_PACKETS);

    uint8_t to_send_msg = mclmac->mac._packets_to_send_message;
    uint8_t to_send_ctrl = mclmac->mac._packets_to_send_control;
    if (to_send_msg + to_send_ctrl == 2 * MAX_NUMBER_DATA_PACKETS) {
        return 0;
    }

    if (elements_on_queue(mclmac->_mac_queue_id) == 0) {
        return 0;
    }

    bool invalid = false;
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
        byteString = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
        create_array(&byteString, size);
#endif
        // Copy the message to the byte string
        for (uint i = 0; i < size; i++) {
            WRITE_ARRAY(REFERENCE byteString, msg[i], i);
        }
        if (msg[0] >= 7 && msg[0] < 10)
        {
            uint8_t last = mclmac->mac._last_send_message;
            datapacket_construct_from_bytestring(&mclmac->mac._message_packets_to_send[last], &byteString);
            mclmac->mac._last_send_message++;
            mclmac->mac._packets_to_send_message++;
        }
        else if (msg[0] >= 2 && msg[0] < 7)
        {
            uint8_t last = mclmac->mac._last_send_control;
            datapacket_construct_from_bytestring(&mclmac->mac._control_packets_to_send[last], &byteString);
            mclmac->mac._last_send_control++;
            mclmac->mac._packets_to_send_control++;
        }
        // Invalid type, return 0
        else
        {
            invalid = true;
        }
#ifdef __LINUX__
    free(msg);
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
#endif
    }
    return (invalid ? 0 : 1);
}

int32_t mclmac_write_queue_element(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    if (mclmac->mac._number_packets_received == 0)
        return 0;
    // When queue is empty, no sure how useful this check is.
    if (mclmac->mac._first_received == mclmac->mac._last_received && mclmac->mac._number_packets_received == 0)
        return 0;
    if (elements_on_queue(mclmac->_routing_queue_id) == MAX_ELEMENTS_ON_QUEUE)
        return 0;
    if (mclmac->_routing_queue_id == 0)
        return 0;

    uint8_t first = mclmac->mac._first_received;
    // Get the first packet of the queue
    DataPacket_t *pkt = &mclmac->mac._packets_received[first];
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
    mclmac->mac._first_received = first;
    mclmac->mac._number_packets_received--;

#ifdef __LINUX__
    free(_byteString);
#endif
#ifdef __RIOT__
    free_array(&_byteString);
#endif

    return 1;
}

void mclmac_change_cf_channel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    // Change frequency to cf channel
    // Change the radio state to standby

}

void mclmac_start_cf_phase(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    // Change the channel
    mclmac_change_cf_channel(mclmac);
    // Change the radio state to rx

    // Reset the cf counter to zero
    mclmac_set_current_cf_slot(mclmac, 0);
}

int32_t mclmac_start_split_phase(MCLMAC_t *mclmac, PowerMode_t state)
{
    assert(mclmac != NULL);
    
    if (state != TRANSMIT && state != RECEIVE)
        return -1;

    return 1;
}

void mclmac_set_radio_sleep(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
}

void mclmac_set_radio_standby(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
}

void mclmac_set_radio_rx(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
}
