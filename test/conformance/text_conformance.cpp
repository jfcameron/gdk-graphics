// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/graphics/test/font_bytes.h>

#include "conformance.h"

#include <gdk/graphics/ext/font.h>
#include <gdk/graphics/ext/text_modeler.h>

#include <iterator>
#include <memory>
#include <vector>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;
using gdk::graphics::conformance::implementation;
using gdk::graphics::conformance::make_context;

namespace {
    [[nodiscard]] std::vector<texture_data::encoded_byte> font_file() {
        return {std::begin(GDK_TEST_FONT_BYTES), std::end(GDK_TEST_FONT_BYTES)};
    }

    [[nodiscard]] std::shared_ptr<font> load_a_font() {
        const auto data = font_file();

        if (data.empty()) return {};

        return std::make_shared<font>(data, 32.f);
    }
}

TEST_CASE("conformance, text_modeler", "[conformance][text_modeler]") {
    INFO("implementation " << implementation());

    auto pFont = load_a_font();

    REQUIRE(pFont);

    const auto pContext = make_context();

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

TEST_CASE("conformance, text_modeler uploads the atlas when new glyphs arrive",
    "[conformance][text_modeler]") {
    INFO("implementation " << implementation());

    auto pFont = load_a_font();

    REQUIRE(pFont);

    const auto pContext = make_context();

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

TEST_CASE("conformance, text_modeler lays out correctly across an atlas growth",
    "[conformance][text_modeler]") {
    INFO("implementation " << implementation());

    const auto data = font_file();

    REQUIRE_FALSE(data.empty());

    auto pFont = std::make_shared<font>(data, 24.f,
        font_settings{.atlas_edge = 64, .max_atlas_edge = 2048});

    const auto pContext = make_context();

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

TEST_CASE("conformance, two modelers sharing a font both see atlas changes",
    "[conformance][text_modeler]") {
    INFO("implementation " << implementation());

    const auto data = font_file();

    REQUIRE_FALSE(data.empty());

    auto pFont = std::make_shared<font>(data, 24.f,
        font_settings{.atlas_edge = 64, .max_atlas_edge = 2048});

    const auto pContext = make_context();

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

TEST_CASE("conformance, text_modeler reports glyphs it could not rasterise",
    "[conformance][text_modeler]") {
    INFO("implementation " << implementation());

    const auto data = font_file();

    REQUIRE_FALSE(data.empty());

    const auto pContext = make_context();

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
