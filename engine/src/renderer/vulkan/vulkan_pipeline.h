#pragma once

#include "vulkan_types.inl"

/**
 * @file vulkan_pipeline.h
 * @brief Vulkan pipeline creation functions.
 *
 * This file contains functions for creating graphics pipelines in Vulkan.
 * It includes the main function to create a graphics pipeline with specified parameters.
 */

/**
 * @brief Creates a Vulkan graphics pipeline with the specified parameters.
 *
 * This function sets up a graphics pipeline including shader stages, vertex input,
 * viewport, rasterization, multisampling, depth/stencil state, color blending,
 * and dynamic states. It also creates the pipeline layout with provided descriptor
 * set layouts and push constant ranges.
 *
 * @param context A pointer to the active Vulkan context.
 * @param renderpass A pointer to the vulkan_renderpass defining the render pass to use.
 * @param attribute_count The number of vertex input attributes.
 * @param attributes An array of VkVertexInputAttributeDescription defining vertex attributes.
 * @param descriptor_set_layout_count The number of descriptor set layouts.
 * @param descriptor_set_layouts An array of VkDescriptorSetLayout for the pipeline layout.
 * @param stage_count The number of shader stages.
 * @param stages An array of VkPipelineShaderStageCreateInfo defining shader stages.
 * @param viewport The VkViewport defining the viewport dimensions.
 * @param scissor The VkRect2D defining the scissor rectangle.
 * @param is_wireframe True to use wireframe rasterization, False for fill mode.
 * @param out_pipeline A pointer to a vulkan_pipeline struct to populate with the created pipeline details.
 * @return True if the pipeline was created successfully, False otherwise.
 */
b8 vulkan_graphics_pipeline_create(
    vulkan_context* context,
    vulkan_renderpass* renderpass,
    u32 attribute_count,
    VkVertexInputAttributeDescription* attributes,
    u32 descriptor_set_layout_count,
    VkDescriptorSetLayout* descriptor_set_layouts,
    u32 stage_count,
    VkPipelineShaderStageCreateInfo* stages,
    VkViewport viewport,
    VkRect2D scissor,
    b8 is_wireframe,
    vulkan_pipeline* out_pipeline);

/**
 * @brief Destroys a Vulkan pipeline and frees its associated resources.
 *
 * This function destroys the VkPipeline and VkPipelineLayout objects associated
 * with the given vulkan_pipeline. It must be called before destroying the Vulkan
 * device or context to avoid memory leaks.
 *
 * @param context A pointer to the active Vulkan context.
 * @param pipeline A pointer to the vulkan_pipeline to destroy.
 */
void vulkan_pipeline_destroy(vulkan_context* context, vulkan_pipeline* pipeline);

/**
 * @brief Binds the specified pipeline to the given command buffer.
 *
 * This function binds the provided vulkan_pipeline to the specified
 * vulkan_command_buffer at the given pipeline bind point (graphics or compute).
 * The command buffer must be in the recording state when this function is called.
 *
 * @param command_buffer A pointer to the vulkan_command_buffer to bind the pipeline to.
 * @param bind_point The VkPipelineBindPoint indicating whether it's a graphics or compute pipeline.
 * @param pipeline A pointer to the vulkan_pipeline to bind.
 */
void vulkan_pipeline_bind(vulkan_command_buffer* command_buffer, VkPipelineBindPoint bind_point, vulkan_pipeline* pipeline);