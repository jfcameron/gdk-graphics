// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_IMAGE_DATA_H
#define GDK_GFX_IMAGE_DATA_H

#include <gdk/texture.h>
#include <cstddef>
#include <memory>

namespace gdk
{
    /// \brief pod struct representing a view on decoded image data 
    /// + metadata that describes its size, format and usage info
    /// \warn a view does not own its data.
    ///
    /// the purpose of this type is in being used to construct a texture object.
    /// \warn the user must ensure the data is not cleaned up until sometime after it has been used. 
    /// \note: it is valid to provide width + height and a nullptr for data, 
    /// the context will an uninitialized texture buffer of the specified size and format.
    struct image_data_2d_view 
    {
        size_t width; //!< number of texels wide
        size_t height; //!< number of texels tall
        texture::data_format format; //!< format of the data

        //wrapmodes should probably be removed from image view, this information is not part of image data
        texture::wrap_mode horizontal_wrap_mode = texture::wrap_mode::repeat;
        texture::wrap_mode vertical_wrap_mode = texture::wrap_mode::repeat;

        //! raw image data
        /// \warn non-owning pointer
        std::byte *data;
    };

    //TODO: move this namespace to ext, these functions are convenient but tie the user to a specific png decoding implementation. png decoding is not a core feature of this library.
    namespace decode_image
    {
        using byte_type = std::underlying_type<std::byte>::type;
        using decoded_data = std::vector<byte_type>;

        /// \brief contains decoded bitmap data and a view to it, which describes its contents and size and usage
        using image_data_and_view_2d_pair = std::pair<image_data_2d_view, std::shared_ptr<decoded_data>>;

        /// \brief decodes a buffer containing PNG encoded data of format RGBA32bit, produces the decoded data buffer and a view to it
        /// first: view, second: decoded data
        /// e.g: auto [image_view, image_data] = image_data_2d_view::decode_png_rgba32bit(png_data);
        image_data_and_view_2d_pair decode_png_rgba32bit(const byte_type *aDataStart, size_t aLength);
        
        image_data_and_view_2d_pair decode_png_rgba32bit(const std::vector<byte_type> &aPNGBuffer);
    }
}

#endif

