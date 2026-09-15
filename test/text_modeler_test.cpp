// © Joseph Cameron - All Rights Reserved

#include <fstream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <jfc/catch.hpp>

#include <gdk/graphics/ext/font.h>
#include <gdk/graphics/ext/text_modeler.h>
#include <gdk/graphics/null_context.h>

using namespace gdk;
using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
    [[nodiscard]] std::shared_ptr<font> load_a_font() {
        std::ifstream file(GDK_TEST_FONT, std::ios::binary);

        if (!file) return {};

        const std::vector<texture_data::encoded_byte> data(
            (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        return std::make_shared<font>(data, 32.f);
    }
}

TEST_CASE("gdk::graphics::ext::text_modeler", "[gdk::graphics::ext::text_modeler]") {
    auto pFont = load_a_font();

    REQUIRE(pFont);

    const auto pContext = null_context::make();

    text_modeler subject(pContext, pFont);

    SECTION("it produces geometry for text, and none for nothing") {
        subject.set_text("");
        subject.upload();

        REQUIRE(subject.model());

        subject.set_text("hello");
        subject.upload();

        REQUIRE(subject.model());
        REQUIRE(subject.size().x > 0);
    }

    SECTION("wider text is wider") {
        subject.set_text("i");
        subject.upload();

        const auto narrow = subject.size().x;

        subject.set_text("wwwwwwwwww");
        subject.upload();

        REQUIRE(subject.size().x > narrow);
    }

    SECTION("glyph advances are used, so text is not monospaced") {
        subject.set_text("ll");
        subject.upload();

        const auto narrow = subject.size().x;

        subject.set_text("WW");
        subject.upload();

        REQUIRE(subject.size().x > narrow);
    }

    SECTION("a newline starts another line and makes the text taller") {
        subject.set_text("one");
        subject.upload();

        const auto oneLine = subject.size().y;

        subject.set_text("one\ntwo");
        subject.upload();

        REQUIRE(subject.size().y > oneLine);
    }

    SECTION("one line of text is one unit tall by default") {
        subject.set_text("x");
        subject.upload();

        REQUIRE(subject.size().y == Approx(1.f));
    }

    SECTION("a tab advances further than a space") {
        subject.set_text("a b");
        subject.upload();

        const auto spaced = subject.size().x;

        subject.set_text("a\tb");
        subject.upload();

        REQUIRE(subject.size().x > spaced);
    }

    SECTION("it accepts UTF-8, not just ASCII") {
        subject.set_text("\xE6\x97\xA5\xE6\x9C\xAC\xE8\xAA\x9E"); // 日本語 as UTF-8 bytes
        subject.upload();

        REQUIRE(subject.size().x > 0);

        const auto cjk = subject.size().x;

        subject.set_text("iii");
        subject.upload();

        REQUIRE(cjk > subject.size().x);
    }

    SECTION("malformed UTF-8 renders rather than throwing or hanging") {
        const std::string malformed("ok\xC3", 3); // a lead byte with no continuation

        REQUIRE_NOTHROW(subject.set_text(malformed));

        subject.upload();

        REQUIRE(subject.size().x > 0);
    }

    SECTION("a lone continuation byte does not run off the end") {
        const std::string malformed("\x80\x80\x80", 3);

        REQUIRE_NOTHROW(subject.set_text(malformed));
    }
}

TEST_CASE("gdk::graphics::ext::text_modeler uploads the atlas when new glyphs arrive",
    "[gdk::graphics::ext::text_modeler]") {
    auto pFont = load_a_font();

    // vendored, so its absence is a broken checkout rather than a reason to skip
    REQUIRE(pFont);

    const auto pContext = null_context::make();

    text_modeler subject(pContext, pFont);

    const auto empty = pFont->atlas_generation();

    subject.set_text("abc");

    REQUIRE(pFont->atlas_generation() != empty);
    REQUIRE(subject.uploaded_atlas_generation() != pFont->atlas_generation());

    subject.upload();

    REQUIRE(subject.uploaded_atlas_generation() == pFont->atlas_generation());

    subject.set_text("xyz");

    REQUIRE(subject.uploaded_atlas_generation() != pFont->atlas_generation());

}

TEST_CASE("text_modeler lays out correctly across an atlas growth",
    "[gdk::graphics::ext::text_modeler]") {
    std::ifstream file(GDK_TEST_FONT, std::ios::binary);

    REQUIRE(file);

    const std::vector<texture_data::encoded_byte> data(
        (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto pFont = std::make_shared<font>(data, 24.f,
        font_settings{.atlas_edge = 64, .max_atlas_edge = 2048});

    const auto pContext = null_context::make();

    text_modeler subject(pContext, pFont);

    REQUIRE(pFont->atlas_edge() == 64);

    REQUIRE_NOTHROW(subject.set_text("the quick brown fox jumps over the lazy dog"));

    subject.upload();

    REQUIRE(pFont->atlas_edge() > 64);

    const auto wide = subject.size().x;

    REQUIRE(wide > 0);

    subject.set_text("the quick brown");
    subject.upload();

    REQUIRE(subject.size().x < wide);
}

TEST_CASE("two modelers sharing a font both see atlas changes",
    "[gdk::graphics::ext::text_modeler]") {
    std::ifstream file(GDK_TEST_FONT, std::ios::binary);

    REQUIRE(file);

    const std::vector<texture_data::encoded_byte> data(
        (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto pFont = std::make_shared<font>(data, 24.f,
        font_settings{.atlas_edge = 64, .max_atlas_edge = 2048});

    const auto pContext = null_context::make();

    text_modeler first(pContext, pFont);
    text_modeler second(pContext, pFont);

    first.set_text("abc");
    first.upload();

    const auto edgeAfterFirst = pFont->atlas_edge();

    REQUIRE(second.uploaded_atlas_generation() != pFont->atlas_generation());

    second.set_text("abc");
    second.upload();

    REQUIRE(second.uploaded_atlas_generation() == pFont->atlas_generation());

    first.set_text("the quick brown fox jumps over the lazy dog 0123456789");
    first.upload();

    REQUIRE(pFont->atlas_edge() > edgeAfterFirst);
    REQUIRE(second.uploaded_atlas_generation() != pFont->atlas_generation());
}

TEST_CASE("text_modeler reports glyphs it could not rasterise",
    "[gdk::graphics::ext::text_modeler]") {
    std::ifstream file(GDK_TEST_FONT, std::ios::binary);

    REQUIRE(file);

    const std::vector<texture_data::encoded_byte> data(
        (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    const auto pContext = null_context::make();

    SECTION("nothing to report when everything fits") {
        auto pFont = std::make_shared<font>(data, 24.f,
            font_settings{.atlas_edge = 64, .max_atlas_edge = 2048});

        text_modeler subject(pContext, pFont);

        subject.set_text("hello");

        REQUIRE(subject.fallback_glyph_count() == 0);
    }

    SECTION("a full atlas is counted, and the text still lays out") {
        auto pFont = std::make_shared<font>(data, 24.f,
            font_settings{.atlas_edge = 64, .max_atlas_edge = 64});

        text_modeler subject(pContext, pFont);

        subject.set_text("abcdefghijklmnopqrstuvwxyz");

        REQUIRE(subject.fallback_glyph_count() > 0);

        REQUIRE_NOTHROW(subject.upload());
        REQUIRE(subject.size().x > 0);
    }

    SECTION("the count is per set_text, not cumulative") {
        auto pFont = std::make_shared<font>(data, 24.f,
            font_settings{.atlas_edge = 64, .max_atlas_edge = 64});

        text_modeler subject(pContext, pFont);

        subject.set_text("abcdefghijklmnopqrstuvwxyz");

        REQUIRE(subject.fallback_glyph_count() > 0);

        subject.set_text("aaa");

        REQUIRE(subject.fallback_glyph_count() == 0);
    }
}

TEST_CASE("text_modeler patches the atlas texture instead of rebuilding it",
    "[gdk::graphics::ext::text_modeler]") {
    std::ifstream file(GDK_TEST_FONT, std::ios::binary);

    REQUIRE(file);

    const std::vector<texture_data::encoded_byte> data(
        (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto pFont = std::make_shared<font>(data, 12.f,
        font_settings{.atlas_edge = 512, .max_atlas_edge = 2048});

    const auto pContext = null_context::make();

    text_modeler subject(pContext, pFont);

    subject.set_text("abc");
    subject.upload();

    const auto afterFirst = pContext->texture_count();

    REQUIRE(afterFirst == 1);

    SECTION("new glyphs are patched in, leaving the texture in place") {
        subject.set_text("defghijklmnop");
        subject.upload();

        REQUIRE(pFont->atlas_edge() == 512);          
        REQUIRE(pContext->texture_count() == afterFirst);
    }

    SECTION("nothing to send when the text uses glyphs already there") {
        subject.set_text("cba");
        subject.upload();

        REQUIRE(pContext->texture_count() == afterFirst);
    }

    SECTION("a growth does rebuild it, because every glyph moved") {
        auto pSmall = std::make_shared<font>(data, 24.f,
            font_settings{.atlas_edge = 64, .max_atlas_edge = 2048});

        text_modeler grower(pContext, pSmall);

        grower.set_text("a");
        grower.upload();

        const auto beforeGrowth = pContext->texture_count();
        const auto edgeBefore = pSmall->atlas_edge();

        grower.set_text("the quick brown fox jumps over the lazy dog 0123456789");
        grower.upload();

        REQUIRE(pSmall->atlas_edge() > edgeBefore);
        REQUIRE(pContext->texture_count() > beforeGrowth);
    }
}

TEST_CASE("glyph coverage reaches the texture instead of being rounded off",
    "[gdk::graphics::ext::text_modeler]") {
    std::ifstream file(GDK_TEST_FONT, std::ios::binary);

    REQUIRE(file);

    const std::vector<texture_data::encoded_byte> data(
        (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto pFont = std::make_shared<font>(data, 13.f,
        font_settings{.atlas_edge = 512, .max_atlas_edge = 2048});

    (void)pFont->try_get(U'W');

    const auto atlas = pFont->atlas();

    size_t partial(0);

    for (size_t i = 0; i < atlas.width * atlas.height; ++i)
        if (atlas.data[i] > 0 && atlas.data[i] < 255) ++partial;

    REQUIRE(partial > 0);
}
