// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_TYPES_H
#define GDK_GRAPHICS_TYPES_H

#include <gdk/graphics/data_types.h>

#include <memory>

namespace gdk::graphics {
    class entity;
    class material;
    class model;
    class scene;
    class screen_camera;
    class shader_program;
    class texture;
    class texture_camera;

    class context;

/// \name Pointer types
///@{
//
    using camera_ptr_type = std::shared_ptr<screen_camera>;
    using context_ptr_type = std::shared_ptr<graphics::context>;
    using entity_ptr_type = std::shared_ptr<entity>;
    using material_ptr_type = std::shared_ptr<material>;
    using model_ptr_type = std::shared_ptr<model>;
    using scene_ptr_type = std::shared_ptr<scene>;
    using shader_ptr_type = std::shared_ptr<shader_program>;
    using texture_camera_ptr_type = std::shared_ptr<texture_camera>;
    using texture_ptr_type = std::shared_ptr<texture>;

    using const_camera_ptr_type = std::shared_ptr<const screen_camera>;
    using const_context_ptr_type = std::shared_ptr<const graphics::context>;
    using const_entity_ptr_type = std::shared_ptr<const entity>;
    using const_material_ptr_type = std::shared_ptr<const material>;
    using const_model_ptr_type = std::shared_ptr<const model>;
    using const_scene_ptr_type = std::shared_ptr<const scene>;
    using const_shader_ptr_type = std::shared_ptr<const shader_program>;
    using const_texture_camera_ptr_type = std::shared_ptr<const texture_camera>;
    using const_texture_ptr_type = std::shared_ptr<const texture>;
///@}
}

#endif
