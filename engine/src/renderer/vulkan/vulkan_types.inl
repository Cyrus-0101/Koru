#pragma once

#include "core/asserts.h"
#include "defines.h"
#include "renderer/renderer_types.inl"

#include <vulkan/vulkan.h>

#define OBJECT_SHADER_STAGE_COUNT 2

#define VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT 2

#define VULKAN_OBJECT_MAX_OBJECT_COUNT 1024

#define MAX_FRAMES_IN_FLIGHT 4

typedef struct vulkan_descriptor_state {
    // One per frame
    u32 generations[MAX_FRAMES_IN_FLIGHT];

    u32 ids[MAX_FRAMES_IN_FLIGHT];
} vulkan_descriptor_state;

typedef struct vulkan_object_shader_object_state {
    // Per frame
    VkDescriptorSet descriptor_sets[MAX_FRAMES_IN_FLIGHT];

    // Per descriptor
    vulkan_descriptor_state descriptor_states[VULKAN_OBJECT_SHADER_DESCRIPTOR_COUNT];
} vulkan_object_shader_object_state;

/**
 * @file vulkan_types.inl
 * @brief Internal Vulkan-specific types used across the Koru engine.
 *
 * This file contains structures and typedefs specific to the Vulkan renderer implementation.
 * It should only be included in Vulkan-related source files (e.g., device, swapchain, renderpass).
 */

/**
 * @def VK_CHECK(expr)
 * @brief Checks if the result of a Vulkan API call is `VK_SUCCESS`.
 *
 * If the expression does not evaluate to `VK_SUCCESS`, this macro triggers an assertion failure,
 * which helps catch Vulkan errors early during development.
 *
 * @param expr A Vulkan API call that returns a VkResult.
 */
#define VK_CHECK(expr)               \
    {                                \
        KASSERT(expr == VK_SUCCESS); \
    }

/**
 * @def OBJECT_SHADER_STAGE_COUNT
 * @brief Number of shader stages used in the object shader.
 *
 * This is used to define how many shader stages are present in the object shader,
 * which is typically 2 (vertex and fragment).
 */
#define OBJECT_SHADER_STAGE_COUNT 2

/**
 * @struct vulkan_buffer
 * @brief Represents a Vulkan buffer with its size, handle, and memory binding.
 *
 * Used for vertex/index buffers, uniform buffers, and other GPU memory allocations.
 * Tracks whether the buffer is currently locked for writing.
 */
typedef struct vulkan_buffer {
    /**
     * @brief Size of the buffer in bytes.
     */
    u64 total_size;
    /**
     * @brief Usage flags indicating how this buffer will be used (e.g., VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).
     */
    VkBuffer handle;

    /**
     * @brief Vulkan buffer usage flags indicating how this buffer will be used.
     *
     * For example, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT for vertex buffers,
     * VK_BUFFER_USAGE_INDEX_BUFFER_BIT for index buffers, etc.
     */
    VkBufferUsageFlagBits usage;
    /**
     * @brief Indicates if the buffer is currently locked for writing.
     *
     * If True, the buffer cannot be used for rendering until unlocked.
     */
    b8 is_locked;
    /**
     * @brief Memory requirements for this buffer (alignment, size, memory type bits).
     */
    VkDeviceMemory memory;
    /**
     * @brief Index of the memory type used for this buffer.
     *
     * This is used to find the correct memory type index when allocating memory.
     */
    i32 memory_index;
    /**
     * @brief Memory property flags indicating how this buffer's memory can be accessed.
     *
     * For example, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT for CPU access.
     */
    u32 memory_property_flags;
} vulkan_buffer;

/**
 * @struct vulkan_swapchain_support_info
 * @brief Stores information about a physical device’s swapchain capabilities.
 *
 * This structure holds:
 * - Surface capabilities (min/max image count, dimensions, transform flags)
 * - Supported pixel formats (e.g., R8G8B8A8_UNORM)
 * - Available presentation modes (e.g., FIFO, MAILBOX)
 *
 * Used when selecting a suitable physical device and creating the swapchain.
 */
typedef struct vulkan_swapchain_support_info {
    /**
     * @brief Surface capabilities such as min/max image count, extent, and usage flags.
     */
    VkSurfaceCapabilitiesKHR capabilities;

    /**
     * @brief Number of available surface formats.
     */
    u32 format_count;

    /**
     * @brief Array of supported surface formats (e.g., B8G8R8A8_UNORM).
     */
    VkSurfaceFormatKHR* formats;

    /**
     * @brief Number of available present modes (e.g., MAILBOX_KHR).
     */
    u32 present_mode_count;

    /**
     * @brief Array of supported presentation modes.
     */
    VkPresentModeKHR* present_modes;
} vulkan_swapchain_support_info;

/**
 * @struct vulkan_device
 * @brief Stores logical and physical device info along with queue data.
 *
 * Contains all necessary device-level state needed by the renderer module.
 * Includes:
 * - Physical/logical device handles
 * - Queue indices and actual queues
 * - Device properties, features, memory info
 * - Swapchain support info
 * - Depth format
 */
typedef struct vulkan_device {
    /**
     * @brief The physical GPU device selected for rendering.
     */
    VkPhysicalDevice physical_device;

    /**
     * @brief Logical device interface to the physical device.
     *
     * Created from the physical device and used for most Vulkan operations.
     */
    VkDevice logical_device;

    /**
     * @brief Swapchain support details for the current surface.
     */
    vulkan_swapchain_support_info swapchain_support;

    /**
     * @brief Index of the graphics queue family.
     */
    i32 graphics_queue_index;

    /**
     * @brief Index of the present queue family.
     */
    i32 present_queue_index;

    /**
     * @brief Index of the transfer queue family.
     */
    i32 transfer_queue_index;

    /**
     * @brief Whether the device supports memory that is both device-local and host-visible.
     */
    b8 supports_device_local_host_visible;

    /**
     * @brief Graphics queue handle for submitting command buffers.
     */
    VkQueue graphics_queue;

    /**
     * @brief Presentation queue handle for displaying rendered images.
     */
    VkQueue present_queue;

    /**
     * @brief Transfer queue handle for memory copy operations.
     */
    VkQueue transfer_queue;

    /**
     * @brief Command pool for graphics commands.
     *
     * Used to allocate command buffers that are submitted to the graphics queue.
     */
    VkCommandPool graphics_command_pool;

    /**
     * @brief Properties of the physical device (name, type, driver version, etc.).
     */
    VkPhysicalDeviceProperties properties;

    /**
     * @brief Features supported by the physical device (e.g., anisotropic filtering).
     */
    VkPhysicalDeviceFeatures features;

    /**
     * @brief Memory properties including heap sizes and locality flags.
     */
    VkPhysicalDeviceMemoryProperties memory;

    /**
     * @brief Format used for depth attachments (e.g., VK_FORMAT_D32_SFLOAT).
     */
    VkFormat depth_format;
} vulkan_device;

/**
 * @struct vulkan_image
 * @brief Represents a single Vulkan image resource with view and memory binding.
 *
 * Used for color/depth attachments and texture resources.
 * Tracks width/height for easier reference.
 */
typedef struct vulkan_image {
    /**
     * @brief Handle to the raw VkImage object.
     */
    VkImage handle;

    /**
     * @brief Device memory bound to this image.
     */
    VkDeviceMemory memory;

    /**
     * @brief Image view used to access the image in shaders/render passes.
     */
    VkImageView view;

    /**
     * @brief Width of the image in pixels.
     */
    u32 width;

    /**
     * @brief Height of the image in pixels.
     */
    u32 height;
} vulkan_image;

/**
 * @enum vulkan_render_pass_state
 * @brief Tracks the current state of a render pass.
 *
 * Helps manage render pass lifecycle and ensures correct command recording flow.
 */
typedef enum vulkan_render_pass_state {
    READY,            ///< Render pass is ready to begin
    RECORDING,        ///< Recording has started but no subpass yet
    IN_RENDER_PASS,   ///< Currently inside a render pass
    RECORDING_ENDED,  ///< Recording completed, not yet submitted
    SUBMITTED,        ///< Command buffer was submitted to the queue
    NOT_ALLOCATED     ///< Command buffer not yet allocated
} vulkan_render_pass_state;

/**
 * @struct vulkan_renderpass
 * @brief Holds state for a single render pass.
 *
 * Includes:
 * - Clear values (color, depth, stencil)
 * - Viewport/scissor settings
 * - Current render pass state
 */
typedef struct vulkan_renderpass {
    /**
     * @brief Handle to the actual render pass object.
     */
    VkRenderPass handle;

    /**
     * @brief X position of the viewport.
     */
    f32 x;

    /**
     * @brief Y position of the viewport.
     */
    f32 y;

    /**
     * @brief Width of the viewport.
     */
    f32 w;

    /**
     * @brief Height of the viewport.
     */
    f32 h;

    /**
     * @brief Red clear value (0–1).
     */
    f32 r;

    /**
     * @brief Green clear value (0–1).
     */
    f32 g;

    /**
     * @brief Blue clear value (0–1).
     */
    f32 b;

    /**
     * @brief Alpha clear value (0–1).
     */
    f32 a;

    /**
     * @brief Depth clear value (0–1).
     */
    f32 depth;

    /**
     * @brief Stencil clear value.
     */
    u32 stencil;

    /**
     * @brief Current state of the render pass (READY, RECORDING, etc.).
     */
    vulkan_render_pass_state state;
} vulkan_renderpass;

/**
 * @struct vulkan_framebuffer
 * @brief Represents a Vulkan framebuffer used for rendering.
 *
 * A framebuffer is an object that binds image views (color, depth, etc.)
 * to a render pass, defining where rendering will occur.
 *
 * Framebuffers are created after:
 * - The swapchain (provides images)
 * - The render pass (defines attachment layout and usage)
 *
 * Must be recreated if the swapchain changes size or format.
 */
typedef struct vulkan_framebuffer {
    /**
     * @brief Handle to the actual VkFramebuffer object.
     *
     * Created using vkCreateFramebuffer and destroyed via vkDestroyFramebuffer.
     */
    VkFramebuffer handle;

    /**
     * @brief Number of attachments bound to this framebuffer.
     *
     * Typically includes:
     * - One color attachment (from swapchain image view)
     * - One depth/stencil attachment (if enabled)
     */
    u32 attachment_count;

    /**
     * @brief Array of image views used as attachments for this framebuffer.
     *
     * Usually includes:
     * - Swapchain image view
     * - Depth/stencil image view (optional)
     */
    VkImageView* attachments;

    /**
     * @brief Pointer to the render pass this framebuffer is associated with.
     *
     * This must match the render pass used during command recording.
     * The render pass defines how many and what type of attachments are used.
     */
    vulkan_renderpass* renderpass;
} vulkan_framebuffer;

/**
 * @struct vulkan_swapchain
 * @brief Represents the application’s connection to the OS for rendering output.
 *
 * Holds:
 * - Swapchain handle
 * - Image views for each frame
 * - Depth/stencil attachment
 * - Frame in-flight tracking
 */
typedef struct vulkan_swapchain {
    /**
     * @brief Format used for the swapchain images (e.g., B8G8R8A8_UNORM).
     */
    VkSurfaceFormatKHR image_format;

    /**
     * @brief Maximum number of frames that can be in flight at any time.
     */
    u8 max_frames_in_flight;

    /**
     * @brief Handle to the actual swapchain object.
     */
    VkSwapchainKHR handle;

    /**
     * @brief Total number of images in the swapchain.
     */
    u32 image_count;

    /**
     * @brief Array of VkImage handles provided by the swapchain.
     */
    VkImage* images;

    /**
     * @brief Array of VkImageView objects for accessing each swapchain image.
     */
    VkImageView* views;

    /**
     * @brief Depth attachment image used for rendering.
     */
    vulkan_image depth_attachment;

    // framebuffers used for on-screen rendering.
    vulkan_framebuffer* framebuffers;
} vulkan_swapchain;

/**
 * @enum vulkan_command_buffer_state
 * @brief Represents the lifecycle state of a Vulkan command buffer.
 *
 * This enumeration defines all possible states a command buffer can be in,
 * from creation to submission. It is used internally by the engine to:
 * - Track valid transitions
 * - Prevent invalid operations (e.g., recording after submission)
 * - Help debug rendering issues
 */
typedef enum vulkan_command_buffer_state {
    /**
     * @brief Command buffer is ready for recording.
     *
     * Can transition to: RECORDING
     */
    COMMAND_BUFFER_STATE_READY,

    /**
     * @brief Command buffer is currently being recorded into.
     *
     * Can transition to: IN_RENDER_PASS, RECORDING_ENDED
     */
    COMMAND_BUFFER_STATE_RECORDING,

    /**
     * @brief Inside a render pass — commands like drawing are allowed.
     *
     * Can transition to: RECORDING_ENDED
     */
    COMMAND_BUFFER_STATE_IN_RENDER_PASS,

    /**
     * @brief Recording has finished but not yet submitted to the GPU.
     *
     * Can transition to: SUBMITTED
     */
    COMMAND_BUFFER_STATE_RECORDING_ENDED,

    /**
     * @brief Command buffer has been submitted to a queue and is pending execution.
     *
     * Cannot transition back until it is reset or re-allocated.
     */
    COMMAND_BUFFER_STATE_SUBMITTED,

    /**
     * @brief Command buffer has not been allocated yet.
     *
     * Initial state before allocation. Must transition to READY before use.
     */
    COMMAND_BUFFER_STATE_NOT_ALLOCATED
} vulkan_command_buffer_state;

/**
 * @struct vulkan_command_buffer
 * @brief Represents a single Vulkan command buffer and its current state.
 *
 * A command buffer stores recorded GPU commands (e.g., draws, transfers)
 * and is submitted to a queue for execution.
 *
 * Used across frames with synchronization primitives to ensure correct rendering.
 */
typedef struct vulkan_command_buffer {
    /**
     * @brief Handle to the underlying VkCommandBuffer object.
     *
     * This is the actual Vulkan object used for command recording and submission.
     */
    VkCommandBuffer handle;

    /**
     * @brief Current state of this command buffer.
     *
     * Used to prevent invalid operations like double-recording or submitting an unrecorded buffer.
     */
    vulkan_command_buffer_state state;

    // /**
    //  * @brief Index of the frame this buffer belongs to (if using multiple frames-in-flight).
    //  *
    //  * Only relevant if part of a command pool that supports frame indexing.
    //  * Helps track which frame's data this buffer contains.
    //  */
    // u32 frame_index;  // Only relevant if part of a pool
} vulkan_command_buffer;

/**
 * @struct vulkan_fence
 * @brief Represents a single Vulkan fence object.
 *
 * Tracks:
 * - The underlying VkFence handle
 * - Whether the fence is currently signaled
 */
typedef struct vulkan_fence {
    /**
     * @brief Handle to the actual VkFence object.
     */
    VkFence handle;

    /**
     * @brief Whether the fence is currently signaled or not.
     */
    b8 is_signaled;
} vulkan_fence;

/**
 * @struct vulkan_shader_stage
 * @brief Represents a single shader stage in Vulkan.
 *
 * Contains:
 * - Shader module creation info
 * - Handle to the shader module
 * - Pipeline shader stage create info
 */
typedef struct vulkan_shader_stage {
    /**
     * @brief Shader module creation info used to create the shader module.
     *
     * Contains details like code size, pointer to shader code, and flags.
     */
    VkShaderModuleCreateInfo create_info;
    /**
     * @brief Handle to the actual VkShaderModule object.
     *
     * This is created from the create_info and used in pipeline creation.
     */
    VkShaderModule handle;
    /**
     * @brief Pipeline shader stage create info used to specify this stage in a pipeline.
     *
     * Contains details like stage type (vertex, fragment), module handle, entry point name,
     * and specialization info for dynamic constants.
     */
    VkPipelineShaderStageCreateInfo shader_stage_create_info;
} vulkan_shader_stage;

/**
 * @struct vulkan_pipeline
 * @brief Represents a Vulkan graphics pipeline.
 *
 * Contains:
 * - Handle to the pipeline object
 * - Layout used for this pipeline (descriptor sets, push constants)
 */
typedef struct vulkan_pipeline {
    /**
     * @brief Handle to the actual VkPipeline object.
     *
     * This is created from pipeline creation info and used for rendering.
     */
    VkPipeline handle;
    /**
     * @brief Layout used by this pipeline.
     *
     * Contains descriptor set layouts and push constant ranges.
     * Must be created before the pipeline and destroyed after.
     */
    VkPipelineLayout pipeline_layout;
} vulkan_pipeline;

/**
 * @struct vulkan_material_shader
 * @brief Represents the object shader used for rendering 3D objects.
 *
 * Contains:
 * - Shader stages (vertex, fragment)
 * - Pipeline used for rendering
 */
typedef struct vulkan_material_shader {
    /**
     * @brief Number of shader stages in this object shader.
     *
     * Typically 2: vertex and fragment.
     */
    vulkan_shader_stage stages[OBJECT_SHADER_STAGE_COUNT];

    /**
     * @brief Descriptor pool used for allocating descriptor sets.
     *
     * This pool is used to manage memory for descriptor sets that hold
     * uniform buffers, textures, and other resources.
     */
    VkDescriptorPool global_descriptor_pool;

    /**
     * @brief Layout for the global descriptor set.
     *
     * Defines the structure of the descriptor set used for global uniform data.
     * Typically includes bindings for uniform buffers, textures, etc.
     */
    VkDescriptorSetLayout global_descriptor_set_layout;

    /**
     * @brief Array of descriptor sets used for global uniform data.
     *
     * Each set corresponds to a frame in flight, allowing for per-frame updates.
     * Typically contains the global uniform buffer object (UBO) for rendering.
     * One descriptor set per frame in flight - for triple buffering.
     */
    VkDescriptorSet global_descriptor_sets[MAX_FRAMES_IN_FLIGHT];

    /**
     * @brief Global uniform buffer object (UBO) used for rendering.
     *
     * Contains projection and view matrices, and is updated per frame.
     * This buffer is bound to the global descriptor set for use in shaders.
     */
    global_uniform_object global_ubo;

    /**
     * @brief Buffer used for global uniform data.
     *
     * This buffer holds the global uniform object data and is updated each frame.
     * It is bound to the global descriptor set for use in shaders.
     */
    vulkan_buffer global_uniform_buffer;

    /**
     * @brief Descriptor pool used for allocating object-specific descriptor sets.
     *
     * This pool manages memory for descriptor sets that hold per-object resources,
     * such as model matrices and material properties.
     */
    VkDescriptorPool object_descriptor_pool;

    /**
     * @brief Layout for the object-specific descriptor set.
     *
     * Defines the structure of the descriptor set used for per-object uniform data.
     * Typically includes bindings for model matrices and material properties.
     */
    VkDescriptorSetLayout object_descriptor_set_layout;

    /**
     * @brief Buffer used for per-object uniform data.
     *
     * This buffer holds the model matrices for individual objects and is updated as needed.
     * It is bound to the object descriptor sets for use in shaders.
     */
    vulkan_buffer object_uniform_buffer;

    // TODO: Manage a free list of some kind here
    /**
     * @brief Index for tracking the next available object uniform buffer slot.
     *
     * This index is used to allocate and manage per-object uniform data within the object uniform buffer.
     * It helps in efficiently reusing buffer space for multiple objects.
     */
    u32 object_uniform_buffer_index;

    // TODO: Make dynamic
    /**
     * @brief Array of object states, one per possible object.
     *
     * Each state contains descriptor sets and tracking info for individual objects.
     * The size is defined by VULKAN_OBJECT_MAX_OBJECT_COUNT.
     */
    vulkan_object_shader_object_state object_states[VULKAN_OBJECT_MAX_OBJECT_COUNT];

    /**
     * @brief Pipeline used for rendering.
     *
     * This pipeline is created from the shader stages and used for drawing 3D objects.
     */
    vulkan_pipeline pipeline;
} vulkan_material_shader;

/**
 * @struct vulkan_context
 * @brief Represents the global state of the Vulkan rendering context.
 *
 * This structure holds all top-level Vulkan objects required for rendering.
 * It is intended to be initialized at startup and persisted for the lifetime of the renderer.
 */
typedef struct vulkan_context {
    f32 frame_delta_time;
    /**
     * @brief The framebuffer's current width.
     */
    u32 framebuffer_width;

    /**
     * @brief The framebuffer's current height.
     */
    u32 framebuffer_height;

    // Current generation of framebuffer size. If it does not match framebuffer_size_last_generation,
    // a new one should be generated.
    u64 framebuffer_size_generation;

    // The generation of the framebuffer when it was last created. Set to framebuffer_size_generation
    // when updated.
    u64 framebuffer_size_last_generation;

    /**
     * @brief The VkInstance object that represents the connection between the application and the Vulkan library.
     *
     * This is the first object created when initializing Vulkan, and it manages per-application state.
     * Must be destroyed before the application exits using `vkDestroyInstance()`.
     */
    VkInstance instance;

    /**
     * @brief Optional custom allocator for Vulkan memory allocations.
     *
     * If NULL, the default system allocator is used.
     * Useful for tracking or debugging memory usage.
     */
    VkAllocationCallbacks* allocator;

    /**
     * @brief Window surface used for rendering output.
     *
     * Created from platform-specific code and used throughout the swapchain lifecycle.
     */
    VkSurfaceKHR surface;

#if defined(_DEBUG)
    /**
     * @brief Debug messenger object used for receiving validation layer messages.
     *
     * Only present in debug builds. Used to register a callback function that receives
     * diagnostic, warning, and error messages from the Vulkan validation layers.
     */
    VkDebugUtilsMessengerEXT debug_messenger;
#endif

    /**
     * @brief Device-specific state including queues, capabilities, and memory info.
     */
    vulkan_device device;

    /**
     * @brief Swapchain object used for rendering and presenting frames.
     */
    vulkan_swapchain swapchain;

    /**
     * @brief Main render pass used for rendering.
     */
    vulkan_renderpass main_renderpass;

    /**
     * @brief Framebuffers used for rendering to the swapchain images.
     *
     * Each framebuffer corresponds to a swapchain image and is used during rendering.
     */
    vulkan_buffer object_vertex_buffer;

    /**
     * @brief Buffer containing indices for rendering 3D objects.
     *
     * Used to store index data for indexed drawing commands.
     */
    vulkan_buffer object_index_buffer;

    /**
     * @brief Array of command buffers for rendering (one per frame).
     */
    vulkan_command_buffer* graphics_command_buffers;

    /**
     * @brief Array of semaphores used to signal when an image is available for rendering.
     */
    VkSemaphore* image_available_semaphores;

    /**
     * @brief Array of semaphores used to signal when a frame is complete.
     *
     * Each semaphore corresponds to a frame in flight and is signaled when the GPU finishes processing that frame.
     */
    VkSemaphore* queue_complete_semaphores;

    /**
     * @brief Number of in-flight frames (frames that are being processed).
     */
    u32 in_flight_fence_count;

    /**
     * @brief Array of fences used to synchronize frame rendering.
     *
     * Each fence corresponds to a frame in flight and is used to wait for GPU completion before reusing resources.
     */
    vulkan_fence* in_flight_fences;

    // Holds pointers to fences which exist and are owned elsewhere.
    vulkan_fence** images_in_flight;

    /**
     * @brief Index of the currently active image in the swapchain.
     */
    u32 image_index;

    /**
     * @brief Index of the current frame being rendered.
     *
     * Used to manage synchronization primitives per-frame.
     */
    u32 current_frame;

    /**
     * @brief Indicates whether the swapchain is currently being recreated.
     *
     * Prevents other operations while resizing or recreating the swapchain.
     */
    b8 recreating_swapchain;

    /**
     * @brief Object shader used for rendering 3D objects.
     *
     * Contains shader stages and the pipeline used for rendering.
     */
    vulkan_material_shader material_shader;

    /**
     * @brief Offset in bytes for the vertex buffer used for geometry data.
     *
     * This is used to calculate where to start writing geometry vertex data in the buffer.
     */
    u64 geometry_vertex_offset;

    /**
     * @brief Offset in bytes for the index buffer used for geometry data.
     *
     * This is used to calculate where to start writing geometry index data in the buffer.
     */
    u64 geometry_index_offset;

    /**
     * @brief Function pointer for finding a compatible memory index based on requirements.
     *
     * Used internally to find suitable memory types for buffer/image allocation.
     */
    i32 (*find_memory_index)(u32 type_filter, u32 property_flags);
} vulkan_context;

typedef struct vulkan_texture_data {
    vulkan_image image;

    VkSampler sampler;
} vulkan_texture_data;