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
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif

    MAC_internals_init(&mac, &radio);

    // Create these packets until necessary
#ifdef __LINUX__
    assert(mac != NULL);
    assert(mac->cfpkt == NULL);
    assert(mac->datapkt == NULL);
    assert(mac->cfpkt == NULL);
#endif
    assert(ARROW(mac)selectedSlot == 0);
    assert(ARROW(mac)transmitChannel == 0);
    assert(ARROW(mac)receiveChannel == 0);
    assert(ARROW(mac)cfChannel == 0);
    assert(ARROW(mac)_collisionDetected == false);
    assert(ARROW(mac)_collisionSlot == 0);
    assert(ARROW(mac)_collisionFrequency == 0);
    assert(ARROW(mac)_cf_message_received == false);
    assert(ARROW(mac)_max_cf_messages == 5 * 2 * sizeof(uint16_t));
    assert(ARROW(mac)_max_number_packets_buffer == 5 * 256);
    assert(ARROW(mac)_packets_read == 0);
    assert(ARROW(mac)_num_packets_received == 0);

    MAC_internals_destroy(&mac);
}

void test_MAC_internals_destroy(void)
{
    MAC_Internals_t SINGLE_POINTER mac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif

    MAC_internals_init(&mac, &radio);

    MAC_internals_destroy(&mac);
#ifdef __LINUX__
    assert(mac == NULL);
#endif
}

void test_MAC_internals_clear(void)
{
    MAC_Internals_t SINGLE_POINTER mac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif

    MAC_internals_init(&mac, &radio);

    MAC_internals_clear(REFERENCE mac);
    assert(ARROW(mac)selectedSlot == 0);
    assert(ARROW(mac)transmitChannel == 0);
    assert(ARROW(mac)cfChannel == 0);
    assert(ARROW(mac)cfChannel == 0);
    assert(ARROW(mac)_collisionDetected == false);
    assert(ARROW(mac)_collisionSlot == 0);
    assert(ARROW(mac)_collisionFrequency == 0);
    assert(ARROW(mac)_cf_message_received == false);

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
