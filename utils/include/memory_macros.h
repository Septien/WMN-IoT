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
#define DOUBLE_POINTER      **
#define SINGLE_POINTER      *
#define ARROW(st)           (st)->
#define REFERENCE
#define ARRAY               uint8_t *
#endif

#ifdef __RIOT__
#include "memory.h"

#define DOUBLE_POINTER      *
#define SINGLE_POINTER      
#define ARROW(st)           (st).
#define REFERENCE           &
#define ARRAY               array_t
#endif

#endif
