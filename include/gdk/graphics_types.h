// © 2019 Joseph Cameron - All Rights Reserved
// TODO: this should be generated

#ifndef GDK_GRAPHICS_TYPES_H
#define GDK_GRAPHICS_TYPES_H

#include <gdk/intvector2.h>
#include <gdk/mat4x4.h>
#include <gdk/opengl.h>
#include <gdk/quaternion.h>
#include <gdk/vector2.h>
#include <gdk/vector3.h>
#include <gdk/vector4.h>

namespace gdk
{
    using graphics_intvector2_type = IntVector2<int>;
    using graphics_mat4x4_type = Mat4x4<float>;
    using graphics_quaternion_type = Quaternion<float>;
    using graphics_vector2_type = Vector2<float>;
    using graphics_vector3_type = Vector3<float>;
    using graphics_vector4_type = Vector4<float>;
}

#endif

