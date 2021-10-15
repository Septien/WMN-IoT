#include <stdio.h>
#include <stdlib.h>

#include "assert.h"
#include "memory_tests.h"
#include "memory.h"

void test_memory_init(void)
{
    memory_init();    
}

void test_create_array(void)
{
    array_t array;
    array.head = NULL;

    /* Test cases:
        -Size = 0, should return 0
        -Size = 1, Should return 1.
        -Size in (1, available-1), should return 1.
        -Size = available - 1, should return 1.
        -Size = available, should return 1.
        -Size > available, should return 0.
        -No available memory, should return 0
    */
    size_t size = 0;
    int ret = create_array(&array, size);
    assert(ret == 0);

    size = 1;
    ret = create_array(&array, size);
    assert(ret == 1);
    assert(array.head != NULL);
    assert(array.size == 1);
    assert(array.head->next == NULL);

    ret = free_array(&array);

    size = 1 + (rand() % MAX_NUMBER_BLOCKS);
    ret = create_array(&array, size);
    assert(ret == 1);
    assert(array.head != NULL);
    assert(array.size == size);
    memory_block_t *block;
    block = array.head;
    size_t count = 1;
    while (block)
    {
        if (block->next != NULL)
        {
            block = block->next;
            count++;
        }
        else
            block = NULL;
    }
    assert(count == size);
    ret = free_array(&array);

    size = MAX_NUMBER_BLOCKS - 1;
    ret = create_array(&array, size);
    assert(ret == 1);
    assert(array.head != NULL);
    assert(array.size == size);
    block = array.head;
    count = 1;
    while (block)
    {
        if (block->next != NULL)
        {
            block = block->next;
            count++;
        }
        else
            block = NULL;
    }
    assert(count == size);
    ret = free_array(&array);

    size = MAX_NUMBER_BLOCKS;
    ret = create_array(&array, size);
    assert(ret == 1);
    assert(array.head != NULL);
    assert(array.size == MAX_NUMBER_BLOCKS);
    block = array.head;
    count = 1;
    while (block)
    {
        if (block->next != NULL)
        {
            block = block->next;
            count++;
        }
        else
            block = NULL;
    }
    assert(count == size);
    ret = free_array(&array);

    size = MAX_NUMBER_BLOCKS + 1;
    ret = create_array(&array, size);
    assert(ret == 0);
    assert(array.head == NULL);
    assert(array.size == 0);    
}

void test_free_array(void)
{
    array_t array;
    array.head = NULL;
    size_t size = 0;

    /** Test cases:
     *  -Head is NULL (not initialized), should return 0.
     *  -Size is 1, should return 1.
     *  -Size is in (1, available - 1), should return 1.
     *  -Size is available -1, should return 1.
     *  -It is not possible to release more than available, as create_array won't allow it.
     *  -Release an already released array, shoud return 0.
     * */
    int ret = free_array(&array);
    assert(ret == 0);

    size = 1;
    ret = create_array(&array, size);
    ret = free_array(&array);
    assert(ret == 1);
    assert(array.size == 0);
    assert(array.head == NULL);

    size = 1 + (rand() % (MAX_NUMBER_BLOCKS - 1));
    ret = create_array(&array, size);
    ret = free_array(&array);
    assert(ret == 1);
    assert(array.size == 0);
    assert(array.head == NULL);
}

void test_write_array(void)
{
    array_t array;
    array.head = NULL;
    size_t size = MAX_NUMBER_BLOCKS;
    int ret = create_array(&array, size);

    /**
     *  Test cases:
     *      -Write the first element, should return 1.
     *      -Write the last element, should return 1.
     *      -Write any element within the range, should return 1.
     *      -Fill all the elements.
     *      -Try to write outside the range, should return 0.
     *  It provides no protection for overwriting.
     * */
    uint8_t element = (uint8_t)rand();
    unsigned int ith = MAX_NUMBER_BLOCKS;
    ret = write_element(&array, element, ith);
    assert(ret == 0);

    element = (uint8_t)rand();
    ith = MAX_NUMBER_BLOCKS + 1;
    ret = write_element(&array, element, ith);
    assert(ret == 0);

    ith = 0;
    ret = write_element(&array, element, ith);
    assert(ret == 0);
    assert(array.head->block[0] == element);

    ith = MAX_NUMBER_BLOCKS - 1;
    ret = write_element(&array, element, ith);
    assert(ret == 0);
    unsigned int i;
    // Go to the corresponding location
    memory_block_t *block = array.head;
    for (i = 1; i < MAX_NUMBER_BLOCKS; i++)
    {
        block = block->next;
    }
    assert(block->block[0] == element);

    ith = rand() % MAX_NUMBER_BLOCKS;
    ret = write_element(&array, element, ith);
    assert(ret == 0);
    block = array.head;
    for (i = 1; i <= ith; i++)
    {
        block = block->next;
    }
    assert(block->block[0] == element);

    ret = free_array(&array);
}

void test_read_element(void)
{
    array_t array;
    array.head = NULL;
    size_t size = MAX_NUMBER_BLOCKS;
    int ret = create_array(&array, size);
    
    unsigned int ith = 0;
    uint8_t element;
    for (ith = 0; ith < size; ith++)
    {
        element = ith % 256;
        ret = write_element(&array, element, ith);
    }

    /**
     * Test cases:
     *  -Read the first element, should return 1.
     *  -Read the last element, should return 1.
     *  -Read some random element within the range, should return 1.
     *  -Read an alement outside the range, should return 0.
    */
   uint8_t elementr = 0;
   ret = read_element(&array, &elementr, 0);
   assert(ret == 1);
   assert(elementr == 0);

   ret = read_element(&array, &elementr, MAX_NUMBER_BLOCKS - 1);
   assert(ret == 1);
   assert(elementr == (MAX_NUMBER_BLOCKS - 1) % 256);

   ith = rand() % size;
   ret = read_element(&array, &elementr, ith);
   assert(ret == 1);
   assert(elementr == ith % 256);

    ith = size;
    ret = read_element(&array, &elementr, ith);
    assert(ret == 0);

    ith = size + 1;
    ret = read_element(&array, &elementr, ith);
    assert(ret == 0);

   ret = free_array(&array);
}

void memory_tests(void)
{
    printf("\nTesting the memory API\n");

    printf("Testing memory_init function.\n");
    test_memory_init();
    printf("Test passed.\n");

    printf("Testing create_array function.\n");
    test_create_array();
    printf("Test passed.\n");

    printf("Testing free_array function.\n");
    test_free_array();
    printf("Test passed.\n");

    printf("Testing write_array function.\n");
    test_write_array();
    printf("Test passed.\n");

    printf("Testing read_element function.\n");
    test_read_element();
    printf("Test passed.\n");

    return;
}