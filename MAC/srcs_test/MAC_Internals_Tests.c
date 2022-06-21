#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "MAC_Internals.h"

#include "cUnit.h"

#ifdef __RIOT__
#include "net/netdev.h"
#endif

struct mac_internals_data {
    MAC_Internals_t SINGLE_POINTER mac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    nrf24l01p_ng_t radio;
    netdev_t *netdev;
#endif
};

void setup_mac_internals(void *arg)
{
    struct mac_internals_data *data = (struct mac_internals_data *)arg;
#ifdef __LINUX__
    MAC_internals_init(&data->mac, data->radio);
#endif
#ifdef __RIOT__
    // Setup the radio
    nrf24l01p_ng_params_t params = {
        .spi = NRF24L01P_NG_PARAM_SPI,
        .spi_clk = NRF24L01P_NG_PARAM_SPI_CLK,
        .pin_cs = NRF24L01P_NG_PARAM_CS,
        .pin_ce = NRF24L01P_NG_PARAM_CE,
        .pin_irq = NRF24L01P_NG_PARAM_IRQ,
        .config = {
            .cfg_crc = NRF24L01P_NG_PARAM_CRC_LEN,
            .cfg_tx_power = NRF24L01P_NG_PARAM_TX_POWER,
            .cfg_data_rate = NRF24L01P_NG_PARAM_DATA_RATE,
            .cfg_channel = NRF24L01P_NG_PARAM_CHANNEL,
            .cfg_max_retr = NRF24L01P_NG_PARAM_MAX_RETRANSM,
            .cfg_retr_delay = NRF24L01P_NG_PARAM_RETRANSM_DELAY,
        }
    };
    int ret = nrf24l01p_ng_setup(&data->radio, &params, 2);
    data->netdev = &data->radio.netdev;
    data->radio.netdev.driver->init(data->netdev);
    MAC_internals_init(&data->mac, data->netdev);
#endif
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
#ifdef __LINUX__
    MAC_internals_init(&data->mac, &data->radio);
#endif
#ifdef __RIOT__
    MAC_internals_init(&data->mac, data->netdev);
#endif
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
