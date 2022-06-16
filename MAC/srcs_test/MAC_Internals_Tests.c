#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "MAC_Internals.h"

#include "cUnit.h"

struct mac_internals_data {
    MAC_Internals_t SINGLE_POINTER mac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
};

void setup_mac_internals(void *arg)
{
    struct mac_internals_data *data = (struct mac_internals_data *)arg;
    MAC_internals_init(&data->mac, &data->radio);
}

void teardown_mac_internal(void *arg)
{
    struct mac_internals_data *data = (struct mac_internals_data *)arg;
    MAC_internals_destroy(&data->mac);
}

void test_MAC_internals_init(void *arg)
{
    struct mac_internals_data *data = (struct mac_internals_data *)arg;
    MAC_Internals_t *mac = REFERENCE data->mac;

#ifdef __LINUX__
    assert(mac != NULL);
    assert(mac->ctrlpkt != NULL);
#endif
    assert(mac->selectedSlot == 0);
    assert(mac->transmitChannel == 0);
    assert(mac->receiveChannel == 0);
    assert(mac->cfChannel == CF_FREQUENCY);
    assert(mac->_collisionDetected == false);
    assert(mac->_is_internal_collision == false);
    assert(mac->_collisionSlot == 0);
    assert(mac->_collisionFrequency == 0);
    assert(mac->_cf_message_received == false);
    assert(mac->_packets_to_send_message == 0);
    assert(mac->_packets_to_send_control == 0);
    assert(mac->_last_send_control == 0);
    assert(mac->_number_packets_received == 0);
}

void test_MAC_internals_destroy(void *arg)
{
    struct mac_internals_data *data = (struct mac_internals_data *)arg;

    MAC_internals_destroy(&data->mac);
#ifdef __LINUX__
    assert(data->mac == NULL);
#endif
    MAC_internals_init(&data->mac, &data->radio);
}

void test_MAC_internals_clear(void *arg)
{
    struct mac_internals_data *data = (struct mac_internals_data *)arg;
    MAC_Internals_t *mac = REFERENCE data->mac;

    MAC_internals_clear(mac);
    assert(mac->selectedSlot == 0);
    assert(mac->transmitChannel == 0);
    assert(mac->cfChannel == 0);
    assert(mac->cfChannel == 0);
    assert(mac->_collisionDetected == false);
    assert(mac->_collisionSlot == 0);
    assert(mac->_collisionFrequency == 0);
    assert(mac->_cf_message_received == false);
}

void executeTestsMACInternals(void)
{
    cUnit_t *tests;
    struct mac_internals_data data;

    cunit_init(&tests, &setup_mac_internals, &teardown_mac_internal, (void *)&data);

    cunit_add_test(tests, &test_MAC_internals_init,     "MAC_internals_init\0");
    cunit_add_test(tests, &test_MAC_internals_destroy,  "MAC_internals_destroy\0");
    cunit_add_test(tests, &test_MAC_internals_clear,    "MAC_internals_clear\0");
    
    cunit_execute_tests(tests);

    cunit_terminate(&tests);
}
