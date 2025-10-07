#include "material_system.h"

#include "core/logger.h"
#include "core/kstring.h"
#include "containers/hashtable.h"
#include "math/kmath.h"

#include "renderer/renderer_frontend.h"

#include "systems/texture_system.h"
#include "systems/resource_system.h"

/**
 * @file material_system.c
 *
 * @brief Implementation of the material system.
 *
 * This module implements the functions defined in `material_system.h`.
 * It manages the creation, acquisition, and release of materials,
 * as well as the default material and material references.
 *
 * Responsibilities:
 * - Initialize the material system with `material_system_initialize()`.
 * - Acquire materials by name using `material_system_acquire()`.
 * - Release materials using `material_system_release()`.
 * - Shutdown the material system with `material_system_shutdown()`.
 */

/**
 *
 * @struct material_system_state
 *
 * @brief Internal state of the material system.
 */
typedef struct material_system_state {
    /** Configuration settings for the material system. */
    material_system_config config;

    /** Default material used when no specific material is assigned. */
    material default_material;

    /** Array of registered materials managed by the system. */
    material* registered_materials;

    /** Hashtable mapping material names to their indices in the registered_materials array. */
    hashtable registered_material_table;
} material_system_state;

/**
 * @struct material_reference
 * @brief Internal structure to track material references and auto-release status.
 */
typedef struct material_reference {
    /** Number of active references to the material. */
    u64 reference_count;
    /** Unique handle identifying the material. */
    u32 handle;
    /** Auto-release flag indicating if the material should be automatically released when no longer referenced. */
    b8 auto_release;
} material_reference;

/** Pointer to the internal state of the material system. */
static material_system_state* state_ptr = 0;

/**
 * @brief Creates the default material used when no specific material is assigned.
 *
 * @param state Pointer to the material system state.
 * @return `True` if the default material was created successfully, `False` otherwise.
 */
b8 create_default_material(material_system_state* state);

/**
 * @brief Loads a material based on the provided configuration.
 *
 * @param config Configuration for the material to be loaded.
 * @param m Pointer to the material structure to be populated.
 * @return `True` if the material was loaded successfully, `False` otherwise.
 */
b8 load_material(material_config config, material* m);

/**
 * @brief Destroys a material, freeing its resources.
 *
 * @param m Pointer to the material to be destroyed.
 */
void destroy_material(material* m);

b8 material_system_initialize(u64* memory_requirement, void* state, material_system_config config) {
    if (config.max_material_count == 0) {
        KFATAL("material_system_initialize - config.max_material_count must be > 0.");
        return False;
    }

    // Block of memory will contain state structure, then block for array, then block for hashtable.
    u64 struct_requirement = sizeof(material_system_state);
    u64 array_requirement = sizeof(material) * config.max_material_count;
    u64 hashtable_requirement = sizeof(material_reference) * config.max_material_count;
    *memory_requirement = struct_requirement + array_requirement + hashtable_requirement;

    if (!state) {
        return True;
    }

    state_ptr = state;
    state_ptr->config = config;

    // The array block is after the state. Already allocated, so set the pointer.
    void* array_block = state + struct_requirement;
    state_ptr->registered_materials = array_block;

    // Hashtable block is after array.
    void* hashtable_block = array_block + array_requirement;

    // Create a hashtable for material lookups.
    hashtable_create(sizeof(material_reference), config.max_material_count, hashtable_block, False, &state_ptr->registered_material_table);

    // Fill the hashtable with invalid references to use as a default.
    material_reference invalid_ref;
    invalid_ref.auto_release = False;
    invalid_ref.handle = INVALID_ID;  // Primary reason for needing default values.
    invalid_ref.reference_count = 0;
    hashtable_fill(&state_ptr->registered_material_table, &invalid_ref);

    // Invalidate all materials in the array.
    u32 count = state_ptr->config.max_material_count;
    for (u32 i = 0; i < count; ++i) {
        state_ptr->registered_materials[i].id = INVALID_ID;
        state_ptr->registered_materials[i].generation = INVALID_ID;
        state_ptr->registered_materials[i].internal_id = INVALID_ID;
    }

    if (!create_default_material(state_ptr)) {
        KFATAL("Failed to create default material. Application cannot continue.");
        return False;
    }

    return True;
}

void material_system_shutdown(void* state) {
    material_system_state* s = (material_system_state*)state;
    if (s) {
        // Invalidate all materials in the array.
        u32 count = s->config.max_material_count;
        for (u32 i = 0; i < count; ++i) {
            if (s->registered_materials[i].id != INVALID_ID) {
                destroy_material(&s->registered_materials[i]);
            }
        }

        // Destroy the default material.
        destroy_material(&s->default_material);
    }

    state_ptr = 0;
}

material* material_system_acquire(const char* name) {
    // Load material configuration from resource;
    resource material_resource;
    if (!resource_system_load(name, RESOURCE_TYPE_MATERIAL, &material_resource)) {
        KERROR("Failed to load material resource, returning nullptr.");
        return 0;
    }

    // Now acquire from loaded config.
    material* m = 0;
    if (material_resource.data) {
        m = material_system_acquire_from_config(*(material_config*)material_resource.data);
    }

    // Clean up
    resource_system_unload(&material_resource);

    if (!m) {
        KERROR("Failed to load material resource, returning nullptr.");
    }

    return m;
}

material* material_system_acquire_from_config(material_config config) {
    // Return default material.
    if (strings_equali(config.name, DEFAULT_MATERIAL_NAME)) {
        return &state_ptr->default_material;
    }

    material_reference ref;
    if (state_ptr && hashtable_get(&state_ptr->registered_material_table, config.name, &ref)) {
        // This can only be changed the first time a material is loaded.
        if (ref.reference_count == 0) {
            ref.auto_release = config.auto_release;
        }

        ref.reference_count++;

        if (ref.handle == INVALID_ID) {
            // This means no material exists here. Find a free index first.
            u32 count = state_ptr->config.max_material_count;
            material* m = 0;

            for (u32 i = 0; i < count; ++i) {
                if (state_ptr->registered_materials[i].id == INVALID_ID) {
                    // A free slot has been found. Use its index as the handle.
                    ref.handle = i;
                    m = &state_ptr->registered_materials[i];

                    break;
                }
            }

            // Make sure an empty slot was actually found.
            if (!m || ref.handle == INVALID_ID) {
                KFATAL("material_system_acquire - Material system cannot hold anymore materials. Adjust configuration to allow more.");
                return 0;
            }

            // Create new material.
            if (!load_material(config, m)) {
                KERROR("Failed to load material '%s'.", config.name);
                return 0;
            }

            if (m->generation == INVALID_ID) {
                m->generation = 0;
            } else {
                m->generation++;
            }

            // Also use the handle as the material id.
            m->id = ref.handle;
            KTRACE("Material '%s' does not yet exist. Created, and ref_count is now %i.", config.name, ref.reference_count);
        } else {
            KTRACE("Material '%s' already exists, ref_count increased to %i.", config.name, ref.reference_count);
        }

        // Update the entry.
        hashtable_set(&state_ptr->registered_material_table, config.name, &ref);
        return &state_ptr->registered_materials[ref.handle];
    }

    // NOTE: This would only happen in the event something went wrong with the state.
    KERROR("material_system_acquire_from_config failed to acquire material '%s'. Null pointer will be returned.", config.name);
    return 0;
}

void material_system_release(const char* name) {
    // Ignore release requests for the default material.
    if (strings_equali(name, DEFAULT_MATERIAL_NAME)) {
        return;
    }

    material_reference ref;
    if (state_ptr && hashtable_get(&state_ptr->registered_material_table, name, &ref)) {
        if (ref.reference_count == 0) {
            KWARN("Tried to release non-existent material: '%s'", name);
            return;
        }

        ref.reference_count--;
        if (ref.reference_count == 0 && ref.auto_release) {
            material* m = &state_ptr->registered_materials[ref.handle];

            // Destroy/reset material.
            destroy_material(m);

            // Reset the reference.
            ref.handle = INVALID_ID;
            ref.auto_release = False;
            KTRACE("Released material '%s'., Material unloaded because reference count=0 and auto_release=true.", name);
        } else {
            KTRACE("Released material '%s', now has a reference count of '%i' (auto_release=%s).", name, ref.reference_count, ref.auto_release ? "true" : "false");
        }

        // Update the entry.
        hashtable_set(&state_ptr->registered_material_table, name, &ref);
    } else {
        KERROR("material_system_release failed to release material '%s'.", name);
    }
}

b8 load_material(material_config config, material* m) {
    kzero_memory(m, sizeof(material));

    // name
    string_ncopy(m->name, config.name, MATERIAL_NAME_MAX_LENGTH);

    // Diffuse colour
    m->diffuse_color = config.diffuse_color;

    // Diffuse map
    if (string_length(config.diffuse_map_name) > 0) {
        m->diffuse_map.use = TEXTURE_USE_MAP_DIFFUSE;
        m->diffuse_map.texture = texture_system_acquire(config.diffuse_map_name, True);

        if (!m->diffuse_map.texture) {
            KWARN("Unable to load texture '%s' for material '%s', using default.", config.diffuse_map_name, m->name);
            m->diffuse_map.texture = texture_system_get_default_texture();
        }
    } else {
        // NOTE: Only set for clarity, as call to kzero_memory above does this already.
        m->diffuse_map.use = TEXTURE_USE_UNKNOWN;
        m->diffuse_map.texture = 0;
    }

    // TODO: other maps, might need restructuring

    // Send it off to the renderer to acquire resources.
    if (!renderer_create_material(m)) {
        KERROR("Failed to acquire renderer resources for material '%s'.", m->name);
        return False;
    }

    return True;
}

void destroy_material(material* m) {
    KTRACE("Destroying material '%s'...", m->name);

    // Release texture references.
    if (m->diffuse_map.texture) {
        texture_system_release(m->diffuse_map.texture->name);
    }

    // Release renderer resources.
    renderer_destroy_material(m);

    // Zero it out, invalidate IDs.
    kzero_memory(m, sizeof(material));

    m->id = INVALID_ID;
    m->generation = INVALID_ID;
    m->internal_id = INVALID_ID;
}

b8 create_default_material(material_system_state* state) {
    kzero_memory(&state->default_material, sizeof(material));

    state->default_material.id = INVALID_ID;
    state->default_material.generation = INVALID_ID;

    string_ncopy(state->default_material.name, DEFAULT_MATERIAL_NAME, MATERIAL_NAME_MAX_LENGTH);

    state->default_material.diffuse_color = vec4_one();  // white
    state->default_material.diffuse_map.use = TEXTURE_USE_MAP_DIFFUSE;
    state->default_material.diffuse_map.texture = texture_system_get_default_texture();

    if (!renderer_create_material(&state->default_material)) {
        KFATAL("Failed to acquire renderer resources for default texture. Application cannot continue.");
        return False;
    }

    return True;
}

material* material_system_get_default() {
    if (state_ptr) {
        return &state_ptr->default_material;
    }

    KFATAL("material_system_get_default called before system is initialized.");
    return 0;
}