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
    assert(ARROW(datapkt)isFragment == false);
    assert(ARROW(datapkt)totalFragments == 0);
    assert(ARROW(datapkt)fragmentNumber == 0);
    assert(ARROW(datapkt)packetLength == 0);
    assert(ARROW(datapkt)dataLength == 0);
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
    assert(datapkt.data.size == 0);
    assert(ARROW(datapkt)isFragment == false);
    assert(ARROW(datapkt)totalFragments == 0);
    assert(ARROW(datapkt)fragmentNumber == 0);
    assert(ARROW(datapkt)packetLength == 0);
    assert(ARROW(datapkt)dataLength == 0);
#endif
}

void test_datapacket_create(void)
{
    DataPacket_t SINGLE_POINTER pkt;
    datapacket_init(&pkt);

    bool isFragment = true;
    uint8_t totalFragments = rand() % 256;
    uint8_t fragmentNumber = rand() % 256;
    ARRAY data;
    size_t size = 249;

    datapacket_create(REFERENCE pkt, isFragment, totalFragments, fragmentNumber, NULL, 0);
    assert(ARROW(pkt)isFragment == isFragment);
    assert(ARROW(pkt)totalFragments == totalFragments);
    assert(ARROW(pkt)fragmentNumber == fragmentNumber);
    assert(ARROW(pkt)dataLength == 0);
#ifdef __LINUX__
    assert(pkt->data == NULL);
#endif
#ifdef __RIOT__
    assert(pkt.data.size == 0);
#endif

    datapacket_destroy(&pkt);
    datapacket_init(&pkt);

#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    uint i;
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    datapacket_create(REFERENCE pkt, isFragment, totalFragments, fragmentNumber, &data, size);
    assert(ARROW(pkt)isFragment == isFragment);
    assert(ARROW(pkt)totalFragments == totalFragments);
    assert(ARROW(pkt)fragmentNumber == fragmentNumber);
    assert(ARROW(pkt)dataLength == size);
    for (i = 0; i < size; i++)
        assert(READ_ARRAY(REFERENCE ARROW(pkt)data, i) == READ_ARRAY(REFERENCE data, i));
    
    datapacket_destroy(&pkt);
}

void test_datapacket_clear(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint8_t size = 249, i;
    ARRAY data;
#ifdef __LINUX__
    data = malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);
    bool isFragment = true;
    uint8_t totalFragments = rand() % 256;
    uint8_t fragmentNumber = rand() % 256;

    datapacket_create(REFERENCE datapkt, isFragment, totalFragments, fragmentNumber, &data, size);
    
    datapacket_clear(REFERENCE datapkt);
    assert(ARROW(datapkt)isFragment == false);
    assert(ARROW(datapkt)totalFragments == 0);
    assert(ARROW(datapkt)totalFragments == 0);
    assert(ARROW(datapkt)fragmentNumber == 0);
    assert(ARROW(datapkt)dataLength == 0);
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

void test_datapacket_set_isFragment(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    bool isFragment = true;
    datapacket_set_isFragment(REFERENCE datapkt, isFragment);
    assert(ARROW(datapkt)isFragment == isFragment);

    isFragment = false;
    datapacket_set_isFragment(REFERENCE datapkt, isFragment);
    assert(ARROW(datapkt)isFragment == isFragment);

    datapacket_destroy(&datapkt);
}

void test_datapacket_get_isFragment(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    datapacket_set_isFragment(REFERENCE datapkt, true);
    bool isFrag = datapacket_get_isFragment(REFERENCE datapkt);
    assert(isFrag);

    datapacket_set_isFragment(REFERENCE datapkt, false);
    isFrag = datapacket_get_isFragment(REFERENCE datapkt);
    assert(!isFrag);

    datapacket_destroy(&datapkt);
}

void test_datapacket_set_total_fragments(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint8_t i;
    for (i = 0; i < 255; i++)
    {
        datapacket_set_total_fragments(REFERENCE datapkt, i);
        assert(ARROW(datapkt)totalFragments == i);
    }

    datapacket_destroy(&datapkt);
}

void test_datapacket_get_total_fragments(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint8_t i, totalF;
    for (i = 0; i < 255; i++)
    {
        datapacket_set_total_fragments(REFERENCE datapkt, i);
        totalF = datapacket_get_total_fragments(REFERENCE datapkt);
        assert(totalF == i);
    }

    datapacket_destroy(&datapkt);
}

void test_datapacket_set_fragment_number(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    datapacket_set_total_fragments(REFERENCE datapkt, 255);
    uint8_t i;
    for (i = 0; i < 255; i++)
    {
        datapacket_set_fragment_number(REFERENCE datapkt, i);
        assert(ARROW(datapkt)fragmentNumber == i);
    }

    datapacket_destroy(&datapkt);
}

void test_datapacket_get_fragment_number(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    datapacket_set_total_fragments(REFERENCE datapkt, 255);
    uint8_t i, fnumber;
    for (i = 0; i < 255; i++)
    {
        datapacket_set_fragment_number(REFERENCE datapkt, i);
        fnumber = datapacket_get_fragment_number(REFERENCE datapkt);
        assert(fnumber == i);
    }

    datapacket_destroy(&datapkt);
}

void test_datapacket_set_data(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint8_t size = 249, i;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);
    
    datapacket_set_data(REFERENCE datapkt, &data, size);
    
    for (i = 0; i < size; i++)
        assert(READ_ARRAY(REFERENCE ARROW(datapkt)data, i) == READ_ARRAY(REFERENCE ARROW(datapkt)data, i));

#ifdef __LINUX__
    free(data);
#endif
#ifdef __RIOT__
    free_array(&data);
#endif

    datapacket_destroy(&datapkt);
}

void test_datapacket_delete_data(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint8_t size = 249, i;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);
    
    datapacket_set_data(REFERENCE datapkt, &data, size);
    
    datapacket_delete_data(REFERENCE datapkt);
    assert(ARROW(datapkt)dataLength == 0);
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

void test_datapacket_clear_data(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint8_t size = 249, i;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));;
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    datapacket_set_data(REFERENCE datapkt, &data, size);
    
    datapacket_clear_data(REFERENCE datapkt);
    assert(ARROW(datapkt)dataLength == size);
    for (i = 0; i < size; i++)
        assert(READ_ARRAY(REFERENCE ARROW(datapkt)data, i) == 0);

#ifdef __LINUX__
    free(data);
#endif
#ifdef __RIOT__
    free_array(&data);
#endif
    datapacket_destroy(&datapkt);
}

void test_datapacket_get_data(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint8_t size = 249, i;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);
    
    datapacket_set_data(REFERENCE datapkt, &data, size);

    ARRAY dataA;
    uint8_t sizeA;
    datapacket_get_data(REFERENCE datapkt, &dataA, &sizeA);

    assert(sizeA == size);
    assert(sizeA == ARROW(datapkt)dataLength);
#ifdef __LINUX__
    assert(dataA != NULL);
#endif
#ifdef __RIOT__
    assert(dataA.size > 0);
#endif
    for (i = 0; i < size; i++)
    {
        assert(READ_ARRAY(REFERENCE dataA, i) == READ_ARRAY(REFERENCE data, i));
        assert(READ_ARRAY(REFERENCE dataA, i) == READ_ARRAY(REFERENCE ARROW(datapkt)data, i));
    }

#ifdef __LINUX__
    free(data);
    free(dataA);
#endif
#ifdef __RIOT__
    free_array(&data);
    free_array(&dataA);
#endif

    datapacket_destroy(&datapkt);
}

void test_datapacket_get_data_length(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint8_t size = 249, i;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    datapacket_set_data(REFERENCE datapkt, &data, size);

    size_t dataS = datapacket_get_data_length(REFERENCE datapkt);
    assert(dataS == size);
    assert(dataS == ARROW(datapkt)dataLength);

#ifdef __LINUX__
    free(data);
#endif
#ifdef __RIOT__
    free_array(&data);
#endif

    datapacket_destroy(&datapkt);
}

void test_datapacket_get_packet_length(void)
{
    DataPacket_t SINGLE_POINTER datapkt;
    datapacket_init(&datapkt);

    uint8_t size = 249, i;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    bool isFragment = true;
    uint8_t totalFragments = rand() % 256;
    uint8_t fragmentNumber = rand() % 256;

    size_t sizeP = sizeof(isFragment);
    sizeP +=sizeof(totalFragments);
    sizeP += sizeof(fragmentNumber);
    sizeP += sizeof(size);
    sizeP += size;

    datapacket_create(REFERENCE datapkt, isFragment, totalFragments, fragmentNumber, &data, size);
    size_t pSize = datapacket_get_packet_length(REFERENCE datapkt);

    assert(pSize == sizeP);
    assert(pSize == ARROW(datapkt)packetLength);
    assert(pSize <= 255);

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

    uint8_t size = 249, i;
    ARRAY data;
#ifdef __LINUX__
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&data, size);
#endif
    for (i = 0; i < size; i++)
        WRITE_ARRAY(REFERENCE data, rand(), i);

    bool isFragment = true;
    uint8_t totalFragments = rand() % 256;
    uint8_t fragmentNumber = rand() % 256;

    datapacket_create(REFERENCE datapkt, isFragment, totalFragments, fragmentNumber, &data, size);

    ARRAY byteString;
    size_t bsSize;

    size_t pSize = datapacket_get_packet_length(REFERENCE datapkt);

    datapacket_get_packet_bytestring(REFERENCE datapkt, &byteString, &bsSize);
    assert(bsSize == pSize);
    assert(READ_ARRAY(REFERENCE byteString, 0) == isFragment);
    assert(READ_ARRAY(REFERENCE byteString, 1) == totalFragments);
    assert(READ_ARRAY(REFERENCE byteString, 2) == fragmentNumber);
    assert(READ_ARRAY(REFERENCE byteString, 3) == size);
    for (i = 0; i < size; i++)
        assert(READ_ARRAY(REFERENCE byteString, i + 4) == READ_ARRAY(REFERENCE data, i));

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

    size_t pSize = 256;
    ARRAY byteString;
#ifdef __LINUX__
    byteString = (uint8_t *)malloc(pSize * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&byteString, pSize);
#endif
    uint8_t size = 249;
    int i;
    WRITE_ARRAY(REFERENCE byteString, 1, 0);
    WRITE_ARRAY(REFERENCE byteString, rand() % 256, 1);
    WRITE_ARRAY(REFERENCE byteString, rand() % 256, 2);
    WRITE_ARRAY(REFERENCE byteString, size, 3);
    for (i = 4; i < size + 4; i++)
        WRITE_ARRAY(REFERENCE byteString, rand() % 256, i);
    WRITE_ARRAY(REFERENCE byteString, 0xff, i);
    WRITE_ARRAY(REFERENCE byteString, 0xff, i + 1);

    datapacket_construct_from_bytestring(REFERENCE datapkt, &byteString, pSize);
    assert(ARROW(datapkt)packetLength == pSize);
    assert(ARROW(datapkt)isFragment == READ_ARRAY(REFERENCE byteString, 0));
    assert(ARROW(datapkt)totalFragments == READ_ARRAY(REFERENCE byteString, 1));
    assert(ARROW(datapkt)fragmentNumber == READ_ARRAY(REFERENCE byteString, 2));
    assert(ARROW(datapkt)dataLength == READ_ARRAY(REFERENCE byteString, 3));
    for (i = 0; i < size; i++)
    {
        assert(READ_ARRAY(REFERENCE ARROW(datapkt)data, i) == READ_ARRAY(REFERENCE byteString, i + 4));
    }

#ifdef __LINUX__
    free(byteString);
#endif
#ifdef __RIOT__
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

    printf("Testing datapacket_destroy function.\n");
    test_datapacket_destroy();
    printf("Test passed.\n");

    printf("Testing datapacket_create function.\n");
    test_datapacket_create();
    printf("Test passed.\n");

    printf("Testing datapacket_clear function.\n");
    test_datapacket_clear();
    printf("Test passed.\n");

    printf("Testing datapacket_set_isFragment function.\n");
    test_datapacket_set_isFragment();
    printf("Test passed.\n");

    printf("Testing datapacket_get_isFragment function.\n");
    test_datapacket_get_isFragment();
    printf("Test passed.\n");

    printf("Testing datapacket_set_total_fragments function.\n");
    test_datapacket_set_total_fragments();
    printf("Test passed.\n");

    printf("Testing datapacket_get_total_fragments function.\n");
    test_datapacket_get_total_fragments();
    printf("Test passed.\n");

    printf("Testing datapacket_set_fragment_number function.\n");
    test_datapacket_set_fragment_number();
    printf("Test passed.\n");

    printf("Testing datapacket_get_fragment_number function.\n");
    test_datapacket_get_fragment_number();
    printf("Test passed.\n");

    printf("Testing datapacket_set_data function.\n");
    test_datapacket_set_data();
    printf("Test passed.\n");

    printf("Testing datapacket_delete_data function.\n");
    test_datapacket_delete_data();
    printf("Test passed.\n");

    printf("Testing datapacket_clear_data function.\n");
    test_datapacket_clear_data();
    printf("Test passed.\n");

    printf("Testing datapacket_get_data function.\n");
    test_datapacket_get_data();
    printf("Test passed.\n");

    printf("Testing datapacket_get_data_length function.\n");
    test_datapacket_get_data_length();
    printf("Test passed.\n");

    printf("Testing datapacket_get_packet_length function.\n");
    test_datapacket_get_packet_length();
    printf("Test passed.\n");

    printf("Teting datapacket_get_packet_bytestring function.\n");
    test_datapacket_get_packet_bytestring();
    printf("Test passed.\n");

    printf("Test datapacket_construct_from_bytestring function.\n");
    test_datapacket_construct_from_bytestring();
    printf("Test passed.\n");
}
