/**
 * @ingroup     utils
 * @{
 * @file
 * @brief       Some macros to simplify code, so it does not get chunky 
 *              with too much #ifdefs #endifs. It differentiates the type
 *              of pointer/reference to use, accordign to the OS.
 * 
 * @author      José Antonio Septién Hernández.
 * @}
 */
#ifndef MAMORY_MACROS_H
#define MAMORY_MACROS_H

#ifdef __LINUX__
#define DOUBLE_POINTER                  **
#define SINGLE_POINTER                  *
#define ARROW(st)                       (st)->
#define REFERENCE
#define ARRAY                           uint8_t *
#define WRITE_ARRAY(ptr, element, i)    (ptr)[(i)] = (element)
#define READ_ARRAY(ptr, i)              (ptr)[(i)]

/* Convert microseconds to seconds */
#define TIME(t)                         (t)/1000000.0
#endif

#ifdef __RIOT__
#include "memory.h"

inline uint8_t readE(array_t *ptr, uint8_t i)
{
    uint8_t element;
    read_element(ptr, &element, i);
    return element;
}

#define DOUBLE_POINTER                  *
#define SINGLE_POINTER
#define ARROW(st)                       (st).
#define REFERENCE                       &
#define ARRAY                           array_t
#define WRITE_ARRAY(ptr, element, i)    write_element((ptr), (element), (i))
#define READ_ARRAY(ptr, i)              readE((ptr), (i))

/* T is in microseconds */
#define TIME(t)                         (t)
#endif

#endif
