// © 2019 Joseph Cameron - All Rights Reserved

#include <gdk/graphics_context.h>
#include <gdk/webgl1es2_context.h>

#include <gdk/entity_owner.h>

#include <stdexcept>

using namespace gdk;
using namespace gdk::graphics;

context::context_ptr_type context::make(const context::implementation &impl)
{
    switch (impl)
    {
        case implementation::opengl_webgl1_gles2: 
            return std::make_unique<webgl1es2_context>(webgl1es2_context()); break;

        default:
            throw std::invalid_argument("unhandled implementation");
    }
}

