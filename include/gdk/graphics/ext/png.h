// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_EXT_PNG_H
#define GDK_GRAPHICS_EXT_PNG_H

#include <gdk/graphics/texture_data.h>

#include <cstddef>
#include <memory>
#include <span>

namespace gdk::graphics::ext {
    /// \brief decode PNG formatted data to channel data and a texture_data::view
    ///
    /// \warning the view does not own its data; the returned channel_data does, and must outlive it.
    std::pair<texture_data::view, std::shared_ptr<texture_data::channel_data>> make_from_png(
        std::span<const texture_data::encoded_byte> aPNGData,
        const texture::format aFormat = texture::format::rgba);
}

#endif
