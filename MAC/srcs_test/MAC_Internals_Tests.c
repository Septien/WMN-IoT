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

#ifdef __LINUX__
    assert(mac != NULL);
    assert(mac->ctrlpkt != NULL);
#endif
    assert(ARROW(mac)selectedSlot == 0);
    assert(ARROW(mac)transmitChannel == 0);
    assert(ARROW(mac)receiveChannel == 0);
    assert(ARROW(mac)cfChannel == 0);
    assert(ARROW(mac)_collisionDetected == false);
    assert(ARROW(mac)_collisionSlot == 0);
    assert(ARROW(mac)_collisionFrequency == 0);
    assert(ARROW(mac)_cf_message_received == false);
    assert(ARROW(mac)_max_cf_messages == MAX_NUMBER_CF_PACKETS);
    assert(ARROW(mac)_max_data_packets == MAX_NUMBER_DATA_PACKETS);
    assert(ARROW(mac)_packets_to_send_message == 0);
    assert(ARROW(mac)_packets_to_send_control == 0);
    assert(ARROW(mac)_last_send_control == 0);
    assert(ARROW(mac)_number_packets_received == 0);

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
