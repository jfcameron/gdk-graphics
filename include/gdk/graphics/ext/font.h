// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_EXT_FONT_H
#define GDK_GRAPHICS_EXT_FONT_H

#include <gdk/graphics/texture_data.h>

#include <cstddef>
#include <memory>
#include <optional>
#include <span>

namespace gdk::graphics::ext {
    enum class font_sizing {
        ascent_to_descent,
        em
    };

    /// \brief everything about a \ref font that has a default
    struct font_settings final {
        /// \brief which font of a collection (.ttc, .otc) to use
        size_t face = 0;

        //! what the constructor's pixel size measures.
        font_sizing sizing = font_sizing::ascent_to_descent;

        //! the atlas starts square with this edge length
        size_t atlas_edge = 512;

        /// \brief how far the atlas may grow
        size_t max_atlas_edge = 2048;
    };

    /// \brief a truetype font
    ///
    /// Glyphs are rasterised the first time they are asked for and packed into a single atlas
    /// texture, rather than a fixed set dont up front. 
    ///
    /// The atlas is a single channel (texture::format::grey) coverage map. Upload it to a texture
    /// with atlas(), and re-upload when atlas_generation() differs from the one last uploaded.
    //TODO: possibly split into "static_font" (preraster, fine for latin) and "dynamic_font" for CJK etc.?
    //TODO: throwing when the atlas is full makes sense, but it will be a source of throws in predictable use cases. eg a text editor 
    class font final {
    public:
        struct glyph {
            float s0 = 0; //!< atlas texture coordinates
            float t0 = 0; //!< atlas texture coordinates
            float s1 = 0; //!< atlas texture coordinates
            float t1 = 0; //!< atlas texture coordinates

            float left = 0;    //!< pen to the quad's left edge, in pixels
            float top = 0;     //!< baseline to the quad's top edge, in pixels, up is positive
            float width = 0;   //!< quad width in pixels
            float height = 0;  //!< quad height in pixels
            float advance = 0; //!< how far the pen moves after this glyph, in pixels
        };

        /// \brief the glyph for a codepoint or nullptr when the atlas has no room left for it.
        ///
        /// \warning the pointer is invalidated by a later call that rasterises something new
        [[nodiscard]] const glyph *try_get(char32_t aCodepoint);

        /// \brief the font's .notdef glyph, which most fonts draw as a visible box.
        ///
        /// Always present: it is rasterised at construction and put back after every growth and
        /// every clear, so it is there to draw even when nothing else will fit. A caller that wants
        /// tofu where try_get() came back empty asks for it, rather than being handed it silently.
        [[nodiscard]] const glyph &fallback() const;

        /// \brief empties the atlas back to the size it was constructed with.
        ///
        /// \warning every texture coordinate previously handed out becomes stale, exactly as when
        /// the atlas grows. Safe between frames, not part way through laying text out.
        void clear();

        /// \brief additional spacing between two adjacent codepoints in pixels.
        [[nodiscard]] float kerning(char32_t aLeft, char32_t aRight) const;

        /// \brief view of the atlas texture
        [[nodiscard]] texture_data::view atlas() const;

        /// \brief a rectangle of the atlas, in texels
        struct atlas_region {
            size_t x = 0, y = 0, width = 0, height = 0;
        };

        /// \brief which part of the atlas has changed since a given generation.
        [[nodiscard]] std::optional<atlas_region> atlas_changed_since(size_t aGeneration) const;

        /// \brief a number that changes whenever the atlas does.
        [[nodiscard]] size_t atlas_generation() const;

        /// \brief baseline to baseline distance in pixels
        [[nodiscard]] float line_height() const;

        /// \brief baseline to the top of the tallest glyph, in pixels
        [[nodiscard]] float ascent() const;

        /// \brief baseline to the bottom of the lowest glyph, in pixels. Negative.
        [[nodiscard]] float descent() const;

        /// \brief builds a font from the contents of a truetype or opentype file, or a collection
        ///
        /// \param aTrueTypeData the bytes of a .ttf, .otf, .ttc or .otc. Copied: the font owns its
        /// own copy for its lifetime
        /// \param aPixelSize the size to rasterise at, in pixels. Of what is `aSettings.sizing`:
        /// ascent to descent unless told otherwise
        /// \param aSettings the face, the sizing and the atlas's bounds, all defaulted.
        font(
            std::span<const texture_data::encoded_byte> aTrueTypeData,
            float aPixelSize,
            font_settings aSettings = {}
        );

        /// \brief the atlas's current edge length in texels. Changes when the atlas grows.
        [[nodiscard]] size_t atlas_edge() const;

        ~font();

        font(font &&) noexcept;
        font &operator=(font &&) noexcept;

        font(const font &) = delete;
        font &operator=(const font &) = delete;

    private:
        struct impl;
        std::unique_ptr<impl> m_pImpl;
    };
}

#endif
