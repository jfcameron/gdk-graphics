// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_DATA_TYPES_H
#define GDK_GRAPHICS_DATA_TYPES_H

#include <gdk/graphics/color.h>
#include <gdk/math.h>

namespace gdk::graphics {
/// \name Floating point types
///@{
//
    using floating_point_type = float;

    using mat3x3_type = matrix3x3<floating_point_type>;
    using matrix4x4_type = matrix4x4<floating_point_type>;
    using quaternion_type = quaternion<floating_point_type>;
    using vector2_type = vector2<floating_point_type>;
    using vector3_type = vector3<floating_point_type>;
    using vector4_type = vector4<floating_point_type>;
///@}

/// \name Signed integral types
///@{
//
    using signed_integral_type = int;

    using intvector2_type = vector2<signed_integral_type>;
    using intvector3_type = vector3<signed_integral_type>;
    using intvector4_type = vector4<signed_integral_type>;
///@}
}

#endif
