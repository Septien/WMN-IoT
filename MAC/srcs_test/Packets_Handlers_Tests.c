#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include "string.h"

#include "MCLMAC.h"

void test_stub_mclmac_receive_ctrlpkt_sync(void)
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
    ARROW(mclmac)_initTime = rand() % 100;
    ARROW(mclmac)_networkTime = (rand() % 1000) + ARROW(mclmac)_initTime;
    ARROW(mclmac)_hopCount = rand() % 10;
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame = rand() % 10;
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot = rand() % MAX_NUMBER_SLOTS;
    stub_mclmac_receive_ctrlpkt_sync(REFERENCE mclmac, REFERENCE ctrlpkt);
    assert(ARROW(ctrlpkt)networkTime == ARROW(mclmac)_networkTime);
    assert(ARROW(ctrlpkt)hopCount == ARROW(mclmac)_hopCount);
    assert(ARROW(ctrlpkt)initTime == ARROW(mclmac)_initTime);
    assert(ARROW(ctrlpkt)currentSlot == ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot);
    assert(ARROW(ctrlpkt)currentFrame == ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame);

    controlpacket_destroy(&ctrlpkt);
    MCLMAC_destroy(&mclmac);
}

void test_mclmac_create_control_packet(void)
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
     * We are at the TRANSMIT or RECEIVE state, and we want to create
     * a control packet for subsequent sending.
     * Create the packet with the available variables on the MCLMAC data structure.
     */
    ARROW(mclmac)_nodeID = rand();
    ARROW(mclmac)_networkTime = rand();
    ARROW(mclmac)_initTime = rand();
    ARROW(mclmac)_hopCount = rand();
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot = rand();
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame = rand();
    ARROW(ARROW(mclmac)mac)_collisionSlot = rand();
    ARROW(ARROW(mclmac)mac)_collisionFrequency = rand();
    ControlPacket_t *pkt = REFERENCE ARROW(ARROW(mclmac)mac)ctrlpkt;
    mclmac_create_control_packet(REFERENCE mclmac);
    assert(pkt->nodeID == ARROW(mclmac)_nodeID);
    assert(pkt->currentFrame == ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame);
    assert(pkt->currentSlot == ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot);
    assert(pkt->collisionSlot == ARROW(ARROW(mclmac)mac)_collisionSlot);
    assert(pkt->collisionFrequency == ARROW(ARROW(mclmac)mac)_collisionFrequency);
    assert(pkt->hopCount == ARROW(mclmac)_hopCount);
    assert(pkt->networkTime == ARROW(mclmac)_networkTime);
    assert(pkt->initTime == ARROW(mclmac)_initTime);

    MCLMAC_destroy(&mclmac);
}

void test_stub_mclmac_send_control_packet(void)
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

    ARROW(mclmac)_nodeID = rand();
    ARROW(mclmac)_networkTime = rand();
    ARROW(mclmac)_initTime = rand();
    ARROW(mclmac)_hopCount = rand();
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot = rand();
    ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame = rand();
    ARROW(ARROW(mclmac)mac)_collisionSlot = rand();
    ARROW(ARROW(mclmac)mac)_collisionFrequency = rand();
    mclmac_create_control_packet(REFERENCE mclmac);
    /**
     * The control packet was already created. We now need to get the 
     * byte string from the packet, and send it to the medium. The packet
     * should be cleared after wards.
     */
    ControlPacket_t *pkt = REFERENCE ARROW(ARROW(mclmac)mac)ctrlpkt;
    stub_mclmac_send_control_packet(REFERENCE mclmac);
    assert(pkt->nodeID == 0);
    assert(pkt->currentFrame == 0);
    assert(pkt->currentSlot == 0);
    assert(pkt->collisionSlot == 0);
    assert(pkt->collisionFrequency == 0);
    assert(pkt->hopCount == 0);
    assert(pkt->networkTime == 0);
    assert(pkt->initTime == 0);

    MCLMAC_destroy(&mclmac);
}

void test_stub_mclmac_receive_control_packet(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 1;

    MCLMAC_init(&mclmac, &radio, nodeid);
    ARROW(mclmac)_hopCount = rand();
    ARROW(mclmac)_networkTime = rand();
    ARROW(mclmac)_initTime = rand();
    mclmac_set_current_frame(REFERENCE mclmac, rand());
    mclmac_set_current_slot(REFERENCE mclmac, rand());

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

    ARROW(mclmac)_state_ctrl = 1;
    stub_mclmac_receive_control_packet(REFERENCE mclmac);
    ControlPacket_t *ctrlpkt = REFERENCE ARROW(ARROW(mclmac)mac)ctrlpkt_recv;
    assert(ctrlpkt->nodeID == ARROW(ARROW(mclmac)mac)transmiterID);
    assert(ctrlpkt->currentFrame == ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame);
    assert(ctrlpkt->currentSlot == ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot == 0);
    assert(ctrlpkt->collisionFrequency == 0);
    assert(ctrlpkt->hopCount == ARROW(mclmac)_hopCount);
    assert(ctrlpkt->networkTime == ARROW(mclmac)_networkTime);
    assert(ctrlpkt->initTime == ARROW(mclmac)_initTime);

    controlpacket_clear(ctrlpkt);
    ARROW(mclmac)_state_ctrl = 2;
    stub_mclmac_receive_control_packet(REFERENCE mclmac);
    assert(ctrlpkt->nodeID == ARROW(ARROW(mclmac)mac)transmiterID);
    assert(ctrlpkt->currentFrame != ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame);
    assert(ctrlpkt->currentSlot != ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot != 0);
    assert(ctrlpkt->collisionFrequency != 0);
    assert(ctrlpkt->hopCount != ARROW(mclmac)_hopCount);
    assert(ctrlpkt->networkTime != ARROW(mclmac)_networkTime);
    assert(ctrlpkt->initTime != ARROW(mclmac)_initTime);

    controlpacket_clear(ctrlpkt);
    ARROW(mclmac)_state_ctrl = 3;
    stub_mclmac_receive_control_packet(REFERENCE mclmac);
    assert(ctrlpkt->nodeID == ARROW(ARROW(mclmac)mac)transmiterID);
    assert(ctrlpkt->currentFrame == ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame);
    assert(ctrlpkt->currentSlot == ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot == ARROW(ARROW(mclmac)mac)selectedSlot);
    assert(ctrlpkt->collisionFrequency == ARROW(ARROW(mclmac)mac)transmitChannel);
    assert(ctrlpkt->hopCount == ARROW(mclmac)_hopCount);
    assert(ctrlpkt->networkTime == ARROW(mclmac)_networkTime);
    assert(ctrlpkt->initTime == ARROW(mclmac)_initTime);

    /* current slot is differet */
    controlpacket_clear(ctrlpkt);
    ARROW(mclmac)_state_ctrl = 4;
    stub_mclmac_receive_control_packet(REFERENCE mclmac);
    assert(ctrlpkt->nodeID == ARROW(ARROW(mclmac)mac)transmiterID);
    assert(ctrlpkt->currentFrame == ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame);
    assert(ctrlpkt->currentSlot != ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot == ARROW(ARROW(mclmac)mac)selectedSlot);
    assert(ctrlpkt->collisionFrequency == ARROW(ARROW(mclmac)mac)transmitChannel);
    assert(ctrlpkt->hopCount == ARROW(mclmac)_hopCount);
    assert(ctrlpkt->networkTime == ARROW(mclmac)_networkTime);
    assert(ctrlpkt->initTime == ARROW(mclmac)_initTime);

    /* current frame is differen */
    controlpacket_clear(ctrlpkt);
    ARROW(mclmac)_state_ctrl = 5;
    stub_mclmac_receive_control_packet(REFERENCE mclmac);
    assert(ctrlpkt->nodeID == ARROW(ARROW(mclmac)mac)transmiterID);
    assert(ctrlpkt->currentFrame != ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame);
    assert(ctrlpkt->currentSlot == ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot == ARROW(ARROW(mclmac)mac)selectedSlot);
    assert(ctrlpkt->collisionFrequency == ARROW(ARROW(mclmac)mac)transmitChannel);
    assert(ctrlpkt->hopCount == ARROW(mclmac)_hopCount);
    assert(ctrlpkt->networkTime == ARROW(mclmac)_networkTime);
    assert(ctrlpkt->initTime == ARROW(mclmac)_initTime);

    /* network time */
    controlpacket_clear(ctrlpkt);
    ARROW(mclmac)_state_ctrl = 6;
    stub_mclmac_receive_control_packet(REFERENCE mclmac);
    assert(ctrlpkt->nodeID == ARROW(ARROW(mclmac)mac)transmiterID);
    assert(ctrlpkt->currentFrame == ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame);
    assert(ctrlpkt->currentSlot == ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot == ARROW(ARROW(mclmac)mac)selectedSlot);
    assert(ctrlpkt->collisionFrequency == ARROW(ARROW(mclmac)mac)transmitChannel);
    assert(ctrlpkt->hopCount == ARROW(mclmac)_hopCount);
    assert(ctrlpkt->networkTime != ARROW(mclmac)_networkTime);
    assert(ctrlpkt->initTime == ARROW(mclmac)_initTime);

    controlpacket_clear(ctrlpkt);
    ARROW(mclmac)_state_ctrl = 0;
    stub_mclmac_receive_control_packet(REFERENCE mclmac);
    assert(ctrlpkt->nodeID != ARROW(ARROW(mclmac)mac)transmiterID);
    assert(ctrlpkt->currentFrame != ARROW(ARROW(ARROW(mclmac)mac)frame)current_frame);
    assert(ctrlpkt->currentSlot != ARROW(ARROW(ARROW(mclmac)mac)frame)current_slot);
    assert(ctrlpkt->collisionSlot != 0);
    assert(ctrlpkt->collisionFrequency != 0);
    assert(ctrlpkt->hopCount != ARROW(mclmac)_hopCount);
    assert(ctrlpkt->networkTime != ARROW(mclmac)_networkTime);
    assert(ctrlpkt->initTime != ARROW(mclmac)_initTime);

    MCLMAC_destroy(&mclmac);
}

void test_stub_mclmac_send_data_packet(void)
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
        DataPacket_t *pkt = &ARROW(ARROW(mclmac)mac)_message_packets_to_send[i];
        datapacket_construct_from_bytestring(pkt, &byteString);
    }
    ARROW(ARROW(mclmac)mac)_packets_to_send_message = 0;
    stub_mclmac_send_data_packet(REFERENCE mclmac);
    assert(ARROW(ARROW(mclmac)mac)_first_send_message == ARROW(ARROW(mclmac)mac)_last_send_message);

    ARROW(ARROW(mclmac)mac)_packets_to_send_message = n;
    uint8_t first_send = ARROW(ARROW(mclmac)mac)_first_send_message;
    uint8_t last_send = ARROW(ARROW(mclmac)mac)_last_send_message;
    uint8_t packet_sent = ARROW(ARROW(mclmac)mac)_packets_to_send_message;
    for (i = 0; i < n; i++)
    {
        stub_mclmac_send_data_packet(REFERENCE mclmac);
        packet_sent--;
        assert(ARROW(ARROW(mclmac)mac)_packets_to_send_message == packet_sent);
        first_send = (first_send + 1) % MAX_NUMBER_DATA_PACKETS;
        assert(ARROW(ARROW(mclmac)mac)_first_send_message == first_send);
        DataPacket_t *pkt = &ARROW(ARROW(mclmac)mac)_message_packets_to_send[i];
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
    assert(ARROW(ARROW(mclmac)mac)_packets_to_send_message == 0);
    assert(ARROW(ARROW(mclmac)mac)_last_send_message == last_send);

#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
#endif

    MCLMAC_destroy(&mclmac);
}

void test_stub_mclmac_receive_data_packet(void)
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
     * Fill the array _packets_received with random data, 
     * assuring that the destination_id corresponds with the node's id.
     */
    int i;
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++)
        stub_mclmac_receive_data_packet(REFERENCE mclmac);
    assert(ARROW(ARROW(mclmac)mac)_number_packets_received == MAX_ELEMENTS_ON_QUEUE);
    assert(ARROW(ARROW(mclmac)mac)_first_received == 0);
    assert(ARROW(ARROW(mclmac)mac)_last_received == MAX_ELEMENTS_ON_QUEUE);
    for (i = 0; i < MAX_ELEMENTS_ON_QUEUE; i++)
    {
        DataPacket_t *pkt = &ARROW(ARROW(mclmac)mac)_packets_received[i];
        assert(pkt->type > 0);
        assert(pkt->destination_id == ARROW(mclmac)_nodeID);
        assert(pkt->size > 0);
#ifdef __LINUX__
        assert(pkt->data != NULL);
#endif
#ifdef __RIOT__
        assert(pkt->data.size > 0);
#endif
    }

    MCLMAC_destroy(&mclmac);
}

void test_stub_mclmac_send_layers_control_packet(void)
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
        DataPacket_t *pkt = &ARROW(ARROW(mclmac)mac)_control_packets_to_send[i];
        datapacket_construct_from_bytestring(pkt, &byteString);
    }
    ARROW(ARROW(mclmac)mac)_packets_to_send_control = 0;
    stub_mclmac_send_data_packet(REFERENCE mclmac);
    assert(ARROW(ARROW(mclmac)mac)_first_send_control == ARROW(ARROW(mclmac)mac)_last_send_control);

    ARROW(ARROW(mclmac)mac)_packets_to_send_control = n;
    uint8_t first_send = ARROW(ARROW(mclmac)mac)_first_send_control;
    uint8_t last_send = ARROW(ARROW(mclmac)mac)_last_send_control;
    uint8_t packet_sent = ARROW(ARROW(mclmac)mac)_packets_to_send_control;
    for (i = 0; i < n; i++)
    {
        stub_mclmac_send_layers_control_packet(REFERENCE mclmac);
        packet_sent--;
        assert(ARROW(ARROW(mclmac)mac)_packets_to_send_control == packet_sent);
        first_send = (first_send + 1) % MAX_NUMBER_DATA_PACKETS;
        assert(ARROW(ARROW(mclmac)mac)_first_send_control == first_send);
        DataPacket_t *pkt = &ARROW(ARROW(mclmac)mac)_control_packets_to_send[i];
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
    assert(ARROW(ARROW(mclmac)mac)_packets_to_send_control == 0);
    assert(ARROW(ARROW(mclmac)mac)_last_send_control == last_send);

#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&byteString);
#endif
    MCLMAC_destroy(&mclmac);
}

#if 0
void test_stub_mclmac_receive_cf_message(void)
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

    mclmac_set_nodeid(REFERENCE mclmac, 1);
//    mclmac_set_destination_id(REFERENCE mclmac, 2);

    mclmac_set_cf_channel(REFERENCE mclmac, 915000000);

    stub_mclmac_change_cf_channel(REFERENCE mclmac);

    stub_mclmac_start_cf_phase(REFERENCE mclmac);

    /* Receive the packet the destination id is already known, and the 
       cf phase is already started. */
    stub_mclmac_receive_cf_message(REFERENCE mclmac);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_receive_cf_message(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    uint16_t nodeid = 0;
    size_t size = 5 * sizeof(uint16_t);

    MCLMAC_init(&mclmac, &radio, nodeid);

    int ret;
#ifdef __LINUX__
    mclmac->mac->_cf_messages = (uint8_t *)malloc(size * sizeof(uint8_t));
    if (mclmac->mac->_cf_messages == NULL)
    {
        printf("Unable to create _cf_messages queue. Linux.\n");
        exit(0);
    }
    memset(mclmac->mac->_cf_messages, 0, size);
#endif
#ifdef __RIOT__
    ret = create_array(&mclmac.mac._cf_messages, size);
    if (ret == 0)
    {
        printf("Unable to craeate _cf_messages queue. RIOT.\n");
        exit(0);
    }
#endif
//    mclmac_create_cf_packet(REFERENCE mclmac);

    for (int i = 0; i < ITERATIONS; i++)
    {
        ret = stub_mclmac_receive_cf_message(REFERENCE mclmac);
        if (ret)
        {
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 0) > 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 1) > 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 2) > 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 3) > 0);
        }
        else
        {
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 0) == 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 1) == 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 2) == 0);
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(mclmac)mac)_cf_messages, 3) == 0);
        }
    }

#ifdef __LINUX__
    free(mclmac->mac->_cf_messages);
#endif
#ifdef __RIOT__
    free_array(&mclmac.mac._cf_messages);
#endif

    MCLMAC_destroy(&mclmac);
}
#endif

void executetests_packets_handlers(void)
{
    init_queues();

    printf("Tetsing test_stub_mclmac_receive_ctrlpkt_sync function.\n");
    test_stub_mclmac_receive_ctrlpkt_sync();
    printf("Test passed.\n");

    printf("Testing the mclmac_create_control_packet function.\n");
    test_mclmac_create_control_packet();
    printf("Test passed.\n");

    printf("Testing the stub_mclmac_send_control_packet function.\n");
    test_stub_mclmac_send_control_packet();
    printf("Test passed.\n");

    printf("Testing the stub_mclmac_receive_control_packet function.\n");
    test_stub_mclmac_receive_control_packet();
    printf("Test passed.\n");

    printf("Testing the stub_mclmac_send_data_packet function.\n");
    test_stub_mclmac_send_data_packet();
    printf("Test passed.\n");

    printf("Testing the stub_mclmac_receive_data_packet function.\n");
    test_stub_mclmac_receive_data_packet();
    printf("Test passed.\n");

    printf("Testing the stub_mclmac_send_layers_control_packet function.\n");
    test_stub_mclmac_send_layers_control_packet();
    printf("Test passed.\n");

    /*printf("Testing mclmac_receive_cf_message function.\n");
    test_mclmac_receive_cf_message();
    printf("Test passed.\n");*/

    end_queues();
    return;
}
