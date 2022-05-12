#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "DataPacket.h"


void test_datapacket_init(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);
#ifdef __LINUX__
    assert(datapkt != NULL);
    assert(datapkt->data == NULL);
#endif
    assert(ARROW(datapkt)type == -1);
    assert(ARROW(datapkt)destination_id == 0);
    assert(ARROW(datapkt)size == 0);
#ifdef __RIOT__
    assert(datapkt.data.size == 0);
#endif

    datapacket_destroy(&datapkt);
}

void test_datapacket_destroy(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    datapacket_destroy(&datapkt);
#ifdef __LINUX__
    assert(datapkt == NULL);
#endif
#ifdef __RIOT__
    assert(datapkt.type == -1);
    assert(datapkt.destination_id == 0);
    assert(datapkt.size == 0);
    assert(datapkt.data.size == 0);
#endif
}

void test_datapacket_create(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint16_t destination_id = rand();
    uint8_t size = rand() % (PACKET_SIZE_MAC - 2);
    int8_t type = rand() % 4;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (uint i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    datapacket_create(REFERENCE datapkt, type, destination_id, &data, size);
    assert(ARROW(datapkt)destination_id == destination_id);
    assert(ARROW(datapkt)size == size);
    assert(ARROW(datapkt)type == type);
#ifdef __LINUX__
    assert(datapkt->data != NULL);
#endif
#ifdef __RIOT__
    assert(datapkt.data.size > 0);
#endif
    uint i = 0;
    for (i = 0; i < size; i++)
        assert(READ_ARRAY(REFERENCE ARROW(datapkt)data, i) == READ_ARRAY(REFERENCE data, i));

#ifdef __LINUX__
    free(data);
#endif
#ifdef __RIOT__
    free_array(&data);
#endif
    datapacket_destroy(&datapkt);
}

void test_datapacket_clear(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint16_t destination_id = rand();
    uint8_t size = rand() % (PACKET_SIZE_MAC - 2);
    size = (size == 0 ? 1 : size);
    int8_t type = rand() % 4;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (uint i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    datapacket_create(REFERENCE datapkt, type, destination_id, &data, size);
    
    datapacket_clear(REFERENCE datapkt);
    assert(ARROW(datapkt)destination_id == 0);
    assert(ARROW(datapkt)size == 0);
    assert(ARROW(datapkt)type == -1);
#ifdef __LINUX__
    assert(datapkt->data == NULL);
#endif
#ifdef __RIOT__
    assert(datapkt.data.size == 0);
#endif

#ifdef __LINUX__
    free(data);
#endif
#ifdef __RIOT__
    free_array(&data);
#endif
    datapacket_destroy(&datapkt);
}

void test_datapacket_get_packet_bytestring(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint16_t destination_id = rand();
    uint8_t size = rand() % (PACKET_SIZE_MAC - 2);
    int8_t type = rand() % 10;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    uint i;
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    datapacket_create(REFERENCE datapkt, type, destination_id, &data, size);

    ARRAY byteString;

    datapacket_get_packet_bytestring(REFERENCE datapkt, &byteString);
    assert(READ_ARRAY(REFERENCE byteString, 0) == type);
    assert(READ_ARRAY(REFERENCE byteString, 1) == (destination_id & 0xff00) >> 8);
    assert(READ_ARRAY(REFERENCE byteString, 2) == (destination_id & 0x00ff));
    assert(READ_ARRAY(REFERENCE byteString, 3) == size);
    for (i = 0; i < size; i++)
        assert(READ_ARRAY(REFERENCE byteString, i + 4) == READ_ARRAY(REFERENCE data, i));
    if (size < PACKET_SIZE_MAC - 4)
    {
        for (; i < PACKET_SIZE_MAC - 4; i++)
            assert(READ_ARRAY(REFERENCE byteString, i + 4) == 0);
    }

#ifdef __LINUX__
    free(data);
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&data);
    free_array(&byteString);
#endif

    datapacket_destroy(&datapkt);
}

void test_datapacket_construct_from_bytestring(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint16_t destination_id = rand();
    uint8_t size = rand() % (PACKET_SIZE_MAC - 2);
    int8_t type = rand() % 10;
    ARRAY data;
    ARRAY byteString;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
    byteString = (uint8_t *)malloc(PACKET_SIZE_MAC * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
    create_array(&byteString, PACKET_SIZE_MAC);
#endif
    uint i;
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    WRITE_ARRAY(REFERENCE byteString, type, 0);
    WRITE_ARRAY(REFERENCE byteString, (destination_id & 0xff00) >> 8, 1);
    WRITE_ARRAY(REFERENCE byteString, (destination_id & 0x00ff), 2);
    WRITE_ARRAY(REFERENCE byteString, size, 3);
    for (i = 0; i < size; i++)
    {
        uint8_t e = READ_ARRAY(REFERENCE data, i);
        WRITE_ARRAY(REFERENCE byteString, e, i + 4);
    }
    if (size < PACKET_SIZE_MAC - 4)
    {
        for (; i < PACKET_SIZE_MAC - 4; i++)
            WRITE_ARRAY(REFERENCE byteString, 0, i);
    }
    datapacket_construct_from_bytestring(REFERENCE datapkt, &byteString);
    assert(ARROW(datapkt)destination_id == destination_id);
    assert(ARROW(datapkt)size == size);
    assert(ARROW(datapkt)type == type);
    for (i = 0; i < size; i++)
    {
        assert(READ_ARRAY(REFERENCE ARROW(datapkt)data, i) == READ_ARRAY(REFERENCE byteString, i + 4));
    }

#ifdef __LINUX__
    free(data);
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&data);
    free_array(&byteString);
#endif

    datapacket_destroy(&datapkt);
}

void executeTestsDP(void)
{

    srand(time(NULL));
    
    printf("Testing datapacket_init function.\n");
    test_datapacket_init();
    printf("Test passed.\n");

    printf("Testing the datapacket_destroy function.\n");
    test_datapacket_init();
    printf("Test passed.\n");

    printf("Testing the datapacket_create function.\n");
    test_datapacket_create();
    printf("Test passed.\n");

    printf("Testing the datapacket_clear function.\n");
    test_datapacket_clear();
    printf("Test passed.\n");

    printf("Teting datapacket_get_packet_bytestring function.\n");
    test_datapacket_get_packet_bytestring();
    printf("Test passed.\n");

    printf("Test datapacket_construct_from_bytestring function.\n");
    test_datapacket_construct_from_bytestring();
    printf("Test passed.\n");
}
