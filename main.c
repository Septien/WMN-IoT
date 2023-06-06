/**
 * Module for testing the implementation of the functions.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#ifdef TESTING
#ifdef __RIOT__
#include "periph/pm.h"
#include "memory.h"
#endif

#include "assert.h"
#include "timeouts.h"
#include "ipc-queues.h"
#include "mclmac_tests.h"
#include "utils_tests.h"
#include "ipc-queues_Tests.h"

int main(void)
{
#if defined __LINUX__
    srand(time(NULL));
    printf("\nTesting the IPC API module.\n");
    ipc_queues_tests();
    printf("\nTesting the UTILS module.\n");
    utils_tests();
    printf("\nTesting the MAC protocol (MCLMAC).\n");
    mac_tests();
#endif
#if defined __RIOT__
    puts("Welcome to RIOT!\n");
    do {
        srand(time(NULL));
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
#endif
#ifdef BDD

int main(void)
{
    return 0;
}

#include "REMA.h"
#endif
