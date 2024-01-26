#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "string.h"

#include "MCLMAC.h"

#include "cUnit.h"


struct packethandlers_data{
    MCLMAC_t mclmac;
};

void setup_packet_handlers(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;

    MCLMAC_init(&data->mclmac);
}

void teardown_packet_handlers(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;

    MCLMAC_destroy(&data->mclmac);
}

bool test_mclmac_start_packet_detection(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = &data->mclmac;

    /**
     * Function used for the INITIALIZATION state of the MAC state machine.
     * It should be called only once at the beginning of the state.
     * Configure the radio so it can begin the detection of cf packets.
     */
    bool passed = true;
    mclmac_start_packet_detection(mclmac);

    return passed;
}

bool test_mclmac_cf_packet_detected(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = &data->mclmac;

    /**
     * This function detects cf packets on the medium. The radio should already have
     * the broadcast address, the channel, and in state rx.
     * It will retrieve the size of the packet on the radio, and
     * if it is 32 bytes, it will return true. Otherwise will return false.
     */
    /* Configure test radio. */
    printf("Configuring test radio.\n");
    bool passed = true;
#ifdef __LINUX__
    (void) mclmac;
#endif

    return passed;
}

bool test_mclmac_receive_ctrlpkt_sync(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = &data->mclmac;

    /**
     * This function will receive the packets from the network.
     * The data of interest is:
     *      -The network time.
     *      -The number of hops (all within the same range.)
     *      -The current slot and current frequency.
     *      -The frequency and slot at which the packet was received.
     *  All other information is discarded.
     */
    ControlPacket_t ctrlpkt, *ctrlpkt_p;
    ctrlpkt_p = &ctrlpkt;

    controlpacket_init(&ctrlpkt_p);
    mclmac->_initTime = rand() % 100;
    mclmac->_networkTime = (rand() % 1000) + mclmac->_initTime;
    mclmac->_hopCount = rand() % 10;
    mclmac->mac.frame.current_frame = rand() % 10;
    mclmac->mac.frame.current_slot = rand() % MAX_NUMBER_SLOTS;
    mclmac_receive_ctrlpkt_sync(mclmac,  ctrlpkt_p);
    bool passed = true;
    passed = passed && (ctrlpkt_p->networkTime == mclmac->_networkTime);
    passed = passed && (ctrlpkt_p->hopCount == mclmac->_hopCount);
    passed = passed && (ctrlpkt_p->initTime == mclmac->_initTime);
    passed = passed && (ctrlpkt_p->currentSlot == mclmac->mac.frame.current_slot);
    passed = passed && (ctrlpkt_p->currentFrame == mclmac->mac.frame.current_frame);
    passed = passed && (memcmp(ctrlpkt_p->occupied_slots, mclmac->_occupied_frequencies_slots, sizeof(mclmac->_occupied_frequencies_slots)) == 0);

    controlpacket_destroy(&ctrlpkt_p);

    return passed;
}

bool test_mclmac_create_control_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = &data->mclmac;

    /**
     * We are at the TRANSMIT or RECEIVE state, and we want to create
     * a control packet for subsequent sending.
     * Create the packet with the available variables on the MCLMAC data structure.
     */
    mclmac->_node_id[0] = rand();
    mclmac->_node_id[1] = rand();
    mclmac->_networkTime = rand();
    mclmac->_initTime = rand();
    mclmac->_hopCount = rand();
    mclmac->mac.frame.current_slot = rand();
    mclmac->mac.frame.current_frame = rand();
    mclmac->mac._collisionSlot = rand();
    mclmac->mac._collisionFrequency = rand();
    ControlPacket_t *pkt = &mclmac->mac.ctrlpkt;
    mclmac_create_control_packet(mclmac);
    bool passed = true;
    passed = passed && (pkt->node_id[0] == mclmac->_node_id[0]);
    passed = passed && (pkt->node_id[1] == mclmac->_node_id[1]);
    passed = passed && (pkt->currentFrame == mclmac->mac.frame.current_frame);
    passed = passed && (pkt->currentSlot == mclmac->mac.frame.current_slot);
    passed = passed && (pkt->collisionSlot == mclmac->mac._collisionSlot);
    passed = passed && (pkt->collisionFrequency == mclmac->mac._collisionFrequency);
    passed = passed && (pkt->hopCount == mclmac->_hopCount);
    passed = passed && (pkt->networkTime == mclmac->_networkTime);
    passed = passed && (pkt->initTime == mclmac->_initTime);
    passed = passed && (memcmp(mclmac->_occupied_frequencies_slots, pkt->occupied_slots, sizeof(mclmac->_occupied_frequencies_slots)) == 0);
    return passed;
}

bool test_mclmac_send_cf_message(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = &data->mclmac;

    mclmac->_node_id[0] = rand();
    mclmac->_node_id[1] = rand();
//    mclmac_set_destination_id( mclmac, 2);
//    mclmac_create_cf_packet( mclmac);

    mclmac->mac.cfChannel = 915000000;

    mclmac_change_cf_channel(mclmac);

    mclmac_start_cf_phase(mclmac);

    /* Send the packet the destination id is already known, and the 
       cf phase is already started. */
    mclmac_send_cf_message(mclmac);
    CFPacket_t *pkt = &mclmac->mac._cf_messages[0];
    bool passed = true;

    return passed;
}

bool test_mclmac_receive_cf_message(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = &data->mclmac;

    bool ret = mclmac_receive_cf_message(mclmac);
    bool passed = true;
    passed = passed && ret;

    return passed;
}

bool test_mclmac_send_control_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = &data->mclmac;

    mclmac->_node_id[0] = rand();
    mclmac->_node_id[1] = rand();
    mclmac->_networkTime = rand();
    mclmac->_initTime = rand();
    mclmac->_hopCount = rand();
    mclmac->mac.frame.current_slot = rand();
    mclmac->mac.frame.current_frame = rand();
    mclmac->mac._collisionSlot = rand();
    mclmac->mac._collisionFrequency = rand();
    mclmac_create_control_packet(mclmac);
    /**
     * The control packet was already created. We now need to get the 
     * byte string from the packet, and send it to the medium. The packet
     * should be cleared after wards.
     */
    ControlPacket_t *pkt = &mclmac->mac.ctrlpkt;
    mclmac_send_control_packet(mclmac);
    bool passed = true;
    passed = passed && (pkt->node_id[0] == 0);
    passed = passed && (pkt->node_id[1] == 0);
    passed = passed && (pkt->currentFrame == 0);
    passed = passed && (pkt->currentSlot == 0);
    passed = passed && (pkt->collisionSlot == 0);
    passed = passed && (pkt->collisionFrequency == 0);
    passed = passed && (pkt->hopCount == 0);
    passed = passed && (pkt->networkTime == 0);
    passed = passed && (pkt->initTime == 0);

    return passed;
}

bool test_mclmac_receive_control_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = &data->mclmac;

    mclmac->_hopCount = rand();
    mclmac->_networkTime = rand();
    mclmac->_initTime = rand();
    mclmac_set_current_frame(mclmac, rand());
    mclmac_set_current_slot(mclmac, rand());
    mclmac_set_slot_duration(mclmac, TIME(SLOT_DURATION));

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
    bool ret = mclmac_receive_control_packet(mclmac);
    bool passed = true;
    passed = passed && (ret == true);
    ControlPacket_t *ctrlpkt = &mclmac->mac.ctrlpkt_recv;
    passed = passed && (ctrlpkt->node_id[0] == mclmac->mac.transmitter_id[0]);
    passed = passed && (ctrlpkt->node_id[1] == mclmac->mac.transmitter_id[1]);
    passed = passed && (ctrlpkt->currentFrame == mclmac->mac.frame.current_frame);
    passed = passed && (ctrlpkt->currentSlot == mclmac->mac.frame.current_slot);
    passed = passed && (ctrlpkt->collisionSlot == 0);
    passed = passed && (ctrlpkt->collisionFrequency == 0);
    passed = passed && (ctrlpkt->hopCount == mclmac->_hopCount);
    passed = passed && (ctrlpkt->networkTime == mclmac->_networkTime);
    passed = passed && (ctrlpkt->initTime == mclmac->_initTime);

    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 2;
    mclmac_receive_control_packet(mclmac);
    passed = passed && (ret == true);
    passed = passed && (ctrlpkt->node_id[0] == mclmac->mac.transmitter_id[0]);
    passed = passed && (ctrlpkt->node_id[1] == mclmac->mac.transmitter_id[1]);
    passed = passed && (ctrlpkt->currentFrame != mclmac->mac.frame.current_frame);
    passed = passed && (ctrlpkt->currentSlot != mclmac->mac.frame.current_slot);
    passed = passed && (ctrlpkt->collisionSlot != 0);
    passed = passed && (ctrlpkt->collisionFrequency != 0);
    passed = passed && (ctrlpkt->hopCount != mclmac->_hopCount);
    passed = passed && (ctrlpkt->networkTime != mclmac->_networkTime);
    passed = passed && (ctrlpkt->initTime != mclmac->_initTime);

    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 3;
    mclmac_receive_control_packet(mclmac);
    passed = passed && (ret == true);
    passed = passed && (ctrlpkt->node_id[0] == mclmac->mac.transmitter_id[0]);
    passed = passed && (ctrlpkt->node_id[1] == mclmac->mac.transmitter_id[1]);
    passed = passed && (ctrlpkt->currentFrame == mclmac->mac.frame.current_frame);
    passed = passed && (ctrlpkt->currentSlot == mclmac->mac.frame.current_slot);
    passed = passed && (ctrlpkt->collisionSlot == mclmac->mac.selectedSlot);
    passed = passed && (ctrlpkt->collisionFrequency == mclmac->mac.transmitChannel);
    passed = passed && (ctrlpkt->hopCount == mclmac->_hopCount);
    passed = passed && (ctrlpkt->networkTime == mclmac->_networkTime);
    passed = passed && (ctrlpkt->initTime == mclmac->_initTime);

    /* current slot is differet */
    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 4;
    mclmac_receive_control_packet(mclmac);
    passed = passed && (ret == true);
    passed = passed && (ctrlpkt->node_id[0] == mclmac->mac.transmitter_id[0]);
    passed = passed && (ctrlpkt->node_id[1] == mclmac->mac.transmitter_id[1]);
    passed = passed && (ctrlpkt->currentFrame == mclmac->mac.frame.current_frame);
    passed = passed && (ctrlpkt->currentSlot != mclmac->mac.frame.current_slot);
    passed = passed && (ctrlpkt->collisionSlot == mclmac->mac.selectedSlot);
    passed = passed && (ctrlpkt->collisionFrequency == mclmac->mac.transmitChannel);
    passed = passed && (ctrlpkt->hopCount == mclmac->_hopCount);
    passed = passed && (ctrlpkt->networkTime == mclmac->_networkTime);
    passed = passed && (ctrlpkt->initTime == mclmac->_initTime);

    /* current frame is differen */
    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 5;
    mclmac_receive_control_packet(mclmac);
    passed = passed && (ret == true);
    passed = passed && (ctrlpkt->node_id[0] == mclmac->mac.transmitter_id[0]);
    passed = passed && (ctrlpkt->node_id[1] == mclmac->mac.transmitter_id[1]);
    passed = passed && (ctrlpkt->currentFrame != mclmac->mac.frame.current_frame);
    passed = passed && (ctrlpkt->currentSlot == mclmac->mac.frame.current_slot);
    passed = passed && (ctrlpkt->collisionSlot == mclmac->mac.selectedSlot);
    passed = passed && (ctrlpkt->collisionFrequency == mclmac->mac.transmitChannel);
    passed = passed && (ctrlpkt->hopCount == mclmac->_hopCount);
    passed = passed && (ctrlpkt->networkTime == mclmac->_networkTime);
    passed = passed && (ctrlpkt->initTime == mclmac->_initTime);

    /* network time */
    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 6;
    mclmac_receive_control_packet(mclmac);
    passed = passed && (ret == true);
    passed = passed && (ctrlpkt->node_id[0] == mclmac->mac.transmitter_id[0]);
    passed = passed && (ctrlpkt->node_id[1] == mclmac->mac.transmitter_id[1]);
    passed = passed && (ctrlpkt->currentFrame == mclmac->mac.frame.current_frame);
    passed = passed && (ctrlpkt->currentSlot == mclmac->mac.frame.current_slot);
    passed = passed && (ctrlpkt->collisionSlot == mclmac->mac.selectedSlot);
    passed = passed && (ctrlpkt->collisionFrequency == mclmac->mac.transmitChannel);
    passed = passed && (ctrlpkt->hopCount == mclmac->_hopCount);
    passed = passed && (ctrlpkt->networkTime != mclmac->_networkTime);
    passed = passed && (ctrlpkt->initTime == mclmac->_initTime);

    controlpacket_clear(ctrlpkt);
    mclmac->_state_ctrl = 0;
    mclmac_receive_control_packet(mclmac);
    passed = passed && (ret == true);
    passed = passed && (ctrlpkt->node_id[0] != mclmac->mac.transmitter_id[0]);
    passed = passed && (ctrlpkt->node_id[1] != mclmac->mac.transmitter_id[1]);
    passed = passed && (ctrlpkt->currentFrame != mclmac->mac.frame.current_frame);
    passed = passed && (ctrlpkt->currentSlot != mclmac->mac.frame.current_slot);
    passed = passed && (ctrlpkt->collisionSlot != 0);
    passed = passed && (ctrlpkt->collisionFrequency != 0);
    passed = passed && (ctrlpkt->hopCount != mclmac->_hopCount);
    passed = passed && (ctrlpkt->networkTime != mclmac->_networkTime);
    passed = passed && (ctrlpkt->initTime != mclmac->_initTime);

    return passed;
}

bool test_mclmac_send_data_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = &data->mclmac;

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
    for (i = 0; i < size; i++) {
        WRITE_ARRAY(REFERENCE byteString, message[i], i);
    }
    // Store packets on array
    uint n = MAX_NUMBER_DATA_PACKETS;
    for (i = 0; i < n; i++)
    {
        DataPacket_t *pkt = &mclmac->mac._message_packets_to_send[i];
        datapacket_construct_from_bytestring(pkt, &byteString);
        pkt = &mclmac->mac._control_packets_to_send[i];
        datapacket_construct_from_bytestring(pkt, &byteString);
    }
    mclmac->mac._packets_to_send_control = 0;
    mclmac_send_data_packet(mclmac);
    bool passed = true;
    passed = passed && (mclmac->mac._first_send_control == mclmac->mac._last_send_control);
    mclmac->mac._packets_to_send_message = 0;
    mclmac_send_data_packet(mclmac);
    passed = passed && (mclmac->mac._first_send_message == mclmac->mac._last_send_message);

    mclmac->mac._packets_to_send_message = n;
    mclmac->mac._packets_to_send_control = n;
    uint8_t first_send = mclmac->mac._first_send_control;
    uint8_t last_send = mclmac->mac._last_send_control;
    uint8_t packet_sent = mclmac->mac._packets_to_send_control;
    // Send the control packets
    for (i = 0; i < n; i++)
    {
        mclmac_send_data_packet(mclmac);
        packet_sent--;
        passed = passed && (mclmac->mac._packets_to_send_control == packet_sent);
        first_send = (first_send + 1) % MAX_NUMBER_DATA_PACKETS;
        passed = passed && (mclmac->mac._first_send_control == first_send);
        DataPacket_t *pkt = &mclmac->mac._control_packets_to_send[i];
        passed = passed && (pkt->type == -1);
        passed = passed && (pkt->size == 0);
        passed = passed && (pkt->destination_id[0] == 0);
        passed = passed && (pkt->destination_id[1] == 0);
#ifdef __LINUX__
        passed = passed && (pkt->data == NULL);
#endif
#ifdef __RIOT__
        passed = passed && (pkt->data.size == 0);
#endif
    }
    passed = passed && (mclmac->mac._packets_to_send_control == 0);
    passed = passed && (mclmac->mac._last_send_control == last_send);
    // Send the data messages
    first_send = mclmac->mac._first_send_message;
    last_send = mclmac->mac._last_send_message;
    packet_sent = mclmac->mac._packets_to_send_message;
    // Send the control packets
    for (i = 0; i < n; i++)
    {
        mclmac_send_data_packet(mclmac);
        packet_sent--;
        passed = passed && (mclmac->mac._packets_to_send_message == packet_sent);
        first_send = (first_send + 1) % MAX_NUMBER_DATA_PACKETS;
        passed = passed && (mclmac->mac._first_send_message == first_send);
        DataPacket_t *pkt = &mclmac->mac._message_packets_to_send[i];
        passed = passed && (pkt->type == -1);
        passed = passed && (pkt->size == 0);
        passed = passed && (pkt->destination_id[0] == 0);
        passed = passed && (pkt->destination_id[1] == 0);
#ifdef __LINUX__
        passed = passed && (pkt->data == NULL);
#endif
#ifdef __RIOT__
        passed = passed && (pkt->data.size == 0);
#endif
    }
    passed = passed && (mclmac->mac._packets_to_send_message == 0);
    passed = passed && (mclmac->mac._last_send_message == last_send);

#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
#endif

    return passed;
}

bool test_mclmac_receive_data_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = &data->mclmac;

    mclmac_set_slot_duration(mclmac, TIME(SLOT_DURATION));
    /**
     * Fill the array _packets_received with random data, 
     * assuring that the destination_id corresponds with the node's id.
     */
    int i;
    bool passed = true;
    for (i = 0; i < 2 * MAX_NUMBER_DATA_PACKETS; i++)
    {
        bool ret = mclmac_receive_data_packet(mclmac);
        passed = passed && (ret == true);
    }
    passed = passed && (mclmac->mac._number_packets_received == 2 * MAX_NUMBER_DATA_PACKETS);
    passed = passed && (mclmac->mac._first_received == 0);
    passed = passed && (mclmac->mac._last_received == 0);
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++)
    {
        DataPacket_t *pkt = &mclmac->mac._packets_received[i];
        passed = passed && (pkt->type > 0);
        passed = passed && (pkt->destination_id[0] == mclmac->_node_id[0]);
        passed = passed && (pkt->destination_id[1] == mclmac->_node_id[1]);
        passed = passed && (pkt->size > 0);
#ifdef __LINUX__
        passed = passed && (pkt->data != NULL);
#endif
#ifdef __RIOT__
        passed = passed && (pkt->data.size > 0);
#endif
    }

    return passed;
}

void executetests_packets_handlers(void)
{
    init_queues();
    timeout_init();

    cUnit_t *tests;
    struct packethandlers_data data;

    cunit_init(&tests, &setup_packet_handlers, &teardown_packet_handlers, (void *)&data);

    cunit_add_test(tests, &test_mclmac_start_packet_detection, "mclmac_start_packet_detection\0");
    cunit_add_test(tests, &test_mclmac_cf_packet_detected, "mclmac_cf_packet_detected\0");
    /*cunit_add_test(tests, &test_mclmac_receive_ctrlpkt_sync, "_mclmac_receive_ctrlpkt_sync\0");
    cunit_add_test(tests, &test_mclmac_create_control_packet, "mclmac_create_control_packet\0");
    cunit_add_test(tests, &test_mclmac_receive_cf_message, "mclmac_receive_cf_message\0");
    cunit_add_test(tests, &test_mclmac_send_control_packet, "mclmac_send_control_packet\0");
    cunit_add_test(tests, &test_mclmac_receive_control_packet, "mclmac_receive_control_packet\0");
    cunit_add_test(tests, &test_mclmac_send_data_packet, "mclmac_send_data_packet\0");
    cunit_add_test(tests, &test_mclmac_receive_data_packet, "mclmac_receive_data_packet\0");
    cunit_add_test(tests, &test_mclmac_send_cf_message, "mclmac_send_cf_message\0");*/

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
    timeout_done();
    end_queues();
}
