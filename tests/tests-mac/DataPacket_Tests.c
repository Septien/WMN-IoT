#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "DataPacket.h"

#include "cUnit.h"

struct datapacket_data {
    DataPacket_t *datapkt;
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

bool test_datapacket_init(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
    bool passed = true;
    // Already called the datapacket_init function within setup
#ifdef __LINUX__
    passed = passed && (data->datapkt != NULL);
    passed = passed && (data->datapkt->data == NULL);
#endif
    passed = passed && (data->datapkt->type == -1);
    passed = passed && (data->datapkt->destination_id[0] == 0);
    passed = passed && (data->datapkt->destination_id[1] == 0);
    passed = passed && (data->datapkt->size == 0);
#ifdef __RIOT__
    passed = passed && (data->datapkt->data.size == 0);
#endif

    return passed;
}

bool test_datapacket_destroy(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;

    datapacket_destroy(&data->datapkt);
    bool passed = true;
#ifdef __LINUX__
    passed = passed && (data->datapkt == NULL);
#endif
#ifdef __RIOT__
    passed = passed && (data->datapkt->type == -1);
    passed = passed && (data->datapkt->destination_id[0] == 0);
    passed = passed && (data->datapkt->destination_id[1] == 0);
    passed = passed && (data->datapkt->size == 0);
    passed = passed && (data->datapkt->data.size == 0);
#endif
    // So the teardown function doesn't break
    datapacket_init(&data->datapkt);

    return passed;
}

bool test_datapacket_create(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
    DataPacket_t *datapkt = data->datapkt;

    uint64_t destination_id[2] = {0};
    destination_id[0] = rand();
    destination_id[1] = rand();
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
    bool passed = true;
    passed = passed && (datapkt->destination_id[0] == destination_id[0]);
    passed = passed && (datapkt->destination_id[1] == destination_id[1]);
    passed = passed && (datapkt->size == size);
    passed = passed && (datapkt->type == type);
#ifdef __LINUX__
    passed = passed && (datapkt->data != NULL);
#endif
#ifdef __RIOT__
    passed = passed && (datapkt->data.size > 0);
#endif
    uint i = 0;
    for (i = 0; i < size; i++) {
        passed = passed && (READ_ARRAY(REFERENCE datapkt->data, i) == READ_ARRAY(REFERENCE data_array, i));
    }

#ifdef __LINUX__
    free(data_array);
#endif
#ifdef __RIOT__
    free_array(&data_array);
#endif

    return passed;
}

bool test_datapacket_clear(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
    DataPacket_t *datapkt = data->datapkt;

    uint64_t destination_id[2] = {0};
    destination_id[0] = rand();
    destination_id[1] = rand();
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
    for (uint i = 0; i < size; i++) {
        WRITE_ARRAY(REFERENCE data_array, rand(), i);
    }

    datapacket_create(datapkt, type, destination_id, &data_array, size);
    
    datapacket_clear( datapkt);
    bool passed = true;
    passed = passed && (datapkt->destination_id[0] == 0);
    passed = passed && (datapkt->destination_id[1] == 0);
    passed = passed && (datapkt->size == 0);
    passed = passed && (datapkt->type == -1);
#ifdef __LINUX__
    passed = passed && (datapkt->data == NULL);
#endif
#ifdef __RIOT__
    passed = passed && (datapkt->data.size == 0);
#endif

#ifdef __LINUX__
    free(data_array);
#endif
#ifdef __RIOT__
    free_array(&data_array);
#endif

    return passed;
}

bool test_datapacket_get_packet_bytestring(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
    DataPacket_t *datapkt = data->datapkt;

    uint64_t destination_id[2] = {0};
    destination_id[0] = rand();
    destination_id[1] = rand();
    uint8_t size = rand() % (PACKET_SIZE_MAC - 18);
    int8_t type = rand() % 10;
    ARRAY data_array;
#ifdef __LINUX__
    data_array = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data_array, size);
#endif
    uint i;
    for (i = 0; i < size; i++) {
        WRITE_ARRAY(REFERENCE data_array, rand(), i);
    }

    datapacket_create(datapkt, type, destination_id, &data_array, size);

    ARRAY byteString;

    datapacket_get_packet_bytestring(datapkt, &byteString);
    bool passed = true;
    passed = passed && (READ_ARRAY(REFERENCE byteString, 0) == type);
    uint64_t destination_id1 = 0;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 1)) << 56;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 2)) << 48;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 3)) << 40;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 4)) << 32;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 5)) << 24;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 6)) << 16;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 7)) << 8;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 8));
    passed = passed && (destination_id1 == destination_id[0]);
    destination_id1 = 0;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 9)) << 56;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 10)) << 48;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 11)) << 40;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 12)) << 32;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 13)) << 24;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 14)) << 16;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 15)) << 8;
    destination_id1 |= ((uint64_t) READ_ARRAY(REFERENCE byteString, 16));
    passed = passed && (READ_ARRAY(REFERENCE byteString, 17) == size);
    for (i = 0; i < size; i++) {
        passed = passed && (READ_ARRAY(REFERENCE byteString, i + 18) == READ_ARRAY(REFERENCE data_array, i));
    }
    if (size < PACKET_SIZE_MAC - 18)
    {
        for (; i < PACKET_SIZE_MAC - 18; i++) {
            passed = passed && (READ_ARRAY(REFERENCE byteString, i + 18) == 0);
        }
    }

#ifdef __LINUX__
    free(data_array);
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&data_array);
    free_array(&byteString);
#endif

    return passed;
}

bool test_datapacket_construct_from_bytestring(void *arg)
{
    struct datapacket_data *data = (struct datapacket_data *)arg;
    DataPacket_t *datapkt = data->datapkt;

    uint64_t destination_id[2] = {0};
    destination_id[0] = 12345678901;
    destination_id[1] = 10987654321;
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
    for (i = 0; i < size; i++) {
        WRITE_ARRAY(REFERENCE data_array, rand(), i);
    }

    WRITE_ARRAY(REFERENCE byteString, type, 0);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0xff00000000000000) >> 56, 1);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x00ff000000000000) >> 48, 2);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x0000ff0000000000) >> 40, 3);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x000000ff00000000) >> 32, 4);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x00000000ff000000) >> 24, 5);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x0000000000ff0000) >> 16, 6);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x000000000000ff00) >> 8,  7);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[0] & 0x00000000000000ff),       8);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0xff00000000000000) >> 56, 9);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x00ff000000000000) >> 48, 10);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x0000ff0000000000) >> 40, 11);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x000000ff00000000) >> 32, 12);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x00000000ff000000) >> 24, 13);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x0000000000ff0000) >> 16, 14);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x000000000000ff00) >> 8,  15);
    WRITE_ARRAY(REFERENCE byteString, (destination_id[1] & 0x00000000000000ff),       16);
    WRITE_ARRAY(REFERENCE byteString, size, 17);
    for (i = 0; i < size; i++)
    {
        uint8_t e = READ_ARRAY(REFERENCE data_array, i);
        WRITE_ARRAY(REFERENCE byteString, e, i + 18);
    }
    if (size < PACKET_SIZE_MAC - 18)
    {
        for (; i < PACKET_SIZE_MAC - 18; i++)
            WRITE_ARRAY(REFERENCE byteString, 0, i);
    }
    datapacket_construct_from_bytestring(datapkt, &byteString);
    bool passed = true;
    passed = passed && (datapkt->destination_id[0] == destination_id[0]);
    passed = passed && (datapkt->destination_id[1] == destination_id[1]);
    passed = passed && (datapkt->size == size);
    passed = passed && (datapkt->type == type);
    for (i = 0; i < size; i++)
    {
        passed = passed && (READ_ARRAY(REFERENCE datapkt->data, i) == READ_ARRAY(REFERENCE byteString, i + 18));
    }

#ifdef __LINUX__
    free(data_array);
    free(byteString);
#endif
#ifdef __RIOT__
    free_array(&data_array);
    free_array(&byteString);
#endif

    return passed;
}

void executeTestsDP(void)
{

    cUnit_t *tests;
    struct datapacket_data data;
    DataPacket_t datapkt;
    data.datapkt = &datapkt;

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
