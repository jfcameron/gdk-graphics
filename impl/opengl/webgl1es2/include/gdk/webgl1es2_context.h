// © 2019 Joseph Cameron - All Rights Reserved

#include <gdk/graphics_context.h>

#ifndef GDK_GFX_WEBGL1ES2_CONTEXT_H
#define GDK_GFX_WEBGL1ES2_CONTEXT_H

namespace gdk
{
    ///! brief webgl1/gles2.0 context implementation
    class webgl1es2_context final : public graphics::context
    {
    public:
        webgl1es2_context() = default;

        virtual ~webgl1es2_context() override = default;
    };
}

#endif
