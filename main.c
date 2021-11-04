/**
 * Module for testing the implementation of the functions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __RIOT__
#include "periph/pm.h"

#endif

#include "assert.h"
#include "timeouts.h"
#include "mclmac_tests.h"
#include "utils_tests.h"

#ifdef __RIOT__
#include "memory.h"
#endif

int main(void)
{
    printf("Testing the MAC protocol (MCLMAC).\n");
    //timeout_init();
#ifdef __LINUX__
    utils_tests(); 
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
