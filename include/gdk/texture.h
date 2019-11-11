// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_TEXTURE_H
#define GDK_GFX_TEXTURE_H

#include <gdk/opengl.h>
#include <jfc/unique_handle.h>

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
            //!:t
            rgb,
            rgba
        };

        //! The texture minifying function is used whenever the pixel being textured maps to an area greater than one texture element.
        enum class minification_filter
        {
            //! Returns the value of the texture element that is nearest (in Manhattan distance) to the center of the pixel being textured.
            linear,
            
            //! Returns the weighted average of the four texture elements that are closest to the center of the pixel being textured.
            nearest,
            
            //! Chooses the mipmap that most closely matches the size of the pixel being textured and uses the GL_NEAREST criterion (the texture element nearest to the center of the pixel) to produce a texture value.
            nearest_mipmap_nearest,

            //!  Chooses the mipmap that most closely matches the size of the pixel being textured and uses the GL_LINEAR criterion (a weighted average of the four texture elements that are closest to the center of the pixel) to produce a texture value.
            linear_mipmap_nearest,

            //!  Chooses the two mipmaps that most closely match the size of the pixel being textured and uses the GL_NEAREST criterion (the texture element nearest to the center of the pixel) to produce a texture value from each mipmap.  The final texture value is a weighted average of those two values.
            nearest_mipmap_linear,

            //!  Chooses the two mipmaps that most closely match the size of the pixel being textured and uses the GL_LINEAR criterion (a weighted average of the four texture elements that are closest to the center of the pixel) to produce a texture value from each mipmap.  The final texture value is a weighted average of those two values.  
            linear_mipmap_linear
        };

        //! The texture magnification function is used when the pixel being textured maps to an area less than or equal to one texture element
        enum class magnification_filter
        {
            //! Returns the value of the texture element that is nearest (in Manhattan distance) to the center of the pixel being textured.
            nearest,
            
            //! Returns the weighted average of the four texture elements that are closest to the center of the pixel being textured.
            linear
        };

        //! affects behaviour of sampling a texel outside of the normal range (0 - 1) along a texture dimension
        enum class wrap_mode
        {
            //! sampling above 1 will sample 1, sampling below 0 will sample 0
            clamp_to_edge,

            //! causes the integer part of the s coordinate to be ignored; the GL uses only the fractional part, thereby creating a repeating pattern.
            repeat,

            //! causes the s coordinate to be set to the fractional part of the texture coordinate if the integer part of s is even; if the integer part of s is odd, then the s texture coordinate is set to 1 - frac ⁡ s , where frac ⁡ s represents the fractional part of s.
            mirrored_repeat
        };

    private:
        jfc::unique_handle<GLuint> m_Handle;
    
    public:
        /// \brief returns the handle to the texture in the opengl context
        GLuint getHandle() const;

        /// \brief move semantics
        texture &operator=(texture &&) = default;
        /// \brief move semantics
        texture(texture &&) = default;
       
        /// \brief creates a texture from decoded image data.
        /// \exception length, width of the texture must be power of 2
        texture(GLubyte *const pDecodedImageData, const long width, 
            const long height, 
            const format = format::rgba,
            const minification_filter minFilter = minification_filter::linear,
            const magnification_filter magFilter = magnification_filter::nearest,
            const wrap_mode wrapMode = wrap_mode::repeat);

        /// \brief texture useful for indicating texture related failure
        /// lazily instantiated.
        static const std::shared_ptr<gdk::texture> GetCheckerboardOfDeath(); 

        /// \brief constructs a texture from png rgba32 file data
        /// \throws invalid_argument if the image could not be decoded (badly formed or not a PNG with component format RGBA32, etc.)
        static texture make_from_png_rgba32(const std::vector<GLubyte> aRGBA32PNGData);
    };
}

#endif
