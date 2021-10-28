#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "MAC_Internals.h"

void test_MAC_internals_init(void)
{
    MAC_Internals_t SINGLE_POINTER mac;
    MAC_internals_init(&mac);

    // Create these packets until necessary
#ifdef __LINUX__
    assert(mac != NULL);
    assert(mac->cfpkt == NULL);
    assert(mac->datapkt == NULL);
    assert(mac->cfpkt == NULL);
#endif
    assert(ARROW(mac)nodeID == 1);
    assert(ARROW(mac)destinationID == 0);

    MAC_internals_destroy(&mac);
}

void test_MAC_internals_destroy(void)
{
    MAC_Internals_t SINGLE_POINTER mac;
    MAC_internals_init(&mac);
#ifdef __LINUX__
    mac->channels = (uint8_t *)malloc(128 * sizeof(uint8_t));
    mac->slots = (uint8_t *)malloc(32 * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&mac.channels, 128);
    create_array(&mac.slots, 32);
#endif

    MAC_internals_destroy(&mac);
#ifdef __LINUX__
    assert(mac == NULL);
#endif
#ifdef __RIOT__
    assert(mac.channels.size == 0);
    assert(mac.slots.size == 0);
#endif
}

void test_MAC_internals_clear(void)
{
    MAC_Internals_t SINGLE_POINTER mac;
    MAC_internals_init(&mac);
#ifdef __LINUX__
    mac->channels = (uint8_t *)malloc(4 * 32 * sizeof(uint32_t));
    mac->slots = (uint8_t *)malloc(32 * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&mac.channels, 4 * 32);
    create_array(&mac.slots, 32);
#endif

    MAC_internals_clear(REFERENCE mac);
#ifdef __LINUX__
    assert(mac->channels == NULL);
    assert(mac->slots == NULL);
#endif
#ifdef __RIOT__
    assert(mac.channels.size == 0);
    assert(mac.slots.size == 0);
#endif
    assert(ARROW(mac)numberChannels == 0);
    assert(ARROW(mac)numberSlots == 0);
    assert(ARROW(mac)nodeID == 0);
    assert(ARROW(mac)selectedSlot == 0);
    assert(ARROW(mac)transmitChannel == 0);
    assert(ARROW(mac)cfChannel == 0);
    assert(ARROW(mac)cfChannel == 0);
    assert(ARROW(mac)hopCount == 0);

    MAC_internals_destroy(&mac);
}

void executeTestsMACInternals(void)
{
    printf("Testing MAC_internals_init function.\n");
    test_MAC_internals_init();
    printf("Test passed.\n");

    printf("Testing MAC_internals_destroy function.\n");
    test_MAC_internals_destroy();
    printf("Test passed.\n");

    printf("Testing MAC_internals_clear function.\n");
    test_MAC_internals_clear();
    printf("Test passed.\n");
}
