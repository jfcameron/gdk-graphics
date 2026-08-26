// © Joseph Cameron - All Rights Reserved

#ifndef GDK_DEMO_ANIMATION_TYPES_H
#define GDK_DEMO_ANIMATION_TYPES_H

#include <gdk/math.h>

namespace gdk::graphics::animation {
    using floating_point_type = float;

    using matrix4x4_type = gdk::matrix4x4<floating_point_type>;
    using quaternion_type = gdk::quaternion<floating_point_type>;
    using vector3_type = gdk::vector3<floating_point_type>;
}

#endif
