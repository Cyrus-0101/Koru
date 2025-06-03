#pragma once

#include "defines.h"

/**
 * @file vulkan_platform.h
 * @brief Platform-specific Vulkan extension utilities.
 *
 * This file provides declarations for functions that handle platform-specific
 * Vulkan instance extensions needed to create a Vulkan surface and manage windowing.
 */

/**
 * @brief Appends the names of required Vulkan instance extensions for the current platform.
 *
 * This function adds platform-specific Vulkan extension names (e.g., for window surface creation)
 * to the provided dynamic array (`names_darray`). These extensions are necessary for creating
 * a Vulkan instance that can interact with the window system.
 *
 * Example usage:
 * @code
 * const char** required_extensions = darray_create(const char*);
 * platform_get_required_extension_names(&required_extensions);
 * @endcode
 *
 * @param names_darray A pointer to a dynamic array of `const char*` where extension names will be appended.
 *                     Must be initialized before calling this function.
 */
void platform_get_required_extension_names(const char*** names_darray);