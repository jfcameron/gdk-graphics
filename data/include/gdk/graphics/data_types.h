// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_DATA_TYPES_H
#define GDK_GRAPHICS_DATA_TYPES_H

#include <gdk/graphics/color.h>
#include <gdk/math/matrix3x3.h>
#include <gdk/math/matrix4x4.h>
#include <gdk/math/ops.h>
#include <gdk/math/quaternion.h>
#include <gdk/math/vector2.h>
#include <gdk/math/vector3.h>
#include <gdk/math/vector4.h>

namespace gdk::graphics {
/// \name Floating point types
///@{
//
    using floating_point_type = float;

    using mat3x3_type = gdk::math::matrix3x3<floating_point_type>;
    using matrix4x4_type = gdk::math::matrix4x4<floating_point_type>;
    using quaternion_type = gdk::math::quaternion<floating_point_type>;
    using vector2_type = gdk::math::vector2<floating_point_type>;
    using vector3_type = gdk::math::vector3<floating_point_type>;
    using vector4_type = gdk::math::vector4<floating_point_type>;
///@}

/// \name Signed integral types
///@{
//
    using signed_integral_type = int;

    using intvector2_type = gdk::math::vector2<signed_integral_type>;
    using intvector3_type = gdk::math::vector3<signed_integral_type>;
    using intvector4_type = gdk::math::vector4<signed_integral_type>;
///@}
}

#endif
