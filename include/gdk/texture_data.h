// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_DATA_H
#define GDK_GFX_TEXTURE_DATA_H

#include <gdk/texture.h>

#include <cstddef>
#include <memory>

namespace gdk::texture_data {
    /// \brief pod struct representing a view on decoded image data 
    /// + metadata that describes its size, format and usage info
    /// \warn a view does not own its data.
    struct view {
        size_t width; //!< number of texels wide
        size_t height; //!< number of texels tall
        texture::format format; //!< format of the data

        //wrapmodes should probably be removed from image view, this information is not part of image data
        texture::wrap_mode horizontal_wrap_mode = texture::wrap_mode::repeat;
        texture::wrap_mode vertical_wrap_mode = texture::wrap_mode::repeat;

        //! raw image data
        /// \warn non-owning pointer
        std::byte *data;
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

