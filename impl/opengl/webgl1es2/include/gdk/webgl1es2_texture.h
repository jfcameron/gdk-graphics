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

namespace gdk {
    namespace texture_data { struct view; }

    class webgl1es2_texture final : public texture {
    public:
    /// \name external interface
    ///@{
    //
        virtual void update_data(const texture_data::view &) override;
        virtual void update_data(const texture_data::view &, const size_t offsetX, const size_t offsetY) override;
    ///@}

        //! The webgl1es2_texture minifying function is used whenever the pixel being 
        /// webgl1es2_textured maps to an area greater than one webgl1es2_texture element.
        enum class minification_filter {
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
        enum class magnification_filter {
            //! Returns the value of the webgl1es2_texture element that is nearest 
            /// (in Manhattan distance) to the center of the pixel being webgl1es2_textured.
            nearest,
            
            //! Returns the weighted average of the four webgl1es2_texture elements that 
            /// are closest to the center of the pixel being webgl1es2_textured.
            linear
        };

        enum class format {
            rgba,
            rgb,
            alpha,
            depth
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
        webgl1es2_texture(
            const texture_data::view &aTextureView,
            const wrap_mode aWrapModeU = wrap_mode::repeat,
            const wrap_mode aWrapModeV = wrap_mode::repeat,
            const minification_filter minFilter = minification_filter::linear,
            const magnification_filter magFilter = magnification_filter::nearest);

        webgl1es2_texture(
            const webgl1es2_texture::format aFormat, 
            const size_t aWidthInTexels,
            const size_t aHeightInTexels,
            texture_data::channel_type *aData,
            const wrap_mode aWrapModeU = wrap_mode::repeat,
            const wrap_mode aWrapModeV = wrap_mode::repeat,
            const minification_filter minFilter = minification_filter::linear,
            const magnification_filter magFilter = magnification_filter::nearest);

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

