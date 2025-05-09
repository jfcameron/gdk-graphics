// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_H
#define GDK_GFX_TEXTURE_H

#include <gdk/graphics_types.h>

#include <cstddef>
#include <vector>

namespace gdk {
    namespace texture_data {
        struct view;
    }
    //class texture_data;
    //class texture_data::view;
    /// \brief 2d color data, usually used to color the surfaces of a 3d model.
    /// more accurately: used to color fragments produced from the rasterization of primitive surfaces.
    /// ultimately textures are just a uniform type, they can be used in any number of ways and may not contribute to the color of any fragments,
    /// Example: being used as a "heightmap", to displace vertex position in the programmable vertex stage. 
    class texture {
    public:
        /// \brief format of data in the byte array
        enum class format {
            rgba, //!< a sequence of 4 channels, single byte colors: red, green, blue, alpha, ...
            rgb,  //!< a sequence of 3 channels, single byte colors: red, green, blue, ...
            grey, //!< a sequence of 1 channel, single byte color: grey, ...
        };

        /// \brief behavior when sampling outside of the normalized texture range (u0-1, v0-1)
        enum class wrap_mode {
            clamped, //!< returns the closest value in range. e.g: {0, 2} OR {0, 1.1} would sample {0, 1}
            repeat,  //!< sampled values repeat. e.g: {2,2}, {3,3}, 100,100} would all sample {1,1}
            mirrored //!< every odd whole value flips the index of the sampled value. {1.25, 0.5} would sample {0.75, 0.5}
        };

        //! replace the texture data with new data
        virtual void update_data(const texture_data::view &) = 0;

        //! update a section of texture data
        /// \warn formats must match
        /// \warn new data must be kept within bounds of the existing data
        virtual void update_data(const texture_data::view &, const size_t offsetX, const size_t offsetY) = 0;

        //! trivial destructor
        virtual ~texture() = default;

    protected:
        //! interface type cannot be instantiated
        texture() = default;
    };
}

#endif

