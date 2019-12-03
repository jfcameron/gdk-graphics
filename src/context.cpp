// © 2019 Joseph Cameron - All Rights Reserved

#include <gdk/context.h>

#include <stdexcept>

using namespace gdk;
using namespace gdk::graphics;

context::context_ptr_type context::make(const context::implementation &impl)
{
    switch (impl)
    {
        case implementation::opengl_webgl1_gles2: 
        {
            return nullptr; //TODO  emit appropriate type
        } break;
    }

    throw std::invalid_argument("unhandled implementation");
}

