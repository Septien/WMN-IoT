#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

void MCLMAC_init(MCLMAC_t DOUBLE_POINTER mclmac, 
#ifdef __LINUX__
    uint8_t *radio
#endif
#ifdef __RIOT__
    netdev_t *netdev
#endif
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

    MAC_internals_init(&(SINGLE_POINTER mclmac)->mac, netdev);
    uint64_t id[2] = UUID;
    (SINGLE_POINTER mclmac)->_node_id[0] = id[0];
    (SINGLE_POINTER mclmac)->_node_id[1] = id[1];
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
    uint32_t frequencies[MAX_NUMBER_FREQS] = FREQUENCIES;
    memcpy(&(SINGLE_POINTER mclmac)->_frequencies, frequencies, sizeof(frequencies));

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

void mclmac_get_nodeid(MCLMAC_t *mclmac, uint64_t *node_id)
{
    assert(mclmac != NULL);
#ifdef __LINUX__    
    assert(mclmac->mac != NULL);
#endif

    node_id[0] = mclmac->_node_id[0];
    node_id[1] = mclmac->_node_id[1];
}

void mclmac_set_transmiterid(MCLMAC_t *mclmac, uint64_t *transmitter_id)
{
    assert(mclmac != NULL);

    ARROW(mclmac->mac)transmitter_id[0] = transmitter_id[0];
    ARROW(mclmac->mac)transmitter_id[1] = transmitter_id[1];
}

void mclmac_set_selected_slot(MCLMAC_t *mclmac, uint8_t slot)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)selectedSlot = slot;

}

uint8_t mclmac_get_selected_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif
    
    return ARROW(mclmac->mac)selectedSlot;
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

void mclmac_set_current_slot(MCLMAC_t *mclmac, uint8_t slot)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif

    ARROW(ARROW(mclmac->mac)frame)current_slot = slot;
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

void mclmac_set_slots_number(MCLMAC_t *mclmac, uint8_t n_slots)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif
    assert(n_slots > 0);

    ARROW(ARROW(mclmac->mac)frame)slots_number = n_slots;
}

void mclmac_set_cf_slots_number(MCLMAC_t *mclmac, uint8_t n_cf_slots)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif
    assert(n_cf_slots > 0);

    ARROW(ARROW(mclmac->mac)frame)cf_slots_number = n_cf_slots;
}

void mclmac_set_current_cf_slot(MCLMAC_t *mclmac, uint8_t cur_cf_slot)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif

    ARROW(ARROW(mclmac->mac)frame)current_cf_slot = cur_cf_slot;
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
double slot_dur
#endif
#ifdef __RIOT__
uint32_t slot_dur
#endif
)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif
    assert(slot_dur > 0);

    ARROW(ARROW(mclmac->mac)frame)slot_duration = slot_dur;
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
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif
    assert(frame_dur > 0);

    ARROW(ARROW(mclmac->mac)frame)frame_duration = frame_dur;
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
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->frame != NULL);
#endif
    assert(cf_dur > 0);
    ARROW(ARROW(mclmac->mac)frame)cf_duration = cf_dur;
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

    uint8_t to_send_msg = ARROW(mclmac->mac)_packets_to_send_message;
    uint8_t to_send_ctrl = ARROW(mclmac->mac)_packets_to_send_control;
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
            uint8_t last = ARROW(mclmac->mac)_last_send_message;
            datapacket_construct_from_bytestring(&ARROW(mclmac->mac)_message_packets_to_send[last], &byteString);
            ARROW(mclmac->mac)_last_send_message++;
            ARROW(mclmac->mac)_packets_to_send_message++;
        }
        else if (msg[0] >= 2 && msg[0] < 7)
        {
            uint8_t last = ARROW(mclmac->mac)_last_send_control;
            datapacket_construct_from_bytestring(&ARROW(mclmac->mac)_control_packets_to_send[last], &byteString);
            ARROW(mclmac->mac)_last_send_control++;
            ARROW(mclmac->mac)_packets_to_send_control++;
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

void mclmac_change_cf_channel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __RIOT__
    assert(ARROW(mclmac->mac)netdev != NULL);
#endif

#ifdef __RIOT__
    uint16_t channel = mclmac->mac.cfChannel;
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_CHANNEL, 
                                    (void *)&channel, sizeof(uint16_t));
    /* Only for the nrf24l01p radio. */
    uint8_t broadcast_addr[NRF24L01P_NG_ADDR_WIDTH] = NRF24L01P_NG_BROADCAST_ADDR;
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_ADDRESS,
                                    (void *)broadcast_addr, NRF24L01P_NG_ADDR_WIDTH);
    netopt_state_t state = NETOPT_STATE_STANDBY;
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_STATE, 
                                    (void *)&state, sizeof(state));
#endif

    // Change frequency to cf channel
    // Change the radio state to standby

}

void mclmac_start_cf_phase(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    assert(ARROW(mclmac->mac)netdev != NULL);

    // Change the channel
    mclmac_change_cf_channel(mclmac);
    // Change the radio state to rx
#ifdef __RIOT__
    netopt_state_t state = NETOPT_STATE_RX;
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_STATE, 
                                    (void *)&state, sizeof(state));
#endif
    // Reset the cf counter to zero
    mclmac_set_current_cf_slot(mclmac, 0);
}

int32_t mclmac_start_split_phase(MCLMAC_t *mclmac, PowerMode_t state)
{
    assert(mclmac != NULL);
    
    if (state != TRANSMIT && state != RECEIVE)
        return -1;

#ifdef __RIOT__
    uint16_t channel = 0;
    netopt_state_t net_state = NETOPT_STATE_OFF;
    if (state == TRANSMIT)
    {
        channel = mclmac->mac.transmitChannel;
        net_state = NETOPT_STATE_TX;
    }
    else if (state == RECEIVE)
    {
        channel = mclmac->mac.receiveChannel;
        net_state = NETOPT_STATE_RX;
    }
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_CHANNEL,
                                    (void *)&channel, sizeof(uint16_t));
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&net_state, sizeof(netopt_state_t));
    ControlPacket_t *pkt;
    pkt = &mclmac->mac.ctrlpkt;
    controlpacket_clear(pkt);
#endif
    return 1;
}

void mclmac_set_radio_sleep(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

#ifdef __RIOT__
    netopt_state_t state = NETOPT_STATE_SLEEP;
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&state, sizeof(netopt_state_t));
#endif
}

void mclmac_set_radio_standby(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __RIOT__
    netopt_state_t state = NETOPT_STATE_STANDBY;
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&state, sizeof(netopt_state_t));    
#endif
}

void mclmac_set_radio_rx(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __RIOT__
    netopt_state_t state = NETOPT_STATE_RX;
    mclmac->mac.netdev->driver->set(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&state, sizeof(netopt_state_t));
#endif
}
