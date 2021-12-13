/**
 * @ingroup     MAC
 * @{
 * @file
 * @brief       API provided for using the MAC protocol along with all its provided services.
 * 
 * @author      José Antonio Septién Hernández.
 * @}
 */
#ifndef MCLMAC_H
#define MCLMAC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "MAC_Internals.h"
#include "memory.h"
#include "memory_macros.h"
#include "timeouts.h"
#include "config_mac.h"

#ifdef __LINUX__
#include <unistd.h>
#include <signal.h>
#include <time.h>
#endif

#ifdef __RIOT__
#include "ztimer.h"
#include "sx127x.h"
#endif

typedef enum STATE {START, INITIALIZATION, SYNCHRONIZATION, TIMESLOT_AND_CHANNEL_SELECTION, MEDIUM_ACCESS, FINISH, NONE} state_t;

typedef enum POWERMODE {STARTP, PASSIVE, ACTIVE, TRANSMIT, RECEIVE, NONEP, FINISHP, IDLEP} PowerMode_t;

enum MAC_STATEMACHINE_ERRORS{E_MAC_TRANSITION_SUCCESS, E_MAC_TRANSITION_ERROR, E_MAC_NO_TRANSITION, E_MAC_INVALID_STATE, E_MAC_NO_UPDATE};
enum MAC_EXECUTION_ERRORS{E_MAC_EXECUTION_SUCCESS, E_MAC_EXECUTION_FAILED};

enum POWERMODE_ERRORS {E_PM_TRANSITION_SUCCESS, E_PM_TRANSITION_ERROR, E_PM_NO_TRANSITION, E_PM_INVALID_STATE};
enum PM_EXECUTION_ERRORS {E_PM_EXECUTION_SUCCESS, E_PM_EXECUTION_FAILED};

typedef struct State
{
    state_t currentState;
    state_t nextState;
}MACState_t;

typedef struct RadioState
{
    PowerMode_t currentState;
    PowerMode_t nextState;
}RadioPowerMode_t;

/**
 * @brief Use this data structure for the overall MAC layer.
 *        It contains:
 *          -The MAC, use in MEDIUM_ACCESS, data structure-
 *          -Variables containing the current and next state of the MAC
 *           and the power mode state machines.
 *          -IPC Queue and all associated data (size, pid, etc.), for receiving from other layers.
 *          -IPC Queue and all associated data (size, pid, etc.), for sending to other layers.
 *          -The available frequencies.
 *          -The number of frequencies.
 *          -The maximum number of slots.
 *          -Information about the occupied slots and frequencies.
 *          -The nodeID of the current node.
 *          -The network time.
 *          -The number of hops to source.
 */
typedef struct MCLMAC
{
    // Internals to the MAC
    MAC_Internals_t     SINGLE_POINTER mac;
    MACState_t          macState;
    RadioPowerMode_t    powerMode;

    // Private members
    uint16_t            _nodeID;
    uint64_t            _networkTime;
    size_t              _dataQSize;
    uint8_t             _nSlots;
    uint8_t             _nChannels;
    uint16_t            _hopCount;
    uint32_t            _frequencies[MAX_NUMBER_FREQS];
    uint8_t             _occupied_frequencies_slots[MAX_NUMBER_FREQS][(MAX_NUMBER_SLOTS / 8U) + ((MAX_NUMBER_SLOTS % 8) != 0 ? 1 : 0)];
    uint32_t            _wakeup_frame;
    uint32_t            _initTime;
    // IPC Queues
}MCLMAC_t;

void MCLMAC_init(MCLMAC_t DOUBLE_POINTER mclmac, 
#ifdef __LINUX__
    uint8_t *radio,
#endif
#ifdef __RIOT__
    sx127x_t *radio,
#endif
    uint16_t nodeid, size_t dataQSize
);

void MCLMAC_destroy(MCLMAC_t DOUBLE_POINTER mclmac);
void MCLMAC_clear(MCLMAC_t *mclmac);

// Protocol Execution
// MAC state machine
void mclmac_init_mac_state_machine(MCLMAC_t *mclmac);
int mclmac_update_mac_state_machine(MCLMAC_t *mclmac);
int mclmac_execute_mac_state_machine(MCLMAC_t *mclmac);
// Device PowerMode State Machine
void mclmac_init_powermode_state_machine(MCLMAC_t *mclmac);
int mclmac_update_powermode_state_machine(MCLMAC_t *mclmac);
int mclmac_execute_powermode_state(MCLMAC_t *mclmac);

// State machines status
void mclmac_set_MAC_state(MCLMAC_t *mclmac, state_t state);
void mclmac_set_next_MAC_state(MCLMAC_t *mclmac, state_t next);
state_t mclmac_get_MAC_state(MCLMAC_t *mclmac);
void mclmac_set_powermode_state(MCLMAC_t *mclmac, PowerMode_t mode);
void mclmac_set_next_powermode_state(MCLMAC_t *mclmac, PowerMode_t next);
PowerMode_t mclmac_get_powermode_state(MCLMAC_t *mclmac);

// Channels
void mclmac_set_cf_channel(MCLMAC_t *mclmac, uint32_t cfChannel);
uint32_t mclmac_get_cf_channel(MCLMAC_t *mclmac);
void mclmac_set_transmit_channel(MCLMAC_t *mclmac, uint32_t channel);
uint32_t mclmac_get_transmit_channel(MCLMAC_t *mclmac);
void mclmac_set_reception_channel(MCLMAC_t *mclmac, uint32_t rChannel);
uint32_t mclmac_get_reception_channel(MCLMAC_t *mclmac);
void mclmac_set_available_channels(MCLMAC_t *mclmac, ARRAY* channels, uint8_t nChannels);
void mclmac_get_available_channels(MCLMAC_t *mclmac, ARRAY* channels, uint8_t *nChannels);

// Properties and configuration
void mclmac_set_nodeid(MCLMAC_t *mclmac, uint16_t id);
uint16_t mclmac_get_nodeid(MCLMAC_t *mclmac);
void mclmac_set_selected_slot(MCLMAC_t *mclmac, uint8_t selectedSlot);
uint8_t mclmac_get_selected_slot(MCLMAC_t *mclmac);
void mclmac_set_number_of_hops(MCLMAC_t *mclmac, uint8_t hops);
uint8_t mclmac_get_number_of_hops(MCLMAC_t *mclmac);
// ------>
void mclmac_compute_free_slots_and_channels(MCLMAC_t *mclmac);
// <-----
void mclmac_set_current_frame(MCLMAC_t *mclmac, uint32_t frame_number);
void mclmac_increase_frame(MCLMAC_t *mclmac);
void mclmac_set_current_slot(MCLMAC_t *mclmac, uint8_t current_slot);
void mclmac_increase_slot(MCLMAC_t *mclmac);
void mclmac_set_slots_number(MCLMAC_t *mclmac, uint8_t slots_number);
void mclmac_set_cf_slots_number(MCLMAC_t *mclmac, uint8_t cf_slots_number);
void mclmac_set_current_cf_slot(MCLMAC_t *mclmac, uint8_t current_cf_slot);
void mclmac_increase_cf_slot(MCLMAC_t *mclmac);
void mclmac_set_slot_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double slotDuration
#endif
#ifdef __RIOT__
uint32_t slotDuration
#endif
);
void mclmac_set_frame_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double frame_duration
#endif
#ifdef __RIOT__
uint32_t frame_duration
#endif
);
void mclmac_set_cf_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double cf_duration
#endif
#ifdef __RIOT__
uint32_t cf_duration 
#endif
);
void mclmac_record_collision(MCLMAC_t *mclmac, uint8_t collisionSlot, uint32_t collisionFrequency);
void mclmac_set_destination_id(MCLMAC_t *mclmac, uint16_t id);
uint16_t mclmac_get_destination_id(MCLMAC_t *mclmac);
void mclmac_set_network_time(MCLMAC_t *mclmac, uint32_t time);
uint32_t mclmac_get_network_time(MCLMAC_t *mclmac);

// Packet functions
void mclmac_create_cf_packet(MCLMAC_t *mclmac);
void mclmac_create_control_packet(MCLMAC_t *mclmac);
void mclmac_create_data_packet(MCLMAC_t *mclmac);
void mclmac_clear_cf_packet(MCLMAC_t *mclmac);
void mclmac_set_packet_data(MCLMAC_t *mclmac, ARRAY* data, uint8_t size);
void mclmac_delete_data_from_packet(MCLMAC_t *mclmac);
void mclmac_clear_data_from_packet(MCLMAC_t *mclmac);

void mclmac_copy_data(MCLMAC_t *mclmac, const ARRAY* data);
void mclmac_send_control_packet(MCLMAC_t *mclmac);
void stub_mclmac_send_cf_message(MCLMAC_t *mclmac);
bool stub_mclmac_receive_cf_message(MCLMAC_t *mclmac);
void mclmac_send_data_packet(MCLMAC_t *mclmac);
void mclmac_receive_control_message(MCLMAC_t *mclmac);
bool stub_mclmac_receive_ctrlpkt_sync(MCLMAC_t *mclmac, ControlPacket_t *ctrlpkt);
bool mclmac_receive_data_message(MCLMAC_t *mclmac);

// Channel selection
void mclmac_change_transmit_channel(MCLMAC_t *mclmac);
void mclmac_change_receive_channel(MCLMAC_t *mclmac);
void stub_mclmac_change_cf_channel(MCLMAC_t *mclmac);

// Radio modes
void mclmac_start_split_phase(MCLMAC_t *mclmac);
void stub_mclmac_start_cf_phase(MCLMAC_t *mclmac);
bool stub_mclmac_cf_packet_detected(MCLMAC_t *mclmac);

int32_t stub_mclmac_read_queue_element(MCLMAC_t *mclmac, uint16_t *bytes, size_t size, uint32_t *read_from);
int32_t stub_mclmac_write_queue_element(MCLMAC_t *mclmac, size_t size);

/* Private functions */
void _select_slot_and_channel(MCLMAC_t *mclmac);

#endif  // MCLMAC_H
