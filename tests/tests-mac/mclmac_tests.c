/**
 * Module for testing the implementation of the functions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "mclmac_tests.h"

#include "ControlPacket.h"
#include "CFPacket.h"
#include "DataPacket.h"
#include "MAC_Internals.h"
#include "MCLMAC.h"

#include "ControlPacket_Tests.h"
#include "CFPacket_Tests.h"
#include "DataPacket_Tests.h"
#include "MAC_Internals_Tests.h"
#include "MCLMAC_Tests.h"
#include "MAC_StateMachine_Tests.h"
#include "MAC_PowerMode_StateMachine_Tests.h"
#include "Packets_Handlers_Tests.h"

void mac_tests(void)
{
    printf("\nTesting the implementation of the packet control structure.\n");
    executeTestsCP();

    printf("\nTesting the implementation of the CF packet structure.\n");
    executeTestsCF();

    printf("\nTesting the implementation of the DataPacket structure.\n");
    executeTestsDP();

    printf("\nTesting the implementation of the MAC_Internals structure.\n");
    executeTestsMACInternals();

    printf("\nTesting the implementation of the MCLMAC structure.\n");
    executeTestsMCLMAC();

    printf("\nTesting the implementation of the MAC's Packets Handlers.\n");
    executetests_packets_handlers();

    printf("\nTesting the implementation of the MAC's PowerMode State Machine.\n");
    executetests_mac_powermode_statemachine();

    printf("\nTesting the implementation of the MAC's state machine.\n");
    executetests_mac_statemachine();

    return;
}
