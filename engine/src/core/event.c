#include "core/event.h"
#include "core/kmemory.h"
#include "containers/darray.h"

/**
 * @file event.c
 * @brief Implementation of a generic event system for inter-component communication.
 *
 * This module implements a centralized event system that allows components to:
 * - Register listeners for specific event codes
 * - Fire events with optional context data
 * - Unregister listeners when no longer needed
 *
 * Internally, it uses dynamic arrays (`darray`) to manage event callbacks per code,
 * and ensures that duplicate registrations are not allowed.
 *
 * Usage:
 * Use the public API functions in `event.h` to interact with this system.
 * Never access internal structures directly.
 */

/**
 * @brief Internal structure representing a registered event listener.
 */
typedef struct registered_event {
    /**
     * @brief A pointer to the listener instance. May be NULL.
     */
    void* listener;

    /**
     * @brief The callback function that will handle the event.
     */
    PFN_on_event callback;
} registered_event;

/**
 * @brief Entry for an event code, containing all registered listeners.
 *
 * Each event code maps to an array of registered events.
 */
typedef struct event_code_entry {
    /**
     * @brief Dynamic array of registered events for this code.
     */
    registered_event* events;
} event_code_entry;

// State structure for the event system
#define MAX_MESSAGE_CODES 16384  ///< Maximum number of unique event codes supported

/**
 * @brief Global state for the event system.
 *
 * Maintains a lookup table mapping event codes to their listeners.
 */
typedef struct event_system_state {
    /**
     * @brief Lookup table for event codes.
     *
     * Maps each code to its list of registered events.
     */
    event_code_entry registered[MAX_MESSAGE_CODES];
} event_system_state;

/**
 * @brief Static global instance of the event system state.
 */
static event_system_state* state_ptr;

void event_system_initialize(u64* memory_requirement, void* state) {
    *memory_requirement = sizeof(event_system_state);
    if (state == 0) {
        return;
    }

    // Zero out the entire state structure
    kzero_memory(state, sizeof(state));
    state_ptr = state;
}

void event_system_shutdown(void* state) {
    if (state_ptr) {
        // Free all event arrays
        for (u16 i = 0; i < MAX_MESSAGE_CODES; ++i) {
            if (state_ptr->registered[i].events != 0) {
                darray_destroy(state_ptr->registered[i].events);
                state_ptr->registered[i].events = 0;
            }
        }
    }
    state_ptr = 0;
}

b8 event_register(u16 code, void* listener, PFN_on_event on_event) {
    if (!state_ptr) {
        return False;
    }

    // Lazily create the array for this event code if not already created
    if (state_ptr->registered[code].events == 0) {
        state_ptr->registered[code].events = darray_create(registered_event);
    }

    // Check for duplicates
    u64 registered_count = darray_length(state_ptr->registered[code].events);
    for (u64 i = 0; i < registered_count; ++i) {
        if (state_ptr->registered[code].events[i].listener == listener &&
            state_ptr->registered[code].events[i].callback == on_event) {
            // Duplicate registration found — skip
            return False;
        }
    }

    // Add new event registration
    registered_event event;
    event.listener = listener;
    event.callback = on_event;
    darray_push(state_ptr->registered[code].events, event);

    return True;
}

b8 event_unregister(u16 code, void* listener, PFN_on_event on_event) {
    if (!state_ptr) {
        return False;
    }

    // No listeners registered for this code
    if (state_ptr->registered[code].events == 0) {
        return False;
    }

    // Search for matching registration
    u64 registered_count = darray_length(state_ptr->registered[code].events);
    for (u64 i = 0; i < registered_count; ++i) {
        registered_event e = state_ptr->registered[code].events[i];
        if (e.listener == listener && e.callback == on_event) {
            // Found match — remove it
            registered_event popped_event;
            darray_pop_at(state_ptr->registered[code].events, i, &popped_event);
            return True;
        }
    }

    // No match found
    return False;
}

b8 event_fire(u16 code, void* sender, event_context context) {
    if (!state_ptr) {
        return False;
    }

    // No listeners for this code
    if (state_ptr->registered[code].events == 0) {
        return False;
    }

    // Notify all listeners until one handles the event
    u64 registered_count = darray_length(state_ptr->registered[code].events);
    for (u64 i = 0; i < registered_count; ++i) {
        registered_event e = state_ptr->registered[code].events[i];
        if (e.callback(code, sender, e.listener, context)) {
            // Event was handled — stop propagation
            return True;
        }
    }

    // Event was not handled
    return False;
}