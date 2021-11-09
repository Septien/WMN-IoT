#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MCLMAC.h"

void MCLMAC_init(MCLMAC_t DOUBLE_POINTER mclmac, 
#ifdef __LINUX__
    uint8_t *radio,
#endif
#ifdef __RIOT__
    sx127x_t *radio,
#endif
    size_t dataQSize, uint8_t _nSlots, uint8_t _nChannels
)
{
#ifdef __LINUX__
    (*mclmac) = (MCLMAC_t *)malloc(sizeof(MCLMAC_t));
    if ((*mclmac) == NULL)
    {
        return;
    }
#endif
    memset((SINGLE_POINTER mclmac), 0, sizeof(MCLMAC_t));
#ifdef __LINUX__
    (*mclmac)->frame = (Frame_t *)malloc(sizeof(Frame_t));
    if ((*mclmac)->frame == NULL)
        return;
    memset((*mclmac)->frame, 0, sizeof(Frame_t));
#endif

    MAC_internals_init(&(SINGLE_POINTER mclmac)->mac);
    ARROW((SINGLE_POINTER mclmac)->mac)radio = radio;
    (SINGLE_POINTER mclmac)->_dataQSize = dataQSize;
    (SINGLE_POINTER mclmac)->macState.currentState = START;
    (SINGLE_POINTER mclmac)->powerMode.currentState = STARTP;
    (SINGLE_POINTER mclmac)->_nChannels = _nChannels;
    (SINGLE_POINTER mclmac)->_nSlots = _nSlots;
    (SINGLE_POINTER mclmac)->_max_number_packets_buffer = 5 * 256;      // Maximum 5 packets of siza 256 each

    int bytes = get_number_bytes(_nChannels * _nSlots);
#ifdef __LINUX__
    (*mclmac)->_occupiedSlots = (uint8_t *)malloc(bytes * sizeof(uint8_t));
    memset((*mclmac)->_occupiedSlots, 0, bytes);
    (*mclmac)->_packets = NULL;
    (*mclmac)->_packets_received = NULL;
#endif
#ifdef __RIOT__
    create_array(&mclmac->_occupiedSlots, bytes);
#endif
}
 
void MCLMAC_destroy(MCLMAC_t DOUBLE_POINTER mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(*mclmac != NULL);
#endif

    MAC_internals_destroy(&(SINGLE_POINTER mclmac)->mac);
#ifdef __LINUX__
    free((*mclmac)->frame);
    free((*mclmac)->_occupiedSlots);
    free((*mclmac));
    *mclmac = NULL;
#endif

#ifdef __RIOT__
    free_array(&mclmac->_occupiedSlots);
    memset(mclmac, 0, sizeof(MCLMAC_t));
#endif
}

void MCLMAC_clear(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    
    MAC_internals_clear(REFERENCE mclmac->mac);
    ARROW(mclmac->frame)current_slot = 0;
    ARROW(mclmac->frame)current_frame = 0;
    ARROW(mclmac->frame)current_cf_slot = 0;
    ARROW(mclmac->frame)cf_slots_number = 0;
    ARROW(mclmac->frame)slots_number = 0;
    mclmac->_dataQSize = 0;
    mclmac->macState.currentState = START;
    mclmac->powerMode.currentState = PASSIVE;
    mclmac->_collisionDetected = 0;
    mclmac->_networkTime = 0;
}

void mclmac_init_powermode_state_machine(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
    mclmac->powerMode.currentState = STARTP;
    mclmac->powerMode.nextState = NONEP;
}

int mclmac_update_powermode_state_machine(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    if (mclmac->powerMode.currentState == mclmac->powerMode.nextState)
        return E_PM_NO_TRANSITION;
 
    switch (mclmac->powerMode.nextState)
    {
    case NONEP:
        mclmac_set_powermode_state(mclmac, NONEP);
        return E_PM_NO_TRANSITION;
    case PASSIVE:
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        mclmac_set_powermode_state(mclmac, PASSIVE);
        break;
    
    case ACTIVE:
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == TRANSMIT)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == RECEIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        mclmac_set_powermode_state(mclmac, ACTIVE);
        break;

    case TRANSMIT:
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == PASSIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == RECEIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        mclmac_set_powermode_state(mclmac, TRANSMIT);
        break;

    case RECEIVE:
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == PASSIVE)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == TRANSMIT)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == FINISHP)
            return E_PM_TRANSITION_ERROR;
        mclmac_set_powermode_state(mclmac, RECEIVE);
        break;

    case FINISHP:
        if (mclmac->powerMode.currentState == STARTP)
            return E_PM_TRANSITION_ERROR;
        if (mclmac->powerMode.currentState == PASSIVE)
            return E_PM_TRANSITION_ERROR;
        mclmac_set_powermode_state(mclmac, FINISHP);
        break;

    default:
        return E_PM_INVALID_STATE;
        break;
    }
    return E_PM_TRANSITION_SUCCESS;
}

int mclmac_execute_powermode_state(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->frame != NULL);
#endif
    assert(mclmac->_nSlots > 0);
    assert(mclmac->_frameDuration > 0);
    assert(mclmac->_slotDuration > 0);
    assert(mclmac->_cfDuration > 0);
    assert(mclmac->_max_number_packets_buffer > 0);

    if (mclmac->powerMode.currentState == NONEP)
        return E_PM_EXECUTION_FAILED;
    
    switch (mclmac->powerMode.currentState)
    {
    case STARTP:
        // Set the number of slots and cfslots
        mclmac_set_slots_number(mclmac, mclmac->_nSlots);
        mclmac_set_cf_slots_number(mclmac, mclmac->_nSlots);

        // Set the value of the timers
        mclmac_set_frame_duration(mclmac, mclmac->_frameDuration);
        mclmac_set_slot_duration(mclmac, mclmac->_slotDuration);
        mclmac_set_cf_duration(mclmac, mclmac->_cfDuration);

        // Initialize frame, slots, and cfslots counter to zero
        mclmac_set_current_frame(mclmac, 0);
        mclmac_set_current_slot(mclmac, 0);
        mclmac_set_current_cf_slot(mclmac, 0);
        mclmac->_packets_read = 0;

        // Pass immediatly to PASSIVE state
        mclmac_set_next_powermode_state(mclmac, PASSIVE);

        // Create an array of size of at most 5 packet of 256 bytes each
#ifdef __LINUX__
        mclmac->_packets = (uint8_t *)malloc(mclmac->_max_number_packets_buffer * sizeof(uint8_t));
        if (mclmac->_packets == NULL)
        {
            exit(0);
        }
        mclmac->_packets_received = (uint8_t *)malloc(mclmac->_max_number_packets_buffer * sizeof(uint8_t));
        if (mclmac->_packets_received == NULL)
        {
            exit(0);
        }
#endif
#ifdef __RIOT__
        int ret = create_array(&mclmac->_packets, mclmac->_max_number_packets_buffer);
        if (ret == 0)
        {
            exit(0);
        }
        ret = create_array(&mclmac->_packets_received, mclmac->_max_number_packets_buffer);
        if (ret == 0)
        {
            exit(0);
        }
#endif

        // Initialize the timeouts API
        timeout_init();

        // Arm the slot timer for the first time.
        ARROW(mclmac->frame)slot_timer = timeout_set(ARROW(mclmac->frame)slot_duration);
        break;
    
    case PASSIVE:
        // Reset the cf counter to zero
        mclmac_set_current_cf_slot(mclmac, 0);

#ifdef __LINUX__
        /* Set readio to sleep */
#endif
#ifdef __RIOT__
        /* Set radio to sleep */
#endif
        bool sleep = true;
        uint8_t packets_read = 0;
        uint16_t bytes = 0;
        uint32_t read_from = 0;
        while (sleep)
        {
            if (timeout_passed(ARROW(mclmac->frame)slot_timer) == 1)
            {
                /* Terminate the cycle and unarm the timeout */
                timeout_unset(ARROW(mclmac->frame)slot_timer);
                sleep = false;
            }

            /* Read at most 5 packets from the queue. */
            if (packets_read < 5)
            {
                uint8_t nelement = stub_mclmac_read_queue_element(mclmac, &bytes, mclmac->_max_number_packets_buffer, &read_from);
                if (nelement == 1)
                    packets_read++;
            }

            /* 'Write' packets into queue. */
            if (mclmac->_num_packets_received > 0)
            {
                uint8_t nelements = stub_mclmac_write_queue_element(mclmac, mclmac->_max_number_packets_buffer);
                if (nelements == 1)
                    mclmac->_num_packets_received--;
            }
        }
        mclmac->_packets_read += packets_read;

        /* Set once again the slot timer before leaving the state. */
        ARROW(mclmac->frame)slot_timer = timeout_set(ARROW(mclmac->frame)slot_duration);
        mclmac_set_next_powermode_state(mclmac, ACTIVE);
        break;
    
    case ACTIVE:
        

#ifdef __LINUX__
        // change radio to common channel
#endif        

        mclmac_set_next_powermode_state(mclmac, TRANSMIT);
        break;
    default:
        break;
    }

    return E_PM_EXECUTION_SUCCESS;
}

void mclmac_set_MAC_state(MCLMAC_t *mclmac, state_t state)
{
    assert(mclmac != NULL);
    assert(state != NONE);
    mclmac->macState.currentState = state;
}

void mclmac_set_next_MAC_state(MCLMAC_t *mclmac, state_t next)
{
    assert(mclmac != NULL);
    assert(next != NONE);
    assert(next != START);

    mclmac->macState.nextState = next;
}

state_t mclmac_get_MAC_state(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->macState.currentState;
}

void mclmac_set_powermode_state(MCLMAC_t *mclmac, PowerMode_t mode)
{
    assert(mclmac != NULL);
 
    mclmac->powerMode.currentState = mode;
}

void mclmac_set_next_powermode_state(MCLMAC_t *mclmac, PowerMode_t next)
{
    assert(mclmac != NULL);
    assert(next != STARTP);

    mclmac->powerMode.nextState = next;
}

PowerMode_t mclmac_get_powermode_state(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->powerMode.currentState;
}

void mclmac_set_cf_channel(MCLMAC_t *mclmac, uint32_t cfchannel)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)cfChannel = cfchannel;
}

uint32_t mclmac_get_cf_channel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    return ARROW(mclmac->mac)cfChannel;
}

void mclmac_set_transmit_channel(MCLMAC_t *mclmac, uint32_t channel)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)transmitChannel = channel;
}

uint32_t mclmac_get_transmit_channel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    return ARROW(mclmac->mac)transmitChannel;
}

void mclmac_set_reception_channel(MCLMAC_t *mclmac, uint32_t rChannel)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)receiveChannel = rChannel;
}

uint32_t mclmac_get_reception_channel(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    return ARROW(mclmac->mac)receiveChannel;
}

void mclmac_set_available_channels(MCLMAC_t *mclmac, ARRAY* channels, uint8_t nChannels)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(channels != NULL);
#endif
#ifdef __RIOT__
    assert(channels->size > 0);
#endif
    assert(nChannels > 0);

    ARROW(mclmac->mac)numberChannels = nChannels;
#ifdef __LINUX__
    mclmac->mac->channels = (uint8_t *)malloc(mclmac->mac->numberChannels * sizeof(uint8_t));
#endif
#ifdef __RIOT__
    create_array(&mclmac->mac.channels, nChannels);
#endif
    for (int i = 0; i < ARROW(mclmac->mac)numberChannels; i++)
    {
        uint8_t element = READ_ARRAY((SINGLE_POINTER channels), i);
        WRITE_ARRAY(REFERENCE ARROW(mclmac->mac)channels, element, i);
    }
}

void mclmac_get_available_channels(MCLMAC_t *mclmac, ARRAY* channels, uint8_t *nChannels)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->channels != NULL);
    assert(channels != NULL);
    assert(*channels != NULL);
#endif
#ifdef __RIOT__
    assert(mclmac->mac.channels.size > 0);
    assert(channels->size > 0);
#endif
    assert(nChannels != NULL);

    *nChannels = ARROW(mclmac->mac)numberChannels;
    for (int i = 0; i < *nChannels; i++)
    {
        uint8_t element = READ_ARRAY(REFERENCE ARROW(mclmac->mac)channels, i);
        WRITE_ARRAY(SINGLE_POINTER channels, element, i);
    }
}

void mclmac_set_nodeid(MCLMAC_t *mclmac, uint16_t id)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)nodeID = id;
}

uint16_t mclmac_get_nodeid(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__    
    assert(mclmac->mac != NULL);
#endif

    return ARROW(mclmac->mac)nodeID;
}

void mclmac_set_selected_slot(MCLMAC_t *mclmac, uint8_t selectedSlot)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)selectedSlot = selectedSlot;

}

uint8_t mclmac_get_selected_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif
    
    return ARROW(mclmac->mac)selectedSlot;
}

void mclmac_set_number_of_hops(MCLMAC_t *mclmac, uint8_t hops)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)hopCount = hops;
}

uint8_t mclmac_get_number_of_hops(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    return ARROW(mclmac->mac)hopCount;
}

void mclmac_set_current_frame(MCLMAC_t *mclmac, uint32_t frame_number)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->frame != NULL);
#endif

    ARROW(mclmac->frame)current_frame = frame_number;
}

void mclmac_increase_frame(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->frame != NULL);
#endif

    ARROW(mclmac->frame)current_frame++;
}

void mclmac_set_current_slot(MCLMAC_t *mclmac, uint8_t current_slot)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->frame != NULL);
#endif

    ARROW(mclmac->frame)current_slot = current_slot;
}

void mclmac_increase_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->frame != NULL);
#endif

    ARROW(mclmac->frame)current_slot++;
}

void mclmac_set_slots_number(MCLMAC_t *mclmac, uint8_t slots_number)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->frame != NULL);
#endif
    assert(slots_number > 0);

    ARROW(mclmac->frame)slots_number = slots_number;
}

void mclmac_set_cf_slots_number(MCLMAC_t *mclmac, uint8_t cf_slots_number)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->frame != NULL);
#endif
    assert(cf_slots_number > 0);

    ARROW(mclmac->frame)cf_slots_number = cf_slots_number;
}

void mclmac_set_current_cf_slot(MCLMAC_t *mclmac, uint8_t current_cf_slot)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->frame != NULL);
#endif

    ARROW(mclmac->frame)current_cf_slot = current_cf_slot;
}

void mclmac_increase_cf_slot(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->frame != NULL);
#endif

    ARROW(mclmac->frame)current_cf_slot++;
}

void mclmac_set_slot_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double slot_duration
#endif
#ifdef __RIOT__
uint32_t slot_duration
#endif
)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->frame != NULL);
#endif
    assert(slot_duration > 0);

    ARROW(mclmac->frame)slot_duration = slot_duration;
}

void mclmac_set_frame_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double frame_duration
#endif
#ifdef __RIOT__
uint32_t frame_duration
#endif
)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->frame != NULL);
#endif
    assert(frame_duration > 0);

    ARROW(mclmac->frame)frame_duration = frame_duration;
}

void mclmac_set_cf_duration(MCLMAC_t *mclmac,
#ifdef __LINUX__
double cf_duration
#endif
#ifdef __RIOT__
uint32_t cf_duration 
#endif
)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->frame != NULL);
#endif
    assert(cf_duration > 0);
    ARROW(mclmac->frame)cf_duration = cf_duration;
}

void mclmac_record_collision(MCLMAC_t *mclmac, uint8_t collisionSlot, uint32_t collisionFrequency)
{
    assert(mclmac != NULL);
    assert(collisionFrequency > 0);

    mclmac->_collisionDetected = true;
    mclmac->_collisionSlot = collisionSlot;
    mclmac->_collisionFrequency = collisionFrequency;
}

void mclmac_set_destination_id(MCLMAC_t *mclmac, uint16_t id)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif

    ARROW(mclmac->mac)destinationID = id;
}

uint16_t mclmac_get_destination_id(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return ARROW(mclmac->mac)destinationID;
}

void mclmac_set_network_time(MCLMAC_t *mclmac, uint32_t time)
{
    assert(mclmac != NULL);

    mclmac->_networkTime = time;
}

uint32_t mclmac_get_network_time(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);

    return mclmac->_networkTime;
}

void mclmac_create_cf_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
#endif
    assert(ARROW(mclmac->mac)nodeID > 0);

#ifdef __LINUX__
    if (mclmac->mac->cfpkt != NULL)
        cfpacket_destroy(&mclmac->mac->cfpkt);
#endif
#ifdef __RIOT__
    cfpacket_destroy(&mclmac->mac.cfpkt);
#endif

    cfpacket_init(&ARROW(mclmac->mac)cfpkt);
    cfpacket_create(REFERENCE ARROW(mclmac->mac)cfpkt, ARROW(mclmac->mac)nodeID, ARROW(mclmac->mac)destinationID, mclmac->_networkTime);
}

void mclmac_create_control_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);

    if (mclmac->mac->ctrlpkt != NULL)
        controlpacket_destroy(&mclmac->mac->ctrlpkt);
#endif
#ifdef __RIOT__
    controlpacket_destroy(&mclmac->mac.ctrlpkt);
#endif

    controlpacket_init(&ARROW(mclmac->mac)ctrlpkt, mclmac->_nSlots, mclmac->_nChannels);
    controlpacket_create(REFERENCE ARROW(mclmac->mac)ctrlpkt, ARROW(mclmac->mac)nodeID, &mclmac->_occupiedSlots, mclmac->_collisionSlot, 
                    mclmac->_collisionFrequency, ARROW(mclmac->mac)hopCount, mclmac->_networkTime, mclmac->_ack);
}

void mclmac_create_data_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);

    if (mclmac->mac->datapkt != NULL)
        datapacket_destroy(&mclmac->mac->datapkt);
#endif
#ifdef __RIOT__
    datapacket_destroy(&mclmac->mac.datapkt);
#endif

    datapacket_init(&ARROW(mclmac->mac)datapkt);
    datapacket_create(REFERENCE ARROW(mclmac->mac)datapkt, mclmac->_isFragment, mclmac->_numberFragments, mclmac->_fragmentNumber, NULL, 0);
}

void mclmac_set_packet_data(MCLMAC_t *mclmac, ARRAY* data, uint8_t size)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->datapkt != NULL);
    assert(mclmac->mac->datapkt->data == NULL);
#endif
#ifdef __RIOT__
    assert(mclmac->mac.datapkt.data.size == 0);
#endif
    assert(size > 0);
    
    datapacket_set_data(REFERENCE ARROW(mclmac->mac)datapkt, data, size);
}

void mclmac_delete_data_from_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->datapkt != NULL);
#endif

    datapacket_delete_data(REFERENCE ARROW(mclmac->mac)datapkt);
}

void mclmac_clear_data_from_packet(MCLMAC_t *mclmac)
{
    assert(mclmac != NULL);
#ifdef __LINUX__
    assert(mclmac->mac != NULL);
    assert(mclmac->mac->datapkt != NULL);
#endif

    datapacket_clear_data(REFERENCE ARROW(mclmac->mac)datapkt);
}

void mclmac_start_CAD_mode(MCLMAC_t *mclmac)
{
    // TO DO
    (void) mclmac;
    return;
}

int32_t stub_mclmac_read_queue_element(MCLMAC_t *mclmac, uint16_t *bytes, size_t size, uint32_t *read_from)
{
    assert(mclmac != NULL);
    assert(bytes != NULL);
    assert(read_from != NULL);
    assert(size > 0);

    if ((*read_from) == size - 1)
    {
        *bytes = 0;
        return 0;
    }
    uint16_t datasize = rand() % 250;
    if ((size - (*read_from)) < (datasize + 7U))
    {
        *bytes = 0;
        return 0;
    }

    /* Node id */
    WRITE_ARRAY(REFERENCE mclmac->_packets, rand(), *read_from);
    WRITE_ARRAY(REFERENCE mclmac->_packets, rand(), (*read_from) + 1);
    /* Is fragment */
    WRITE_ARRAY(REFERENCE mclmac->_packets, (rand() != 0 ? 1 : 0), (*read_from) + 2);
    /* total fragment */
    WRITE_ARRAY(REFERENCE mclmac->_packets, rand(), (*read_from) + 3);
    /* Fragment number */
    WRITE_ARRAY(REFERENCE mclmac->_packets, rand(), (*read_from) + 4);
    /* data size */
    WRITE_ARRAY(REFERENCE mclmac->_packets, (uint8_t)datasize, (*read_from) + 5);
    uint8_t i;
    for (i = 0; i < datasize; i++)
    {
        uint8_t element = rand();
        element = (element == 0 ? 2 : element);
        WRITE_ARRAY(REFERENCE mclmac->_packets, element, (*read_from) + i + 6);
    }
    WRITE_ARRAY(REFERENCE mclmac->_packets, '\0', (*read_from) + datasize + 6);
    *bytes = 7 + datasize;
    *read_from += *bytes;
    
    return 1;
}

int32_t stub_mclmac_write_queue_element(MCLMAC_t *mclmac, size_t size)
{
    assert(mclmac != NULL);

    if (size <= 0)
        return 0;
    
    size_t i;
    /* "Write the packet into de queue* (just pretend to do something.) */
    for (i = 0; i < 255; i++)
        WRITE_ARRAY(REFERENCE mclmac->_packets_received, 0, i);
    /* Displace the rest of the elements to the front. */
    for (i = 256; i < size; i++)
    {
        uint8_t element = READ_ARRAY(REFERENCE mclmac->_packets_received, i);
        WRITE_ARRAY(REFERENCE mclmac->_packets_received, element, i - 255);
    }
    return 1;
}