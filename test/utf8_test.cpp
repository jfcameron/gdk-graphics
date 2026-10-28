// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/graphics/ext/utf8.h>

#include <string>
#include <string_view>
#include <vector>

using namespace gdk::graphics::ext;

namespace {
    [[nodiscard]] std::vector<char32_t> decoded(const std::string_view aText) {
        std::vector<char32_t> out;

        for (std::size_t i = 0; i < aText.size();) out.push_back(decode_utf8(aText, i));

        return out;
    }

    constexpr auto R = REPLACEMENT_CHARACTER;
}

TEST_CASE("decode_utf8 decodes every length of sequence", "[utf8]") {
    REQUIRE(decoded("a") == std::vector<char32_t>{U'a'});
    REQUIRE(decoded("\xC3\xA9") == std::vector<char32_t>{0xE9});
    REQUIRE(decoded("\xE6\x97\xA5") == std::vector<char32_t>{0x65E5});
    REQUIRE(decoded("\xF0\x9F\x98\x80") == std::vector<char32_t>{0x1F600});
    REQUIRE(decoded("\xF4\x8F\xBF\xBF") == std::vector<char32_t>{0x10FFFF});
}

TEST_CASE("decode_utf8 gives one replacement for each maximal subpart of a malformed sequence", "[utf8]") {
    SECTION("a surrogate is refused, a byte at a time") {
        REQUIRE(decoded("\xED\xA0\x80") == std::vector<char32_t>{R, R, R});
    }

    SECTION("an overlong form is refused") {
        REQUIRE(decoded("\xC0\xAF") == std::vector<char32_t>{R, R});
        REQUIRE(decoded("\xE0\x80\xAF") == std::vector<char32_t>{R, R, R});
        REQUIRE(decoded("\xF0\x80\x80\xAF") == std::vector<char32_t>{R, R, R, R});
    }

    SECTION("past U+10FFFF is refused") {
        REQUIRE(decoded("\xF4\x90\x80\x80") == std::vector<char32_t>{R, R, R, R});
        REQUIRE(decoded("\xF5\x80\x80\x80") == std::vector<char32_t>{R, R, R, R});
    }

    SECTION("a sequence cut short is one replacement, and what follows is read") {
        REQUIRE(decoded("\xE6\x97" "a") == std::vector<char32_t>{R, U'a'});
        REQUIRE(decoded("\xF0\x9F\x98") == std::vector<char32_t>{R});
    }

    SECTION("a stray continuation byte is one replacement") {
        REQUIRE(decoded("\x80" "a") == std::vector<char32_t>{R, U'a'});
    }
}
