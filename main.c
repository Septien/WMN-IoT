/**
 * Module for testing the implementation of the functions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#ifdef __RIOT__
#include "periph/pm.h"

#endif

#include "assert.h"
#include "mclmac_tests.h"

#ifdef __RIOT__
#include "utils_tests.h"
#include "memory.h"
#endif

int main(void)
{
#ifdef __LINUX__ 
    printf("Testing the MAC protocol (MCLMAC).\n");
    mac_tests();
#endif
#ifdef __RIOT__
    puts("Welcome to RIOT!\n");
    do {
        utils_tests();
        mac_tests();
    }while (0) ;
#endif
    exit(0);
}
