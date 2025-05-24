// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_TYPES_H
#define GDK_GRAPHICS_TYPES_H

#include <gdk/matrix4x4.h>
#include <gdk/quaternion.h>
#include <gdk/vector2.h>
#include <gdk/vector3.h>
#include <gdk/vector4.h>

#include <memory>

namespace gdk {
    class entity;
    class material;
    class model;
    class scene;
    class screen_camera;
    class shader_program;
    class texture;
    class texture_camera;

    namespace graphics {
        class context;
    }

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

/// \name Floating point types
///@{
//
    using graphics_floating_point_type = float;

    using graphics_matrix4x4_type = matrix4x4<graphics_floating_point_type>;
    using graphics_quaternion_type = quaternion<graphics_floating_point_type>;
    using graphics_vector2_type = vector2<graphics_floating_point_type>;
    using graphics_vector3_type = vector3<graphics_floating_point_type>;
    using graphics_vector4_type = vector4<graphics_floating_point_type>;
///@}

/// \name Signed integral types    
///@{
//
    using graphics_signed_integral_type = int;

    using graphics_intvector2_type = vector2<graphics_signed_integral_type>;
    using graphics_intvector3_type = vector3<graphics_signed_integral_type>;
///@}
}

#endif

