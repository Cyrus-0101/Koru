#pragma once

#include "defines.h"

/**
 * @file clock.h
 * @brief Provides a simple clock utility for timing operations.
 *
 * This module allows measuring elapsed time between start and stop events.
 * It integrates with the platform abstraction layer to retrieve high-resolution time.
 */

/**
 * @struct clock
 * @brief Represents a simple clock that tracks elapsed time.
 */
typedef struct clock {
    /**
     * @brief The absolute time when the clock was started.
     *
     * A value of 0 indicates the clock is not running.
     */
    f64 start_time;

    /**
     * @brief The total time elapsed since the clock was started.
     */
    f64 elapsed;
} clock;

/**
 * @brief Updates the clock's elapsed time based on current time.
 *
 * This function should be called before querying the elapsed time.
 * If the clock is not running (start_time == 0), this function has no effect.
 *
 * @param clock A pointer to the clock instance to update.
 */
void clock_update(clock* clock);

/**
 * @brief Starts or restarts the clock.
 *
 * Resets the elapsed time to zero and records the current time as the start time.
 *
 * @param clock A pointer to the clock instance to start.
 */
void clock_start(clock* clock);

/**
 * @brief Stops the clock without resetting its elapsed time.
 *
 * Sets the start_time to 0, effectively halting further updates to elapsed time.
 *
 * @param clock A pointer to the clock instance to stop.
 */
void clock_stop(clock* clock);
