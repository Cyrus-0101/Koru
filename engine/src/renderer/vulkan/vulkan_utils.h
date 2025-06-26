#pragma once
#include "vulkan_types.inl"

/**
 * @file vulkan_utils.h
 * @brief Utility functions for working with VkResult values.
 *
 * This module provides:
 * - Human-readable strings for VkResult codes
 * - Helper function to check if a result is considered a success or failure
 *
 * These are used throughout the engine to:
 * - Log errors during initialization
 * - Handle validation layer warnings
 * - Debug GPU operations like image creation, command submission, etc.
 */


/**
 * @brief Converts a VkResult into a human-readable string.
 *
 * Returns either:
 * - A short code (e.g., "VK_SUCCESS")
 * - Or an extended message (e.g., "Command successfully completed")
 *
 * Useful for logging and debugging Vulkan API calls.
 *
 * @param result The VkResult value to convert.
 * @param get_extended True to get a descriptive message; False to get only the enum name.
 * @return const char* A static string representation of the result.
 */
const char* vulkan_result_string(VkResult result, b8 get_extended);

/**
 * @brief Checks whether a VkResult represents a successful operation.
 *
 * Based on the Vulkan spec, some results are treated as "success" even if not VK_SUCCESS,
 * such as VK_NOT_READY or VK_SUBOPTIMAL_KHR.
 *
 * @param result The VkResult to evaluate.
 * @return True if the result indicates success (operation completed), False otherwise (error occurred).
 */
b8 vulkan_result_is_success(VkResult result);