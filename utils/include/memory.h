/**
 * @ingroup utils
 * @{
 * 
 * @file memory.h
 * @brief An API providing pseudo-dynamic memory, only for use with RIOT.
 * 
 * @author José Antonio Septién Hernández <ja.septienhernandez@ugto.mx>
 * 
 * @}
 */
#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __RIOT__

#include "memarray.h"

#define BLOCK_SIZE          (1U)

#ifndef MAX_NUMBER_BLOCKS
#define MAX_NUMBER_BLOCKS   10240
#endif

typedef struct memory
{
    struct memory  *next;
    uint8_t         block[BLOCK_SIZE];
}memory_block_t;

typedef struct Array
{
    memory_block_t  *head;
    size_t          size;
}array_t;

static memory_block_t   MEMORY[MAX_NUMBER_BLOCKS];
static memarray_t       storage;

int memory_init(void);
int create_array(array_t *array, size_t size);
int free_array(array_t *array);
int write_element(array_t *array, const uint8_t element, const unsigned int i);
int read_element(array_t *array, uint8_t *element, const unsigned int i);

#endif      // __RIOT__

#endif      // MEMORY_H