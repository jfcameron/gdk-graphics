// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_TYPES_H
#define GDK_GRAPHICS_TYPES_H

#include <gdk/mat4x4.h>
#include <gdk/quaternion.h>
#include <gdk/vector2.h>
#include <gdk/vector3.h>
#include <gdk/vector4.h>

namespace gdk
{
/// \name Floating point types
///@{
//
    using graphics_floating_point_type = float;
    using graphics_mat4x4_type = Mat4x4<graphics_floating_point_type>;
    using graphics_quaternion_type = Quaternion<graphics_floating_point_type>;
    using graphics_vector2_type = Vector2<graphics_floating_point_type>;
    using graphics_vector3_type = Vector3<graphics_floating_point_type>;
    using graphics_vector4_type = Vector4<graphics_floating_point_type>;
///@}

/// \name Signed integral types    
///@{
//
    using graphics_signed_integral_type = int;
    using graphics_intvector2_type = Vector2<graphics_signed_integral_type>;
///@}
}

#endif

