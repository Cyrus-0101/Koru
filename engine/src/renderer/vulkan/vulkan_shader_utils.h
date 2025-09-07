#pragma once

#include "vulkan_types.inl"

/**
 * @file vulkan_shader_utils.h
 * @brief Utility functions for Vulkan shader module creation.
 *
 * This module provides functions to load SPIR-V shader binaries from files,
 * create Vulkan shader modules, and set up shader stage info for pipeline creation.
 * It supports multiple shader stages such as vertex and fragment shaders.
 */

/**
 * @brief Creates a Vulkan shader module from a SPIR-V binary file.
 *
 * This function loads the specified SPIR-V file, creates a VkShaderModule,
 * and populates the corresponding vulkan_shader_stage structure with
 * creation info and stage info for pipeline use.
 *
 * @param context A pointer to the active Vulkan context.
 * @param name The base name of the shader (without extension).
 * @param type_str The shader type string (e.g., "vert", "frag").
 * @param shader_stage_flag The VkShaderStageFlagBits indicating the shader stage.
 * @param stage_index The index in the shader_stages array to populate.
 * @param shader_stages An array of vulkan_shader_stage to hold created stages.
 * @return True if the shader module was created successfully, False otherwise.
 */
b8 create_shader_module(
    vulkan_context* context,
    const char* name,
    const char* type_str,
    VkShaderStageFlagBits shader_stage_flag,
    u32 stage_index,
    vulkan_shader_stage* shader_stages);