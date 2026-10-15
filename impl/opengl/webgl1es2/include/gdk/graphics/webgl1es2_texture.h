// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_TEXTURE_H
#define GDK_GFX_WEBGL1ES2_TEXTURE_H

#include <gdk/graphics/opengl.h>
#include <gdk/graphics/texture.h>
#include <gdk/graphics/texture_data.h>
#include <jfc/unique_handle.h>

#include <array>
#include <cstddef>
#include <string>
#include <vector>
#include <gdk/graphics/profile.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>
#endif

namespace gdk::graphics {
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
        ///
        /// Only the four mipmap filters read mipmap levels, so only a texture made with one of
        /// them has levels generated, at construction and on every update.
        enum class minification_filter {
            //! Returns the weighted average of the four webgl1es2_texture elements that are 
            /// closest to the center of the pixel being webgl1es2_textured.
            linear,
            
            //! Returns the value of the webgl1es2_texture element that is nearest 
            /// (in Manhattan distance) to the center of the pixel being webgl1es2_textured.
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
            alpha,
            depth,
            luminance_alpha,
            rgb,
            rgba,
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

        /// \brief move semantics
        webgl1es2_texture &operator=(webgl1es2_texture &&) = default;
        /// \brief move semantics
        webgl1es2_texture(webgl1es2_texture &&) = default;

        /// \brief creates a 2d webgl1es2_texture from decoded image data.
        /// \exception exception the width or height is not a power of two, unless the texture is
        /// clamped on both axes and its filter reads no mipmaps, when GLES 2 and WebGL 1 take any
        /// size
        webgl1es2_texture(
            const texture_data::view &aTextureView,
            const wrap_mode aWrapModeU = wrap_mode::repeat,
            const wrap_mode aWrapModeV = wrap_mode::repeat,
            const minification_filter minFilter = minification_filter::nearest,
            const magnification_filter magFilter = magnification_filter::nearest);

        webgl1es2_texture(
            const webgl1es2_texture::format aFormat, 
            const size_t aWidthInTexels,
            const size_t aHeightInTexels,
            const texture_data::channel_type *aData,
            const wrap_mode aWrapModeU = wrap_mode::repeat,
            const wrap_mode aWrapModeV = wrap_mode::repeat,
            const minification_filter minFilter = minification_filter::nearest,
            const magnification_filter magFilter = magnification_filter::nearest);

        /// \brief a texture useful for indicating a texture related failure
        ///
        /// **Built fresh, and the caller owns it.** This was a function-local static, so every caller
        /// in the process shared one gl handle -- which is a handle belonging to whichever gl context
        /// happened to be current when the first caller asked. \see
        /// webgl1es2_shader_program::make_pink_shader_of_death
        [[nodiscard]] static std::shared_ptr<webgl1es2_texture> make_checkerboard_of_death();

        /// \brief gets the max texture size supported by the current graphics device 
        static GLint getMaxTextureSize();

        void hold_to(std::shared_ptr<const profile> pProfile);

#ifdef __EMSCRIPTEN__
        /// \brief replaces the texture's contents with a JS image source: a VideoFrame, an
        /// ImageBitmap, an image or video element, a canvas. The browser draws it in on the GPU,
        /// converting as it goes; nothing is copied through the CPU.
        ///
        /// The texture takes the source's size. As that is seldom a power of two, filtering
        /// becomes linear without mipmaps and wrapping clamped, which is what WebGL 1 takes then.
        /// \warning on the thread that owns the WebGL context, where a VideoFrame also lives
        /// \exception exception the source has no size, or is past the profile's largest texture
        void update_data(const emscripten::val &aImageSource);
#endif

#if GDK_GRAPHICS_HAS_EGL
        /// \brief an EGLImage, as EGL's own `EGLImage` type: a `void *`, so that EGL's headers are
        /// not this one's
        using egl_image_type = void *;

        /// \brief whether this context can sample an EGLImage: GL_OES_EGL_image is there, which
        /// needs the context to be EGL's (GLFW_EGL_CONTEXT_API on X11; Wayland's always is)
        [[nodiscard]] static bool can_adopt_egl_images();

        /// \brief makes the texture's contents an EGLImage: the image's memory, sampled where it
        /// is, not copied. A hardware decoder's frame, say, imported from its DMA-BUF.
        ///
        /// Size as given; no mipmaps, and clamped. The image must outlive the texture's use of it:
        /// keep it, and what it was made from, until a later frame has replaced it on screen and
        /// the GPU has finished with it. A later update_data gives the texture storage of its own
        /// again.
        /// \exception exception the context cannot adopt, or the image is null
        void adopt(egl_image_type apImage, const std::size_t aWidth, const std::size_t aHeight);
#endif

    private:
        void take_external_contents(const std::size_t aWidth, const std::size_t aHeight);

        std::shared_ptr<const profile> m_pProfile;

        wrap_mode m_WrapU;
        wrap_mode m_WrapV;

        //! whether the minification filter reads mipmap levels, so they must be kept up to date
        bool m_HasMipmaps;

        //! handle to the webgl1es2_texture buffer
        jfc::unique_handle<GLuint> m_Handle;

        size_t m_CurrentDataWidth;
        size_t m_CurrentDataHeight;
        GLint m_CurrentDataFormat;
    };
}

#endif

