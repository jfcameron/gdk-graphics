// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_DATA_H
#define GDK_GFX_TEXTURE_DATA_H

#include <cstddef>
#include <vector>

/// \file
/// \brief what the core takes: decoded channel data and the metadata describing it
namespace gdk::graphics::texture_data {
    /// \brief format of data in the component_type array
    enum class format {
        grey, //!< a sequence of 1 channel, single byte color: grey, ...
        rg,   //!< a sequence of 2 channels, single byte colors: red, green, ...
        rgb,  //!< a sequence of 3 channels, single byte colors: red, green, blue, ...
        rgba, //!< a sequence of 4 channels, single byte colors: red, green, blue, alpha, ...
    };

    using channel_type = unsigned char;
    using channel_data = std::vector<channel_type>;
    using encoded_byte = unsigned char;

    /// \brief provides a pointer a contiguous list of channel data representing 2D texture
    /// metadata that contains its size, format and usage info
    /// \warning a view does not own its data.
    struct view {
        size_t width;               //!< number of texels wide
        size_t height;              //!< number of texels tall
        texture_data::format format;     //!< format of the data
        const channel_type *data;   //!< ptr to the start of decoded texture data
    };
}

#endif

