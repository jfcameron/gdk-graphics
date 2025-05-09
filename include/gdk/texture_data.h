// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_DATA_H
#define GDK_GFX_TEXTURE_DATA_H

#include <gdk/texture.h>

#include <cstddef>
#include <memory>

namespace gdk::texture_data {
    /// \brief provides a pointer to the start of decoded texture data + 
    /// metadata that contains its size, format and usage info
    /// \warn a view does not own its data.
    struct view {
        size_t width; //!< number of texels wide
        size_t height; //!< number of texels tall
        texture::format format; //!< format of the data
        /// \warn non-owning pointer
        std::byte *data; //!< ptr to decoded texture data
    };

    using byte_underlying_type = std::underlying_type<std::byte>::type;
    using decoded_data = std::vector<byte_underlying_type>;

    /// \brief decode PNG formatted data to raw texture data and a texture_data::view
    std::pair<view, std::shared_ptr<decoded_data>> decode_from_png(const byte_underlying_type *aDataStart, const size_t aLength,
        const texture::format aFormat = texture::format::rgba );
    std::pair<view, std::shared_ptr<decoded_data>> decode_from_png(const std::vector<byte_underlying_type> &aPNGBuffer, 
        const texture::format aFormat = texture::format::rgba);
}

#endif

