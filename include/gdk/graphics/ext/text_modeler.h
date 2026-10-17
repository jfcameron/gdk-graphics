// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_EXT_TEXT_MODELER_H
#define GDK_GRAPHICS_EXT_TEXT_MODELER_H

#include <gdk/graphics/context.h>
#include <gdk/graphics/ext/batch_modeler.h>
#include <gdk/graphics/ext/font.h>
#include <gdk/graphics/ext/font_texture.h>
#include <gdk/graphics/material.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/types.h>

#include <cstddef>
#include <memory>
#include <string_view>

namespace gdk::graphics::ext {
    /// \brief lays a string out into a single model, using a font's glyphs.
    class text_modeler final {
    public:
        //! get the model, contains the uploaded geometry for the text
        [[nodiscard]] const_model_ptr_type model() const;

        //! get the material, used to render the model
        [[nodiscard]] const_material_ptr_type material() const;

        /// \brief lay out new text, replacing whatever was there.
        ///
        /// \param aUTF8 the text. Invalid UTF-8 is rendered as the replacement character rather
        /// than throwing, since text often arrives from outside the program.
        ///
        /// Newline returns the pen to the left and moves it down one line. Tab advances four
        /// spaces. Glyphs not yet in the font's atlas are rasterised as they are met.
        void set_text(std::string_view aUTF8);

        /// \brief upload the geometry, and the atlas texture if it has changed since last time
        void upload();

        /// \brief the font atlas generation of the texture this modeler draws from
        [[nodiscard]] size_t uploaded_atlas_generation() const;

        /// \brief how many glyphs in the last set_text could not be rasterised, and were drawn as
        /// the font's fallback instead.
        [[nodiscard]] size_t fallback_glyph_count() const;

        /// \brief the size of the text laid out by the last set_text, in model units
        [[nodiscard]] vector2_type size() const;

        /// \brief builds a modeler
        ///
        /// \param aScale model units per pixel. The default makes one line of text one unit tall,
        /// so a string's size does not depend on the pixel height the font was rasterised at.
        text_modeler(
            context_ptr_type aContext,
            std::shared_ptr<font> aFont,
            material_ptr_type aMaterial = {},
            float aScale = 0.f
        );

        /// \brief builds a modeler that draws from a texture of the font's atlas it shares with
        /// others
        ///
        /// \param aFontTexture the font, as its atlas's texture
        text_modeler(
            context_ptr_type aContext,
            std::shared_ptr<font_texture> aFontTexture,
            material_ptr_type aMaterial = {},
            float aScale = 0.f
        );

    private:
        std::shared_ptr<font> m_pFont;

        context_ptr_type m_pContext;

        std::shared_ptr<font_texture> m_pFontTexture;

        //! the texture the material has
        texture_ptr_type m_pBoundTexture;

        material_ptr_type m_pMaterial;

        batch_modeler m_BatchModeler;

        size_t m_FallbackGlyphCount = 0;

        float m_Scale = 0;

        vector2_type m_Size = {0, 0};
    };
}

#endif
