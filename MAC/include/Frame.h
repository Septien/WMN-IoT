/**
 * @ingroup     MAC
 * @{
 * @file
 * @brief       The data structure containing information about the frames used for data transmission.
 * 
 * @author      José Antonio Septién Hernández.
 * @}
 */
#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Headers for the timers
#ifdef __LINUX__
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#endif

#include "timeouts.h"
#include "memory.h"
#include "memory_macros.h"

typedef struct Frame
{
    uint32_t current_frame;
    uint8_t current_slot;
    uint8_t slots_number;
    uint8_t current_cf_slot;
    uint8_t cf_slots_number;
#ifdef __LINUX__
    // Tiemr descriptor (identifier)
    int frame_timer;
    int slot_timer;
    int cf_timer;
    // Timer duration
    double frame_duration;
    double slot_duration;
    double cf_duration;
#endif
#ifdef __RIOT__
    // Tiemr descriptor (identifier)
    uint32_t frame_timer;
    uint32_t slot_timer;
    uint32_t cf_timer;
    // Timer duration
    uint32_t frame_duration;
    uint32_t slot_duration;
    uint32_t cf_duration;
#endif
}Frame_t;

#endif  // FRAME_H