#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "MAC_Internals.h"
#include "ControlPacket.h"
#include "Frame.h"

#include "cUnit.h"

/* Data structure for testing */
struct mac_internals_data {
    MAC_Internals_t mac;
};

void setup_mac_internals(void *arg)
{
    struct mac_internals_data *data = (struct mac_internals_data *)arg;

    MAC_internals_init(&data->mac);
}

void teardown_mac_internal(void *arg)
{
    struct mac_internals_data *data = (struct mac_internals_data *)arg;
    MAC_internals_destroy(&data->mac);
}

bool test_MAC_internals_init(void *arg)
{
    struct mac_internals_data *data = (struct mac_internals_data *)arg;
    MAC_Internals_t *mac = &data->mac;

    bool passed = true;
#ifdef __LINUX__
    passed = passed && (mac != NULL);
    passed = passed && (mac->ctrlpkt != NULL);
#endif
    passed = passed && (mac->selectedSlot == 0);
    passed = passed && (mac->transmitChannel == 0);
    passed = passed && (mac->receiveChannel == 0);
    passed = passed && (mac->cfChannel == CF_FREQUENCY);
    passed = passed && (mac->_collisionDetected == false);
    passed = passed && (mac->_is_internal_collision == false);
    passed = passed && (mac->_collisionSlot == 0);
    passed = passed && (mac->_collisionFrequency == 0);
    passed = passed && (mac->_cf_message_received == false);
    passed = passed && (mac->_packets_to_send_message == 0);
    passed = passed && (mac->_packets_to_send_control == 0);
    passed = passed && (mac->_last_send_control == 0);
    passed = passed && (mac->_number_packets_received == 0);

    return passed;
}

bool test_MAC_internals_destroy(void *arg)
{
    struct mac_internals_data *data = (struct mac_internals_data *)arg;

    MAC_internals_destroy(&data->mac);
    bool passed = true;

    // So the teardown function does not break
    return passed;
}

bool test_MAC_internals_clear(void *arg)
{
    struct mac_internals_data *data = (struct mac_internals_data *)arg;
    MAC_Internals_t *mac = &data->mac;

    MAC_internals_clear(mac);
    bool passed = true;
    passed = passed && (mac->selectedSlot == 0);
    passed = passed && (mac->transmitChannel == 0);
    passed = passed && (mac->cfChannel == 0);
    passed = passed && (mac->cfChannel == 0);
    passed = passed && (mac->_collisionDetected == false);
    passed = passed && (mac->_collisionSlot == 0);
    passed = passed && (mac->_collisionFrequency == 0);
    passed = passed && (mac->_cf_message_received == false);

    return passed;
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
