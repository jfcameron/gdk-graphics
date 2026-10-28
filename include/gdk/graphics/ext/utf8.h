// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXT_UTF8_H
#define GDK_GRAPHICS_EXT_UTF8_H

#include <cstddef>
#include <string_view>

namespace gdk::graphics::ext {
    inline constexpr char32_t REPLACEMENT_CHARACTER = 0xFFFD;

    /// \brief the code point that starts at aIndex, which is moved past it
    ///
    /// A malformed sequence decodes to REPLACEMENT_CHARACTER, once for each maximal subpart of it, as Unicode
    /// recommends: an overlong form, a surrogate, a code point past U+10FFFF, a stray continuation byte or a
    /// sequence cut short. aIndex must be less than aText's size.
    [[nodiscard]] char32_t decode_utf8(std::string_view aText, std::size_t &aIndex);
}

#endif
