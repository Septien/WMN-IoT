#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include "string.h"

#include "MCLMAC.h"

/*void test_mclmac_create_cf_packet(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;
    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize);

    mclmac_create_cf_packet(REFERENCE mclmac);
#ifdef __LINUX__
    assert(ARROW(ARROW(mclmac)mac)cfpkt != NULL);
#endif
    assert(ARROW(ARROW(ARROW(mclmac)mac)cfpkt)nodeID > 0);

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
    size_t dataQsize = 256;

    uint8_t bytes = get_number_bytes(_nSlots * _nChannels);
    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize);
    ARROW(ARROW(mclmac)mac)nodeID = rand();
    ARROW(mclmac)_collisionFrequency = rand();
    ARROW(mclmac)_collisionSlot = rand();
    ARROW(ARROW(mclmac)mac)hopCount = rand();
    ARROW(mclmac)_ack = rand();
    ARROW(mclmac)_networkTime = rand();
    int i;
    for (i = 0; i < bytes; i++)
        WRITE_ARRAY(REFERENCE ARROW(mclmac)_occupiedSlots, rand(), i);

    mclmac_create_control_packet(REFERENCE mclmac);
#ifdef __LINUX__
    assert(mclmac->mac->ctrlpkt != NULL);
#endif
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)nodeID == ARROW(ARROW(mclmac)mac)nodeID);
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)collisionSlot ==ARROW(mclmac)_collisionSlot);
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)collisionFrequency == ARROW(mclmac)_collisionFrequency);
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)hopCount == ARROW(ARROW(mclmac)mac)hopCount);
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)ack == ARROW(mclmac)_ack);
    assert(ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)networkTime == ARROW(mclmac)_networkTime);
    for (i = 0; i < bytes; i++)
        assert(READ_ARRAY(REFERENCE ARROW(mclmac)_occupiedSlots, i) == READ_ARRAY(REFERENCE ARROW(ARROW(ARROW(mclmac)mac)ctrlpkt)occupiedSlots, i));

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_create_data_packet(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize);
    ARROW(mclmac)_isFragment = rand();
    ARROW(mclmac)_numberFragments = rand();
    ARROW(mclmac)_fragmentNumber = rand();

    mclmac_create_data_packet(REFERENCE mclmac);
#ifdef __LINUX__
    assert(mclmac->mac->datapkt != NULL);
#endif
    assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)isFragment == ARROW(mclmac)_isFragment);
    assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)totalFragments == ARROW(mclmac)_numberFragments);
    assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)fragmentNumber == ARROW(mclmac)_fragmentNumber);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_clear_cf_packet(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize);

    mclmac_set_nodeid(REFERENCE mclmac, 1);
    mclmac_set_destination_id(REFERENCE mclmac, 2);
    mclmac_create_cf_packet(REFERENCE mclmac);

    mclmac_clear_cf_packet(REFERENCE mclmac);
    // Assert the data is clear
    assert(ARROW(ARROW(ARROW(mclmac)mac)cfpkt)nodeID == 0);
    assert(ARROW(ARROW(ARROW(mclmac)mac)cfpkt)nodeID == 0);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_packet_data(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize);

    mclmac_create_data_packet(REFERENCE mclmac);

    // The total length of the data packet, including the fields, should not exceed 256 bytes. This as the
    // maximum buffer size for the SX1276 modem is 256 bytes.
    uint8_t size;
    int n = 1;//rand() % ITERATIONS;
    int j;
    for (int i = 0; i < n; i++)
    {
        size = 1 + rand() % 201;
        ARRAY data;
#ifdef __LINUX__
        data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
        create_array(&data, size);
#endif
        for (j = 0; j < size; j++)
            WRITE_ARRAY(REFERENCE data, rand(), j);

        mclmac_set_packet_data(REFERENCE mclmac, &data, size);

        assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)dataLength == size);
#ifdef __LINUX__
        assert(mclmac->mac->datapkt->data != NULL);
#endif
#ifdef __RIOT__
        assert(mclmac.mac.datapkt.data.size > 0);
#endif
        for (j = 0; j < size; j++)
        {
            assert(READ_ARRAY(REFERENCE ARROW(ARROW(ARROW(mclmac)mac)datapkt)data, j) == READ_ARRAY(REFERENCE data, j));
        }

        //mclmac_delete_data_from_packet(mclmac);
#ifdef __LINUX__
        free(data);
        data = NULL;
#endif
#ifdef __RIOT__
        free_array(&data);
#endif
    }

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_delete_data_from_packet(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize);

    mclmac_create_data_packet(REFERENCE mclmac);
    uint8_t size = 249;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (int i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    mclmac_set_packet_data(REFERENCE mclmac, &data, size);

    mclmac_delete_data_from_packet(REFERENCE mclmac);
    assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)dataLength == 0);
#ifdef __LINUX__
    assert(mclmac->mac->datapkt->data == NULL);
#endif
#ifdef __RIOT__
    assert(mclmac.mac.datapkt.data.size == 0);
#endif

#ifdef __LINUX__
    free(data);
#endif
#ifdef __RIOT__
    free_array(&data);
#endif

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_clear_data_from_packet(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize);

    mclmac_create_data_packet(REFERENCE mclmac);
    uint8_t size = 249;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (int i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    mclmac_set_packet_data(REFERENCE mclmac, &data, size);
    mclmac_clear_data_from_packet(REFERENCE mclmac);
    assert(ARROW(ARROW(ARROW(mclmac)mac)datapkt)dataLength == size);
    for (int i = 0; i < size; i++)
        assert(READ_ARRAY(REFERENCE ARROW(ARROW(ARROW(mclmac)mac)datapkt)data, i) == 0);

#ifdef __LINUX__
    free(data);
#endif
#ifdef __RIOT__
    free_array(&data);
#endif

    MCLMAC_destroy(&mclmac);
}*/

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
    size_t  dataQsize = 256;

    MCLMAC_init(&mclmac, &radio, nodeid, dataQsize);

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
    stub_mclmac_receive_ctrlpkt_sync(REFERENCE mclmac, REFERENCE ctrlpkt);
    assert(ARROW(ctrlpkt)networkTime > 0);
    assert(ARROW(ctrlpkt)hopCount > 0);
    assert(ARROW(ctrlpkt)currentSlot < MAX_NUMBER_SLOTS);
    assert(ARROW(ctrlpkt)initTime > 0);

    controlpacket_destroy(&ctrlpkt);
    MCLMAC_destroy(&mclmac);
}

void executetests_packets_handlers(void)
{
    /*pintf("Testing mclmac_create_cf_packet function.\n");
    test_mclmac_create_cf_packet();
    printf("Test passed.\n");

    printf("Testing mclmac_create_control_packet function.\n");
    test_mclmac_create_control_packet();
    printf("Test passed.\n");

    printf("Testing mclmac_create_data_packet function.\n");
    test_mclmac_create_data_packet();
    printf("Test passed.\n");

    printf("Testing mclmac_clear_cf_packet function.\n");
    test_mclmac_clear_cf_packet();
    printf("Test passed.\n");

    printf("Testing mclmac_set_packet_data function.\n");
    test_mclmac_set_packet_data();
    printf("Test passed.\n");

    printf("Testing mclmac_delete_data_from_packet function.\n");
    test_mclmac_delete_data_from_packet();
    printf("Test passed.\n");

    printf("Testing mclmac_clear_data_from_packet function.\n");
    test_mclmac_clear_data_from_packet();
    printf("Test passed.\n");*/

    printf("Tetsing test_stub_mclmac_receive_ctrlpkt_sync function.\n");
    test_stub_mclmac_receive_ctrlpkt_sync();
    printf("Test passed.\n");
    return;
}