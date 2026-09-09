// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_DATA_H
#define GDK_GFX_TEXTURE_DATA_H

#include <gdk/graphics/texture.h>

#include <cstddef>
#include <vector>

/// \file
/// \brief what the core takes: decoded channel data and the metadata describing it
namespace gdk::graphics::texture_data {
    using channel_type = unsigned char;
    using channel_data = std::vector<channel_type>;
    using encoded_byte = unsigned char;

    /// \brief provides a pointer a contiguous list of channel data representing 2D texture
    /// metadata that contains its size, format and usage info
    /// \warning a view does not own its data.
    struct view {
        size_t width;               //!< number of texels wide
        size_t height;              //!< number of texels tall
        texture::format format;     //!< format of the data
        const channel_type *data;   //!< ptr to the start of decoded texture data
    };

    /*/// \brief provides a pointer a contiguous list of channel data representing 3D texture
    /// metadata that contains its size, format and usage info
    /// \warning a view does not own its data.
    struct view3d {
        size_t width; //!< number of texels wide
        size_t height; //!< number of texels tall
        size_t length; //!< number of texels long 
        texture::format format; //!< format of the data
        const channel_type *data; //!< ptr to the start of decoded texture data
    };*/

}

#endif

