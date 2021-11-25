/**
 * @file config_mac.h
 * @author José Antonio Septién Hernández (jaseptienh@gmail.com)
 * @brief Important configuration variables for proper working of the MAC layer.
 *        The value of the variables can be changed via compiler defines or by 
 *        directly editing this file. The variables here included are the following:
 *        -MAX_NUMBER_FREQS: 
 *          Maximum number of channels available to the node. It depends
 *          a lot on the type of radio in use, the frequencies available according to 
 *          the regional regulations, and the system's design.
 * 
 *        -MAX_NUMBER_SLOTS:
 *          Number of slots available to the network. It depends on the network's design.
 * 
 *        -FRAME_DURATION:
 *          The duration of each frame, determined by the network's design.
 * 
 *        -SLOT_DURATION:
 *          The duration of each slot, determined by the network's design.
 * 
 *        -INTIALIZATION_TIMEOUT:
 *          Maximum number of time a node will wait in the INITIALIZATION state, in search 
 *          of other nodes.AFter this time, if no node is found, it will be considered to be
 *          the first node of the network, and should proceed to the other states.
 *          It depends on the type of technology in use and the nertwork's design.
 * 
 *        -CF_PHASE_PERCENTAGE.
 *          During packet transmission, an important phase is the CF one, for letting other 
 *          nodes which nodes will transmit, and to whom. This define specifies the percentage
 *          it takes from the slot's time.
 * 
 *        -DATA_PACKET_QUEUE_SIZE:
 *          The maximum number of elements the queue of data packets will hold in a given moment.
 * 
 *        -CF_PACKET_QUEUE_SIZE:
 *          The maximum number of elements the queue of cf packets will hold in a given moment.
 * 
 *        -frequencies:
 *          Array holding the allowed frequencies to the network. It should comply with regulations
 *          and the technology in use. The total number should be equal to MAX_NUMBER_FREQS.
 * 
 *        -CF_FREQUENCY:
 *          The channel for the common frequency phase.
 * @version 0.1
 * @date 2021-11-25
 * 
 */
#ifndef CONFIG_MAC_H
#define CONFIG_MAC_H

#ifdef __LINUX__
#include <stdint.h>
#endif
#ifdef __RIOT__
#include <ztimer.h>
#endif

#ifndef MAX_NUMBER_FREQS
#define MAX_NUMBER_FREQS        8
#endif

#ifndef MAX_NUMBER_SLOTS
#define MAX_NUMBER_SLOTS        8
#endif

/* The following three variables are in milliseconds, the value can later 
    be chaned of units. */
#ifndef FRAME_DURATION
#define FRAME_DURATION          1000000
#endif

/* The slot duration depends on the frame duration and the desired number of slots. */
#ifndef SLOT_DURATION
#define SLOT_DURATION           FRAME_DURATION / MAX_NUMBER_SLOTS
#endif

#ifndef INITIALIZATION_TIMEOUT
#define INITIALIZATION_TIMEOUT  30000000
#endif

#ifndef CF_PHASE_PERCENTAGE
#define CF_PHASE_PERCENTAGE     0.2
#endif

#ifndef DATA_PACKET_QUEUE_SIZE
#define DATA_PACKET_QUEUE_SIZE  5
#endif

#ifndef CF_PACKET_QUEUE_SIZE
#define CF_PACKET_QUEUE_SIZE    5
#endif

/* This variable cannot be changed via compiler commands, it needs directly editing.
    All frequencies should be different. */
static uint32_t frequencies[MAX_NUMBER_FREQS] = {
    903000000, 905000000, 907000000, 912000000, 917000000, 922000000, 927000000, 928000000
};

#ifndef CF_FREQUENCY
#define CF_FREQUENCY            902000000
#endif

/* Clock type for RIOT. */
#ifdef __RIOT__
#define CLOCK                   ZTIMER_USEC
#endif

#endif      // CONFIG_MAC_H