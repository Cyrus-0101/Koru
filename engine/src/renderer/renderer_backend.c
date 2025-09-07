#include "core/logger.h"
#include "renderer_backend.h"
#include "vulkan/vulkan_backend.h"

/**
 * @file renderer_backend.c
 * @brief Implementation of renderer backend creation and destruction logic.
 *
 * This file defines the functions used to instantiate and destroy specific rendering backends,
 * such as Vulkan. It maps function pointers in the renderer_backend structure to the appropriate
 * implementation functions for the selected backend type.
 */

/**
 * @brief Creates and initializes a renderer backend of the specified type.
 *
 * This function sets up the function table for the given backend type and initializes its state.
 * Currently only supports Vulkan, but can be extended to support OpenGL, DirectX, etc.
 *
 * @param type The type of renderer backend to create.
 * @param plat_state A pointer to the platform-specific state.
 * @param out_renderer_backend A pointer to a renderer_backend structure to populate.
 * @return True if the backend was successfully created; otherwise False.
 */
b8 renderer_backend_create(renderer_backend_type type, renderer_backend* out_renderer_backend) {
    // Assign function pointers based on the requested backend type
    if (type == RENDERER_BACKEND_TYPE_VULKAN) {
        out_renderer_backend->initialize = vulkan_renderer_backend_initialize;
        out_renderer_backend->shutdown = vulkan_renderer_backend_shutdown;
        out_renderer_backend->begin_frame = vulkan_renderer_backend_begin_frame;
        out_renderer_backend->update_global_state = vulkan_renderer_update_global_state;
        out_renderer_backend->end_frame = vulkan_renderer_backend_end_frame;
        out_renderer_backend->resized = vulkan_renderer_backend_on_resized;
        out_renderer_backend->update_object = vulkan_backend_update_object;
        out_renderer_backend->create_texture = vulkan_renderer_create_texture;
        out_renderer_backend->destroy_texture = vulkan_renderer_destroy_texture;

        return True;
    }

    // Unsupported backend type
    KERROR("Unsupported renderer backend type: %d", type);
    return False;
}

/**
 * @brief Destroys a renderer backend by clearing all function pointers.
 *
 * This does not free memory or clean up internal resources — it simply resets
 * the function pointers to NULL. Any backend-specific cleanup should be done
 * before calling this function.
 *
 * @param renderer_backend A pointer to the renderer backend instance to destroy.
 */
void renderer_backend_destroy(renderer_backend* renderer_backend) {
    // Clear all function pointers
    renderer_backend->initialize = 0;
    renderer_backend->shutdown = 0;
    renderer_backend->begin_frame = 0;
    renderer_backend->update_global_state = 0;
    renderer_backend->end_frame = 0;
    renderer_backend->resized = 0;
    renderer_backend->update_object = 0;
    renderer_backend->create_texture = 0;
    renderer_backend->destroy_texture = 0;
}