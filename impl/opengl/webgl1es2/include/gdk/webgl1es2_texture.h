// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_TEXTURE_H
#define GDK_GFX_WEBGL1ES2_TEXTURE_H

#include <gdk/opengl.h>
#include <gdk/texture.h>
#include <jfc/unique_handle.h>

#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace gdk
{
    /// \brief a texture generally represents the color of a surface
    class webgl1es2_texture final : public texture
    {
    public:
        /// \brief format of uncompressed image data provided to the ctor & format of the webgl1es2_texture data within the gl
        enum class format
        {
            rgb, //!< red, green, blue channels only
            rgba //!< red, green, blue, alpha channels
        };

        //! The webgl1es2_texture minifying function is used whenever the pixel being webgl1es2_textured maps to an area greater than one 
        /// webgl1es2_texture element.
        enum class minification_filter
        {
            //! Returns the value of the webgl1es2_texture element that is nearest (in Manhattan distance) to the 
            /// center of the pixel being webgl1es2_textured.
            linear,
            
            //! Returns the weighted average of the four webgl1es2_texture elements that are closest to the center of 
            /// the pixel being webgl1es2_textured.
            nearest,
            
            //! Chooses the mipmap that most closely matches the size of the pixel being webgl1es2_textured and uses the 
            /// GL_NEAREST criterion (the webgl1es2_texture element nearest to the center of the pixel) to produce a webgl1es2_texture value.
            nearest_mipmap_nearest,

            //! Chooses the mipmap that most closely matches the size of the pixel being webgl1es2_textured and uses the GL_LINEAR 
            /// criterion (a weighted average of the four webgl1es2_texture elements that are closest to the center of the pixel) 
            /// to produce a webgl1es2_texture value.
            linear_mipmap_nearest,

            //! Chooses the two mipmaps that most closely match the size of the pixel being webgl1es2_textured and uses the GL_NEAREST 
            /// criterion (the webgl1es2_texture element nearest to the center of the pixel) to produce a webgl1es2_texture value from each mipmap.  
            /// The final webgl1es2_texture value is a weighted average of those two values.
            nearest_mipmap_linear,

            //! Chooses the two mipmaps that most closely match the size of the pixel being webgl1es2_textured and uses the GL_LINEAR 
            /// criterion (a weighted average of the four webgl1es2_texture elements that are closest to the center of the pixel) 
            /// to produce a webgl1es2_texture value from each mipmap.  The final webgl1es2_texture value is a weighted average of those two values.  
            linear_mipmap_linear
        };

        //! The webgl1es2_texture magnification function is used when the pixel being webgl1es2_textured maps to an area less than or equal to one 
        /// webgl1es2_texture element
        enum class magnification_filter
        {
            //! Returns the value of the webgl1es2_texture element that is nearest (in Manhattan distance) to the center of the pixel 
            /// being webgl1es2_textured.
            nearest,
            
            //! Returns the weighted average of the four webgl1es2_texture elements that are closest to the center of the pixel being 
            /// webgl1es2_textured.
            linear
        };

        //! affects behaviour of sampling a texel outside of the normal range (0 - 1) along a webgl1es2_texture dimension
        enum class wrap_mode
        {
            clamp_to_edge, //!< sampling above 1 will sample 1, sampling below 0 will sample 0

            //! causes the integer part of the s coordinate to be ignored; the GL uses only the fractional part, thereby 
            /// creating a repeating pattern.
            repeat,

            //! causes the s coordinate to be set to the fractional part of the webgl1es2_texture coordinate if the integer part 
            /// of s is even;  if the integer part of s is odd, then the s webgl1es2_texture coordinate is set to 1 - frac ⁡ s , where
            /// frac ⁡ s represents the fractional part of s.
            mirrored_repeat
        };

        //! GLES2.0/Web1.0 defines 2 bind targets
        enum class bind_target
        {
            texture_2d, //!< 2 dimensional image, generally used for coloring surfaces
            cube_map //!< 6 2d images, generally used for coloring a skybox, reflection map etc.
        };

        //! pod struct representing decoded image data + metadata that describes a 2d image
        /// primary purpose is used in construction of a texture object.
        /// \warn a view does not own its data. The user must ensure the data observed by the view
        /// is cleaned up sometime after it has been used. By used, generally I mean sometime after it has been
        /// passed to a texture ctor
        struct webgl1es2_texture_2d_data_view_type
        {
            size_t width; //!< pix width of data
            size_t height; //!< pix height of data

            webgl1es2_texture::format format; //!< format of the image data.

            //! decoded image data
            /// \warning non-owning pointer
            std::byte *data; 
        };

        //! description of a cubic image
        //TODO make use of this
        /// \warn a view does not own its data.
        struct webgl1es2_texture_cubic_data_view_type
        {
            size_t width; //!< width of a single image in the cube (all must be same)
            size_t height; //!< height of a single image in the cube (all must be same)

            webgl1es2_texture::format format; //!< format of the image data. must be same for all surfaces

            //! image data for the 6 surfaces of the cube
            /// \warwning non-owning pointer
            std::array<std::byte *, 6> data; 
        };

    private:
        //! the target type. Cannot be changed after construction. Decides whether the webgl1es2_texture data is 2d or cubic
        GLenum m_BindTarget;

        //! handle to the webgl1es2_texture buffer
        jfc::unique_handle<GLuint> m_Handle;
    
    public:
        /// \brief returns the handle to the webgl1es2_texture in the opengl context
        //TODO consider abstracting this away. Currently only used by Shader, to bind the webgl1es2_texture. Exposing the raw handle
        // like this makes it trivial to put a webgl1es2_texture into an unintended state for example by getting the handle and deleting it.)
        GLuint getHandle() const;

        /// \brief equality semantics
        bool operator==(const webgl1es2_texture &) const;
        /// \brief equality semantics
        bool operator!=(const webgl1es2_texture &) const;

        /// \brief move semantics
        webgl1es2_texture &operator=(webgl1es2_texture &&) = default;
        /// \brief move semantics
        webgl1es2_texture(webgl1es2_texture &&) = default;

        //TODO: consider supporting copy semantics. How useful are they to a user? they are expensive. need to read about usecases.

        /// \brief creates a 2d webgl1es2_texture from decoded image data.
        /// \exception length, width of the webgl1es2_texture must be power of 2
        webgl1es2_texture(
            const webgl1es2_texture_2d_data_view_type &textureData2d,
            //GLubyte *const pDecodedImageData, const long width, //TODO why is image data all separate params? 
            //const long height,                                      // ptr, w, h, format should be a 2d_image_data pod struct 
            //const format = format::rgba,
            const minification_filter minFilter = minification_filter::linear,
            const magnification_filter magFilter = magnification_filter::nearest,
            const wrap_mode wrapMode = wrap_mode::repeat);

        //TODO cubic ctor
        /// \brief creates a cubic webgl1es2_texture from decoded image data.
        /// \exception dimensions must be power of 2
        /// webgl1es2_texture(const cubic_image_data &data, min, max, wrap)
        /// struct cubic_image_data{ array<6, vec<byte>> data, w, h, format}

        //! blarblarbalr
        static const std::shared_ptr<gdk::webgl1es2_texture> GetTestTexture();

        /// \brief webgl1es2_texture useful for indicating webgl1es2_texture related failure
        /// lazily instantiated.
        static const std::shared_ptr<gdk::webgl1es2_texture> GetCheckerboardOfDeath(); 

        /// \brief constructs a 2d webgl1es2_texture from png rgba32 encoded file data
        /// \throws invalid_argument if the image could not be decoded 
        /// (badly formed or not a PNG with component format RGBA32.)
        static webgl1es2_texture make_from_png_rgba32(const std::vector<GLubyte> aRGBA32PNGData);
    };
}

#endif
