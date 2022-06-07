#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "DataPacket.h"

#include "cUnit.h"

struct datapacket_data {
    DataPacket_t SINGLE_POINTER datapkt;
};

void setup_datapacket(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
    datapacket_init(&data->datapkt);
}

void teardown_datapacket(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
    datapacket_destroy(&data->datapkt);
}

void test_datapacket_init(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
#ifdef __LINUX__
    assert(data->datapkt != NULL);
    assert(data->datapkt->data == NULL);
#endif
    assert(ARROW(data->datapkt)type == -1);
    assert(ARROW(data->datapkt)destination_id == 0);
    assert(ARROW(data->datapkt)size == 0);
#ifdef __RIOT__
    assert(data->datapkt.data.size == 0);
#endif
}

void test_datapacket_destroy(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;

    datapacket_destroy(&data->datapkt);
#ifdef __LINUX__
    assert(data->datapkt == NULL);
#endif
#ifdef __RIOT__
    assert(data->datapkt.type == -1);
    assert(data->datapkt.destination_id == 0);
    assert(data->datapkt.size == 0);
    assert(data->datapkt.data.size == 0);
#endif
    datapacket_init(&data->datapkt);
}

void test_datapacket_create(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
    DataPacket_t *datapkt = REFERENCE data->datapkt;

    uint16_t destination_id = rand();
    uint8_t size = rand() % (PACKET_SIZE_MAC - 2);
    int8_t type = rand() % 4;
    ARRAY data_array;
#ifdef __LINUX__
    data_array = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data_array, size);
#endif
    for (uint i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data_array, rand(), i);

    datapacket_create(datapkt, type, destination_id, &data_array, size);
    assert(datapkt->destination_id == destination_id);
    assert(datapkt->size == size);
    assert(datapkt->type == type);
#ifdef __LINUX__
    assert(datapkt->data != NULL);
#endif
#ifdef __RIOT__
    assert(datapkt->data.size > 0);
#endif
    uint i = 0;
    for (i = 0; i < size; i++)
        assert(READ_ARRAY(REFERENCE datapkt->data, i) == READ_ARRAY(REFERENCE data_array, i));

#ifdef __LINUX__
    free(data_array);
#endif
#ifdef __RIOT__
    free_array(&data_array);
#endif
}

void test_datapacket_clear(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
    DataPacket_t *datapkt = REFERENCE data->datapkt;

    uint16_t destination_id = rand();
    uint8_t size = rand() % (PACKET_SIZE_MAC - 2);
    size = (size == 0 ? 1 : size);
    int8_t type = rand() % 4;
    ARRAY data_array;
#ifdef __LINUX__
    data_array = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data_array, size);
#endif
    for (uint i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data_array, rand(), i);

    datapacket_create(datapkt, type, destination_id, &data_array, size);
    
    datapacket_clear( datapkt);
    assert(datapkt->destination_id == 0);
    assert(datapkt->size == 0);
    assert(datapkt->type == -1);
#ifdef __LINUX__
    assert(datapkt->data == NULL);
#endif
#ifdef __RIOT__
    assert(datapkt->data.size == 0);
#endif

#ifdef __LINUX__
    free(data_array);
#endif
#ifdef __RIOT__
    free_array(&data_array);
#endif
}

void test_datapacket_get_packet_bytestring(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
    DataPacket_t *datapkt = REFERENCE data->datapkt;

    uint16_t destination_id = rand();
    uint8_t size = rand() % (PACKET_SIZE_MAC - 2);
    int8_t type = rand() % 10;
    ARRAY data_array;
#ifdef __LINUX__
    data_array = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data_array, size);
#endif
    uint i;
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data_array, rand(), i);

    datapacket_create(datapkt, type, destination_id, &data_array, size);

    ARRAY byteString;

    datapacket_get_packet_bytestring(datapkt, &byteString);
    assert(READ_ARRAY(REFERENCE byteString, 0) == type);
    assert(READ_ARRAY(REFERENCE byteString, 1) == (destination_id & 0xff00) >> 8);
    assert(READ_ARRAY(REFERENCE byteString, 2) == (destination_id & 0x00ff));
    assert(READ_ARRAY(REFERENCE byteString, 3) == size);
    for (i = 0; i < size; i++)
        assert(READ_ARRAY(REFERENCE byteString, i + 4) == READ_ARRAY(REFERENCE data_array, i));
    if (size < PACKET_SIZE_MAC - 4)
    {
        for (; i < PACKET_SIZE_MAC - 4; i++)
            assert(READ_ARRAY(REFERENCE byteString, i + 4) == 0);
    }

#ifdef __LINUX__
    free(data_array);
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&data_array);
    free_array(&byteString);
#endif
}

void test_datapacket_construct_from_bytestring(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
    DataPacket_t *datapkt = REFERENCE data->datapkt;

    uint16_t destination_id = rand();
    uint8_t size = rand() % (PACKET_SIZE_MAC - 2);
    int8_t type = rand() % 10;
    ARRAY data_array;
    ARRAY byteString;
#ifdef __LINUX__
    data_array = (uint8_t *)malloc(size * sizeof(uint8_t));
    byteString = (uint8_t *)malloc(PACKET_SIZE_MAC * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data_array, size);
    create_array(&byteString, PACKET_SIZE_MAC);
#endif
    uint i;
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data_array, rand(), i);

    WRITE_ARRAY(REFERENCE byteString, type, 0);
    WRITE_ARRAY(REFERENCE byteString, (destination_id & 0xff00) >> 8, 1);
    WRITE_ARRAY(REFERENCE byteString, (destination_id & 0x00ff), 2);
    WRITE_ARRAY(REFERENCE byteString, size, 3);
    for (i = 0; i < size; i++)
    {
        uint8_t e = READ_ARRAY(REFERENCE data_array, i);
        WRITE_ARRAY(REFERENCE byteString, e, i + 4);
    }
    if (size < PACKET_SIZE_MAC - 4)
    {
        for (; i < PACKET_SIZE_MAC - 4; i++)
            WRITE_ARRAY(REFERENCE byteString, 0, i);
    }
    datapacket_construct_from_bytestring(datapkt, &byteString);
    assert(datapkt->destination_id == destination_id);
    assert(datapkt->size == size);
    assert(datapkt->type == type);
    for (i = 0; i < size; i++)
    {
        assert(READ_ARRAY(REFERENCE datapkt->data, i) == READ_ARRAY(REFERENCE byteString, i + 4));
    }

#ifdef __LINUX__
    free(data_array);
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&data_array);
    free_array(&byteString);
#endif
}

void executeTestsDP(void)
{

    cUnit_t *tests;
    struct datapacket_data data;

    cunit_init(&tests, &setup_datapacket, &teardown_datapacket, (void *)&data);

    cunit_add_test(tests, &test_datapacket_init,    "datapacket_init\0");
    cunit_add_test(tests, &test_datapacket_destroy, "datapacket_destroy\0");
    cunit_add_test(tests, &test_datapacket_create,  "datapacket_create\0");
    cunit_add_test(tests, &test_datapacket_clear,   "datapacket_clear\0");
    cunit_add_test(tests, &test_datapacket_get_packet_bytestring,       "datapacket_get_packet_bytestring\0");
    cunit_add_test(tests, &test_datapacket_construct_from_bytestring,   "datapacket_construct_from_bytestring\0");

    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
