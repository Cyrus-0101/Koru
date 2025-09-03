#include "core/kmemory.h"
#include "core/logger.h"
#include "math/math_types.h"
#include "math/kmath.h"

#include "renderer/vulkan/vulkan_buffer.h"
#include "renderer/vulkan/vulkan_pipeline.h"
#include "renderer/vulkan/vulkan_shader_utils.h"

/**
 * @brief Name of the built-in object shader.
 */
#define BUILTIN_SHADER_NAME_OBJECT "Builtin.ObjectShader"

/**
 * @brief Number of shader stages in the object shader.
 */
#define ATTRIBUTE_COUNT 2

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

    // Global Descriptors
    VkDescriptorSetLayoutBinding global_ubo_layout_binding;
    global_ubo_layout_binding.binding = 0;
    global_ubo_layout_binding.descriptorCount = 1;
    global_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    global_ubo_layout_binding.pImmutableSamplers = 0;
    global_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo global_layout_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    global_layout_info.bindingCount = 1;
    global_layout_info.pBindings = &global_ubo_layout_binding;
    VK_CHECK(vkCreateDescriptorSetLayout(context->device.logical_device, &global_layout_info, context->allocator, &out_shader->global_descriptor_set_layout));

    // Global descriptor pool: Used for global items such as view/projection matrix.
    // Global Descriptor Pool - supports both UBO and Combined Image Samplers
    VkDescriptorPoolSize global_pool_sizes[2];

    // Uniform buffer descriptors
    global_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    global_pool_sizes[0].descriptorCount = context->swapchain.image_count;

    global_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    global_pool_sizes[1].descriptorCount = context->swapchain.image_count;

    VkDescriptorPoolCreateInfo global_pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    global_pool_info.poolSizeCount = 2;
    global_pool_info.pPoolSizes = global_pool_sizes;
    global_pool_info.maxSets = context->swapchain.image_count * 2;  // One per frame x 2 types
    VK_CHECK(vkCreateDescriptorPool(context->device.logical_device, &global_pool_info, context->allocator, &out_shader->global_descriptor_pool));

    // Local/Object Descriptors
    const u32 local_sampler_count = 1;
    VkDescriptorType descriptor_types[VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT] = {
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          /// Binding 0 - uniform buffer
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // Binding 1 - Diffuse sampler layout.
    };

    VkDescriptorSetLayoutBinding bindings[VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT];

    kzero_memory(&bindings, sizeof(VkDescriptorSetLayoutBinding) * VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT);
    for (u32 i = 0; i < VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT; ++i) {
        bindings[i].binding = i;
        bindings[i].descriptorCount = 1;
        bindings[i].descriptorType = descriptor_types[i];
        bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    VkDescriptorSetLayoutCreateInfo layout_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    layout_info.bindingCount = VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT;
    layout_info.pBindings = bindings;
    VK_CHECK(vkCreateDescriptorSetLayout(context->device.logical_device, &layout_info, 0, &out_shader->object_descriptor_set_layout));

    // Local/Object descriptor pool: Used for object specific items like diffuse color
    VkDescriptorPoolSize object_pool_sizes[2];
    // The first section will be used by uniform buffers
    object_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    object_pool_sizes[0].descriptorCount = VULKAN_OBJECT_MAX_OBJECT_COUNT;
    // The second section will be used for image samplers.
    object_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    object_pool_sizes[1].descriptorCount = local_sampler_count * VULKAN_OBJECT_MAX_OBJECT_COUNT;

    VkDescriptorPoolCreateInfo object_pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    object_pool_info.poolSizeCount = 2;
    object_pool_info.pPoolSizes = object_pool_sizes;
    object_pool_info.maxSets = VULKAN_OBJECT_MAX_OBJECT_COUNT;

    // Create Object Descriptor Pool;
    VK_CHECK(vkCreateDescriptorPool(context->device.logical_device, &object_pool_info, context->allocator, &out_shader->object_descriptor_pool));

    // Pipeline creation
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = (f32)context->framebuffer_height;
    viewport.width = (f32)context->framebuffer_width;
    viewport.height = -(f32)context->framebuffer_height;  // Invert Y for Vulkan
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Scissor rectangle
    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = context->framebuffer_width;
    scissor.extent.height = context->framebuffer_height;

    // Attributes
    u32 offset = 0;
    VkVertexInputAttributeDescription attribute_descriptions[ATTRIBUTE_COUNT];
    // Position, texCoord
    VkFormat formats[ATTRIBUTE_COUNT] = {VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT};

    u64 sizes[ATTRIBUTE_COUNT] = {
        sizeof(vec3),
        sizeof(vec2)};

    for (u32 i = 0; i < ATTRIBUTE_COUNT; ++i) {
        attribute_descriptions[i].binding = 0;   // Binding Index should match binding desc
        attribute_descriptions[i].location = i;  // Attribute location in the shader
        attribute_descriptions[i].format = formats[i];
        attribute_descriptions[i].offset = offset;

        offset += sizes[i];
    }

    // Descriptor Set Layouts
    const i32 descriptor_set_layout_count = 2;
    VkDescriptorSetLayout layouts[2] = {
        out_shader->global_descriptor_set_layout,
        out_shader->object_descriptor_set_layout};

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
            attribute_descriptions,
            descriptor_set_layout_count,
            layouts,
            OBJECT_SHADER_STAGE_COUNT,
            stage_create_infos,
            viewport,
            scissor,
            False,
            &out_shader->pipeline)) {
        KERROR("Failed to create graphics pipeline for object shader.");
        return False;
    }

    // Create uniform buffer.
    if (!vulkan_buffer_create(
            context,
            sizeof(global_uniform_object),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            True,
            &out_shader->global_uniform_buffer)) {
        KERROR("Vulkan buffer creation failed in object shader.");
        return False;
    }

    // Allocate global descriptor sets.
    u32 frame_count = context->swapchain.image_count;

    VkDescriptorSetLayout* global_layouts = kallocate(sizeof(VkDescriptorSetLayout) * frame_count, MEMORY_TAG_ARRAY);
    for (u32 i = 0; i < frame_count; ++i) {
        global_layouts[i] = out_shader->global_descriptor_set_layout;
    }

    VkDescriptorSetAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    alloc_info.descriptorPool = out_shader->global_descriptor_pool;
    alloc_info.descriptorSetCount = frame_count;
    alloc_info.pSetLayouts = global_layouts;

    VK_CHECK(vkAllocateDescriptorSets(context->device.logical_device, &alloc_info, out_shader->global_descriptor_sets));

    kfree(global_layouts, sizeof(VkDescriptorSetLayout) * frame_count, MEMORY_TAG_ARRAY);

    if (!vulkan_buffer_create(
            context,
            sizeof(object_uniform_object),  ///< MAX_MATERIAL_INSTANCE_COUNT
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            True,
            &out_shader->object_uniform_buffer)) {
        KERROR("Material Instance buffer creation failed for shader.");
        return False;
    }

    return True;
}

void vulkan_object_shader_destroy(vulkan_context* context, struct vulkan_object_shader* shader) {
    VkDevice logical_device = context->device.logical_device;

    // Destroy object descriptor pool.
    vkDestroyDescriptorPool(logical_device, shader->object_descriptor_pool, context->allocator);

    // Destroy object descriptor set.
    vkDestroyDescriptorSetLayout(logical_device, shader->object_descriptor_set_layout, context->allocator);

    // Destroy object uniform buffers
    vulkan_buffer_destroy(context, &shader->object_uniform_buffer);
    // Destroy uniform buffer.
    vulkan_buffer_destroy(context, &shader->global_uniform_buffer);

    // Destroy pipeline
    vulkan_pipeline_destroy(context, &shader->pipeline);

    // Destroy global descriptor pool.
    KDEBUG("Destroying global descriptor pool %p", shader->global_descriptor_pool);
    vkDestroyDescriptorPool(logical_device, shader->global_descriptor_pool, context->allocator);

    // Destroy descriptor set layouts.
    KDEBUG("Destroying global descriptor set layout %p", shader->global_descriptor_set_layout);
    vkDestroyDescriptorSetLayout(logical_device, shader->global_descriptor_set_layout, context->allocator);

    // Destroy shader modules.
    for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i) {
        KDEBUG("Destroying shader module %p", shader->stages[i].handle);
        vkDestroyShaderModule(
            context->device.logical_device,
            shader->stages[i].handle,
            context->allocator);

        shader->stages[i].handle = 0;
    }
}

void vulkan_object_shader_use(vulkan_context* context, struct vulkan_object_shader* shader) {
    u32 image_index = context->image_index;
    vulkan_pipeline_bind(&context->graphics_command_buffers[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, &shader->pipeline);
}

void vulkan_object_shader_update_global_state(vulkan_context* context, struct vulkan_object_shader* shader, f32 delta_time) {
    u32 image_index = context->image_index;
    VkCommandBuffer command_buffer = context->graphics_command_buffers[image_index].handle;
    VkDescriptorSet global_descriptor = shader->global_descriptor_sets[image_index];

    // Configure the descriptors for the given index.
    u32 range = sizeof(global_uniform_object);
    u64 offset = 0;

    // Copy data to buffer
    vulkan_buffer_load_data(context, &shader->global_uniform_buffer, offset, range, 0, &shader->global_ubo);

    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = shader->global_uniform_buffer.handle;
    bufferInfo.offset = offset;
    bufferInfo.range = range;

    // Update descriptor sets.
    VkWriteDescriptorSet descriptor_write = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    descriptor_write.dstSet = shader->global_descriptor_sets[image_index];
    descriptor_write.dstBinding = 0;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(context->device.logical_device, 1, &descriptor_write, 0, 0);

    // Bind the global descriptor set to be updated.
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->pipeline.pipeline_layout, 0, 1, &global_descriptor, 0, 0);
}

/**
 * Updates per-object descriptor data (UBO + texture) and binds the descriptor set.
 * Should be called once per object before drawing.
 */
void vulkan_object_shader_update_object(vulkan_context* context, struct vulkan_object_shader* shader, geometry_render_data data) {
    u32 image_index = context->image_index;
    VkCommandBuffer command_buffer = context->graphics_command_buffers[image_index].handle;

    // Push constants: model matrix
    vkCmdPushConstants(
        command_buffer,
        shader->pipeline.pipeline_layout,
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(mat4),
        &data.model);

    // Get object state and descriptor set
    if (data.object_id >= VULKAN_OBJECT_MAX_OBJECT_COUNT) {
        KERROR("Invalid object_id: %u", data.object_id);
        return;
    }
    vulkan_object_shader_object_state* object_state = &shader->object_states[data.object_id];
    VkDescriptorSet object_descriptor_set = object_state->descriptor_sets[image_index];

    // Descriptor writes will be batched
    VkWriteDescriptorSet descriptor_writes[VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT] = {0};
    u32 write_count = 0;

    // === Binding 0: Uniform Buffer (object_ubo.diffuse_color) ===
    u32 ubo_range = sizeof(object_uniform_object);
    u64 ubo_offset = data.object_id * ubo_range;

    // Generate test color (pulsing gray)
    static f32 accumulator = 0.0f;
    accumulator += context->frame_delta_time;
    f32 s = (ksin(accumulator) + 1.0f) / 2.0f;
    object_uniform_object obo = {{s, s, s, 1.0f}};

    vulkan_buffer_load_data(context, &shader->object_uniform_buffer, ubo_offset, ubo_range, 0, &obo);

    // Only update UBO descriptor if not already updated
    u32* ubo_generation = &object_state->descriptor_states[0].generations[image_index];
    if (*ubo_generation == INVALID_ID) {
        VkDescriptorBufferInfo buffer_info = {
            .buffer = shader->object_uniform_buffer.handle,
            .offset = ubo_offset,
            .range = ubo_range};

        VkWriteDescriptorSet* write = &descriptor_writes[write_count++];
        *write = (VkWriteDescriptorSet){
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = object_descriptor_set,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &buffer_info};

        *ubo_generation = 1;  // Mark as updated (only needs one-time update)
    }

    // === Binding 1: Combined Image Sampler (diffuse_sampler) ===
    texture* t = data.textures[0];  // Assume one texture
    if (!t) {
        t = shader->default_diffuse;  // Fallback to default texture
    }

    u32* sampler_generation = &object_state->descriptor_states[1].generations[image_index];

    // Skip update if already up to date
    if (t && (*sampler_generation != t->generation || *sampler_generation == INVALID_ID)) {
        if (!t->internal_data) {
            // KWARN("Texture '%s' has no internal_data! Using default.", t->name);
            t = shader->default_diffuse;
            if (!t || !t->internal_data) {
                KERROR("Default texture is invalid! Cannot recover.");
                return;
            }
        }

        vulkan_texture_data* tex_data = (vulkan_texture_data*)t->internal_data;

        VkDescriptorImageInfo image_info = {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = tex_data->image.view,
            .sampler = tex_data->sampler};

        VkWriteDescriptorSet* write = &descriptor_writes[write_count++];
        *write = (VkWriteDescriptorSet){
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = object_descriptor_set,
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &image_info};

        *sampler_generation = t->generation;
    }

    // === Update all descriptors at once ===
    if (write_count > 0) {
        vkUpdateDescriptorSets(
            context->device.logical_device,
            write_count,
            descriptor_writes,
            0,
            NULL);
    }

    // === Bind the descriptor set ===
    vkCmdBindDescriptorSets(
        command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        shader->pipeline.pipeline_layout,
        1,  // set = 1
        1,
        &object_descriptor_set,
        0,
        NULL);
}

b8 vulkan_object_shader_acquire_resources(vulkan_context* context, struct vulkan_object_shader* shader, u32* out_object_id) {
    // TO-DO: Free list
    *out_object_id = shader->object_uniform_buffer_index;
    shader->object_uniform_buffer_index++;

    u32 object_id = *out_object_id;
    vulkan_object_shader_object_state* object_state = &shader->object_states[object_id];
    for (u32 i = 0; i < VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT; ++i) {
        for (u32 j = 0; j < 3; ++j) {
            object_state->descriptor_states[i].generations[j] = INVALID_ID;
        }
    }

    // Allocate descriptor sets
    VkDescriptorSetLayout* layouts = kallocate(sizeof(VkDescriptorSetLayout) * context->swapchain.image_count, MEMORY_TAG_ARRAY);
    for (u32 i = 0; i < context->swapchain.image_count; ++i) {
        layouts[i] = shader->object_descriptor_set_layout;
    }

    VkDescriptorSetAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    alloc_info.descriptorPool = shader->object_descriptor_pool;
    alloc_info.descriptorSetCount = context->swapchain.image_count;
    alloc_info.pSetLayouts = layouts;

    VkResult result = vkAllocateDescriptorSets(context->device.logical_device, &alloc_info, object_state->descriptor_sets);

    kfree(layouts, sizeof(VkDescriptorSetLayout) * context->swapchain.image_count, MEMORY_TAG_ARRAY);

    if (result != VK_SUCCESS) {
        KERROR("Error allocating descriptor sets in shader!");

        return False;
    }

    return True;
}

void vulkan_object_shader_release_resources(vulkan_context* context, struct vulkan_object_shader* shader, u32 object_id) {
    vulkan_object_shader_object_state* object_state = &shader->object_states[object_id];

    const u32 descriptor_set_count = 3;

    // Release object descriptor sets
    VkResult result = vkFreeDescriptorSets(context->device.logical_device, shader->object_descriptor_pool, descriptor_set_count, object_state->descriptor_sets);
    if (result != VK_SUCCESS) {
        KERROR("Error Freeing object shader descriptor sets!");
    }

    for (u32 i = 0; i < VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT; ++i) {
        for (u32 j = 0; j < 3; ++j) {
            object_state->descriptor_states[i].generations[j] = INVALID_ID;
        }
    }

    // TO-DO: Add the object_id to the free list
}