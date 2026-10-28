// © Joseph Cameron - MIT License

#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <jfc/catch.hpp>

#include <gdk/graphics/test/font_bytes.h>

#include <gdk/graphics/ext/font.h>
#include <gdk/graphics/ext/text_modeler.h>
#include <gdk/graphics/null_context.h>

using namespace gdk;
using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
    [[nodiscard]] std::vector<texture_data::encoded_byte> test_font_bytes() {
        return {std::begin(GDK_TEST_FONT_BYTES), std::end(GDK_TEST_FONT_BYTES)};
    }

    [[nodiscard]] const null_model &uploaded(const const_model_ptr_type &pModel) {
        return *std::static_pointer_cast<const null_model>(pModel);
    }

    [[nodiscard]] std::shared_ptr<font> load_a_font() { return std::make_shared<font>(test_font_bytes(), 32.f); }
}

TEST_CASE("a text_modeler's model holds its text, however set_text and upload are called",
    "[gdk::graphics::ext::text_modeler]") {
    auto pFont = load_a_font();

    REQUIRE(pFont);

    text_modeler subject(null_context::make(), pFont);

    subject.set_text("abc");
    subject.upload();

    const auto once = uploaded(subject.model()).vertex_count();

    REQUIRE(once == 18);

    SECTION("text set twice before an upload is the second text, not both") {
        subject.set_text("abc");
        subject.set_text("abc");
        subject.upload();

        REQUIRE(uploaded(subject.model()).vertex_count() == once);
    }

    SECTION("uploaded again with nothing changed, it keeps its text and sends nothing") {
        const auto sent = uploaded(subject.model()).upload_count();

        subject.upload();

        REQUIRE(uploaded(subject.model()).vertex_count() == once);
        REQUIRE(uploaded(subject.model()).upload_count() == sent);
    }
}

TEST_CASE("a text_modeler lays its text out again when another modeler grows the atlas they share",
    "[gdk::graphics::ext::text_modeler]") {
    const auto data = test_font_bytes();

    auto pFont = std::make_shared<font>(data, 24.f, font_settings{.atlas_edge = 64, .max_atlas_edge = 2048});

    const auto pContext = null_context::make();

    text_modeler grower(pContext, pFont), other(pContext, pFont);

    other.set_text("a");
    other.upload();

    const auto edgeBefore = pFont->atlas_edge();

    grower.set_text("the quick brown fox jumps over the lazy dog 0123456789");
    grower.upload();

    REQUIRE(pFont->atlas_edge() > edgeBefore);

    other.upload();

    const auto *const pGlyph = pFont->try_get(U'a');

    REQUIRE(pGlyph);

    const auto pModel = other.model();

    const auto &uvs = uploaded(pModel).last_upload().attributes().at("a_UV").components();

    REQUIRE(uploaded(pModel).vertex_count() == 6);
    REQUIRE(uvs[2] == Approx(pGlyph->s0));
    REQUIRE(uvs[3] == Approx(pGlyph->t0));
}

TEST_CASE("text_modeler patches the atlas texture instead of rebuilding it",
    "[gdk::graphics::ext::text_modeler]") {
    const auto data = test_font_bytes();

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
    const auto data = test_font_bytes();

    auto pFont = std::make_shared<font>(data, 13.f,
        font_settings{.atlas_edge = 512, .max_atlas_edge = 2048});

    (void)pFont->try_get(U'W');

    const auto atlas = pFont->atlas();

    size_t partial(0);

    for (size_t i = 0; i < atlas.width * atlas.height; ++i)
        if (atlas.data[i] > 0 && atlas.data[i] < 255) ++partial;

    REQUIRE(partial > 0);
}

TEST_CASE("text_modelers given one font_texture draw from one texture, uploaded once",
    "[gdk::graphics::ext::text_modeler]") {
    const auto data = test_font_bytes();

    const auto pContext = null_context::make();

    SECTION("each modeler given only the font keeps a texture of its own") {
        auto pFont = std::make_shared<font>(data, 12.f, font_settings{.atlas_edge = 512, .max_atlas_edge = 2048});

        text_modeler first(pContext, pFont), second(pContext, pFont);

        first.set_text("abc");
        first.upload();
        second.set_text("abc");
        second.upload();

        REQUIRE(pContext->texture_count() == 2);
    }

    SECTION("modelers given the same font_texture share one") {
        auto pTexture = std::make_shared<font_texture>(pContext,
            std::make_shared<font>(data, 12.f, font_settings{.atlas_edge = 512, .max_atlas_edge = 2048}));

        text_modeler first(pContext, pTexture), second(pContext, pTexture), third(pContext, pTexture);

        first.set_text("abc");
        first.upload();
        second.set_text("defghijklmnop");
        second.upload();
        third.set_text("cab");
        third.upload();

        REQUIRE(pContext->texture_count() == 1);
        REQUIRE(pTexture->uploaded_atlas_generation() == pTexture->typeface()->atlas_generation());
        REQUIRE(first.uploaded_atlas_generation() == pTexture->uploaded_atlas_generation());
    }

    SECTION("a growth makes the shared texture again, and a modeler takes it as it uploads") {
        auto pFont = std::make_shared<font>(data, 24.f, font_settings{.atlas_edge = 64, .max_atlas_edge = 2048});
        auto pTexture = std::make_shared<font_texture>(pContext, pFont);

        text_modeler first(pContext, pTexture), second(pContext, pTexture);

        first.set_text("a");
        first.upload();
        second.set_text("a");
        second.upload();

        const auto beforeGrowth = pTexture->texture();
        const auto edgeBefore = pFont->atlas_edge();

        first.set_text("the quick brown fox jumps over the lazy dog 0123456789");
        first.upload();

        REQUIRE(pFont->atlas_edge() > edgeBefore);
        REQUIRE(pTexture->texture() != beforeGrowth);
        REQUIRE(pContext->texture_count() == 2);

        second.set_text("a");
        second.upload();

        REQUIRE(pContext->texture_count() == 2);
        REQUIRE(second.uploaded_atlas_generation() == pFont->atlas_generation());
    }
}
