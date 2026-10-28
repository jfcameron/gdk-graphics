// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_TEXTURE_H
#define GDK_GRAPHICS_TEXTURE_H

#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/types.h>

#include <cstddef>
#include <vector>

namespace gdk::graphics {
    /// \brief uniform color data, usually used to color the surface of a 3d model, but also used for height maps, volumetric lights etc.
    class texture {
    public:
        /// \brief format of data in the component_type array. \see texture_data::format
        using format = texture_data::format;

        /// \brief behavior when sampling outside of the normalized texture range (u0-1, v0-1)
        enum class wrap_mode {
            clamped, //!< returns the closest value in range. e.g: {0, 2} OR {0, 1.1} would sample {0, 1}
            repeat,  //!< sampled values repeat. e.g: {2,2}, {3,3}, 100,100} would all sample {1,1}
            mirrored //!< every odd whole value flips the index of the sampled value. {1.25, 0.5} would sample {0.75, 0.5}
        };

        /// \brief how a texel is chosen when a pixel does not land exactly on one
        enum class filter_mode {
            sharp,  //!< nearest: the one texel closest to the sample, so texels stay square
            smooth  //!< linear: a weighted average of the texels around the sample
        };

        //! replace the texture data with new data
        virtual void update_data(const texture_data::view &) = 0;

        /// \brief update a section of texture data
        /// \throws gdk::graphics::exception the section is in another format than the texture, or not wholly
        /// within it
        virtual void update_data(const texture_data::view &, const std::size_t offsetX, const std::size_t offsetY) = 0;

        //! trivial destructor
        virtual ~texture() = default;

    protected:
        //! interface type cannot be instantiated
        texture() = default;
    };
}

#endif

