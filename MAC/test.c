/**
 * Module for testing the implementation of the functions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "controlPacket.h"
#include "CFPacket.h"

#include "tests/controlPacket_Tests.h"
#include "tests/CFPacket_Tests.h"

int main(void)
{
    printf("Testing the implementation of the packet control structure.\n");
    executeTestsCP();

    printf("\nTesting the implementation of the CF packet structure.\n");
    executeTestsCF();

    return 0;
}