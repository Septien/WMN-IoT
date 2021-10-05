/**
 * Module for testing the implementation of the functions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#ifdef __RIOT__
#include <periph/gpio.h>
#include <periph_cpu.h>
#include <gpio_arch.h>

#include "periph/pm.h"

#endif

#include "mclmac_tests.h"

int main(void)
{
#ifdef __LINUX__ 
    printf("Testing the MAC protocol (MCLMAC).\n");
#endif
#ifdef __RIOT__
    //pm_reboot();
    puts("Welcome to RIOT!\n");
    mac_tests();
    while (1) ;
#endif
    return 0;
}
