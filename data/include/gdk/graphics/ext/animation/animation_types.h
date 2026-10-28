// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXT_ANIMATION_ANIMATION_TYPES_H
#define GDK_GRAPHICS_EXT_ANIMATION_ANIMATION_TYPES_H

#include <gdk/math/matrix4x4.h>
#include <gdk/math/ops.h>
#include <gdk/math/quaternion.h>
#include <gdk/math/vector3.h>

namespace gdk::graphics::animation {
    using floating_point_type = float;

    using matrix4x4_type = gdk::math::matrix4x4<floating_point_type>;
    using quaternion_type = gdk::math::quaternion<floating_point_type>;
    using vector3_type = gdk::math::vector3<floating_point_type>;
}

#endif
