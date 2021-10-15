#include "memory.h"

static memory_block_t   MEMORY[MAX_NUMBER_BLOCKS];
extern memarray_t       storage;

int memory_init(void)
{
    memarray_init(&storage, MEMORY, sizeof(memory_block_t), MAX_NUMBER_BLOCKS);
    if (storage.free_data == NULL)
        return 0;
    return 1;
}

int create_array(array_t *array, size_t size)
{
    assert(array != NULL);

    if (size == 0)
        return 0;
    
    // Check whether there is sufficient memory available
    size_t available = memarray_available(&storage);
    if (size > available)
    {
        return 0;
    }

    memory_block_t *block;
    array->size = size;
    if ((block = (memory_block_t *)memarray_alloc(&storage)) == NULL)
        return 0;
    block->block[0] = 0;
    array->head = block;
    array->head->next = NULL;
    for (unsigned int i = 0; i < size - 1; i++)
    {
        block->next = (memory_block_t *)memarray_alloc(&storage);
        block = block->next;
        block->next = NULL;
        block->block[0] = 0;
    }
    return 1;
}

int free_array(array_t *array)
{
    assert(array != NULL);

    if (array->head == NULL)
        return 0;
    
    if (array->size == 0)
        return 0;

    memory_block_t *old, *head;
    head = array->head;
    while (head)
    {
        if (head->next != NULL)
        {
            old = head;
            head = head->next;
            memarray_free(&storage, old);
        }
        else
        {
            memarray_free(&storage, head);
            head = NULL;
        }
    }
    array->head = NULL;
    array->size = 0;
    return 1;
}

int write_element(array_t *array, const uint8_t element, const unsigned int i)
{
    assert(array != NULL);
    
    if (i >= array->size)
    {
        return 0;
    }

    memory_block_t *block;
    unsigned int ith;
    block = array->head;
    ith = 0;
    for (ith = 0; ith < array->size; ith++)
    {
        if (ith == i)
            break;
        else
            block = block->next;
    }
    block->block[0] = element;
    

    return 0;
}

int read_element(array_t *array, uint8_t *element, const unsigned int i)
{
    assert(array != NULL);
    if (i >= array->size)
        return 0;

    memory_block_t *block;
    unsigned int ith;
    block = array->head;
    ith = 0;
    for (ith = 0; ith < array->size; ith++)
    {
        if(ith == i)
            break;
        else
            block = block->next;
    }
    *element = block->block[0];
    return 1;
}
