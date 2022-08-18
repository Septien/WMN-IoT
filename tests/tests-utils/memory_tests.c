#include <stdio.h>
#include <stdlib.h>

#include "assert.h"
#include "memory_tests.h"
#include "memory.h"

#include "cUnit.h"

void setup_mem(void *arg)
{
    (void) arg;
}

bool test_memory_init(void *arg)
{
    (void) arg;
    memory_init();
    return true;
}

bool test_create_array(void *arg)
{
    (void) arg;
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
    bool passed = true;
    passed = passed && (ret == 0);

    size = 1;
    ret = create_array(&array, size);
    passed = passed && (ret == 1);
    passed = passed && (array.head != NULL);
    passed = passed && (array.size == 1);
    passed = passed && (array.head->next == NULL);

    ret = free_array(&array);
    passed = passed && (ret == 1);

    size = 1 + (rand() % MAX_NUMBER_BLOCKS);
    ret = create_array(&array, size);
    passed = passed && (ret == 1);
    passed = passed && (array.head != NULL);
    passed = passed && (array.size == size);
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
    passed = passed && (count == size);
    free_array(&array);

    size = MAX_NUMBER_BLOCKS - 1;
    ret = create_array(&array, size);
    passed = passed && (ret == 1);
    passed = passed && (array.head != NULL);
    passed = passed && (array.size == size);
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
    passed = passed && (count == size);
    free_array(&array);

    size = MAX_NUMBER_BLOCKS;
    ret = create_array(&array, size);
    passed = passed && (ret == 1);
    passed = passed && (array.head != NULL);
    passed = passed && (array.size == MAX_NUMBER_BLOCKS);
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
    passed = passed && (count == size);
    free_array(&array);

    size = MAX_NUMBER_BLOCKS + 1;
    ret = create_array(&array, size);
    passed = passed && (ret == 0);
    passed = passed && (array.head == NULL);
    passed = passed && (array.size == 0);

    return passed;
}

bool test_free_array(void *arg)
{
    (void) arg;
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
    bool passed = true;
    passed = passed && (ret == 0);

    size = 1;
    create_array(&array, size);
    ret = free_array(&array);
    passed = passed && (ret == 1);
    passed = passed && (array.size == 0);
    passed = passed && (array.head == NULL);

    size = 1 + (rand() % (MAX_NUMBER_BLOCKS - 1));
    create_array(&array, size);
    ret = free_array(&array);
    passed = passed && (ret == 1);
    passed = passed && (array.size == 0);
    passed = passed && (array.head == NULL);

    return passed;
}

bool test_write_array(void *arg)
{
    (void) arg;
    array_t array;
    array.head = NULL;
    size_t size = MAX_NUMBER_BLOCKS;
    create_array(&array, size);

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
    int ret = write_element(&array, element, ith);
    bool passed = true;
    passed = passed && (ret == 0);

    element = (uint8_t)rand();
    ith = MAX_NUMBER_BLOCKS + 1;
    ret = write_element(&array, element, ith);
    passed = passed && (ret == 0);

    ith = 0;
    ret = write_element(&array, element, ith);
    passed = passed && (ret == 1);
    passed = passed && (array.head->block[0] == element);

    ith = MAX_NUMBER_BLOCKS - 1;
    ret = write_element(&array, element, ith);
    passed = passed && (ret == 1);
    unsigned int i;
    // Go to the corresponding location
    memory_block_t *block = array.head;
    for (i = 1; i < MAX_NUMBER_BLOCKS; i++)
    {
        block = block->next;
    }
    passed = passed && (block->block[0] == element);

    ith = rand() % MAX_NUMBER_BLOCKS;
    ret = write_element(&array, element, ith);
    passed = passed && (ret == 1);
    block = array.head;
    for (i = 1; i <= ith; i++)
    {
        block = block->next;
    }
    passed = passed && (block->block[0] == element);

    ret = free_array(&array);
    passed = passed && (ret == 1);

    return passed;
}

bool test_read_element(void *arg)
{
    (void) arg;
    array_t array;
    array.head = NULL;
    size_t size = MAX_NUMBER_BLOCKS;
    create_array(&array, size);
    
    unsigned int ith = 0;
    for (ith = 0; ith < size; ith++)
    {
        uint8_t element = ith % 256;
        write_element(&array, element, ith);
    }

    /**
     * Test cases:
     *  -Read the first element, should return 1.
     *  -Read the last element, should return 1.
     *  -Read some random element within the range, should return 1.
     *  -Read an alement outside the range, should return 0.
    */
   uint8_t elementr = 0;
   int ret = read_element(&array, &elementr, 0);
   bool passed = true;
   passed = passed && (ret == 1);
   passed = passed && (elementr == 0);

   ret = read_element(&array, &elementr, MAX_NUMBER_BLOCKS - 1);
   assert(ret == 1);
   assert(elementr == (MAX_NUMBER_BLOCKS - 1) % 256);

   ith = rand() % size;
   ret = read_element(&array, &elementr, ith);
   passed = passed && (ret == 1);
   passed = passed && (elementr == ith % 256);

    ith = size;
    ret = read_element(&array, &elementr, ith);
    passed = passed && (ret == 0);

    ith = size + 1;
    ret = read_element(&array, &elementr, ith);
    passed = passed && (ret == 0);

   free_array(&array);

   return passed;
}

void teardown_mem(void *arg)
{
    (void) arg;
}

void memory_tests(void)
{
    cUnit_t *tests;
    uint8_t dummy_data = rand();

    cunit_init(&tests, &setup_mem, &teardown_mem, (void *)&dummy_data);

    printf("\nTesting the memory API\n");

    cunit_add_test(tests, &test_memory_init,    "memory_init\0");
    cunit_add_test(tests, &test_create_array,   "create_array\0");
    cunit_add_test(tests, &test_free_array,     "free_array\0");
    cunit_add_test(tests, &test_write_array,    "write_array\0");
    cunit_add_test(tests, &test_read_element,   "read_element\0");

    cunit_execute_tests(tests);

    cunit_terminate(&tests);

    return;
}
