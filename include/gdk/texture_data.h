// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_DATA_H
#define GDK_GFX_TEXTURE_DATA_H

#include <gdk/texture.h>

#include <array>
#include <cstddef>
#include <memory>

namespace gdk::texture_data {
    using channel_type = unsigned char;
    using channel_data = std::vector<channel_type>;
    using encoded_byte = unsigned char;

    /// \brief provides a pointer to the start of decoded texture data + 
    /// metadata that contains its size, format and usage info
    /// \warn a view does not own its data.
    struct view {
        size_t width; //!< number of texels wide
        size_t height; //!< number of texels tall
        texture::format format; //!< format of the data
        const channel_type *data; //!< ptr to the start of decoded texture data
    };

    /// \brief decode PNG formatted data to channel data and a texture_data::view
    std::pair<view, std::shared_ptr<channel_data>> decode_from_png(
        const encoded_byte* aDataStart, const size_t aLength,
        const texture::format aFormat = texture::format::rgba );

    std::pair<view, std::shared_ptr<channel_data>> decode_from_png(
        const std::vector<encoded_byte>& aPNGBuffer, const texture::format aFormat = texture::format::rgba );

    template <std::size_t N>
    std::pair<view, std::shared_ptr<channel_data>> decode_from_png(
        const std::array<encoded_byte, N>& data, const texture::format aFormat = texture::format::rgba) {
        return decode_from_png(data.data(), N, aFormat);
    }
}

#endif

