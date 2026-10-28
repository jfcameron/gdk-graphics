// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_TEXTURE_PATCH_H
#define GDK_GRAPHICS_TEXTURE_PATCH_H

#include <gdk/graphics/texture_data.h>

#include <cstddef>
#include <optional>

namespace gdk::graphics {
    /// \brief refuses a patch of a texture aWidth by aHeight, in aFormat, that is in another format or not wholly
    /// within it. A texture with no format yet has nothing a patch can go into.
    /// \throws gdk::graphics::exception
    void check_patch(const texture_data::view &aPatch, std::size_t aOffsetX, std::size_t aOffsetY,
        std::size_t aWidth, std::size_t aHeight, std::optional<texture_data::format> aFormat);
}

#endif
