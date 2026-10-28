// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXT_PNG_H
#define GDK_GRAPHICS_EXT_PNG_H

#include <gdk/graphics/texture_data.h>

#include <cstddef>
#include <memory>
#include <span>
#include <utility>

namespace gdk::graphics::ext {
    //! the widest and tallest png make_from_png decodes
    inline constexpr std::size_t MOST_PNG_EDGE = 8192;

    /// \brief decode PNG formatted data to channel data and a texture_data::view, in aFormat
    ///
    /// PNG alone: no other image format is decoded. The size is read before anything is decoded, so a small file
    /// claiming a huge image is refused without the memory being asked for.
    /// \warning the view does not own its data; the returned channel_data does, and must outlive it.
    /// \throws gdk::graphics::exception not a PNG, damaged, or past MOST_PNG_EDGE a side
    [[nodiscard]] std::pair<texture_data::view, std::shared_ptr<texture_data::channel_data>> make_from_png(
        std::span<const texture_data::encoded_byte> aPNGData,
        const texture_data::format aFormat = texture_data::format::rgba);
}

#endif
