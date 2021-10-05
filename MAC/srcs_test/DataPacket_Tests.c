#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "DataPacket.h"

#if 0
void testinitDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    assert(datapkt != NULL);
    assert(datapkt->data == NULL);

    destroyPacketDP(&datapkt);
}

void testdestroyPacketDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    destroyPacketDP(&datapkt);
    assert(datapkt == NULL);
}

void testsetIsFragmentDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    bool isFragment = true;
    setIsFragmentDP(datapkt, isFragment);
    assert(datapkt->isFragment == isFragment);

    isFragment = false;
    setIsFragmentDP(datapkt, isFragment);
    assert(datapkt->isFragment == isFragment);

    destroyPacketDP(&datapkt);
}

void testisFragmentDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    setIsFragmentDP(datapkt, true);
    bool isFrag = isFragmentDP(datapkt);
    assert(isFrag);

    setIsFragmentDP(datapkt, false);
    isFrag = isFragmentDP(datapkt);
    assert(isFrag == false);

    destroyPacketDP(&datapkt);
}

void testsetTotalFragmentsDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    uint8_t i;
    for (i = 0; i < 255; i++)
    {
        setTotalFragmentsDP(datapkt, i);
        assert(datapkt->totalFragments == i);
    }

    destroyPacketDP(&datapkt);
}

void testgetTotalFragmentsDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    uint8_t i, totalF;
    for (i = 0; i < 255; i++)
    {
        setTotalFragmentsDP(datapkt, i);
        totalF = getTotalFragmentsDP(datapkt);
        assert(totalF == i);
    }

    destroyPacketDP(&datapkt);
}

void testsetFragmentNumberDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    setTotalFragmentsDP(datapkt, 255);
    uint8_t i;
    for (i = 0; i < 255; i++)
    {
        setFragmentNumberDP(datapkt, i);
        assert(datapkt->fragmentNumber == i);
    }

    destroyPacketDP(&datapkt);
}

void testgetFragmentNumberDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    setTotalFragmentsDP(datapkt, 255);
    uint8_t i, fnumber;
    for (i = 0; i < 255; i++)
    {
        setFragmentNumberDP(datapkt, i);
        fnumber = getFragmentNumberDP(datapkt);
        assert(fnumber == i);
    }

    destroyPacketDP(&datapkt);
}

void testsetPacketDataDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    uint8_t size = 249, i;
    uint8_t *data = malloc(size * sizeof(void));
    for (i = 0; i < size; i++)
    {
        data[i] = rand();
    }
    setPacketDataDP(datapkt, (void **)&data, size);
    for (i = 0; i < size; i++)
        assert(datapkt->data[i] == data[i]);

    free(data);
    destroyPacketDP(&datapkt);
}

void testdeleteDataDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    uint8_t size = 249, i;
    uint8_t *data = malloc(size * sizeof(void));
    for (i = 0; i < size; i++)
    {
        data[i] = rand();
    }
    setPacketDataDP(datapkt, (void **)&data, size);
    
    deleteDataDP(datapkt);
    assert(datapkt->dataLength == 0);
    assert(datapkt->data == NULL);

    free(data);
    destroyPacketDP(&datapkt);
}

void testclearDataDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    uint8_t size = 249, i;
    uint8_t *data = malloc(size * sizeof(void));
    for (i = 0; i < size; i++)
    {
        data[i] = rand();
    }
    setPacketDataDP(datapkt, (void **)&data, size);
    
    clearDataDP(datapkt);
    assert(datapkt->dataLength == size);
    for (i = 0; i < size; i++)
        assert(datapkt->data[i] == 0);

    free(data);
    destroyPacketDP(&datapkt);
}

void testgetPacketDataDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    uint8_t size = 249, i;
    uint8_t *data = malloc(size * sizeof(void));
    for (i = 0; i < size; i++)
    {
        data[i] = rand();
    }
    setPacketDataDP(datapkt, (void **)&data, size);

    uint8_t *dataA;
    uint8_t sizeA;
    getPacketDataDP(datapkt, (void **)&dataA, &sizeA);

    assert(sizeA == size);
    assert(sizeA == datapkt->dataLength);
    assert(dataA != NULL);
    for (i = 0; i < size; i++)
    {
        assert(dataA[i] == data[i]);
        assert(dataA[i] == datapkt->data[i]);
    }

    free(data);
    free(dataA);
    destroyPacketDP(&datapkt);
}

void testgetDataLengthDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    uint8_t size = 249, i;
    uint8_t *data = malloc(size * sizeof(void));
    for (i = 0; i < size; i++)
    {
        data[i] = rand();
    }
    setPacketDataDP(datapkt, (void **)&data, size);

    size_t dataS = getDataLengthDP(datapkt);
    assert(dataS == size);
    assert(dataS == datapkt->dataLength);

    free(data);
    destroyPacketDP(&datapkt);
}

void testcreatePacketDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    uint8_t size = 249, i;
    uint8_t *data = malloc(size * sizeof(void));
    for (i = 0; i < size; i++)
    {
        data[i] = rand();
    }
    bool isFragment = true;
    uint8_t totalFragments = rand() % 256;
    uint8_t fragmentNumber = rand() % 256;

    createPacketDP(datapkt, isFragment, totalFragments, fragmentNumber, (void **)&data, size);
    assert(datapkt->isFragment == isFragment);
    assert(datapkt->totalFragments == totalFragments);
    assert(datapkt->fragmentNumber == fragmentNumber);
    assert(datapkt->dataLength == size);
    for (i = 0; i < size; i++)
    {
        assert(datapkt->data[i] == data[i]);
    }

    free(datapkt->data);
    size = 0;
    createPacketDP(datapkt, isFragment, totalFragments, fragmentNumber, NULL, size);
    assert(datapkt->data == NULL);


    free(data);
    destroyPacketDP(&datapkt);
}

void testclearPacketDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    uint8_t size = 249, i;
    uint8_t *data = malloc(size * sizeof(void));
    for (i = 0; i < size; i++)
    {
        data[i] = rand();
    }
    bool isFragment = true;
    uint8_t totalFragments = rand() % 256;
    uint8_t fragmentNumber = rand() % 256;

    createPacketDP(datapkt, isFragment, totalFragments, fragmentNumber, (void **)&data, size);
    
    clearPacketDP(datapkt);
    assert(datapkt->isFragment == false);
    assert(datapkt->totalFragments == 0);
    assert(datapkt->totalFragments == 0);
    assert(datapkt->fragmentNumber == 0);
    assert(datapkt->dataLength == 0);
    assert(datapkt->data == NULL);

    free(data);
    destroyPacketDP(&datapkt);
}

void testgetPacketLengthDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    uint8_t size = 249, i;
    uint8_t *data = malloc(size * sizeof(void));
    for (i = 0; i < size; i++)
    {
        data[i] = rand();
    }
    bool isFragment = true;
    uint8_t totalFragments = rand() % 256;
    uint8_t fragmentNumber = rand() % 256;

    size_t sizeP = sizeof(isFragment);
    sizeP +=sizeof(totalFragments);
    sizeP += sizeof(fragmentNumber);
    sizeP += sizeof(size);
    sizeP += size;

    createPacketDP(datapkt, isFragment, totalFragments, fragmentNumber, (void **)&data, size);
    size_t pSize = getPacketLengthDP(datapkt);

    assert(pSize == sizeP);
    assert(pSize == datapkt->packetLength);
    assert(pSize <= 255);

    free(data);
    destroyPacketDP(&datapkt);
}

void testgetPacketByteStringDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    uint8_t size = 249, i;
    uint8_t *data = malloc(size * sizeof(void));
    for (i = 0; i < size; i++)
    {
        data[i] = rand();
    }
    bool isFragment = true;
    uint8_t totalFragments = rand() % 256;
    uint8_t fragmentNumber = rand() % 256;

    createPacketDP(datapkt, isFragment, totalFragments, fragmentNumber, (void **)&data, size);

    uint8_t *byteString;
    size_t bsSize;

    size_t pSize = getPacketLengthDP(datapkt);

    getPacketByteStringDP(datapkt, &byteString, &bsSize);
    assert(bsSize == pSize);
    assert(byteString[0] == isFragment);
    assert(byteString[1] == totalFragments);
    assert(byteString[2] == fragmentNumber);
    assert(byteString[3] == size);
    for (i = 0; i < size; i++)
        assert(byteString[i + 4] == data[i]);

    free(data);
    free(byteString);
    destroyPacketDP(&datapkt);
}

void testconstructPktFromByteStringDP(void)
{
    DataPacket_t *datapkt;
    initDP(&datapkt);

    size_t pSize = 256;
    uint8_t *byteString = (uint8_t *)malloc(pSize * sizeof(uint8_t));
    uint8_t size = 249;
    int i;
    byteString[0] = 1;
    byteString[1] = rand() % 256;
    byteString[2] = rand() % 256;
    byteString[3] = size;
    for (i = 4; i < size + 4; i++)
        byteString[i] = rand() % 256;
    byteString[i] = 0xff;
    byteString[i + 1] = 0xff;
    
    constructPktFromByteStringDP(datapkt, byteString, pSize);
    assert(datapkt->packetLength == pSize);
    assert(datapkt->isFragment == byteString[0]);
    assert(datapkt->totalFragments == byteString[1]);
    assert(datapkt->fragmentNumber == byteString[2]);
    assert(datapkt->dataLength == byteString[3]);
    for (i = 0; i < size; i++)
        assert(datapkt->data[i] == byteString[i + 4]);

    free(byteString);
    destroyPacketDP(&datapkt);
}

void executeTestsDP(void)
{
    srand(time(NULL));
    printf("Testing initDP function.\n");
    testinitDP();
    printf("Test passed.\n");

    printf("Testing destroyPacketDP function.\n");
    testdestroyPacketDP();
    printf("Test passed.\n");

    printf("Testing setIsFragmentDP function.\n");
    testsetIsFragmentDP();
    printf("Test passed.\n");

    printf("Testing isFragmentDP function.\n");
    testisFragmentDP();
    printf("Test passed.\n");

    printf("Testing setTotalFragmentsDP function.\n");
    testsetTotalFragmentsDP();
    printf("Test passed.\n");

    printf("Testing getTotalFragmentsDP function.\n");
    testgetTotalFragmentsDP();
    printf("Test passed.\n");

    printf("Testing setFragmentNumberDP function.\n");
    testsetFragmentNumberDP();
    printf("Test passed.\n");

    printf("Testing getFragmentNumberDP function.\n");
    testgetFragmentNumberDP();
    printf("Test passed.\n");

    printf("Testing setPacketDataDP function.\n");
    testsetPacketDataDP();
    printf("Test passed.\n");

    printf("Testing deleteDataDP function.\n");
    testdeleteDataDP();
    printf("Test passed.\n");

    printf("Testing clearDataDP function.\n");
    testclearDataDP();
    printf("Test passed.\n");

    printf("Testing getPacketDataDP function.\n");
    testgetPacketDataDP();
    printf("Test passed.\n");

    printf("Testing getDataLengthDP function.\n");
    testgetDataLengthDP();
    printf("Test passed.\n");

    printf("Testing createPacketDP function.\n");
    testcreatePacketDP();
    printf("Test passed.\n");

    printf("Testing clearPacketDP function.\n");
    testclearPacketDP();
    printf("Test passed.\n");

    printf("Testing getPacketLengthDP function.\n");
    testgetPacketLengthDP();
    printf("Test passed.\n");

    printf("Teting getPacketByteStringDP function.\n");
    testgetPacketByteStringDP();
    printf("Test passed.\n");

    printf("Test constructPktFromByteString function.\n");
    testconstructPktFromByteStringDP();
    printf("Test passed.\n");
}
#endif