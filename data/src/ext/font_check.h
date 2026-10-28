// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXT_FONT_CHECK_H
#define GDK_GRAPHICS_EXT_FONT_CHECK_H

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace gdk::graphics::ext::font_check {
    //! a table's place in the font's bytes
    struct table final {
        std::size_t offset = 0;
        std::size_t length = 0;
    };

    //! what a checked font face is, for what is done with it after stb_truetype has read it
    struct checked final {
        std::size_t start = 0;
        std::size_t glyphs = 0;
        std::optional<table> cff;
        bool kern = false;
        bool gpos = false;

        //! per glyph, whether stb_truetype would read past what it should drawing it: a glyph whose last contour is
        //! a single point off the curve, or a composite of one, or of a glyph numbered 0x8000 or more, which it reads
        //! as negative. Such a glyph is to be drawn as the fallback.
        std::vector<bool> undrawable;
    };

    /// \brief checks every part of a font face that stb_truetype reads without bounds, against the bytes it is
    /// given: the collection header, the table directory, head, hhea, maxp, hmtx, cmap, loca and glyf, or CFF.
    /// kern and GPOS are optional: one that does not check out is not to be read. Three quirks of real fonts are
    /// let through, as stb_truetype's reads stay within the font: an hmtx a few entries short, read into the next
    /// table; a glyph whose end comes before its start, checked to the end of the glyph data; and a character map's
    /// closing segment for U+FFFF pointing nowhere, which is never looked up.
    /// \throws gdk::graphics::exception
    [[nodiscard]] checked check(std::span<const unsigned char> aFont, std::size_t aFace);
}

#endif
