// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXT_FONT_TEXTURE_H
#define GDK_GRAPHICS_EXT_FONT_TEXTURE_H

#include <gdk/graphics/context.h>
#include <gdk/graphics/ext/font.h>
#include <gdk/graphics/texture.h>
#include <gdk/graphics/texture_data.h>

#include <cstddef>
#include <memory>

namespace gdk::graphics::ext {
    /// \brief a font's atlas as a texture
    class font_texture final {
    public:
        /// \brief brings the texture up to the atlas: what has changed since the last upload is
        /// patched in, or the texture made again if the atlas has grown or been cleared since.
        /// Nothing is sent when the atlas hasn't changed.
        void upload();

        /// \brief the texture, null before the first upload
        [[nodiscard]] texture_ptr_type texture() const;

        /// \brief the atlas generation the texture holds
        [[nodiscard]] std::size_t uploaded_atlas_generation() const;

        /// \brief the font whose atlas this is
        [[nodiscard]] const std::shared_ptr<font> &typeface() const;

        font_texture(context_ptr_type aContext, std::shared_ptr<font> aFont);

    private:
        context_ptr_type m_pContext;
        std::shared_ptr<font> m_pFont;
        texture_ptr_type m_pTexture;
        std::size_t m_UploadedAtlasGeneration = 0;
        texture_data::channel_data m_Rgba;
    };
}

#endif
