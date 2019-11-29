// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_H
#define GDK_GFX_TEXTURE_H

#include <gdk/opengl.h>
#include <jfc/unique_handle.h>

#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace gdk
{
    /// \brief a texture generally represents the color of a surface
    class texture final
    {
    public:
        /// \brief format of uncompressed image data provided to the ctor & format of the texture data within the gl
        enum class format
        {
            rgb, //!< red, green, blue channels only
            rgba //!< red, green, blue, alpha channels
        };

        //! The texture minifying function is used whenever the pixel being textured maps to an area greater than one 
        /// texture element.
        enum class minification_filter
        {
            //! Returns the value of the texture element that is nearest (in Manhattan distance) to the 
            /// center of the pixel being textured.
            linear,
            
            //! Returns the weighted average of the four texture elements that are closest to the center of 
            /// the pixel being textured.
            nearest,
            
            //! Chooses the mipmap that most closely matches the size of the pixel being textured and uses the 
            /// GL_NEAREST criterion (the texture element nearest to the center of the pixel) to produce a texture value.
            nearest_mipmap_nearest,

            //! Chooses the mipmap that most closely matches the size of the pixel being textured and uses the GL_LINEAR 
            /// criterion (a weighted average of the four texture elements that are closest to the center of the pixel) 
            /// to produce a texture value.
            linear_mipmap_nearest,

            //! Chooses the two mipmaps that most closely match the size of the pixel being textured and uses the GL_NEAREST 
            /// criterion (the texture element nearest to the center of the pixel) to produce a texture value from each mipmap.  
            /// The final texture value is a weighted average of those two values.
            nearest_mipmap_linear,

            //! Chooses the two mipmaps that most closely match the size of the pixel being textured and uses the GL_LINEAR 
            /// criterion (a weighted average of the four texture elements that are closest to the center of the pixel) 
            /// to produce a texture value from each mipmap.  The final texture value is a weighted average of those two values.  
            linear_mipmap_linear
        };

        //! The texture magnification function is used when the pixel being textured maps to an area less than or equal to one 
        /// texture element
        enum class magnification_filter
        {
            //! Returns the value of the texture element that is nearest (in Manhattan distance) to the center of the pixel 
            /// being textured.
            nearest,
            
            //! Returns the weighted average of the four texture elements that are closest to the center of the pixel being 
            /// textured.
            linear
        };

        //! affects behaviour of sampling a texel outside of the normal range (0 - 1) along a texture dimension
        enum class wrap_mode
        {
            clamp_to_edge, //!< sampling above 1 will sample 1, sampling below 0 will sample 0

            //! causes the integer part of the s coordinate to be ignored; the GL uses only the fractional part, thereby 
            /// creating a repeating pattern.
            repeat,

            //! causes the s coordinate to be set to the fractional part of the texture coordinate if the integer part 
            /// of s is even;  if the integer part of s is odd, then the s texture coordinate is set to 1 - frac ⁡ s , where
            /// frac ⁡ s represents the fractional part of s.
            mirrored_repeat
        };

        //! GLES2.0/Web1.0 defines 2 bind targets
        enum class bind_target
        {
            texture_2d, //!< 2 dimensional image, generally used for coloring surfaces
            cube_map //!< 6 2d images, generally used for coloring a skybox, reflection map etc.
        };

        //! decoded image data + metadata that describes a 2d image
        //TODO make use of this
        struct texture_2d_data_type
        {
            long width; //!< pix width of data
            long height; //!< pix height of data

            texture::format format; //!< format of the image data.

            std::vector<std::byte> data; //!< decoded image data
        };

        //! description of a cubic image
        //TODO make use of this
        struct texture_cubic_data_type
        {
            long width; //!< width of a single image in the cube (all must be same)
            long height; //!< height of a single image in the cube (all must be same)

            texture::format format; //!< format of the image data. must be same for all surfaces

            std::array<std::byte, 6> data; //!< image data for the 6 surfaces of the cube
        };

    private:
        //! the target type. Cannot be changed after construction. Decides whether the texture data is 2d or cubic
        GLenum m_BindTarget;

        //! handle to the texture buffer
        jfc::unique_handle<GLuint> m_Handle;
    
    public:
        /// \brief returns the handle to the texture in the opengl context
        //TODO consider abstracting this away. Currently only used by Shader, to bind the texture. Exposing the raw handle
        // like this makes it trivial to put a texture into an unintended state for example by getting the handle and deleting it.)
        GLuint getHandle() const;

        /// \brief equality semantics
        bool operator==(const texture &) const;
        /// \brief equality semantics
        bool operator!=(const texture &) const;

        /// \brief move semantics
        texture &operator=(texture &&) = default;
        /// \brief move semantics
        texture(texture &&) = default;

        //TODO: consider supporting copy semantics. How useful are they to a user? they are expensive. need to read about usecases.

        /// \brief creates a 2d texture from decoded image data.
        /// \exception length, width of the texture must be power of 2
        texture(GLubyte *const pDecodedImageData, const long width, //TODO why is image data all separate params? 
            const long height,                                      // ptr, w, h, format should be a 2d_image_data pod struct 
            const format = format::rgba,
            const minification_filter minFilter = minification_filter::linear,
            const magnification_filter magFilter = magnification_filter::nearest,
            const wrap_mode wrapMode = wrap_mode::repeat);

        //TODO cubic ctor
        /// \brief creates a cubic texture from decoded image data.
        /// \exception dimensions must be power of 2
        /// texture(const cubic_image_data &data, min, max, wrap)
        /// struct cubic_image_data{ array<6, vec<byte>> data, w, h, format}

        /// \brief texture useful for indicating texture related failure
        /// lazily instantiated.
        static const std::shared_ptr<gdk::texture> GetCheckerboardOfDeath(); 

        /// \brief constructs a 2d texture from png rgba32 encoded file data
        /// \throws invalid_argument if the image could not be decoded 
        /// (badly formed or not a PNG with component format RGBA32.)
        static texture make_from_png_rgba32(const std::vector<GLubyte> aRGBA32PNGData);
    };
}

#endif
