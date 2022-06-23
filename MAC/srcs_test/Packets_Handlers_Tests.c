#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include "string.h"

#include "MCLMAC.h"

#include "cUnit.h"

#ifdef __RIOT__
#include "net/netdev.h"
#endif


struct packethandlers_data{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    nrf24l01p_ng_t radio;
    netdev_t *netdev;
#endif
    uint64_t node_id[2];
};

void setup_packet_handlers(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    data->node_id[0] = rand();
    data->node_id[1] = rand();
#ifdef __LINUX__
    MCLMAC_init(&data->mclmac, data->radio, data->node_id);
#endif
#ifdef __RIOT__
    // Setup the radio
    nrf24l01p_ng_params_t params = {
        .spi = NRF24L01P_NG_PARAM_SPI,
        .spi_clk = NRF24L01P_NG_PARAM_SPI_CLK,
        .pin_cs = NRF24L01P_NG_PARAM_CS,
        .pin_ce = NRF24L01P_NG_PARAM_CE,
        .pin_irq = NRF24L01P_NG_PARAM_IRQ,
        .config = {
            .cfg_crc = NRF24L01P_NG_PARAM_CRC_LEN,
            .cfg_tx_power = NRF24L01P_NG_PARAM_TX_POWER,
            .cfg_data_rate = NRF24L01P_NG_PARAM_DATA_RATE,
            .cfg_channel = NRF24L01P_NG_PARAM_CHANNEL,
            .cfg_max_retr = NRF24L01P_NG_PARAM_MAX_RETRANSM,
            .cfg_retr_delay = NRF24L01P_NG_PARAM_RETRANSM_DELAY,
        }
    };
    int ret = nrf24l01p_ng_setup(&data->radio, &params, 2);
    data->netdev = &data->radio.netdev;
    data->radio.netdev.driver->init(data->netdev);
    MCLMAC_init(&data->mclmac, data->netdev, data->node_id);
#endif
}

void teardown_packet_handlers(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_destroy(&data->mclmac);
}

void test_mclmac_start_packet_detection(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * Function used for the INITIALIZATION state of the MAC state machine.
     * It should be called only once at the beginning of the state.
     * Configure the radio so it can begin the detection of cf packets.
     */
#ifdef __RIOT__
    /*uint8_t broadcast_addr[NRF24L01P_NG_ADDR_WIDTH] = NRF24L01P_NG_BROADCAST_ADDR;
    uint8_t addr[NRF24L01P_NG_ADDR_WIDTH] = {0};
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_ADDRESS,
                                    (void *)addr, NRF24L01P_NG_ADDR_WIDTH);
    assert(memcmp(broadcast_addr, addr, NRF24L01P_NG_ADDR_WIDTH) == 0);
    uint16_t cf_channel = (uint16_t) mclmac->mac.cfChannel;
    uint16_t radio_channel = 0;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_CHANNEL,
                                    (void *)&radio_channel, sizeof(uint16_t));
    assert(radio_channel == cf_channel);
    netopt_state_t radio_state = NETOPT_STATE_OFF;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&radio_state, sizeof(netopt_state_t));
    assert(radio_state == NETOPT_STATE_RX);*/
#endif
}

void test_stub_mclmac_cf_packet_detected(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * This function detects cf packets on the medium. The radio should already have
     * the broadcast address, the channel, and in state rx.
     * It will retrieve the size of the packet on the radio, and
     * if it is 32 bytes, it will return true. Otherwise will return false.
     */
    bool detected = mclmac_cf_packet_detected(mclmac);
#ifdef __RIOT__
    (void) detected;
#endif
}

void test_mclmac_set_radio_standby(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    /**
     * Set the radio state to standby.
     */
    mclmac_set_radio_standby(mclmac);
#ifdef __RIOT__
    /*netopt_state_t state = NETOPT_STATE_OFF;
    mclmac->mac.netdev->driver->get(mclmac->mac.netdev, NETOPT_STATE,
                                    (void *)&state, sizeof(netopt_state_t));
    assert(state == NETOPT_STATE_STANDBY);*/
#endif
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
    mclmac->_node_id[0] = rand();
    mclmac->_node_id[1] = rand();
    mclmac->_networkTime = rand();
    mclmac->_initTime = rand();
    mclmac->_hopCount = rand();
    ARROW(ARROW(mclmac->mac)frame)current_slot = rand();
    ARROW(ARROW(mclmac->mac)frame)current_frame = rand();
    ARROW(mclmac->mac)_collisionSlot = rand();
    ARROW(mclmac->mac)_collisionFrequency = rand();
    ControlPacket_t *pkt = REFERENCE ARROW(mclmac->mac)ctrlpkt;
    mclmac_create_control_packet(mclmac);
    assert(pkt->node_id[0] == mclmac->_node_id[0]);
    assert(pkt->node_id[1] == mclmac->_node_id[1]);
    assert(pkt->currentFrame == ARROW(ARROW(mclmac->mac)frame)current_frame);
    assert(pkt->currentSlot == ARROW(ARROW(mclmac->mac)frame)current_slot);
    assert(pkt->collisionSlot == ARROW(mclmac->mac)_collisionSlot);
    assert(pkt->collisionFrequency == ARROW(mclmac->mac)_collisionFrequency);
    assert(pkt->hopCount == mclmac->_hopCount);
    assert(pkt->networkTime == mclmac->_networkTime);
    assert(pkt->initTime == mclmac->_initTime);

}

void test_mclmac_send_cf_message(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac->_node_id[0] = rand();
    mclmac->_node_id[1] = rand();
//    mclmac_set_destination_id(REFERENCE mclmac, 2);
//    mclmac_create_cf_packet(REFERENCE mclmac);

    ARROW(mclmac->mac)cfChannel = 915000000;

    mclmac_change_cf_channel(mclmac);

    mclmac_start_cf_phase(mclmac);

    /* Send the packet the destination id is already known, and the 
       cf phase is already started. */
    stub_mclmac_send_cf_message(mclmac);
}

void test_stub_mclmac_send_control_packet(void *arg)
{
    struct packethandlers_data *data = (struct packethandlers_data *) arg;
    MCLMAC_t *mclmac = REFERENCE data->mclmac;

    mclmac->_node_id[0] = rand();
    mclmac->_node_id[1] = rand();
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
    assert(pkt->node_id[0] == 0);
    assert(pkt->node_id[1] == 0);
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
    assert(ctrlpkt->node_id[0] == ARROW(mclmac->mac)transmitter_id[0]);
    assert(ctrlpkt->node_id[1] == ARROW(mclmac->mac)transmitter_id[1]);
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
    assert(ctrlpkt->node_id[0] == ARROW(mclmac->mac)transmitter_id[0]);
    assert(ctrlpkt->node_id[1] == ARROW(mclmac->mac)transmitter_id[1]);
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
    assert(ctrlpkt->node_id[0] == ARROW(mclmac->mac)transmitter_id[0]);
    assert(ctrlpkt->node_id[1] == ARROW(mclmac->mac)transmitter_id[1]);
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
    assert(ctrlpkt->node_id[0] == ARROW(mclmac->mac)transmitter_id[0]);
    assert(ctrlpkt->node_id[1] == ARROW(mclmac->mac)transmitter_id[1]);
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
    assert(ctrlpkt->node_id[0] == ARROW(mclmac->mac)transmitter_id[0]);
    assert(ctrlpkt->node_id[1] == ARROW(mclmac->mac)transmitter_id[1]);
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
    assert(ctrlpkt->node_id[0] == ARROW(mclmac->mac)transmitter_id[0]);
    assert(ctrlpkt->node_id[1] == ARROW(mclmac->mac)transmitter_id[1]);
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
    assert(ctrlpkt->node_id[0] != ARROW(mclmac->mac)transmitter_id[0]);
    assert(ctrlpkt->node_id[1] != ARROW(mclmac->mac)transmitter_id[1]);
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
        assert(pkt->destination_id[0] == 0);
        assert(pkt->destination_id[1] == 0);
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
        assert(pkt->destination_id[0] == mclmac->_node_id[0]);
        assert(pkt->destination_id[1] == mclmac->_node_id[1]);
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
        assert(pkt->destination_id[0] == 0);
        assert(pkt->destination_id[1] == 0);
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

    cunit_add_test(tests, &test_mclmac_start_packet_detection, "mclmac_start_packet_detection\0");
    cunit_add_test(tests, &test_stub_mclmac_receive_ctrlpkt_sync, "stub_mclmac_receive_ctrlpkt_sync\0");
    cunit_add_test(tests, &test_mclmac_set_radio_standby, "mclmac_set_radio_standby\0");
    cunit_add_test(tests, &test_mclmac_create_control_packet, "mclmac_create_control_packet\0");
    cunit_add_test(tests, &test_stub_mclmac_send_control_packet, "stub_mclmac_send_control_packet\0");
    cunit_add_test(tests, &test_stub_mclmac_receive_control_packet, "stub_mclmac_receive_control_packet\0");
    cunit_add_test(tests, &test_stub_mclmac_send_data_packet, "stub_mclmac_send_data_packet\0");
    cunit_add_test(tests, &test_stub_mclmac_receive_data_packet, "stub_mclmac_receive_data_packet\0");
    cunit_add_test(tests, &test_stub_mclmac_send_layers_control_packet, "stub_mclmac_send_layers_control_packet\0");
    cunit_add_test(tests, &test_mclmac_send_cf_message, "mclmac_send_cf_message\0");

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
    end_queues();
}
