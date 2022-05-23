#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include "string.h"

#include "MCLMAC.h"

#define ITERATIONS 1000

void test_MCLMAC_init(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;
    
    MCLMAC_init(&mclmac, &radio, nodeid);
#ifdef __LINUX__
    assert(mclmac != NULL);
    assert(mclmac->mac != NULL);
#endif
    assert(ARROW(mclmac)_nodeID == nodeid);
    assert(ARROW(mclmac)_networkTime == 0);
    assert(ARROW(mclmac)_nSlots == MAX_NUMBER_SLOTS);
    assert(ARROW(mclmac)_nChannels == MAX_NUMBER_FREQS);
    assert(ARROW(mclmac)_networkTime == 0);
    assert(ARROW(mclmac)_hopCount == 0);
    int n = MAX_NUMBER_FREQS;
    int m = (MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0);
    for (int i = 0; i < n; i++)
    {
        assert(ARROW(mclmac)_frequencies[i] >= 902000000 && ARROW(mclmac)_frequencies[i] <= 928000000);
        for (int j = 0; j < m; j++)
            assert(ARROW(mclmac)_occupied_frequencies_slots[i][j] == 0);
    }
#ifdef __LINUX__
    assert(mclmac->stack == NULL);
#endif
#ifdef __RIOT__
    assert(mclmac.stack != NULL);
#endif
    assert(ARROW(mclmac)_mac_queue_id != 0);
    assert(ARROW(mclmac)_routing_queue_id == 0);
    assert(ARROW(mclmac)_transport_queue_id == 0);
    assert(ARROW(mclmac)_app_queue_id == 0);

    MCLMAC_destroy(&mclmac);
}

void test_MCLMAC_destroy(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    MCLMAC_destroy(&mclmac);
#ifdef __LINUX__
    assert(mclmac == NULL);
#endif
}

void test_MCLMAC_clear(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    MCLMAC_clear(REFERENCE mclmac);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame == 0);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot == 0);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == 0);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)cf_slots_number == 0);
    assert(ARROW(ARROW(ARROW(mclmac)mac)frame)slots_number == 0);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    assert(ARROW(mclmac)macState.currentState == START);
    assert(ARROW(mclmac)_hopCount == 0);
    assert(ARROW(mclmac)_networkTime == 0);
    assert(ARROW(mclmac)_mac_queue_id == 0);
    assert(ARROW(mclmac)stack == NULL);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_transmit_channel(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t channel = (uint32_t)rand();
        mclmac_set_transmit_channel(REFERENCE mclmac, channel);
        assert(ARROW(ARROW(mclmac)mac)transmitChannel == channel);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_transmit_channel(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t channel = (uint32_t)rand();
        mclmac_set_transmit_channel(REFERENCE mclmac, channel);
        uint32_t channelS = mclmac_get_transmit_channel(REFERENCE mclmac);
        assert(channelS == channel);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_reception_channel(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t channel = (uint32_t)rand();
        mclmac_set_reception_channel(REFERENCE mclmac, channel);
        assert(ARROW(ARROW(mclmac)mac)receiveChannel == channel);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_reception_channel(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t channel = (uint32_t)rand();
        mclmac_set_reception_channel(REFERENCE mclmac, channel);
        uint32_t channelS = mclmac_get_reception_channel(REFERENCE mclmac);
        assert(channelS == channel);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_frequency(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    for (int i = 0; i < MAX_NUMBER_FREQS; i++)
    {
        uint32_t freq = mclmac_get_frequency(REFERENCE mclmac, i);
        assert(freq == ARROW(mclmac)_frequencies[i]);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_nodeid(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint16_t nodeid = (uint16_t)rand();
        ARROW(mclmac)_nodeID = nodeid;
        uint16_t nodeidR = mclmac_get_nodeid(REFERENCE mclmac);
        assert(nodeidR == nodeid);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_transmiterid(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 100;

    MCLMAC_init(&mclmac, &radio, nodeid);

    /**
     * Set the sender id, it should comply that id is different to the
     * current node's id.
     */
    uint16_t senderid = rand() % 16;
    senderid = (senderid == ARROW(mclmac)_nodeID ? senderid + 1 : senderid);
    mclmac_set_transmiterid(REFERENCE mclmac, senderid);
    assert(ARROW(ARROW(mclmac)mac)transmiterID == senderid);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_selected_slot(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = (uint8_t) rand() % 256;
        mclmac_set_selected_slot(REFERENCE mclmac, slot);
        assert(ARROW(ARROW(mclmac)mac)selectedSlot == slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_selected_slot(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = (uint8_t) rand() % 256;
        mclmac_set_selected_slot(REFERENCE mclmac, slot);
        uint8_t slotR = mclmac_get_selected_slot(REFERENCE mclmac);
        assert(slotR == slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_current_frame(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint32_t frame_number = (uint32_t)rand();
        mclmac_set_current_frame(REFERENCE mclmac, frame_number);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame == frame_number);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_increase_frame(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    uint32_t frame_number = 0;
    mclmac_set_current_frame(REFERENCE mclmac, frame_number);

    for (int i = 0; i < ITERATIONS; i++)
    {
        mclmac_increase_frame(REFERENCE mclmac);
        frame_number++;
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame == frame_number);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_current_slot(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = (uint8_t)rand() % 256;
        mclmac_set_current_slot(REFERENCE mclmac, slot);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot == slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_current_slot(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slot = (uint8_t)rand() % 256;
        mclmac_set_current_slot(REFERENCE mclmac, slot);
        uint8_t current_slot = mclmac_get_current_slot(REFERENCE mclmac);
        assert(current_slot == slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_increase_slot(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    uint8_t slot = 0;
    mclmac_set_current_slot(REFERENCE mclmac, slot);
    int n = rand() % 512;
    for (int i = 0; i < n; i++)
    {
        mclmac_increase_slot(REFERENCE mclmac);
        slot++;
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot == slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_slots_number(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t slots_number = (uint8_t) rand();
        slots_number = (slots_number == 0 ? 1 : slots_number);
        mclmac_set_slots_number(REFERENCE mclmac, slots_number);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)slots_number == slots_number);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_cf_slots_number(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t cf_slots_number = 1 + ((uint8_t) rand());
        cf_slots_number = (cf_slots_number == 0 ? 1 : cf_slots_number);
        mclmac_set_cf_slots_number(REFERENCE mclmac, cf_slots_number);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)cf_slots_number == cf_slots_number);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_current_cf_slot(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t current_cf_slot = ((uint8_t) rand() % 256);
        current_cf_slot = (current_cf_slot == 0 ? 1 : current_cf_slot);
        mclmac_set_current_cf_slot(REFERENCE mclmac, current_cf_slot);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == current_cf_slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_current_cf_slot(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t current_cf_slot = ((uint8_t) rand() % 256);
        current_cf_slot = (current_cf_slot == 0 ? 1 : current_cf_slot);
        mclmac_set_current_cf_slot(REFERENCE mclmac, current_cf_slot);
        uint8_t slot = mclmac_get_current_cf_slot(REFERENCE mclmac);
        assert(slot == current_cf_slot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_increase_cf_slot(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);
    
    uint8_t nCFSlot = rand();
    nCFSlot = (nCFSlot == 0 ? 1 : nCFSlot);
    mclmac_set_current_cf_slot(REFERENCE mclmac, nCFSlot);
    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        mclmac_increase_cf_slot(REFERENCE mclmac);
        nCFSlot++;
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)current_cf_slot == nCFSlot);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_slot_duration(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
    double slot_duration = 0;
#endif
#ifdef __RIOT__
    sx127x_t radio;
    uint32_t slot_duration = 0;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        slot_duration = rand();
        mclmac_set_slot_duration(REFERENCE mclmac, slot_duration);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)slot_duration == slot_duration);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_frame_duration(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
    double frame_duration;
#endif
#ifdef __RIOT__
    sx127x_t radio;
    uint32_t frame_duration;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        frame_duration = rand();
        mclmac_set_frame_duration(REFERENCE mclmac, frame_duration);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)frame_duration == frame_duration);
    }
    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_cf_duration(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
    double cf_duration;
#endif
#ifdef __RIOT__
    sx127x_t radio;
    uint32_t cf_duration;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % 1000;
    for (int i = 0; i < n; i++)
    {
        cf_duration = rand();
        mclmac_set_cf_duration(REFERENCE mclmac, cf_duration);
        assert(ARROW(ARROW(ARROW(mclmac)mac)frame)cf_duration == cf_duration);
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_available_data_packets(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    int n = rand() % ITERATIONS;
    for (int i = 0; i < n; i++)
    {
        uint8_t message_packets = rand() % MAX_NUMBER_DATA_PACKETS;
        uint8_t control_packets = rand() % MAX_NUMBER_DATA_PACKETS;
        ARROW(ARROW(mclmac)mac)_packets_to_send_message = message_packets;
        ARROW(ARROW(mclmac)mac)_packets_to_send_control = control_packets;
        uint16_t total_packets = mclmac_available_data_packets(REFERENCE mclmac);
        assert(total_packets == message_packets + control_packets);
    }

    MCLMAC_destroy(&mclmac);
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
    assert(nelements == 0);
    assert(ARROW(mclmac->mac)_packets_to_send_message == MAX_NUMBER_DATA_PACKETS);
    assert(ARROW(mclmac->mac)_packets_to_send_control == MAX_NUMBER_DATA_PACKETS);
    /** Test case 2:
     * The queue is empty. It should not increase the number of elements on the queue,
     * and the function should return 0.
    */
    ARROW(mclmac->mac)_packets_to_send_message = 0;
    ARROW(mclmac->mac)_packets_to_send_control = 0;
    nelements = mclmac_read_queue_element(mclmac);
    assert(nelements == 0);
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
    for (uint i = 1; i < size; i++)
        message[i] = rand();
    send_message(mclmac->_mac_queue_id, message, size, mclmac->_self_pid);
    nelements = mclmac_read_queue_element(mclmac);
    assert(nelements == 1);
    assert(ARROW(mclmac->mac)_first_send_message == 0);
    assert(ARROW(mclmac->mac)_last_send_message == 1);
    assert(ARROW(mclmac->mac)_packets_to_send_message == 1);
    DataPacket_t *pkt = &ARROW(mclmac->mac)_message_packets_to_send[0];
    uint16_t destination_id = (((uint16_t)message[1]) << 8) | (message[2]);
    assert(pkt->destination_id == destination_id);
    assert(pkt->type == message[0]);
    // Only the packet's data
    assert(pkt->size == message[3]);
    for (uint i = 0; i < pkt->size; i++)
        assert(READ_ARRAY(REFERENCE pkt->data, i) == message[i + 4]);
    // Type 1, a control packet
    message[0] = 2;
    send_message(mclmac->_mac_queue_id, message, size, mclmac->_self_pid);
    nelements = mclmac_read_queue_element(mclmac);
    assert(nelements == 1);
    assert(ARROW(mclmac->mac)_first_send_control == 0);
    assert(ARROW(mclmac->mac)_last_send_control == 1);
    assert(ARROW(mclmac->mac)_packets_to_send_control == 1);
    pkt = &ARROW(mclmac->mac)_control_packets_to_send[0];
    assert(pkt->destination_id == destination_id);
    assert(pkt->size == message[3]);
    for (uint i = 0; i < pkt->size; i++)
        assert(READ_ARRAY(REFERENCE pkt->data, i) == message[i + 4]);
    
    // Invalid packet type
    message[0] = rand() + 10;
    send_message(mclmac->_mac_queue_id, message, size, mclmac->_self_pid);
    nelements = mclmac_read_queue_element(mclmac);
    assert(nelements == 0);
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
void test_mclmac_read_queue_element(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

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
    kernel_pid_t pid = thread_create(mclmac.stack, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1,
                        THREAD_CREATE_SLEEPING, read_queue, (void *)&mclmac, "Read queue");
    thread_wakeup(pid);
    while (thread_getstatus(pid) == STATUS_RUNNING) ;
#endif

    MCLMAC_destroy(&mclmac);
}

void _write_queue(MCLMAC_t *mclmac)
{
    open_queue(mclmac->_mac_queue_id, mclmac->_self_pid);

    /**
     * Test case 0:
     *  -The routing layer is not open yet. Return 0.
     */
    int ret = mclmac_write_queue_element(mclmac);
    assert(ret == 0);

    /* The message will be sent to the routing layer, for the moment simulate
     that the mac queue is the routing queue. */
    mclmac->_routing_queue_id = mclmac->_mac_queue_id;
    /**
     * Test case 1:
     *  -_elements_read = 0.
     * It should return 0.
    */
    ret = mclmac_write_queue_element(mclmac);
    assert(ret == 0);

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
    assert(ret == 0);
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
    uint16_t destination_id = rand();
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
    assert(ret == 1);
    assert(ARROW(mclmac->mac)_number_packets_received == 0);
    assert(ARROW(mclmac->mac)_first_received == 1);
    assert(ARROW(mclmac->mac)_last_received == 1);
    assert(pkt->type == -1);
    assert(pkt->destination_id == 0);
    assert(pkt->size == 0);
#ifdef __LINUX__
    assert(pkt->data == NULL);
#endif
#ifdef __RIOT__
    assert(pkt->data.size == 0);
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
        assert(ret == 1);
    }
    assert(ARROW(mclmac->mac)_number_packets_received == 0);
    assert(ARROW(mclmac->mac)_first_received == 0);
    assert(ARROW(mclmac->mac)_last_received == 0);
    for (i = 0; i < MAX_NUMBER_DATA_PACKETS; i++)
    {
        DataPacket_t *pkt = &ARROW(mclmac->mac)_packets_received[i];
        assert(pkt->type == -1);
        assert(pkt->destination_id == 0);
        assert(pkt->size == 0);
#ifdef __LINUX__
        assert(pkt->data == NULL);
#endif
#ifdef __RIOT__
        assert(pkt->data.size == 0);
#endif
    }
#ifdef __LINUX__
    free(data);
#endif
#ifdef __RIOT__
    free_array(&data);
#endif
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
void test_mclmac_write_queue_element(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

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
    kernel_pid_t pid = thread_create(mclmac.stack, THREAD_STACKSIZE_DEFAULT, THREAD_PRIORITY_MAIN - 1,
                                  THREAD_CREATE_SLEEPING, write_queue, (void *)&mclmac, "Write Queue");
    thread_wakeup(pid);
    while (thread_getstatus(pid) == STATUS_RUNNING) ;
#endif

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_change_cf_channel(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    ARROW(ARROW(mclmac)mac)cfChannel = 915000000;

    stub_mclmac_change_cf_channel(REFERENCE mclmac);
    
    // Check the state is standby
    // Check the channel is cf

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_start_cf_phase(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    ARROW(ARROW(mclmac)mac)cfChannel = 915000000;

    stub_mclmac_change_cf_channel(REFERENCE mclmac);

    stub_mclmac_start_cf_phase(REFERENCE mclmac);

    // Check for the frequency

    // Check the state of the radio is rx-single

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_send_cf_message(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

    ARROW(mclmac)_nodeID = 1;
//    mclmac_set_destination_id(REFERENCE mclmac, 2);
//    mclmac_create_cf_packet(REFERENCE mclmac);

    ARROW(ARROW(mclmac)mac)cfChannel = 915000000;

    stub_mclmac_change_cf_channel(REFERENCE mclmac);

    stub_mclmac_start_cf_phase(REFERENCE mclmac);

    /* Send the packet the destination id is already known, and the 
       cf phase is already started. */
    stub_mclmac_send_cf_message(REFERENCE mclmac);

    MCLMAC_destroy(&mclmac);
}

void test_stub_mclmac_start_split_phase(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;

    MCLMAC_init(&mclmac, &radio, nodeid);

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
    int32_t ret = stub_mclmac_start_split_phase(REFERENCE mclmac, STARTP);
    assert(ret == -1);
    ret = stub_mclmac_start_split_phase(REFERENCE mclmac, PASSIVE);
    assert(ret == -1);
    ret = stub_mclmac_start_split_phase(REFERENCE mclmac, ACTIVE);
    assert(ret == -1);
    ret = stub_mclmac_start_split_phase(REFERENCE mclmac, FINISHP);
    assert(ret == -1);

    ret = stub_mclmac_start_split_phase(REFERENCE mclmac, TRANSMIT);
    assert(ret == 1);
    ret = stub_mclmac_start_split_phase(REFERENCE mclmac, RECEIVE);
    assert(ret == 1);

    MCLMAC_destroy(&mclmac);
}

void executeTestsMCLMAC(void)
{
    srand(time(NULL));
    init_queues();

    printf("Testing MAC_init function.\n");
    test_MCLMAC_init();
    printf("Test passed.\n");

    printf("Testing MCLMAC_destroy function.\n");
    test_MCLMAC_destroy();
    printf("Test passed.\n");

    printf("Test MCLMAC_clear function.\n");
    test_MCLMAC_clear();
    printf("Test passed.\n");

    printf("Testing mclmac_set_transmit_channel function.\n");
    test_mclmac_set_transmit_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_get_transmite_channel function.\n");
    test_mclmac_get_transmit_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_set_reception_channel function.\n");
    test_mclmac_set_reception_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_get_reception_channel function.\n");
    test_mclmac_get_reception_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_get_frequency function.\n");
    test_mclmac_get_frequency();
    printf("Test passed.\n");

    printf("Testing mclmac_get_nodeid function.\n");
    test_mclmac_get_nodeid();
    printf("Test passed.\n");

    printf("Testing mclmac_set_transmiterid function.\n");
    test_mclmac_set_transmiterid();
    printf("Test passed.\n");

    printf("Testing mclmac_set_selected_slot function.\n");
    test_mclmac_set_selected_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_get_selected_slot function.\n");
    test_mclmac_get_selected_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_set_current_frame function.\n");
    test_mclmac_set_current_frame();
    printf("Test passed.\n");

    printf("Test mclmac_increase_frame function.\n");
    test_mclmac_increase_frame();
    printf("Test passed.\n");

    printf("Testing mclmac_set_current_slot function.\n");
    test_mclmac_set_current_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_get_current_slot function.\n");
    test_mclmac_get_current_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_increase_slot function.\n");
    test_mclmac_increase_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_set_slots_number function.\n");
    test_mclmac_set_slots_number();
    printf("Test passed.\n");

    printf("Testing mclmac_set_cf_slots_number function.\n");
    test_mclmac_set_cf_slots_number();
    printf("Test passed.\n");

    printf("Testing mclmac_set_current_cf_slot function.\n");
    test_mclmac_set_current_cf_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_get_current_cf_slot function.\n");
    test_mclmac_get_current_cf_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_increase_cf_slot function.\n");
    test_mclmac_increase_cf_slot();
    printf("Test passed.\n");

    printf("Testing mclmac_set_slot_duration function.\n");
    test_mclmac_set_slot_duration();  
    printf("Test passed.\n");

    printf("Testing mclmac_set_frame_duration function.\n");
    test_mclmac_set_frame_duration();
    printf("Test passed.\n");

    printf("Testing mclmac_set_cf_duration function.\n");
    test_mclmac_set_cf_duration();
    printf("Test passed.\n");

    printf("Testing the mclmac_available_data_packets function.\n");
    test_mclmac_available_data_packets();
    printf("Test passed.\n");

    printf("Testing mclmac_read_queue_element function.\n");
    test_mclmac_read_queue_element();
    printf("Test passed.\n");

    printf("Testing mclmac_write_queue_element function.\n");
    test_mclmac_write_queue_element();
    printf("Test passed.\n");

    printf("Testing mclmac_change_cf_channel function.\n");
    test_mclmac_change_cf_channel();
    printf("Test passed.\n");

    printf("Testing mclmac_start_cf_phase function.\n");
    test_mclmac_start_cf_phase();
    printf("Test passed.\n");

    printf("Testing mclmac_send_cf_message function.\n");
    test_mclmac_send_cf_message();
    printf("Test passed.\n");

    printf("Testing the stub_mclmac_start_split_phase function.\n");
    test_stub_mclmac_start_split_phase();
    printf("Test passed.\n");

    end_queues();
    return;
}
