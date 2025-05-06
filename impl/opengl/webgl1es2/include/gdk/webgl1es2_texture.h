// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_TEXTURE_H
#define GDK_GFX_WEBGL1ES2_TEXTURE_H

#include <gdk/opengl.h>
#include <gdk/texture.h>
#include <gdk/texture_data.h>
#include <jfc/unique_handle.h>

#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace gdk
{
    class webgl1es2_texture final : public texture
    {
    public:
    /// \name external interface
    ///@{
    //
        virtual void update_data(const texture_data::view &) override;
        
        virtual void update_data(const texture_data::view &, const size_t offsetX, const size_t offsetY) override;
    ///@}

        /// \brief format of uncompressed image data provided to the ctor & format of the webgl1es2_texture 
        /// data within the gl
        ///
        enum class format
        {
            rgba, //!< 4channel input: red, green, blue, alpha channels
            rgb, //!< 3 channel input: red, green, blue channels only, alpha is set to 1
            luminance_alpha, //!< 2 channel input: rgb = luminance, a = alpha
            luminance, //!< 1 channel input: rgb = luminance, a = 1
            a, //!< 1 channel input: rgb=0, a=alpha
            depth_component, //!< Used to attach a texture to one of an FBO's buffers
        };

        //! The webgl1es2_texture minifying function is used whenever the pixel being 
        /// webgl1es2_textured maps to an area greater than one webgl1es2_texture element.
        enum class minification_filter
        {
            //! Returns the value of the webgl1es2_texture element that is nearest 
            /// (in Manhattan distance) to the center of the pixel being webgl1es2_textured.
            linear,
            
            //! Returns the weighted average of the four webgl1es2_texture elements that are 
            /// closest to the center of the pixel being webgl1es2_textured.
            nearest,
            
            //! Chooses the mipmap that most closely matches the size of the pixel being 
            /// webgl1es2_textured and uses the GL_NEAREST criterion (the webgl1es2_texture 
            /// element nearest to the center of the pixel) to produce a webgl1es2_texture value.
            nearest_mipmap_nearest,

            //! Chooses the mipmap that most closely matches the size of the pixel being webgl1es2_textured 
            /// and uses the GL_LINEAR criterion (a weighted average of the four webgl1es2_texture elements 
            /// that are closest to the center of the pixel) to produce a webgl1es2_texture value.
            linear_mipmap_nearest,

            //! Chooses the two mipmaps that most closely match the size of the pixel being webgl1es2_textured 
            /// and uses the GL_NEAREST criterion 
            /// (the webgl1es2_texture element nearest to the center of the pixel) to produce a 
            /// webgl1es2_texture value from each mipmap. The final webgl1es2_texture value is a weighted 
            /// average of those two values.
            nearest_mipmap_linear,

            //! Chooses the two mipmaps that most closely match the size of the pixel being webgl1es2_textured 
            /// and uses the GL_LINEAR criterion (a weighted average of the four webgl1es2_texture elements 
            /// that are closest to the center of the pixel) to produce a webgl1es2_texture value from each mipmap.  
            /// The final webgl1es2_texture value is a weighted average of those two values.  
            linear_mipmap_linear
        };

        //! The webgl1es2_texture magnification function is used when the pixel being webgl1es2_textured maps to an 
        /// area less than or equal to one webgl1es2_texture element
        enum class magnification_filter
        {
            //! Returns the value of the webgl1es2_texture element that is nearest 
            /// (in Manhattan distance) to the center of the pixel being webgl1es2_textured.
            nearest,
            
            //! Returns the weighted average of the four webgl1es2_texture elements that 
            /// are closest to the center of the pixel being webgl1es2_textured.
            linear
        };

        //! affects behaviour of sampling a texel outside of the normal range (0 - 1) along a 
        /// webgl1es2_texture dimension
        enum class wrap_mode
        {
            //! sampling above 1 will sample 1, sampling below 0 will sample 0
            clamp_to_edge, 

            //! causes the integer part of the s coordinate to be ignored; the GL uses only the fractional 
            /// part, thereby creating a repeating pattern.
            repeat,

            //! causes the s coordinate to be set to the fractional part of the webgl1es2_texture coordinate 
            /// if the integer part of s is even;  if the integer part of s is odd, then the s webgl1es2_texture 
            /// coordinate is set to 1 - frac ⁡ s , wherefrac ⁡ s represents the fractional part of s.
            mirrored_repeat
        };
    
    public:
        /// \brief returns the handle to the webgl1es2_texture in the opengl context
        [[nodiscard]] GLuint getHandle() const;

        /// \brief activates the texture in given unit and binds it to GL_TEXTURE_2D, 
        /// so that it can be manipulated by other opengl calls
        void activateAndBind(const GLint aUnit) const;

        /// \brief equality semantics
        bool operator==(const webgl1es2_texture &) const;
        /// \brief equality semantics
        bool operator!=(const webgl1es2_texture &) const;

        /// \brief move semantics
        webgl1es2_texture &operator=(webgl1es2_texture &&) = default;
        /// \brief move semantics
        webgl1es2_texture(webgl1es2_texture &&) = default;

        /// \brief creates a 2d webgl1es2_texture from decoded image data.
        /// \exception length, width of the webgl1es2_texture must be power of 2
        webgl1es2_texture(const texture_data::view &,
            const minification_filter minFilter = minification_filter::linear,
            const magnification_filter magFilter = magnification_filter::nearest,
            const wrap_mode wrapMode = wrap_mode::repeat);

        /// \brief webgl1es2_texture useful for indicating webgl1es2_texture related failure
        /// lazily instantiated.
        static const std::shared_ptr<gdk::webgl1es2_texture> GetCheckerboardOfDeath(); 

        /// \brief gets the max texture size supported by the current graphics device 
        static const GLint getMaxTextureSize();

    private:
        //! handle to the webgl1es2_texture buffer
        jfc::unique_handle<GLuint> m_Handle;

        size_t m_CurrentDataWidth;
        size_t m_CurrentDataHeight;
        GLint m_CurrentDataFormat;
    };
}

#endif
