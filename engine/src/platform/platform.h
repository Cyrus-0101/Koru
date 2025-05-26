#pragma once

#include "defines.h"

typedef struct platform_state {
    /* No type - depends on its implementation */
    void* internal_state;
} platform_state;

KAPI b8 platform_startup(
    platform_state* plat_state,
    const char* application_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height);

KAPI void platform_shutdown(platform_state* plat_state);

KAPI b8 platform_pump_messages(platform_state* plat_state);

void* platform_allocate(u64 size, b8 aligned);

void platform_free(void* block, b8 aligned);

void* platform_zero_memory(void* block, u64 size);

void* platform_copy_memory(void* dest, const void* source, u64 size);

void* platform_set_memory(void* dest, i32 value, u64 size);

// Writing to a platform's console if any
void platform_console_write(const char* message, u8 colour);

// Seperate errors
void platform_console_write_error(const char* message, u8 colour);

// How to get time on each platform
f64 platform_get_absolute_time();

// Sleep on the thread for the provided ms. This blocks the main thread
// Should only be used for giving time back to the OS for unused update power
// Therefor not exported
void platform_sleep(u64 ms);