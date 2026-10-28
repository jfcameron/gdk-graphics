// © Joseph Cameron - MIT License

#include <gdk/graphics/ext/utf8.h>

using namespace gdk::graphics;

namespace {
    struct lead final {
        std::size_t length;
        unsigned char secondLowest;
        unsigned char secondHighest;
        char32_t bits;
    };

    [[nodiscard]] lead lead_of(const unsigned char aByte) {
        if (aByte >= 0xC2 && aByte <= 0xDF) return {2, 0x80, 0xBF, aByte & 0x1Fu};
        if (aByte == 0xE0) return {3, 0xA0, 0xBF, aByte & 0x0Fu};
        if (aByte == 0xED) return {3, 0x80, 0x9F, aByte & 0x0Fu};
        if (aByte >= 0xE1 && aByte <= 0xEF) return {3, 0x80, 0xBF, aByte & 0x0Fu};
        if (aByte == 0xF0) return {4, 0x90, 0xBF, aByte & 0x07u};
        if (aByte == 0xF4) return {4, 0x80, 0x8F, aByte & 0x07u};
        if (aByte >= 0xF1 && aByte <= 0xF3) return {4, 0x80, 0xBF, aByte & 0x07u};

        return {0, 0, 0, 0};
    }
}

char32_t ext::decode_utf8(const std::string_view aText, std::size_t &aIndex) {
    const auto first = static_cast<unsigned char>(aText[aIndex++]);

    if (first < 0x80) return first;

    const auto sequence = lead_of(first);

    if (!sequence.length) return REPLACEMENT_CHARACTER;

    char32_t value = sequence.bits;

    for (std::size_t i = 1; i < sequence.length; ++i) {
        if (aIndex >= aText.size()) return REPLACEMENT_CHARACTER;

        const auto byte = static_cast<unsigned char>(aText[aIndex]);

        const auto lowest = i == 1 ? sequence.secondLowest : static_cast<unsigned char>(0x80);
        const auto highest = i == 1 ? sequence.secondHighest : static_cast<unsigned char>(0xBF);

        if (byte < lowest || byte > highest) return REPLACEMENT_CHARACTER;

        value = (value << 6) | (byte & 0x3Fu);

        ++aIndex;
    }

    return value;
}
