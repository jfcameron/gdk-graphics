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
    using graphics_camera_ptr_type = std::shared_ptr<screen_camera>;
    using graphics_context_ptr_type = std::shared_ptr<graphics::context>;
    using graphics_entity_ptr_type = std::shared_ptr<entity>;
    using graphics_material_ptr_type = std::shared_ptr<material>;
    using graphics_model_ptr_type = std::shared_ptr<model>;
    using graphics_scene_ptr_type = std::shared_ptr<scene>;
    using graphics_shader_ptr_type = std::shared_ptr<shader_program>;
    using graphics_texture_camera_ptr_type = std::shared_ptr<texture_camera>;
    using graphics_texture_ptr_type = std::shared_ptr<texture>;

    using const_graphics_camera_ptr_type = std::shared_ptr<const screen_camera>;
    using const_graphics_context_ptr_type = std::shared_ptr<const graphics::context>;
    using const_graphics_entity_ptr_type = std::shared_ptr<const entity>;
    using const_graphics_material_ptr_type = std::shared_ptr<const material>;
    using const_graphics_model_ptr_type = std::shared_ptr<const model>;
    using const_graphics_scene_ptr_type = std::shared_ptr<const scene>;
    using const_graphics_shader_ptr_type = std::shared_ptr<const shader_program>;
    using const_graphics_texture_camera_ptr_type = std::shared_ptr<const texture_camera>;
    using const_graphics_texture_ptr_type = std::shared_ptr<const texture>;
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
    using graphics_intvector4_type = vector4<graphics_signed_integral_type>;
///@}
}

#endif

