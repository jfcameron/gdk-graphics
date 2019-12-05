// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_H
#define GDK_GFX_TEXTURE_H

namespace gdk
{
    /// \brief 2d data generally used to color primitive surfaces
    class texture
    {
    public:
        /// \brief pod struct for 2d texture data
        struct image_data_2d
        {
            enum class data_format //!< format of data in the byte array
            {
                rgb, //!< data is a sequence of bytes representing red, green, blue, ...
                rgba //!< data is a sequence of bytes representing red, green, blue, alpha, ...
            };

            size_t width; //!< number of texels wide
            size_t height; //!< number of texels tall
            
            data_format format;

            //! raw image data
            std::vector<std::byte> data;
        };

        virtual ~texture() = default;

    protected:
        texture() = default;
    };
}

#endif
