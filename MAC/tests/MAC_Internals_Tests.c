#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "../MAC_Internals.h"

void testinitMACIn()
{
    MAC_Internals_t *mac;
    initMACIn(&mac);

    assert(mac != NULL);
    // Create these packets until necessary
    assert(mac->cfpkt == NULL);
    assert(mac->datapkt == NULL);
    assert(mac->cfpkt == NULL);
    assert(mac->nodeID == 1);
    assert(mac->destinationID == 0);

    destroyMAC(&mac);
}

void testdestroyMAC()
{
    MAC_Internals_t *mac;
    initMACIn(&mac);
    mac->channels = (uint32_t *)malloc(32 * sizeof(uint32_t));
    mac->slots = (uint8_t *)malloc(32 * sizeof(uint8_t));

    destroyMAC(&mac);
    assert(mac == NULL);
}

void testclearMAC()
{
    MAC_Internals_t *mac;
    initMACIn(&mac);
    mac->channels = (uint32_t *)malloc(32 * sizeof(uint32_t));
    mac->slots = (uint8_t *)malloc(32 * sizeof(uint8_t));

    clearMAC(mac);
    assert(mac->channels == NULL);
    assert(mac->slots == NULL);
    assert(mac->numberChannels == 0);
    assert(mac->numberSlots == 0);
    assert(mac->nodeID == 0);
    assert(mac->selectedSlot == 0);
    assert(mac->transmitChannel == 0);
    assert(mac->cfChannel == 0);
    assert(mac->cfChannel == 0);
    assert(mac->hopCount == 0);

    destroyMAC(&mac);
}

void executeTestsMACInternals()
{
    printf("Testing initMACIn function.\n");
    testinitMACIn();
    printf("Test passed.\n");
    
    printf("Testing destroyMAC function.\n");
    testdestroyMAC();
    printf("Test passed.\n");

    printf("Testing clearMAC function.\n");
    testclearMAC();
    printf("Test passed.\n");
}