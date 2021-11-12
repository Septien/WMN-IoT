#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "MAC_PowerMode_StateMachine_Tests.h"
#include "MCLMAC.h"

void test_mclmac_init_powermode_state_machine(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    mclmac_init_powermode_state_machine(REFERENCE mclmac);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    assert(ARROW(mclmac)powerMode.nextState == NONEP);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_powermode_state(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    PowerMode_t mode = PASSIVE;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.currentState == mode);

    mode = ACTIVE;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.currentState == mode);

    mode = TRANSMIT;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.currentState == mode);

    mode = RECEIVE;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.currentState == mode);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_set_next_powermode_state(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    PowerMode_t mode = ACTIVE;
    mclmac_set_next_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.nextState == mode);

    mode = TRANSMIT;
    mclmac_set_next_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.nextState == mode);

    mode = RECEIVE;
    mclmac_set_next_powermode_state(REFERENCE mclmac, mode);
    assert(ARROW(mclmac)powerMode.nextState == mode);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_get_powermode_state(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    PowerMode_t mode = PASSIVE, modeR;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    modeR = mclmac_get_powermode_state(REFERENCE mclmac);
    assert(modeR == mode);

    mode = ACTIVE;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    modeR = mclmac_get_powermode_state(REFERENCE mclmac);
    assert(modeR == mode);

    mode = TRANSMIT;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    modeR = mclmac_get_powermode_state(REFERENCE mclmac);
    assert(modeR == mode);

    mode = RECEIVE;
    mclmac_set_powermode_state(REFERENCE mclmac, mode);
    modeR = mclmac_get_powermode_state(REFERENCE mclmac);
    assert(modeR == mode);
    
    MCLMAC_destroy(&mclmac);
}

void test_mclmac_update_powermode_state_machine(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;
    int r;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    mclmac_init_powermode_state_machine(REFERENCE mclmac);

    // Try to transition from STARTP state to ACTIVE, TRANSMIT,  RECEIVE or FINISHP states.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_powermode_state(REFERENCE mclmac, STARTP);
    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    
    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    
    mclmac_set_next_powermode_state(REFERENCE mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);
    
    mclmac_set_next_powermode_state(REFERENCE mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == STARTP);

    // Try to transition from PASSIVE state to TRANSMIT,  RECEIVE or FINISHP states.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_powermode_state(REFERENCE mclmac, PASSIVE);
    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    // Try to transit from FINISHP to any other state
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_powermode_state(REFERENCE mclmac, FINISHP);

    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    mclmac_set_next_powermode_state(REFERENCE mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // Once in finish state, to use once again the state machine,
    // call mclmac_init_powermode_state_machine, and go to PASSIVE state
    mclmac_init_powermode_state_machine(REFERENCE mclmac);
    mclmac_set_powermode_state(REFERENCE mclmac, STARTP);
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);

    // Transit from PASSIVE to ACTIVE
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    // Transit from ACTIVE to PASSIVE
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    // Transit from PASSIVE to TRANSMIT. Remember to pass through ACTIVE,
    // as specified by the state machine.
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == TRANSMIT);

    // Try to get to RECEIVE mode.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(REFERENCE mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);
    assert(ARROW(mclmac)powerMode.currentState == TRANSMIT);

    // Try to get to ACTIVE mode.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // You are in TRANSMIT mode, in order to get to RECEIVE mode, 
    // you have to get to PASSIVE again, an travers the path to 
    // RECEIVE state, as indicated by the state machine.
    // Return E_PM_TRANSITION_SUCCESS
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, RECEIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == RECEIVE);

    // Try to get to TRANSMIT mode.
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // Try to get to ACTIVE mode
    // Return E_PM_TRANSITION_ERROR
    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_ERROR);

    // Follow the path: PASSIVE -> ACTIVE -> FINISHP
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == FINISHP);

    // Travers the path: PASSIVE -> ACTIVE -> TRANSMIT -> FINISHP
    mclmac_init_powermode_state_machine(REFERENCE mclmac);
    mclmac_set_powermode_state(REFERENCE mclmac, STARTP);
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == TRANSMIT);

    mclmac_set_next_powermode_state(REFERENCE mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == FINISHP);

    // Travers the path: PASSIVE -> ACTIVE -> RECEIVE -> FINISHP
    mclmac_init_powermode_state_machine(REFERENCE mclmac);
    mclmac_set_powermode_state(REFERENCE mclmac, STARTP);
    mclmac_set_next_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == PASSIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == ACTIVE);

    mclmac_set_next_powermode_state(REFERENCE mclmac, TRANSMIT);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == TRANSMIT);

    mclmac_set_next_powermode_state(REFERENCE mclmac, FINISHP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_TRANSITION_SUCCESS);
    assert(ARROW(mclmac)powerMode.currentState == FINISHP);

    // If nextState is NONEP, return E_PM_NO_TRANSITION
    mclmac_set_next_powermode_state(REFERENCE mclmac, NONEP);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_NO_TRANSITION);

    // If invalid state, return E_PM_INVALID_STATE
    mclmac_set_next_powermode_state(REFERENCE mclmac, 10);
    r = mclmac_update_powermode_state_machine(REFERENCE mclmac);
    assert(r == E_PM_INVALID_STATE);

    MCLMAC_destroy(&mclmac);
}

void test_mclmac_execute_powermode_state(void)
{
    MCLMAC_t SINGLE_POINTER mclmac;
#ifdef __LINUX__
    uint8_t radio;
#endif
#ifdef __RIOT__
    sx127x_t radio;
#endif
    size_t dataQsize = 256;
    uint8_t _nSlots = 8;
    uint8_t _nChannels = 8;
    int r;

    MCLMAC_init(&mclmac, &radio, dataQsize, _nSlots, _nChannels);

    mclmac_init_powermode_state_machine(REFERENCE mclmac);

    ARROW(mclmac)_frameDuration = TIME(1600000);
    ARROW(mclmac)_slotDuration = TIME(200000);
    ARROW(mclmac)_cfDuration = TIME(5000);
    mclmac_set_powermode_state(REFERENCE mclmac, NONEP);
    r = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(r == E_PM_EXECUTION_FAILED);

    /* State STARTP. Execute the following tasks:
    *   -Set the number of slots and cfslots.
    *   -Set the timers value.
    *   -Initialize the values of frame, slot, and cfslot to zero.
    *   -Immediately pass to PASSIVE state.
    *  Return E_PM_EXECUTION_SUCESS when successfully executed.
    */
    mclmac_set_powermode_state(REFERENCE mclmac, STARTP);
    r = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(r == E_PM_EXECUTION_SUCCESS);
#ifdef __LINUX__
   assert(mclmac->mac->cfpkt == NULL);
   assert(mclmac->mac->datapkt == NULL);
   assert(mclmac->mac->ctrlpkt == NULL);
   assert(mclmac->_packets != NULL);
   assert(mclmac->_packets_received != NULL);
#endif
#ifdef __RIOT__
    assert(mclmac._packets.size > 0);
    assert(mclmac._packets_received.size > 0);
#endif
    assert(ARROW(ARROW(mclmac)frame)slots_number == ARROW(mclmac)_nSlots);
    assert(ARROW(ARROW(mclmac)frame)cf_slots_number == ARROW(mclmac)_nSlots);
    assert(ARROW(ARROW(mclmac)frame)frame_duration == ARROW(mclmac)_frameDuration);
    assert(ARROW(ARROW(mclmac)frame)slot_duration == ARROW(mclmac)_slotDuration);
    assert(ARROW(ARROW(mclmac)frame)cf_duration == ARROW(mclmac)_cfDuration);
    assert(ARROW(ARROW(mclmac)frame)current_frame == 0);
    assert(ARROW(ARROW(mclmac)frame)current_slot == 0);
    assert(ARROW(ARROW(mclmac)frame)current_cf_slot == 0);
    assert(ARROW(mclmac)_packets_read == 0);
    assert(ARROW(mclmac)powerMode.nextState == PASSIVE);

    /* State PASSIVE. Execute the following tasks:
    *   -Set the radio to SLEEP mode.
    *   -Start the slot timer.
    *   -On a loop, do the following:
    *       -Check whether the timeout expired.
    *       -Once the timeout expires, unarm the timeout and end the cycle.
    *       -Possible do other activities, like reading/writing packets from/to other layers.
    *       -With the received packets from the upper layers, create an array containing
    *        the ids of the nodes to send the packet.
    */
    int i;
    for (i = 0; i < ARROW(mclmac)_max_number_packets_buffer; i++)
        WRITE_ARRAY(REFERENCE ARROW(mclmac)_packets_received, rand(), i);
    ARROW(mclmac)_num_packets_received = 5;

    mclmac_set_powermode_state(REFERENCE mclmac, PASSIVE);
    r = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(r == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(mclmac)powerMode.nextState == ACTIVE);
    assert(ARROW(ARROW(mclmac)frame)current_cf_slot == 0);
    int count = 0;
    // Check that there is less than 5 packets.
    for (i = 0; i < ARROW(mclmac)_max_number_packets_buffer; i++)
    {
        uint8_t element = READ_ARRAY(REFERENCE ARROW(mclmac)_packets, i);
        if (element == '\0')
            count++;
        uint8_t element2 = READ_ARRAY(REFERENCE ARROW(mclmac)_packets, i + 1);
        if (element == '\0' && element2 == '\0')
            break;
    }
    assert(count >= 1 && count <= 5);
    assert(ARROW(mclmac)_packets_read >= 0);
    // For the packets 'written' into the queue
    assert(ARROW(mclmac)_num_packets_received < 5);
#ifdef __LINUX__
    // Assert mode is sleep
#endif
#ifdef __RIOT__
    /*uint8_t mode = sx127x_get_op_mode(mclmac->mac->radio);
    assert(mode == sx127x_RF_OPMODE_SLEEP);
    uint8_t state = sx127x_get_state(mclmac->mac->radio);
    assert(state == SX127X_RF_IDLE);*/
#endif

    /** State ACTIVE. Executes the following tasks:
     *      - Initialize the cf packet.
     *      - Initiates the cf phase, in which:
     *          - Changes the radio to the common channel.
     *          - Sets the cf counter to zero.
     *          - Initiates the cf timer.
     *          - Checks whether there is a packet on the queue, if so, create cf packet, and set 
     *            send flag to true.
     *          - It checks wether the current cf slot is equal to the 
     *            selected slot, in which case it transmits a cf packet with destination ID.
     *            Set the next state to TRANSMIT.
     *          - Otherwise, it puts the radio in listen mode, and if packet arrives in which the 
     *            destinationID == nodeID or destinationID == 0, save the frequency and set 
     *            the next state to RECEIVE.
     *          - When the CF timer expires, increment the cf counter.
     *          - When the CF counter equals the number of slots, check whether we should transmit,
     *            receive, or none. If none required, set next state to PASSIVE, otherwise, change 
     *            to the required frequency.
     *          - In case a collision is detected: save frequency and slot of collision, and wait
     *            for the next time we send a control packet to tell the network. Save the number
     *            cf packets received.
     */
    mclmac_set_cf_channel(REFERENCE mclmac, 915000000);
    mclmac_set_powermode_state(REFERENCE mclmac, ACTIVE);
    r = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(r == E_PM_EXECUTION_SUCCESS);
    assert(ARROW(ARROW(mclmac)frame)current_cf_slot == ARROW(ARROW(mclmac)frame)cf_slots_number);
    assert(READ_ARRAY(REFERENCE ARROW(mclmac)_packets, 0) == ARROW(ARROW(mclmac)mac)destinationID);
    assert(ARROW(ARROW(ARROW(mclmac)mac)cfpkt)destinationID == 0);
    assert(ARROW(mclmac)powerMode.nextState == PASSIVE || ARROW(mclmac)powerMode.nextState == TRANSMIT || ARROW(mclmac)powerMode.nextState == RECEIVE);
#ifdef __LINUX__
    //assert(mclmac->mac->cfpkt != NULL);
    // Check channel is changed
#endif
#ifdef __RIOT__
    // Wait 30 us for radio to change frequency.
    /*uint32_t channel = sx127x_get_channel(mclmac->mac->radio);
    assert(channel == mclmac->mac->cfChannel);*/
#endif
    /*mclmac_set_current_cf_slot(REFERENCE mclmac, ARROW(ARROW(mclmac)mac)selectedSlot);
    r = mclmac_execute_powermode_state(REFERENCE mclmac);
    assert(ARROW(ARROW(ARROW(mclmac)mac)cfpkt)destinationID == ARROW(ARROW(mclmac)mac)destinationID);
    assert(ARROW(mclmac)powerMode.nextState == TRANSMIT);*/

    MCLMAC_destroy(&mclmac);
}

void executetests_mac_powermode_statemachine(void)
{
    printf("Testing _mclmac_init_powermode_state_machine function.\n");
    test_mclmac_init_powermode_state_machine();
    printf("Test passed.\n");

    printf("Testing mclmac_set_powermode_state function.\n");
    test_mclmac_set_powermode_state();
    printf("Test passed.\n");

    printf("Testing mclmac_set_next_powermode_state function.\n");
    test_mclmac_set_next_powermode_state();
    printf("Test passed.\n");

    printf("Testing mclmac_get_powermode_state function.\n");
    test_mclmac_get_powermode_state();
    printf("Test passed.\n");

    printf("Test mclmac_update_powermode_state_machine function.\n");
    test_mclmac_update_powermode_state_machine();
    printf("Test passed.\n");

    printf("Testing mclmac_execute_powermode_state function.\n");
    test_mclmac_execute_powermode_state();
    printf("Test passed.\n");
    return;
}