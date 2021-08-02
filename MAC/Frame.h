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

#ifdef __RIOT__
#include "config.h"
#include "periodic.h"
#include "ztimer.h"
#endif

typedef struct Frame
{
    uint32_t currentFrame;
    uint8_t currentSlot;
    uint8_t slotsNumber;
    uint8_t currentCFSlot;
    uint8_t cfSlotsNumber;
#ifdef __LINUX__
    // linux timers
    timer_t frameTimer;
    timer_t slotTimer;
    timer_t cfTimer;
    // linux timer values
    struct itimerval frameDuration;
    struct itimerval slotDuration;
    struct itimerval cfDuration;
#endif
#ifdef __RIOT__
    // RIOT timers
    ztimer_t frameTimer;
    ztimer_t slotTimer;
    ztimer_t sfTimer;
    // RIOT timer values
    uint32_t frameDuration;
    uint32_t slotDuration;
    uint32_t cfDuration;
#endif
}Frame_t;

#endif  // FRAME_H