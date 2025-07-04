#include "core/kmemory.h"
#include "core/logger.h"
#include "math/math_types.h"

#include "renderer/vulkan/vulkan_pipeline.h"
#include "renderer/vulkan/vulkan_shader_utils.h"
#include "vulkan_object_shader.h"


/**
 * @brief Name of the built-in object shader.
 */
#define BUILTIN_SHADER_NAME_OBJECT "Builtin.ObjectShader"

/**
 * @brief Number of shader stages in the object shader.
 */
#define ATTRIBUTE_COUNT 1

b8 vulkan_object_shader_create(vulkan_context* context, vulkan_object_shader* out_shader) {
    // TODO: MAKE CONFIGURABLE

    // Shader module init per stage.
    char stage_type_strs[OBJECT_SHADER_STAGE_COUNT][5] = {"vert", "frag"};
    VkShaderStageFlagBits stage_types[OBJECT_SHADER_STAGE_COUNT] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};

    for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) {
        if (!create_shader_module(context, BUILTIN_SHADER_NAME_OBJECT, stage_type_strs[i], stage_types[i], i, out_shader->stages)) {
            KERROR("Unable to create %s shader module for '%s'.", stage_type_strs[i], BUILTIN_SHADER_NAME_OBJECT);
            return False;
        }
    }

    // TO-DO: Descriptors

    // Pipeline creation
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = (f32)context->framebuffer_height;
    viewport.width = (f32)context->framebuffer_width;
    viewport.height = -(f32)context->framebuffer_height; // Invert Y for Vulkan
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Scissor rectangle
    VkRect2D scissor = {};
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = context->framebuffer_width;
    scissor.extent.height = context->framebuffer_height;

    // Attributes
    u32 offset = 0;
    VkVertexInputAttributeDescription attribute_description[ATTRIBUTE_COUNT];
    // Position
    VkFormat formats[ATTRIBUTE_COUNT] = {VK_FORMAT_R32G32B32_SFLOAT};

    u64 sizes[ATTRIBUTE_COUNT] = {
        sizeof(vec3)
    };

    for (u32 i = 0; i < ATTRIBUTE_COUNT; ++i) {
        attribute_description[i].binding = 0; // Binding Index should match binding desc
        attribute_description[i].location = i; // Attribute location in the shader
        attribute_description[i].format = formats[i];
        attribute_description[i].offset = offset;

        offset += sizes[i];
    }

    // TO-DO: Descriptor Set Layouts

    // Stages
    // NOTE: Should match the number of shader->stages
    VkPipelineShaderStageCreateInfo stage_create_infos[OBJECT_SHADER_STAGE_COUNT] = {};
    kzero_memory(stage_create_infos, sizeof(stage_create_infos));

    for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) {
        stage_create_infos[i].sType = out_shader->stages[i].shader_stage_create_info.sType;
        stage_create_infos[i] = out_shader->stages[i].shader_stage_create_info;
    }

    if (!vulkan_graphics_pipeline_create(
        context,
        &context->main_renderpass,
        ATTRIBUTE_COUNT,
        attribute_description,
        0,0,
        OBJECT_SHADER_STAGE_COUNT,
        stage_create_infos,
        viewport,
        scissor,
        False,
        &out_shader->pipeline
    )) {
        KERROR("Failed to create graphics pipeline for object shader.");
        return False;
    }


    return True;
}

void vulkan_object_shader_destroy(vulkan_context* context, struct vulkan_object_shader* shader) {
    vulkan_pipeline_destroy(context, &shader->pipeline);

    for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) {
        vkDestroyShaderModule(
            context->device.logical_device,
            shader->stages[i].handle,
            context->allocator);

        shader->stages[i].handle = 0;
    }
}

void vulkan_object_shader_use(vulkan_context* context, struct vulkan_object_shader* shader) {
}