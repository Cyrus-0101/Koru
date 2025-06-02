#include "clock.h"

#include "platform/platform.h"

/**
 * @file clock.c
 * @brief Implementation of the clock utility
 *
 * This file contains the internal logic for updating, starting, and stopping clocks,
 * using the platform abstraction to get accurate time values.
 */

void clock_update(clock* clock) {
    if (clock->start_time != 0) {
        clock->elapsed = platform_get_absolute_time() - clock->start_time;
    }
}

void clock_start(clock* clock) {
    clock->start_time = platform_get_absolute_time();
    clock->elapsed = 0;
}

void clock_stop(clock* clock) {
    clock->start_time = 0;
}