// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_H
#define GDK_GFX_TEXTURE_H

#include <cstddef>
#include <vector>

#include <gdk/graphics_types.h>

namespace gdk
{
    /// \brief 2d color data, usually used to color the surfaces of a 3d model.
    /// more accurately: used to color fragments produced from the rasterization of primitive surfaces.
    /// ultimately textures are just a uniform type, they can be used in any number of ways and may not contribute to the color of any fragments,
	/// Example: being used as a "heightmap", to displace vertex position in the programmable vertex stage. 
    class texture
    {
    public:
        enum class data_format //!< format of data in the byte array
        {
            rgb, //!< data is a sequence of bytes representing red, green, blue, ...
            rgba, //!< data is a sequence of bytes representing red, green, blue, alpha, ...
            depth_component //!< special format used by textures attached to the depth buffer of a texture_camera
        };

        /// \brief pod struct representing a view on decoded image data 
        /// + metadata that describes its size and format
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
            
            data_format format; //!< format of the data

            //! raw image data
            /// \warn non-owning pointer
            std::byte *data;
        };

        //! replace the texture data with new data
        virtual void update_data(const image_data_2d_view &) = 0;

        //! update a section of texture data
        /// \warn formats must match
        /// \warn new data must be kept within bounds of the existing data
        virtual void update_data(const image_data_2d_view &, const size_t offsetX, const size_t offsetY) = 0;

        //! trivial destructor
        virtual ~texture() = default;

    protected:
        //! interface type cannot be instantiated
        texture() = default;
    };
}

#endif
