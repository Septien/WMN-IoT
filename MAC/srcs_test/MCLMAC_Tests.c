#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "string.h"

#include "MCLMAC.h"

#include "cUnit.h"

#ifdef __RIOT__
#include "nrf24l01p_ng.h"
#endif

#define ITERATIONS 1000

struct mclmac_data {
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t *radio;
#endif
#ifdef __RIOT__
    nrf24l01p_ng_t *radio;
    netdev_t *netdev;
    nrf24l01p_ng_t *radio_test;
    netdev_t *netdev_test;
#endif
};

void setup_mclmac(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
#ifdef __LINUX__
    MCLMAC_init(&data->mclmac, data->radio);
#endif
#ifdef __RIOT__
#ifndef NATIVE
    netopt_state_t state = NETOPT_STATE_STANDBY;
    data->netdev->driver->set(data->netdev, NETOPT_STATE, (void *)&state, sizeof(state));
#endif
    MCLMAC_init(&data->mclmac, data->netdev);
#endif
}

void teardown_mclmac(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
#ifdef __RIOT__
#ifndef NATIVE
    netopt_state_t state = NETOPT_STATE_SLEEP;
    data->netdev->driver->set(data->netdev, NETOPT_STATE, (void *)&state, sizeof(state));
#endif
#endif
    MCLMAC_destroy(&data->mclmac);
}

bool test_MCLMAC_init(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;

    bool passed = true;
#ifdef __LINUX__
    passed = passed && (data->mclmac != NULL);
    passed = passed && (data->mclmac->mac != NULL);
#endif
    uint64_t id[2] = UUID;
    passed = passed && (ARROW(data->mclmac)_node_id[0] == id[0]);
    passed = passed && (ARROW(data->mclmac)_node_id[1] == id[1]);
    passed = passed && (ARROW(data->mclmac)_networkTime == 0);
    passed = passed && (ARROW(data->mclmac)_nSlots == MAX_NUMBER_SLOTS);
    passed = passed && (ARROW(data->mclmac)_nChannels == MAX_NUMBER_FREQS);
    passed = passed && (ARROW(data->mclmac)_networkTime == 0);
    passed = passed && (ARROW(data->mclmac)_hopCount == 0);
    int n = MAX_NUMBER_FREQS;
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < n; i++)
    {
        passed = passed && (ARROW(data->mclmac)_frequencies[i] >= 902000000 && ARROW(data->mclmac)_frequencies[i] <= 928000000);
        for (int j = 0; j < m; j++)
            passed = passed && (ARROW(data->mclmac)_occupied_frequencies_slots[i][j] == 0);
    }
#ifdef __LINUX__
    passed = passed && (data->mclmac->stack == NULL);
#endif
#ifdef __RIOT__
    passed = passed && (data->mclmac.stack != NULL);
#endif
    passed = passed && (ARROW(data->mclmac)_mac_queue_id != 0);
    passed = passed && (ARROW(data->mclmac)_routing_queue_id == 0);
    passed = passed && (ARROW(data->mclmac)_transport_queue_id == 0);
    passed = passed && (ARROW(data->mclmac)_app_queue_id == 0);

    return passed;
}

bool test_MCLMAC_destroy(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;

    MCLMAC_destroy(&data->mclmac);
    bool passed = true;
#ifdef __LINUX__
    passed = passed && (data->mclmac == NULL);
#endif
#ifdef __LINUX__
    MCLMAC_init(&data->mclmac, data->radio);
#endif
#ifdef __RIOT__
    MCLMAC_init(&data->mclmac, data->netdev);
#endif

    return passed;
}

bool test_MCLMAC_clear(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    MCLMAC_clear(mclmac);
    bool passed = true;
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_frame == 0);
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_slot == 0);
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_cf_slot == 0);
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)cf_slots_number == 0);
    passed = passed && (ARROW(ARROW(mclmac->mac)frame)slots_number == 0);
    passed = passed && (mclmac->powerMode.currentState == STARTP);
    passed = passed && (mclmac->macState.currentState == START);
    passed = passed && (mclmac->_hopCount == 0);
    passed = passed && (mclmac->_networkTime == 0);
    passed = passed && (mclmac->_mac_queue_id == 0);
    passed = passed && (mclmac->stack == NULL);

    return passed;
}

bool test_mclmac_set_transmit_channel(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t channel = (uint32_t)rand();
        mclmac_set_transmit_channel(mclmac, channel);
        passed = passed && (ARROW(mclmac->mac)transmitChannel == channel);
    }

    return passed;
}

bool test_mclmac_get_transmit_channel(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t channel = (uint32_t)rand();
        mclmac_set_transmit_channel(mclmac, channel);
        uint32_t channelS = mclmac_get_transmit_channel(mclmac);
        passed = passed && (channelS == channel);
    }

    return passed;
}

bool test_mclmac_set_reception_channel(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t channel = (uint32_t)rand();
        mclmac_set_reception_channel(mclmac, channel);
        passed = passed && (ARROW(mclmac->mac)receiveChannel == channel);
    }

    return passed;
}

bool test_mclmac_get_reception_channel(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t channel = (uint32_t)rand();
        mclmac_set_reception_channel(mclmac, channel);
        uint32_t channelS = mclmac_get_reception_channel(mclmac);
        passed = passed && (channelS == channel);
    }

    return passed;
}

bool test_mclmac_get_frequency(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    for (int i = 0; i < MAX_NUMBER_FREQS; i++)
    {
        uint32_t freq = mclmac_get_frequency(mclmac, i);
        passed = passed && (freq == mclmac->_frequencies[i]);
    }

    return passed;
}

bool test_mclmac_get_nodeid(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint64_t nodeid[2] = {0};
        nodeid[0] = rand();
        nodeid[1] = rand();
        mclmac->_node_id[0] = nodeid[0];
        mclmac->_node_id[1] = nodeid[1];
        uint64_t node_id_r[2] = {0};
        mclmac_get_nodeid(mclmac, node_id_r);
        passed = passed && (node_id_r[0] == nodeid[0]);
        passed = passed && (node_id_r[1] == nodeid[1]);
    }

    return passed;
}

bool test_mclmac_set_transmiterid(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * Set the sender id, it should comply that id is different to the
     * current node's id.
     */
    uint64_t sender_id[2] = {0};
    sender_id[0] = rand();
    sender_id[1] = rand();
    mclmac_set_transmiterid(mclmac, sender_id);
    bool passed = true;
    passed = passed && (ARROW(mclmac->mac)transmitter_id[0] == sender_id[0]);
    passed = passed && (ARROW(mclmac->mac)transmitter_id[1] == sender_id[1]);

    return passed;
}

bool test_mclmac_set_selected_slot(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = (uint8_t) rand() % 256;
        mclmac_set_selected_slot(mclmac, slot);
        passed = passed && (ARROW(mclmac->mac)selectedSlot == slot);
    }

    return passed;
}

bool test_mclmac_get_selected_slot(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = (uint8_t) rand() % 256;
        mclmac_set_selected_slot(mclmac, slot);
        uint8_t slotR = mclmac_get_selected_slot(mclmac);
        passed = passed && (slotR == slot);
    }

    return passed;
}

bool test_mclmac_set_current_frame(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t frame_number = (uint32_t)rand();
        mclmac_set_current_frame(mclmac, frame_number);
        passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_frame == frame_number);
    }

    return passed;
}

bool test_mclmac_increase_frame(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    uint32_t frame_number = 0;
    mclmac_set_current_frame(mclmac, frame_number);

    bool passed = true;
    for (int i = 0; i < ITERATIONS; i++)
    {
        mclmac_increase_frame(mclmac);
        frame_number++;
        passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_frame == frame_number);
    }

    return passed;
}

bool test_mclmac_set_current_slot(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = (uint8_t)rand() % 256;
        mclmac_set_current_slot(mclmac, slot);
        passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_slot == slot);
    }

    return passed;
}

bool test_mclmac_get_current_slot(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = (uint8_t)rand() % 256;
        mclmac_set_current_slot(mclmac, slot);
        uint8_t current_slot = mclmac_get_current_slot(mclmac);
        passed = passed && (current_slot == slot);
    }

    return passed;
}

bool test_mclmac_increase_slot(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    uint8_t slot = 0;
    mclmac_set_current_slot(mclmac, slot);
    int n = rand() % 512;
    for (int i = 0; i < n; i++)
    {
        mclmac_increase_slot(mclmac);
        slot++;
        passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_slot == slot);
    }

    return passed;
}

bool test_mclmac_set_slots_number(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slots_number = (uint8_t) rand();
        slots_number = (slots_number == 0 ? 1 : slots_number);
        mclmac_set_slots_number(mclmac, slots_number);
        passed = passed && (ARROW(ARROW(mclmac->mac)frame)slots_number == slots_number);
    }

    return passed;
}

bool test_mclmac_set_cf_slots_number(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t cf_slots_number = 1 + ((uint8_t) rand());
        cf_slots_number = (cf_slots_number == 0 ? 1 : cf_slots_number);
        mclmac_set_cf_slots_number(mclmac, cf_slots_number);
        passed = passed && (ARROW(ARROW(mclmac->mac)frame)cf_slots_number == cf_slots_number);
    }

    return passed;
}

bool test_mclmac_set_current_cf_slot(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t current_cf_slot = ((uint8_t) rand() % 256);
        current_cf_slot = (current_cf_slot == 0 ? 1 : current_cf_slot);
        mclmac_set_current_cf_slot(mclmac, current_cf_slot);
        passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_cf_slot == current_cf_slot);
    }

    return passed;
}

bool test_mclmac_get_current_cf_slot(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t current_cf_slot = ((uint8_t) rand() % 256);
        current_cf_slot = (current_cf_slot == 0 ? 1 : current_cf_slot);
        mclmac_set_current_cf_slot(mclmac, current_cf_slot);
        uint8_t slot = mclmac_get_current_cf_slot(mclmac);
        passed = passed && (slot == current_cf_slot);
    }

    return passed;
}

bool test_mclmac_increase_cf_slot(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;
    
    uint8_t nCFSlot = rand();
    nCFSlot = (nCFSlot == 0 ? 1 : nCFSlot);
    mclmac_set_current_cf_slot(mclmac, nCFSlot);
    int n = rand() % ITERATIONS;
    bool passed = true;
    for (int i = 0; i < n; i++)
    {
        mclmac_increase_cf_slot(mclmac);
        nCFSlot++;
        passed = passed && (ARROW(ARROW(mclmac->mac)frame)current_cf_slot == nCFSlot);
    }

    return passed;
}

bool test_mclmac_set_slot_duration(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

#ifdef __LINUX__
    double slot_duration;
#endif
#ifdef __RIOT__
    uint32_t slot_duration;
#endif

    bool passed = true;
    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        slot_duration = rand();
        mclmac_set_slot_duration(mclmac, slot_duration);
        passed = passed && (ARROW(ARROW(mclmac->mac)frame)slot_duration == slot_duration);
    }

    return passed;
}

bool test_mclmac_set_frame_duration(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

#ifdef __LINUX__
    double frame_duration;
#endif
#ifdef __RIOT__
    uint32_t frame_duration;
#endif

    bool passed = true;
    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        frame_duration = rand();
        mclmac_set_frame_duration(mclmac, frame_duration);
        passed = passed && (ARROW(ARROW(mclmac->mac)frame)frame_duration == frame_duration);
    }
    
    return passed;
}

bool test_mclmac_set_cf_duration(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

#ifdef __LINUX__
    double cf_duration;
#endif
#ifdef __RIOT__
    uint32_t cf_duration;
#endif

    bool passed = true;
    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        cf_duration = rand();
        mclmac_set_cf_duration(mclmac, cf_duration);
        passed = passed && (ARROW(ARROW(mclmac->mac)frame)cf_duration == cf_duration);
    }

    return passed;
}

bool test_mclmac_available_data_packets(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    bool passed = true;
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t message_packets = rand() % MAX_NUMBER_DATA_PACKETS;
        uint8_t control_packets = rand() % MAX_NUMBER_DATA_PACKETS;
        ARROW(mclmac->mac)_packets_to_send_message = message_packets;
        ARROW(mclmac->mac)_packets_to_send_control = control_packets;
        uint16_t total_packets = mclmac_available_data_packets(mclmac);
        passed = passed && (total_packets == message_packets + control_packets);
    }

    return passed;
}

void _read_queue(MCLMAC_t *mclmac)
{
    open_queue(mclmac->_mac_queue_id, mclmac->_self_pid);
    
    /* Test case 1:
    *   When the array is full, that is, when _packets_read == MAX_NUMER_DATA_PACKETS.
    *       -nelements should be 0.
    *       -_packets_read should be MAX_NUMBER_DATA_PACKETS
    * */
    ARROW(mclmac->mac)_packets_to_send_message = MAX_NUMBER_DATA_PACKETS;
    ARROW(mclmac->mac)_packets_to_send_control = MAX_NUMBER_DATA_PACKETS;
    int nelements = mclmac_read_queue_element(mclmac);
    bool passed = true;
    passed = passed && (nelements == 0);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_message == MAX_NUMBER_DATA_PACKETS);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_control == MAX_NUMBER_DATA_PACKETS);
    if (!passed) {
        printf("Test case 1 \033[1;31mfailed\033[0m.\n");
        passed = true;
    }
    /** Test case 2:
     * The queue is empty. It should not increase the number of elements on the queue,
     * and the function should return 0.
    */
    ARROW(mclmac->mac)_packets_to_send_message = 0;
    ARROW(mclmac->mac)_packets_to_send_control = 0;
    nelements = mclmac_read_queue_element(mclmac);
    passed = passed && (nelements == 0);
    if (!passed) {
        printf("Test case 2 \033[1;31mfailed\033[0m.\n");
        passed = true;
    }
    /** Test case 3:
     * There is only one element on the queue (a message). The changes should be the following:
     *      -At position zero, there should be a packet on the array _messages_packet_to_send.
     *      -first_send_message should remaing the same.
     *      -last_send_message should increase by one.
     *      -packets_read should increase by one.
     *      -the functino should return one.
    */
    size_t size = MAX_MESSAGE_SIZE;
    uint8_t message[size];
    // Store the type of message
    message[0] = 7;
    for (uint i = 1; i < size; i++) {
        message[i] = rand();
    }
    send_message(mclmac->_mac_queue_id, message, size, mclmac->_self_pid);
    nelements = mclmac_read_queue_element(mclmac);
    passed = passed && (nelements == 1);
    passed = passed && (ARROW(mclmac->mac)_first_send_message == 0);
    passed = passed && (ARROW(mclmac->mac)_last_send_message == 1);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_message == 1);
    DataPacket_t *pkt = &ARROW(mclmac->mac)_message_packets_to_send[0];
    // Incoming message
    passed = passed && (pkt->type == message[0]);
    uint64_t destination_id[2] = {0};
    destination_id[0] |= ((uint64_t)message[1]) << 56;
    destination_id[0] |= ((uint64_t)message[2]) << 48;
    destination_id[0] |= ((uint64_t)message[3]) << 40;
    destination_id[0] |= ((uint64_t)message[4]) << 32;
    destination_id[0] |= ((uint64_t)message[5]) << 24;
    destination_id[0] |= ((uint64_t)message[6]) << 16;
    destination_id[0] |= ((uint64_t)message[7]) << 8;
    destination_id[0] |= ((uint64_t)message[8]);
    destination_id[1] |= ((uint64_t)message[9])  << 56;
    destination_id[1] |= ((uint64_t)message[10]) << 48;
    destination_id[1] |= ((uint64_t)message[11]) << 40;
    destination_id[1] |= ((uint64_t)message[12]) << 32;
    destination_id[1] |= ((uint64_t)message[13]) << 24;
    destination_id[1] |= ((uint64_t)message[14]) << 16;
    destination_id[1] |= ((uint64_t)message[15]) << 8;
    destination_id[1] |= ((uint64_t)message[16]);
    passed = passed && (pkt->destination_id[0] == destination_id[0]);
    passed = passed && (pkt->destination_id[1] == destination_id[1]);
    // Only the packet's data
    passed = passed && (pkt->size == message[17]);
    for (uint i = 0; i < pkt->size; i++)
        passed = passed && (READ_ARRAY(REFERENCE pkt->data, i) == message[i + 18]);
    if (!passed) {
        printf("Test case 3.1 \033[1;31mfailed\033[0m.\n");
        passed = true;
    }
    // Type 1, a control packet
    message[0] = 2;
    send_message(mclmac->_mac_queue_id, message, size, mclmac->_self_pid);
    nelements = mclmac_read_queue_element(mclmac);
    passed = passed && (nelements == 1);
    passed = passed && (ARROW(mclmac->mac)_first_send_control == 0);
    passed = passed && (ARROW(mclmac->mac)_last_send_control == 1);
    passed = passed && (ARROW(mclmac->mac)_packets_to_send_control == 1);
    pkt = &ARROW(mclmac->mac)_control_packets_to_send[0];
    passed = passed && (pkt->size == message[17]);
    for (uint i = 0; i < pkt->size; i++) {
        passed = passed && (READ_ARRAY(REFERENCE pkt->data, i) == message[i + 18]);
    }
    if (!passed) {
        printf("Test case 3.2 \033[1;31mfailed\033[0m.\n");
        passed = true;
    }
    // Invalid packet type
    message[0] = rand() + 10;
    send_message(mclmac->_mac_queue_id, message, size, mclmac->_self_pid);
    nelements = mclmac_read_queue_element(mclmac);
    passed = passed && (nelements == 0);
    if (!passed) {
        printf("Test case 3.3 \033[1;31mfailed\033[0m.\n");
    }

    if (!passed) {
        printf("\033[0;31mmclmac_read_queue_element function tests failed\033[0m.\n");
    }
}
#ifdef __LINUX__
void *read_queue(void *arg)
{
    MCLMAC_t *mclmac;
    mclmac = (MCLMAC_t *)arg;
    mclmac->_self_pid = pthread_self();
    
    _read_queue(mclmac);
    return NULL;
}
#endif
#ifdef __RIOT__
static void* read_queue(void *arg)
{
    MCLMAC_t *mclmac;
    mclmac = (MCLMAC_t *)arg;
    mclmac->_self_pid = thread_getpid();
    
    _read_queue(mclmac);
    return NULL;
}
#endif
bool test_mclmac_read_queue_element(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * read_queue_element.
     * This function should read data sent from other layers via a shared queue.
     * We should get the following data from the upper layer:
     *  -Destination ID.
     *  -The size of the data.
     *  -The data itself.
     * Store this in the array of data packets on the mac data structure.
     * Increase by one the number of elements read, when appropiate.
     */
#ifdef __LINUX__
    pthread_t pid;
    pthread_create(&pid, NULL, read_queue, (void *)mclmac);
    pthread_join(pid, NULL);
#endif
#ifdef __RIOT__
    kernel_pid_t pid = thread_create(mclmac->stack, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1,
                        THREAD_CREATE_SLEEPING, read_queue, (void *)mclmac, "Read queue");
    thread_wakeup(pid);
    while (thread_getstatus(pid) == STATUS_RUNNING) ;
#endif
    return true;
}

void _write_queue(MCLMAC_t *mclmac)
{
    open_queue(mclmac->_mac_queue_id, mclmac->_self_pid);

    /**
     * Test case 0:
     *  -The routing layer is not open yet. Return 0.
     */
    int ret = mclmac_write_queue_element(mclmac);
    bool passed = true;
    passed = passed && (ret == 0);

    /* The message will be sent to the routing layer, for the moment simulate
     that the mac queue is the routing queue. */
    mclmac->_routing_queue_id = mclmac->_mac_queue_id;
    /**
     * Test case 1:
     *  -_elements_read = 0.
     * It should return 0.
    */
    ret = mclmac_write_queue_element(mclmac);
    passed = passed && (ret == 0);

    /**
     * Test case 2:
     *      -The queue is full, return 0.
     */
    uint8_t msg[MAX_MESSAGE_SIZE] = {0};
    for (uint i = 0; i < MAX_MESSAGE_SIZE; i++)
        msg[i] = rand();
    for (uint i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++)
        send_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, mclmac->_self_pid);
    ARROW(mclmac->mac)_number_packets_received = 1;
    ret = mclmac_write_queue_element(mclmac);
    passed = passed && (ret == 0);
#ifdef __LINUX__
    pthread_t pid;
#endif
#ifdef __RIOT__
    kernel_pid_t pid;
#endif
    // Empty queue
    for (uint i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++)
    {
        memset(msg, 0, MAX_MESSAGE_SIZE);
        recv_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, &pid);
    }
    /**
    * Test case 3:
    *   -One element is on the array, so first = 0, last = 1, and packet_read = 1.
    *   -It should return 1.
    *   -first should be 1.
    *   -last should be 1.
    *   -packets_read should be 0.
    *   -the packet should be cleared
    */
    // Fill the packet
    DataPacket_t *pkt = &ARROW(mclmac->mac)_packets_received[0];
    uint64_t destination_id[2] = {0};
    destination_id[0] = rand();
    destination_id[1] = rand();
    uint8_t type = rand() % 3;
    uint8_t size = rand();
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (uint i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);
    datapacket_create(pkt, type, destination_id, &data, size);

    // Increase the number of packets store.
    ARROW(mclmac->mac)_number_packets_received = 1;
    ARROW(mclmac->mac)_last_received = 1;
    ret = mclmac_write_queue_element(mclmac);
    passed = passed && (ret == 1);
    passed = passed && (ARROW(mclmac->mac)_number_packets_received == 0);
    passed = passed && (ARROW(mclmac->mac)_first_received == 1);
    passed = passed && (ARROW(mclmac->mac)_last_received == 1);
    passed = passed && (pkt->type == -1);
    passed = passed && (pkt->destination_id[0] == 0);
    passed = passed && (pkt->destination_id[1] == 0);
    passed = passed && (pkt->size == 0);
#ifdef __LINUX__
    passed = passed && (pkt->data == NULL);
#endif
#ifdef __RIOT__
    passed = passed && (pkt->data.size == 0);
#endif
    // Clear queue
    memset(msg, 0, MAX_MESSAGE_SIZE);
    recv_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, &pid);

    /**
     * Test case 4: store MAX_NUMBER_DATA_PACKETS, then push all on queue.
     * The following should comply:
     *  -last_received should be 0.
     *  -first_received should be 0.
     *  -_number_packets_received should be 0.
     */
    ARROW(mclmac->mac)_number_packets_received = 0;
    ARROW(mclmac->mac)_last_received = 0;
    ARROW(mclmac->mac)_first_received = 0;
    uint i;
    for (i = 0; i < MAX_MESSAGE_SIZE; i++)
        msg[i] = rand();
    for (i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
    {
        DataPacket_t *pkt = &ARROW(mclmac->mac)_packets_received[i];
        datapacket_create(pkt, type, destination_id, &data, size);
    }
    // Total of packets to store.
    ARROW(mclmac->mac)_number_packets_received = MAX_NUMBER_DATA_PACKETS;
    ARROW(mclmac->mac)_last_received = 0;

    for (i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
    {
        ret = mclmac_write_queue_element(mclmac);
        recv_message(mclmac->_mac_queue_id, msg, MAX_MESSAGE_SIZE, &pid);
        passed = passed && (ret == 1);
    }
    passed = passed && (ARROW(mclmac->mac)_number_packets_received == 0);
    passed = passed && (ARROW(mclmac->mac)_first_received == 0);
    passed = passed && (ARROW(mclmac->mac)_last_received == 0);
    for (i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
    {
        DataPacket_t *pkt = &ARROW(mclmac->mac)_packets_received[i];
        passed = passed && (pkt->type == -1);
        passed = passed && (pkt->destination_id[0] == 0);
        passed = passed && (pkt->destination_id[1] == 0);
        passed = passed && (pkt->size == 0);
#ifdef __LINUX__
        passed = passed && (pkt->data == NULL);
#endif
#ifdef __RIOT__
        passed = passed && (pkt->data.size == 0);
#endif
    }
#ifdef __LINUX__
    free(data);
#endif
#ifdef __RIOT__
    free_array(&data);
#endif

    if (!passed) {
        printf("\033[0;31mmclmac_read_queue_element function tests failed\033[0m.\n");
    }
}
#ifdef __LINUX__
void *write_queue(void *arg)
{
    MCLMAC_t *mclmac;
    mclmac = (MCLMAC_t *)arg;
    mclmac->_self_pid = pthread_self();

    _write_queue(mclmac);
    return NULL;
}
#endif
#ifdef __RIOT__
static void *write_queue(void *arg)
{
    MCLMAC_t *mclmac;
    mclmac = (MCLMAC_t *)arg;
    mclmac->_self_pid = thread_getpid();

    _write_queue(mclmac);
    return NULL;
}
#endif
bool test_mclmac_write_queue_element(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * This function will store the packets received from the medium
     * onto the IPC Queues, for upper layer processing.
     * We assume the array _packets_received contains only packet for
     * upper layers. Internal packets are handled else where.
     */
#ifdef __LINUX__
    pthread_t pid;
    pthread_create(&pid, NULL, write_queue, (void *)mclmac);
    pthread_join(pid, NULL);
#endif
#ifdef __RIOT__
    kernel_pid_t pid = thread_create(mclmac->stack, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1,
                                  THREAD_CREATE_SLEEPING, write_queue, (void *)mclmac, "Write Queue");
    thread_wakeup(pid);
    while (thread_getstatus(pid) == STATUS_RUNNING) ;
#endif

    return true;
}

bool test_mclmac_change_cf_channel(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    ARROW(mclmac->mac)cfChannel = 5;

    mclmac_change_cf_channel(mclmac);

    bool passed = true;
#ifdef __RIOT__
    /*uint8_t channel = (uint8_t)ARROW(mclmac->mac)cfChannel;
    uint8_t radio_channel;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_CHANNEL,
                                    (void *)&channel, sizeof(uint16_t));

    passed = passed && (channel == radio_channel);
    // Address is broadcast address
    uint8_t broadcast[NRF24L01P_NG_ADDR_WIDTH] = NRF24L01P_NG_BROADCAST_ADDR;
    uint8_t addr[NRF24L01P_NG_ADDR_WIDTH] = {0};
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_ADDRESS, 
                                    (void *)addr, NRF24L01P_NG_ADDR_WIDTH);
    passed = passed && (memcmp(broadcast, addr, NRF24L01P_NG_ADDR_WIDTH) == 0);
    netopt_state_t state = NETOPT_STATE_OFF;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&state, sizeof(netopt_state_t));
    passed = passed && (state == NETOPT_STATE_STANDBY);*/
#endif
    // Check the state is standby
    // Check the channel is cf

    return passed;
}

bool test_mclmac_start_cf_phase(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    ARROW(mclmac->mac)cfChannel = 5;

    mclmac_change_cf_channel(mclmac);

    mclmac_start_cf_phase(mclmac);

    // Check for the frequency
    // Check the state of the radio is rx
    bool passed = true;
#ifdef __RIOT__
    /*uint8_t channel = (uint8_t)mclmac->mac.cfChannel;
    uint16_t radio_channel = 0;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_CHANNEL,
                                    (void *)&radio_channel, sizeof(uint16_t));
    passed = passed && (channel == (uint8_t)radio_channel);
    netopt_state_t state = NETOPT_STATE_RX, radio_state = NETOPT_STATE_OFF;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&state, sizeof(netopt_state_t));
    passed = passed && (state == radio_state);*/
#endif

    return passed;
}

bool test_mclmac_start_split_phase(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * We are now at either the TRANSMIT or RECEIVE state of the power mode, 
     * states at which the split phase is executed. It will send/receive
     * any packet to/from other nodes.
     * For this phase to execute, it should comply the following:
     *  -The frequency is already known.
     *  -Set the radio to TX or RX, depending on the state.
     *  -Initialize and clear the control packet.
     *  -The function can only be called from within the TRANSMIT or RECEIVE states,
     */
    int32_t ret = mclmac_start_split_phase(mclmac, STARTP);
    bool passed = true;
    passed = passed && (ret == -1);
    ret = mclmac_start_split_phase(mclmac, PASSIVE);
    passed = passed && (ret == -1);
    ret = mclmac_start_split_phase(mclmac, ACTIVE);
    passed = passed && (ret == -1);
    ret = mclmac_start_split_phase(mclmac, FINISHP);
    passed = passed && (ret == -1);

    ret = mclmac_start_split_phase(mclmac, TRANSMIT);
    passed = passed && (ret == 1);
#ifdef __RIOT__
    // Check the frequency of the radio
    /*uint16_t freq = (uint16_t)mclmac->mac.transmitChannel, channel = 0;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_CHANNEL,
                                    (void *)&channel, sizeof(uint16_t));
    // Check the state of the radio
    netopt_state_t state = NETOPT_STATE_OFF;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_STATE,
                                    (void*)&state, sizeof(netopt_state_t));
    passed = passed && (state == NETOPT_STATE_TX);*/
    // Check the contorl packet
    ControlPacket_t *pkt = &mclmac->mac.ctrlpkt;
    passed = passed && (pkt->node_id[0] == 0);
    passed = passed && (pkt->node_id[1] == 0);
    passed = passed && (pkt->currentFrame == 0);
    passed = passed && (pkt->currentSlot == 0);
    passed = passed && (pkt->collisionSlot == 0);
    passed = passed && (pkt->collisionFrequency == 0);
    passed = passed && (pkt->hopCount == 0);
    passed = passed && (pkt->networkTime == 0);
    passed = passed && (pkt->initTime == 0);
#endif
    ret = mclmac_start_split_phase(mclmac, RECEIVE);
    passed = passed && (ret == 1);
#ifdef __RIOT__
    // Check the frequency of the radio
    /*uint16_t freq = (uint16_t)mclmac->mac.receiveChannel, channel = 0;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_CHANNEL,
                                    (void *)&channel, sizeof(uint16_t));
    // Check the state of the radio
    netopt_state_t state = NETOPT_STATE_OFF;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_STATE,
                                    (void*)&state, sizeof(netopt_state_t));
    passed = passed && (state == NETOPT_STATE_RX);*/
    // Check the contorl packet
    pkt = &mclmac->mac.ctrlpkt;
    passed = passed && (pkt->node_id[0] == 0);
    passed = passed && (pkt->node_id[1] == 0);
    passed = passed && (pkt->currentFrame == 0);
    passed = passed && (pkt->currentSlot == 0);
    passed = passed && (pkt->collisionSlot == 0);
    passed = passed && (pkt->collisionFrequency == 0);
    passed = passed && (pkt->hopCount == 0);
    passed = passed && (pkt->networkTime == 0);
    passed = passed && (pkt->initTime == 0);
#endif

    return passed;
}

bool test_mclmac_set_radio_sleep(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * The radio should be set to sleep. This function is executed at the beginning of the
     * PASSIVE state. It should handle any requirements and workings related to the 
     * selected radio design.
     */
    mclmac_set_radio_sleep(mclmac);
    bool passed = true;
#ifdef __RIOT__
    /*netopt_state_t state = NETOPT_STATE_OFF;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&state, sizeof(netopt_state_t));
    passed = passed && (state == NETOPT_STATE_SLEEP);*/
#endif

    return passed;
}

bool test_mclmac_set_radio_standby(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * Set the radio state to standby.
     */
    mclmac_set_radio_standby(mclmac);
    bool passed = true;
#ifdef __RIOT__
    /*netopt_state_t state = NETOPT_STATE_OFF;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&state, sizeof(netopt_state_t));
    passed = passed && (state == NETOPT_STATE_STANDBY);*/
#endif

    return passed;
}

bool test_mclmac_set_radio_rx(void *arg)
{
    struct mclmac_data *data = (struct mclmac_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac_set_radio_rx(mclmac);
    bool passed = true;
#ifdef __RIOT__
    /*netopt_state_t state = NETOPT_STATE_OFF;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&state, sizeof(netopt_state_t));
    passed = passed && (state == NETOPT_STATE_RX);*/
#endif

    return passed;
}

void executeTestsMCLMAC(void)
{
    init_queues();

    cUnit_t *tests;
    struct mclmac_data data;
#if defined __RIOT__ && !defined NATIVE
    // Setup radios
    nrf24l01p_ng_t radio;
    nrf24l01p_ng_t radio_test;
    nrf24l01p_ng_params_t params = {
        .spi = SPI_DEV(0),
        .spi_clk = NRF24L01P_NG_PARAM_SPI_CLK,
        .pin_cs = GPIO5,
        .pin_ce = GPIO17,
        .pin_irq = GPIO21,
        .config = {
            .cfg_crc = NRF24L01P_NG_PARAM_CRC_LEN,
            .cfg_tx_power = NRF24L01P_NG_PARAM_TX_POWER,
            .cfg_data_rate = NRF24L01P_NG_PARAM_DATA_RATE,
            .cfg_channel = NRF24L01P_NG_PARAM_CHANNEL,
            .cfg_max_retr = NRF24L01P_NG_PARAM_MAX_RETRANSM,
            .cfg_retr_delay = NRF24L01P_NG_PARAM_RETRANSM_DELAY,
        }
    };
    nrf24l01p_ng_params_t params_test = {
        .spi = SPI_DEV(1),
        .spi_clk = NRF24L01P_NG_PARAM_SPI_CLK,
        .pin_cs = GPIO15,
        .pin_ce = GPIO2,
        .pin_irq = GPIO4,
        .config = {
            .cfg_crc = NRF24L01P_NG_PARAM_CRC_LEN,
            .cfg_tx_power = NRF24L01P_NG_PARAM_TX_POWER,
            .cfg_data_rate = NRF24L01P_NG_PARAM_DATA_RATE,
            .cfg_channel = NRF24L01P_NG_PARAM_CHANNEL,
            .cfg_max_retr = NRF24L01P_NG_PARAM_MAX_RETRANSM,
            .cfg_retr_delay = NRF24L01P_NG_PARAM_RETRANSM_DELAY,
        }
    };
    data.radio = &radio;
    data.radio_test = &radio_test;
    nrf24l01p_ng_setup(data.radio, &params, 2);
    nrf24l01p_ng_setup(data.radio_test, &params_test, 2);
    // Setup netdev
    data.netdev = &data.radio->netdev;
    data.netdev_test = &data.radio_test->netdev;
    data.radio->netdev.driver->init(data.netdev);
    data.radio_test->netdev.driver->init(data.netdev_test);

    printf("\nRadio info:\n");
    nrf24l01p_ng_diagnostics_print_dev_info(data.radio);
    printf("\nRadio test info:\n");
    nrf24l01p_ng_diagnostics_print_dev_info(data.radio_test);
#elif defined _RIOT__ && defined NATIVE
    data.netdev = NULL;
    data.netdev_test = NULL;
    data.radio = NULL;
    data.radio_test = NULL;
#elif defined __LINUX__
    data.radio = NULL;
#endif

    cunit_init(&tests, &setup_mclmac, &teardown_mclmac, (void *)&data);

    cunit_add_test(tests, &test_MCLMAC_init, "MCLMAC_init\0");
    cunit_add_test(tests, &test_MCLMAC_destroy, "MCLMAC_destroy\0");
    cunit_add_test(tests, &test_MCLMAC_clear, "MCLMAC_clear\0");
    cunit_add_test(tests, &test_mclmac_set_transmit_channel, "mclmac_set_transmit_channel\0");
    cunit_add_test(tests, &test_mclmac_get_transmit_channel, "mclmac_get_transmit_channel\0");
    cunit_add_test(tests, &test_mclmac_set_reception_channel, "mclmac_set_reception_channel\0");
    cunit_add_test(tests, &test_mclmac_get_reception_channel, "mclmac_get_reception_channel\0");
    cunit_add_test(tests, &test_mclmac_get_frequency, "mclmac_get_frequency\0");
    cunit_add_test(tests, &test_mclmac_get_nodeid, "mclmac_get_nodeid\0");
    cunit_add_test(tests, &test_mclmac_set_transmiterid, "mclmac_set_transmiterid\0");
    cunit_add_test(tests, &test_mclmac_set_selected_slot, "mclmac_set_selected_slot\0");
    cunit_add_test(tests, &test_mclmac_get_selected_slot, "mclmac_get_selected_slot\0");
    cunit_add_test(tests, &test_mclmac_set_current_frame, "mclmac_set_current_frame\0");
    cunit_add_test(tests, &test_mclmac_increase_frame, "mclmac_increase_frame\0");
    cunit_add_test(tests, &test_mclmac_set_current_slot, "mclmac_set_current_slot\0");
    cunit_add_test(tests, &test_mclmac_get_current_slot, "mclmac_get_current_slot\0");
    cunit_add_test(tests, &test_mclmac_increase_slot, "mclmac_increase_slot\0");
    cunit_add_test(tests, &test_mclmac_set_slots_number, "mclmac_set_slots_number\0");
    cunit_add_test(tests, &test_mclmac_set_cf_slots_number, "mclmac_set_cf_slots_number\0");
    cunit_add_test(tests, &test_mclmac_set_current_cf_slot, "mclmac_set_current_cf_slot\0");
    cunit_add_test(tests, &test_mclmac_get_current_cf_slot, "mclmac_get_current_cf_slot\0");
    cunit_add_test(tests, &test_mclmac_increase_cf_slot, "mclmac_increase_cf_slot\0");
    cunit_add_test(tests, &test_mclmac_set_slot_duration, "mclmac_set_slot_duration\0");
    cunit_add_test(tests, &test_mclmac_set_frame_duration, "mclmac_set_frame_duration\0");
    cunit_add_test(tests, &test_mclmac_set_cf_duration, "mclmac_set_cf_duration\0");
    cunit_add_test(tests, &test_mclmac_available_data_packets, "mclmac_available_data_packets\0");
    cunit_add_test(tests, &test_mclmac_read_queue_element, "mclmac_read_queue_element\0");
    cunit_add_test(tests, &test_mclmac_write_queue_element, "mclmac_write_queue_element\0");
    /*cunit_add_test(tests, &test_mclmac_change_cf_channel, "mclmac_change_cf_channel\0");
    cunit_add_test(tests, &test_mclmac_start_cf_phase, "mclmac_start_cf_phase\0");
    cunit_add_test(tests, &test_stub_mclmac_start_split_phase, "mclmac_start_split_phase\0");
    cunit_add_test(tests, &test_mclmac_set_radio_sleep, "mclmac_set_radio_sleep\0");
    cunit_add_test(tests, &test_mclmac_set_radio_standby, "mclmac_set_radio_standby\0");
    cunit_add_test(tests, &test_mclmac_set_radio_rx, "mclmac_set_radio_rx\0");*/

    cunit_execute_tests(tests);

    cunit_terminate(&tests);

    end_queues();
}
