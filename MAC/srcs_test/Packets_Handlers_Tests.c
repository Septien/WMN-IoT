#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include "string.h"

#include "MCLMAC.h"

#include "cUnit.h"

struct packethandlers_data{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid;
};

void setup_packet_handlers(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    data->nodeid = 0;
    MCLMAC_init(&data->mclmac, &data->radio, data->nodeid);
}

void teardown_packet_handlers(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_destroy(&data->mclmac);
}

void test_stub_mclmac_receive_ctrlpkt_sync(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * This function will receive the packets from the network.
     * The data of interest is:
     *      -The network time.
     *      -The number of hops (all within the same range.)
     *      -The current slot and current frequency.
     *      -The frequency and slot at which the packet was received.
     *  All other information is discarded.
     */
    ControlPacket_t SINGLE_POINTER ctrlpkt;

    controlpacket_init(&ctrlpkt);
    mclmac->_initTime = rand() % 100;
    mclmac->_networkTime = (rand() % 1000) + mclmac->_initTime;
    mclmac->_hopCount = rand() % 10;
    ARROW(ARROW(mclmac->mac)frame)current_frame = rand() % 10;
    ARROW(ARROW(mclmac->mac)frame)current_slot = rand() % MAX_NUMBER_SLOTS;
    stub_mclmac_receive_ctrlpkt_sync(mclmac, REFERENCE ctrlpkt);
    assert(ARROW(ctrlpkt)networkTime == mclmac->_networkTime);
    assert(ARROW(ctrlpkt)hopCount == mclmac->_hopCount);
    assert(ARROW(ctrlpkt)initTime == mclmac->_initTime);
    assert(ARROW(ctrlpkt)currentSlot == ARROW(ARROW(mclmac->mac)frame)current_slot);
    assert(ARROW(ctrlpkt)currentFrame == ARROW(ARROW(mclmac->mac)frame)current_frame);

    controlpacket_destroy(&ctrlpkt);
}

void test_mclmac_create_control_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * We are at the TRANSMIT or RECEIVE state, and we want to create
     * a control packet for subsequent sending.
     * Create the packet with the available variables on the MCLMAC data structure.
     */
    mclmac->_nodeID = rand();
    mclmac->_networkTime = rand();
    mclmac->_initTime = rand();
    mclmac->_hopCount = rand();
    ARROW(ARROW(mclmac->mac)frame)current_slot = rand();
    ARROW(ARROW(mclmac->mac)frame)current_frame = rand();
    ARROW(mclmac->mac)_collisionSlot = rand();
    ARROW(mclmac->mac)_collisionFrequency = rand();
    ControlPacket_t *pkt = REFERENCE ARROW(mclmac->mac)ctrlpkt;
    mclmac_create_control_packet(mclmac);
    assert(pkt->nodeID == mclmac->_nodeID);
    assert(pkt->currentFrame == ARROW(ARROW(mclmac->mac)frame)current_frame);
    assert(pkt->currentSlot == ARROW(ARROW(mclmac->mac)frame)current_slot);
    assert(pkt->collisionSlot == ARROW(mclmac->mac)_collisionSlot);
    assert(pkt->collisionFrequency == ARROW(mclmac->mac)_collisionFrequency);
    assert(pkt->hopCount == mclmac->_hopCount);
    assert(pkt->networkTime == mclmac->_networkTime);
    assert(pkt->initTime == mclmac->_initTime);

}

void test_stub_mclmac_send_control_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac->_nodeID = rand();
    mclmac->_networkTime = rand();
    mclmac->_initTime = rand();
    mclmac->_hopCount = rand();
    ARROW(ARROW(mclmac->mac)frame)current_slot = rand();
    ARROW(ARROW(mclmac->mac)frame)current_frame = rand();
    ARROW(mclmac->mac)_collisionSlot = rand();
    ARROW(mclmac->mac)_collisionFrequency = rand();
    mclmac_create_control_packet(mclmac);
    /**
     * The control packet was already created. We now need to get the 
     * byte string from the packet, and send it to the medium. The packet
     * should be cleared after wards.
     */
    ControlPacket_t *pkt = REFERENCE ARROW(mclmac->mac)ctrlpkt;
    stub_mclmac_send_control_packet(mclmac);
    assert(pkt->nodeID == 0);
    assert(pkt->currentFrame == 0);
    assert(pkt->currentSlot == 0);
    assert(pkt->collisionSlot == 0);
    assert(pkt->collisionFrequency == 0);
    assert(pkt->hopCount == 0);
    assert(pkt->networkTime == 0);
    assert(pkt->initTime == 0);
}

void test_stub_mclmac_receive_control_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac->_hopCount = rand();
    mclmac->_networkTime = rand();
    mclmac->_initTime = rand();
    mclmac_set_current_frame(mclmac, rand());
    mclmac_set_current_slot(mclmac, rand());

    /**
     * Receive a control packet from the medium.
     * Three cases:
     *      -The control packet's nodeid is the same as the transmitter_id, and has the
     *      correct values on the fields.
     *      -The control packet is destined for the current node, but the
     *      fields are different from those of the current field.
     *      -The collision frequency and slot is different to zero, and are equal to the
     *      node's ones. This is a case of collision.
     *      -The following variables are the same as the node's ones: currentSlot, 
     *       currentFrame, networkTime. This is a case of synchronization error.
     *      -The packet is not destined for the current node.
     */

    mclmac->_state_ctrl = 1;
    stub_mclmac_receive_control_packet(mclmac);
    ControlPacket_t *ctrlpkt = REFERENCE ARROW(mclmac->mac)ctrlpkt_recv;
    assert(ctrlpkt->nodeID == ARROW(mclmac->mac)transmiterID);
    assert(ctrlpkt->currentFrame == ARROW(ARROW(mclmac->mac)frame)current_frame);
    assert(ctrlpkt->currentSlot == ARROW(ARROW(mclmac->mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot == 0);
    assert(ctrlpkt->collisionFrequency == 0);
    assert(ctrlpkt->hopCount == mclmac->_hopCount);
    assert(ctrlpkt->networkTime == mclmac->_networkTime);
    assert(ctrlpkt->initTime == mclmac->_initTime);

    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 2;
    stub_mclmac_receive_control_packet(mclmac);
    assert(ctrlpkt->nodeID == ARROW(mclmac->mac)transmiterID);
    assert(ctrlpkt->currentFrame != ARROW(ARROW(mclmac->mac)frame)current_frame);
    assert(ctrlpkt->currentSlot != ARROW(ARROW(mclmac->mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot != 0);
    assert(ctrlpkt->collisionFrequency != 0);
    assert(ctrlpkt->hopCount != mclmac->_hopCount);
    assert(ctrlpkt->networkTime != mclmac->_networkTime);
    assert(ctrlpkt->initTime != mclmac->_initTime);

    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 3;
    stub_mclmac_receive_control_packet(mclmac);
    assert(ctrlpkt->nodeID == ARROW(mclmac->mac)transmiterID);
    assert(ctrlpkt->currentFrame == ARROW(ARROW(mclmac->mac)frame)current_frame);
    assert(ctrlpkt->currentSlot == ARROW(ARROW(mclmac->mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot == ARROW(mclmac->mac)selectedSlot);
    assert(ctrlpkt->collisionFrequency == ARROW(mclmac->mac)transmitChannel);
    assert(ctrlpkt->hopCount == mclmac->_hopCount);
    assert(ctrlpkt->networkTime == mclmac->_networkTime);
    assert(ctrlpkt->initTime == mclmac->_initTime);

    /* current slot is differet */
    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 4;
    stub_mclmac_receive_control_packet(mclmac);
    assert(ctrlpkt->nodeID == ARROW(mclmac->mac)transmiterID);
    assert(ctrlpkt->currentFrame == ARROW(ARROW(mclmac->mac)frame)current_frame);
    assert(ctrlpkt->currentSlot != ARROW(ARROW(mclmac->mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot == ARROW(mclmac->mac)selectedSlot);
    assert(ctrlpkt->collisionFrequency == ARROW(mclmac->mac)transmitChannel);
    assert(ctrlpkt->hopCount == mclmac->_hopCount);
    assert(ctrlpkt->networkTime == mclmac->_networkTime);
    assert(ctrlpkt->initTime == mclmac->_initTime);

    /* current frame is differen */
    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 5;
    stub_mclmac_receive_control_packet(mclmac);
    assert(ctrlpkt->nodeID == ARROW(mclmac->mac)transmiterID);
    assert(ctrlpkt->currentFrame != ARROW(ARROW(mclmac->mac)frame)current_frame);
    assert(ctrlpkt->currentSlot == ARROW(ARROW(mclmac->mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot == ARROW(mclmac->mac)selectedSlot);
    assert(ctrlpkt->collisionFrequency == ARROW(mclmac->mac)transmitChannel);
    assert(ctrlpkt->hopCount == mclmac->_hopCount);
    assert(ctrlpkt->networkTime == mclmac->_networkTime);
    assert(ctrlpkt->initTime == mclmac->_initTime);

    /* network time */
    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 6;
    stub_mclmac_receive_control_packet(mclmac);
    assert(ctrlpkt->nodeID == ARROW(mclmac->mac)transmiterID);
    assert(ctrlpkt->currentFrame == ARROW(ARROW(mclmac->mac)frame)current_frame);
    assert(ctrlpkt->currentSlot == ARROW(ARROW(mclmac->mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot == ARROW(mclmac->mac)selectedSlot);
    assert(ctrlpkt->collisionFrequency == ARROW(mclmac->mac)transmitChannel);
    assert(ctrlpkt->hopCount == mclmac->_hopCount);
    assert(ctrlpkt->networkTime != mclmac->_networkTime);
    assert(ctrlpkt->initTime == mclmac->_initTime);

    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 0;
    stub_mclmac_receive_control_packet(mclmac);
    assert(ctrlpkt->nodeID != ARROW(mclmac->mac)transmiterID);
    assert(ctrlpkt->currentFrame != ARROW(ARROW(mclmac->mac)frame)current_frame);
    assert(ctrlpkt->currentSlot != ARROW(ARROW(mclmac->mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot != 0);
    assert(ctrlpkt->collisionFrequency != 0);
    assert(ctrlpkt->hopCount != mclmac->_hopCount);
    assert(ctrlpkt->networkTime != mclmac->_networkTime);
    assert(ctrlpkt->initTime != mclmac->_initTime);
}

void test_stub_mclmac_send_data_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    size_t size = MAX_MESSAGE_SIZE;
    uint8_t message[size];
    // Store the type of message
    message[0] = 7;
    uint i;
    for (i = 1; i < size; i++)
        message[i] = rand();
    ARRAY byteString;
#ifdef __LINUX__
    byteString = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteString, size);
#endif
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE byteString, message[i], i);
    // Store packets on array
    uint n = MAX_NUMBER_DATA_PACKETS;
    for (i = 0; i < n; i++)
    {
        DataPacket_t *pkt = &ARROW(mclmac->mac)_message_packets_to_send[i];
        datapacket_construct_from_bytestring(pkt, &byteString);
    }
    ARROW(mclmac->mac)_packets_to_send_message = 0;
    stub_mclmac_send_data_packet(mclmac);
    assert(ARROW(mclmac->mac)_first_send_message == ARROW(mclmac->mac)_last_send_message);

    ARROW(mclmac->mac)_packets_to_send_message = n;
    uint8_t first_send = ARROW(mclmac->mac)_first_send_message;
    uint8_t last_send = ARROW(mclmac->mac)_last_send_message;
    uint8_t packet_sent = ARROW(mclmac->mac)_packets_to_send_message;
    for (i = 0; i < n; i++)
    {
        stub_mclmac_send_data_packet(mclmac);
        packet_sent--;
        assert(ARROW(mclmac->mac)_packets_to_send_message == packet_sent);
        first_send = (first_send + 1) % MAX_NUMBER_DATA_PACKETS;
        assert(ARROW(mclmac->mac)_first_send_message == first_send);
        DataPacket_t *pkt = &ARROW(mclmac->mac)_message_packets_to_send[i];
        assert(pkt->type == -1);
        assert(pkt->size == 0);
        assert(pkt->destination_id == 0);
#ifdef __LINUX__
        assert(pkt->data == NULL);
#endif
#ifdef __RIOT__
        assert(pkt->data.size == 0);
#endif
    }
    assert(ARROW(mclmac->mac)_packets_to_send_message == 0);
    assert(ARROW(mclmac->mac)_last_send_message == last_send);

#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
#endif
}

void test_stub_mclmac_receive_data_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * Fill the array _packets_received with random data, 
     * assuring that the destination_id corresponds with the node's id.
     */
    int i;
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++)
        stub_mclmac_receive_data_packet(mclmac);
    assert(ARROW(mclmac->mac)_number_packets_received == MAX_ELEMENTS_ON_QUEUE);
    assert(ARROW(mclmac->mac)_first_received == 0);
    assert(ARROW(mclmac->mac)_last_received == MAX_ELEMENTS_ON_QUEUE);
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++)
    {
        DataPacket_t *pkt = &ARROW(mclmac->mac)_packets_received[i];
        assert(pkt->type > 0);
        assert(pkt->destination_id == mclmac->_nodeID);
        assert(pkt->size > 0);
#ifdef __LINUX__
        assert(pkt->data != NULL);
#endif
#ifdef __RIOT__
        assert(pkt->data.size > 0);
#endif
    }
}

void test_stub_mclmac_send_layers_control_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    size_t size = MAX_MESSAGE_SIZE;
    uint8_t message[size];
    // Store the type of message
    message[0] = 2 + (rand() % 7);
    uint i;
    for (i = 1; i < size; i++)
        message[i] = rand();
    ARRAY byteString;
#ifdef __LINUX__
    byteString = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteString, size);
#endif
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE byteString, message[i], i);
    // Store packets on array
    uint n = MAX_NUMBER_DATA_PACKETS;
    for (i = 0; i < n; i++)
    {
        DataPacket_t *pkt = &ARROW(mclmac->mac)_control_packets_to_send[i];
        datapacket_construct_from_bytestring(pkt, &byteString);
    }
    ARROW(mclmac->mac)_packets_to_send_control = 0;
    stub_mclmac_send_data_packet(mclmac);
    assert(ARROW(mclmac->mac)_first_send_control == ARROW(mclmac->mac)_last_send_control);

    ARROW(mclmac->mac)_packets_to_send_control = n;
    uint8_t first_send = ARROW(mclmac->mac)_first_send_control;
    uint8_t last_send = ARROW(mclmac->mac)_last_send_control;
    uint8_t packet_sent = ARROW(mclmac->mac)_packets_to_send_control;
    for (i = 0; i < n; i++)
    {
        stub_mclmac_send_layers_control_packet(mclmac);
        packet_sent--;
        assert(ARROW(mclmac->mac)_packets_to_send_control == packet_sent);
        first_send = (first_send + 1) % MAX_NUMBER_DATA_PACKETS;
        assert(ARROW(mclmac->mac)_first_send_control == first_send);
        DataPacket_t *pkt = &ARROW(mclmac->mac)_control_packets_to_send[i];
        assert(pkt->type == -1);
        assert(pkt->size == 0);
        assert(pkt->destination_id == 0);
#ifdef __LINUX__
        assert(pkt->data == NULL);
#endif
#ifdef __RIOT__
        assert(pkt->data.size == 0);
#endif
    }
    assert(ARROW(mclmac->mac)_packets_to_send_control == 0);
    assert(ARROW(mclmac->mac)_last_send_control == last_send);

#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
#endif
}

void executetests_packets_handlers(void)
{
    init_queues();

    cUnit_t *tests;
    struct packethandlers_data data;

    cunit_init(&tests, &setup_packet_handlers, &teardown_packet_handlers, (void *)&data);

    cunit_add_test(tests, &test_stub_mclmac_receive_ctrlpkt_sync, "stub_mclmac_receive_ctrlpkt_sync\0");
    cunit_add_test(tests, &test_mclmac_create_control_packet, "mclmac_create_control_packet\0");
    cunit_add_test(tests, &test_stub_mclmac_send_control_packet, "stub_mclmac_send_control_packet\0");
    cunit_add_test(tests, &test_stub_mclmac_receive_control_packet, "stub_mclmac_receive_control_packet\0");
    cunit_add_test(tests, &test_stub_mclmac_send_data_packet, "stub_mclmac_send_data_packet\0");
    cunit_add_test(tests, &test_stub_mclmac_receive_data_packet, "stub_mclmac_receive_data_packet\0");
    cunit_add_test(tests, &test_stub_mclmac_send_layers_control_packet, "stub_mclmac_send_layers_control_packet\0");

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
    end_queues();
}
