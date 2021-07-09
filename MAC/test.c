/**
 * Module for testing the implementation of the functions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "controlPacket.h"
#include "tests/controlPacket_Tests.h"

int main(void)
{
    printf("Testing the implementation of the packet control structure.\n\n");
    executeTests();

    return 0;
}