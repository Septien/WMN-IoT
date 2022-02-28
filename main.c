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
#include "ipc-queues_Tests.h"

#ifdef __RIOT__
#include "memory.h"
#endif

int main(void)
{
#ifdef __LINUX__
    printf("\nTesting the IPC API module.\n");
    ipc_queues_tests();
    printf("\nTesting the UTILS module.\n");
    utils_tests();
    printf("\nTesting the MAC protocol (MCLMAC).\n");
    mac_tests();
#endif
#ifdef __RIOT__
    puts("Welcome to RIOT!\n");
    do {
        printf("\nTesting the IPC API module.\n");
        ipc_queues_tests();
        printf("\nTesting the UTILS module.\n");
        utils_tests();
        printf("\nTesting the MAC protocol (MCLMAC).\n");
        mac_tests();
    }while (0) ;
#endif
    exit(0);
}
