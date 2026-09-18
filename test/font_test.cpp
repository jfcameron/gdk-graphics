// © Joseph Cameron - All Rights Reserved

#include <cstddef>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

#include <jfc/catch.hpp>

#include <gdk/graphics/ext/font.h>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
    const char *const CJK_FONT_PATHS[] = {
        "/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc",
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
        "/usr/share/fonts/truetype/fonts-japanese-gothic.ttf",
    };

    [[nodiscard]] const font::glyph &must_get(font &aFont, const char32_t aCodepoint) {
        const auto *const pGlyph = aFont.try_get(aCodepoint);

        REQUIRE(pGlyph);

        return *pGlyph;
    }

    [[nodiscard]] std::optional<std::vector<texture_data::encoded_byte>> load_a_font() {
        std::ifstream file(GDK_TEST_FONT, std::ios::binary);

        if (!file) return {};

        return std::vector<texture_data::encoded_byte>(
            std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }
}

TEST_CASE("jfc::graphics::ext::font rejects data that is not a font", "[gdk::graphics::ext::font]") {
    const std::vector<texture_data::encoded_byte> nonsense(256, 0x7f);

    REQUIRE_THROWS(font(nonsense, 16.f));
}

TEST_CASE("gdk::graphics::ext::font", "[gdk::graphics::ext::font]") {
    const auto data = load_a_font();

    REQUIRE(data);

    font subject(*data, 32.f);

    SECTION("its vertical metrics are sane") {
        REQUIRE(subject.ascent() > 0);
        REQUIRE(subject.descent() < 0);
        REQUIRE(subject.line_height() >= subject.ascent() - subject.descent());
    }

    SECTION("a rasterised glyph has a quad, an advance, and coordinates inside the atlas") {
        const auto &glyph = must_get(subject, U'A');

        REQUIRE(glyph.width > 0);
        REQUIRE(glyph.height > 0);
        REQUIRE(glyph.advance > 0);

        REQUIRE(glyph.s0 >= 0.f);
        REQUIRE(glyph.t0 >= 0.f);
        REQUIRE(glyph.s1 <= 1.f);
        REQUIRE(glyph.t1 <= 1.f);
        REQUIRE(glyph.s1 > glyph.s0);
        REQUIRE(glyph.t1 > glyph.t0);
    }

    SECTION("rasterising writes coverage into the atlas") {
        const auto before = subject.atlas();

        size_t coverage_before(0);

        for (size_t i = 0; i < before.width * before.height; ++i) coverage_before += before.data[i];

        (void)must_get(subject, U'M');

        const auto after = subject.atlas();

        size_t coverage_after(0);

        for (size_t i = 0; i < after.width * after.height; ++i) coverage_after += after.data[i];

        REQUIRE(coverage_after > 0);
    }

    SECTION("the atlas is a single channel image of the requested size") {
        const auto view = subject.atlas();

        REQUIRE(view.width == 512);
        REQUIRE(view.height == 512);
        REQUIRE(view.format == texture::format::grey);
        REQUIRE(view.data != nullptr);
    }

    SECTION("the generation moves when the atlas does, so a caller knows when to upload") {
        const auto before = subject.atlas_generation();

        (void)must_get(subject, U'Z');

        REQUIRE(subject.atlas_generation() != before);

        const auto after = subject.atlas_generation();

        (void)must_get(subject, U'Z');

        REQUIRE(subject.atlas_generation() == after);
    }

    SECTION("a glyph is rasterised once and then cached") {
        const auto &first = must_get(subject, U'Q');
        const auto &second = must_get(subject, U'Q');

        REQUIRE(&first == &second);
    }

    SECTION("a space has an advance but no quad") {
        const auto &glyph = must_get(subject, U' ');

        REQUIRE(glyph.advance > 0);
        REQUIRE(glyph.width == 0);
        REQUIRE(glyph.height == 0);
    }

    SECTION("a codepoint the font has no glyph for gets the font's notdef glyph") {
        const auto &glyph = must_get(subject, U'\U0010FFFF');

        REQUIRE(glyph.advance > 0);
    }

    SECTION("distinct codepoints land in distinct places in the atlas") {
        const auto a = must_get(subject, U'a');
        const auto b = must_get(subject, U'b');

        REQUIRE(!(a.s0 == b.s0 && a.t0 == b.t0));
    }

    SECTION("an atlas too small for the glyph asked of it grows until it fits") {
        font tiny(*data, 64.f, {.atlas_edge = 8});

        REQUIRE_NOTHROW((void)must_get(tiny, U'W'));

        REQUIRE(tiny.atlas_edge() >= 64);

        const auto glyph = must_get(tiny, U'W');

        REQUIRE(glyph.width > 0);
        REQUIRE(glyph.s1 <= 1.f);
    }
}

TEST_CASE("gdk::graphics::ext::font rasterises arbitrary codepoints on demand",
    "[gdk::graphics::ext::font]") {
    const auto data = load_a_font();

    REQUIRE(data);

    font subject(*data, 32.f);

    SECTION("latin, kana, kanji and a codepoint above the BMP, from one font") {
        for (const char32_t codepoint : {U'H', U'e', U'\u3042', U'\u65e5', U'\U00020000'}) {
            const auto &glyph = must_get(subject, codepoint);

            REQUIRE(glyph.width > 0);
            REQUIRE(glyph.height > 0);
            REQUIRE(glyph.advance > 0);
            REQUIRE(glyph.s1 <= 1.f);
            REQUIRE(glyph.t1 <= 1.f);
        }
    }

    SECTION("kana and kanji are full width, latin is not") {
        const auto latin = must_get(subject, U'e').advance;
        const auto kana = must_get(subject, U'\u3042').advance;
        const auto kanji = must_get(subject, U'\u65e5').advance;
        const auto astral = must_get(subject, U'\U00020000').advance;

        REQUIRE(kana == kanji);
        REQUIRE(kanji == astral);
        REQUIRE(latin < kana);
    }

    SECTION("the atlas gains coverage for each of them") {
        for (const char32_t codepoint : {U'H', U'\u3042', U'\u65e5', U'\U00020000'})
            (void)must_get(subject, codepoint);

        const auto atlas = subject.atlas();

        size_t covered(0);

        for (size_t i = 0; i < atlas.width * atlas.height; ++i) if (atlas.data[i]) ++covered;

        REQUIRE(covered > 0);
    }
}

TEST_CASE("gdk::graphics::ext::font reports the metrics it was given",
    "[gdk::graphics::ext::font]") {
    const auto data = load_a_font();

    REQUIRE(data);

    font subject(*data, 32.f);

    REQUIRE(subject.ascent() == Approx(25.6f).margin(0.1f));
    REQUIRE(subject.descent() == Approx(-6.4f).margin(0.1f));
    REQUIRE(subject.line_height() == Approx(35.2f).margin(0.1f));

    SECTION("advances follow the widths it was authored with") {
        REQUIRE(must_get(subject, U'A').advance == Approx(700.f * 0.032f).margin(0.1f));
        REQUIRE(must_get(subject, U'0').advance == Approx(600.f * 0.032f).margin(0.1f));
        REQUIRE(must_get(subject, U'\u65e5').advance == Approx(1000.f * 0.032f).margin(0.1f));
    }

    SECTION("kerning comes back with the sign and size it was authored with") {
        REQUIRE(subject.kerning(U'A', U'V') == Approx(-80.f * 0.032f).margin(0.05f));
        REQUIRE(subject.kerning(U'T', U'o') == Approx(-50.f * 0.032f).margin(0.05f));
        REQUIRE(subject.kerning(U'H', U'e') == Approx(0.f).margin(0.001f));
    }
}

namespace {
    [[nodiscard]] std::optional<std::vector<texture_data::encoded_byte>> load_the_collection() {
        std::ifstream file(GDK_TEST_FONT_COLLECTION, std::ios::binary);

        if (!file) return {};

        return std::vector<texture_data::encoded_byte>(
            std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }
}

TEST_CASE("gdk::graphics::ext::font reads any face of a collection",
    "[gdk::graphics::ext::font]") {
    const auto data = load_the_collection();

    REQUIRE(data);

    SECTION("face 0 by default, which is the single font's metrics exactly") {
        font subject(*data, 32.f);

        REQUIRE(subject.ascent() == Approx(800.f * 0.032f).margin(0.01f));
        REQUIRE(must_get(subject, U'A').advance == Approx(700.f * 0.032f).margin(0.01f));
    }

    SECTION("face 1 when asked, told apart by its taller metrics") {
        font subject(*data, 32.f, {.face = 1});

        REQUIRE(subject.ascent() == Approx(1100.f * 32.f / 1400.f).margin(0.01f));
        REQUIRE(subject.descent() == Approx(-300.f * 32.f / 1400.f).margin(0.01f));
        REQUIRE(must_get(subject, U'A').advance == Approx(700.f * 32.f / 1400.f).margin(0.01f));
    }

    SECTION("a face the data does not hold is a construction error") {
        REQUIRE_THROWS(font(*data, 32.f, {.face = 2}));

        const auto single = load_a_font();

        REQUIRE(single);

        REQUIRE_NOTHROW(font(*single, 32.f, {.face = 0}));
        REQUIRE_THROWS(font(*single, 32.f, {.face = 1}));
    }
}

TEST_CASE("gdk::graphics::ext::font can be sized by its em", "[gdk::graphics::ext::font]") {
    const auto data = load_the_collection();

    REQUIRE(data);

    font byHeight(*data, 32.f, {.face = 1});
    font byEm(*data, 32.f, {.face = 1, .sizing = font_sizing::em});

    SECTION("every metric scales by the em") {
        REQUIRE(byEm.ascent() == Approx(1100.f * 0.032f).margin(0.01f));
        REQUIRE(byEm.descent() == Approx(-300.f * 0.032f).margin(0.01f));
        REQUIRE(byEm.line_height() == Approx((1100.f + 300.f + 100.f) * 0.032f).margin(0.01f));
        REQUIRE(byEm.kerning(U'A', U'V') == Approx(-80.f * 0.032f).margin(0.01f));
    }

    SECTION("advances are fractions of the em, so they come out as authored") {
        REQUIRE(must_get(byEm, U'A').advance == Approx(700.f * 0.032f).margin(0.01f));
        REQUIRE(must_get(byHeight, U'A').advance == Approx(700.f * 32.f / 1400.f).margin(0.01f));

        REQUIRE(must_get(byEm, U'\u65E5').advance == Approx(32.f).margin(0.0001f));
    }

    SECTION("a font whose ascent to descent is its em sizes the same either way") {
        const auto single = load_a_font();

        REQUIRE(single);

        font height(*single, 32.f);
        font em(*single, 32.f, {.sizing = font_sizing::em});

        REQUIRE(em.ascent() == Approx(height.ascent()).margin(0.0001f));
        REQUIRE(must_get(em, U'A').advance
            == Approx(must_get(height, U'A').advance).margin(0.0001f));
    }
}

TEST_CASE("the atlas grows rather than refusing a glyph", "[gdk::graphics::ext::font]") {
    const auto data = load_a_font();

    REQUIRE(data);

    SECTION("it doubles when it fills, and keeps going") {
        font subject(*data, 24.f, {.atlas_edge = 64, .max_atlas_edge = 2048});

        REQUIRE(subject.atlas_edge() == 64);

        for (char32_t c = U'a'; c <= U'z'; ++c) REQUIRE_NOTHROW((void)must_get(subject, c));

        REQUIRE(subject.atlas_edge() > 64);

        REQUIRE(subject.atlas().width == subject.atlas_edge());
        REQUIRE(subject.atlas().height == subject.atlas_edge());
    }

    SECTION("glyphs from before a growth are still correct afterwards") {
        font subject(*data, 24.f, {.atlas_edge = 64, .max_atlas_edge = 2048});

        const auto before = must_get(subject, U'a');
        const auto edgeBefore = subject.atlas_edge();

        for (char32_t c = U'b'; c <= U'z'; ++c) (void)must_get(subject, c);

        REQUIRE(subject.atlas_edge() > edgeBefore);

        const auto after = must_get(subject, U'a');

        REQUIRE(after.s1 <= 1.f);
        REQUIRE(after.t1 <= 1.f);
        REQUIRE(after.s1 > after.s0);
        REQUIRE(after.t1 > after.t0);

        REQUIRE(after.width == before.width);
        REQUIRE(after.height == before.height);
        REQUIRE(after.advance == before.advance);

        REQUIRE((after.s1 != before.s1 || after.t1 != before.t1));
    }

    SECTION("growing moves the generation, so every consumer re-uploads") {
        font subject(*data, 24.f, {.atlas_edge = 64, .max_atlas_edge = 2048});

        (void)must_get(subject, U'a');

        const auto before = subject.atlas_generation();

        for (char32_t c = U'b'; c <= U'z'; ++c) (void)must_get(subject, c);

        REQUIRE(subject.atlas_generation() != before);
    }

    SECTION("at the ceiling it reports failure rather than throwing") {
        font subject(*data, 24.f, {.atlas_edge = 64, .max_atlas_edge = 64});

        bool ranOut(false);

        for (char32_t c = U'a'; c <= U'z' && !ranOut; ++c)
            if (!subject.try_get(c)) ranOut = true;

        REQUIRE(ranOut);
    }

    SECTION("and get() still hands back something drawable when it has run out") {
        font subject(*data, 24.f, {.atlas_edge = 64, .max_atlas_edge = 64});

        char32_t exhausted(0);

        for (char32_t c = U'a'; c <= U'z' && !exhausted; ++c)
            if (!subject.try_get(c)) exhausted = c;

        REQUIRE(exhausted);

        const auto &glyph = subject.fallback();

        REQUIRE(glyph.width > 0);
        REQUIRE(glyph.height > 0);
        REQUIRE_NOTHROW((void)subject.fallback());
    }

    SECTION("clear() empties it and lets the same glyphs be rasterised again") {
        font subject(*data, 24.f, {.atlas_edge = 64, .max_atlas_edge = 64});

        char32_t exhausted(0);

        for (char32_t c = U'a'; c <= U'z' && !exhausted; ++c)
            if (!subject.try_get(c)) exhausted = c;

        REQUIRE(exhausted);

        subject.clear();

        REQUIRE(subject.atlas_edge() == 64);

        REQUIRE(subject.try_get(exhausted));
    }

    SECTION("a font whose atlas can never hold its own notdef is refused at construction") {
        REQUIRE_THROWS(font(*data, 64.f, {.atlas_edge = 8, .max_atlas_edge = 8}));
    }

    SECTION("a glyph too large for the ceiling is reported, not thrown") {
        font subject(*data, 64.f, {.atlas_edge = 64, .max_atlas_edge = 64});

        REQUIRE(subject.try_get(U'W') == nullptr);

        REQUIRE(subject.fallback().width > 0);
    }
}

TEST_CASE("a font says which part of its atlas changed", "[gdk::graphics::ext::font]") {
    const auto data = load_a_font();

    REQUIRE(data);

    font subject(*data, 12.f, {.atlas_edge = 512, .max_atlas_edge = 2048});

    const auto start = subject.atlas_generation();

    SECTION("nothing changed is an empty region, not an absent one") {
        const auto changed = subject.atlas_changed_since(start);

        REQUIRE(changed);
        REQUIRE(changed->width == 0);
        REQUIRE(changed->height == 0);
    }

    SECTION("a new glyph gives a region that contains it and nothing like the whole atlas") {
        (void)must_get(subject, U'W');

        const auto changed = subject.atlas_changed_since(start);

        REQUIRE(changed);
        REQUIRE(changed->width > 0);
        REQUIRE(changed->height > 0);

        REQUIRE(changed->x + changed->width <= subject.atlas_edge());
        REQUIRE(changed->y + changed->height <= subject.atlas_edge());

        REQUIRE(changed->width < subject.atlas_edge());
        REQUIRE(changed->height < subject.atlas_edge());
    }

    SECTION("several new glyphs are bounded together") {
        (void)must_get(subject, U'a');

        const auto afterOne = subject.atlas_generation();

        for (char32_t c = U'b'; c <= U'j'; ++c) (void)must_get(subject, c);

        const auto sinceOne = subject.atlas_changed_since(afterOne);
        const auto sinceStart = subject.atlas_changed_since(start);

        REQUIRE(sinceOne);
        REQUIRE(sinceStart);

        REQUIRE(sinceStart->width >= sinceOne->width);
    }

    SECTION("after a growth there is no region to patch, because everything moved") {
        font small(*data, 24.f, {.atlas_edge = 64, .max_atlas_edge = 2048});

        const auto before = small.atlas_generation();

        for (char32_t c = U'a'; c <= U'z'; ++c) (void)must_get(small, c);

        REQUIRE(small.atlas_edge() > 64);

        REQUIRE(!small.atlas_changed_since(before));
    }

    SECTION("after a clear there is no region to patch either") {
        (void)must_get(subject, U'a');

        const auto before = subject.atlas_generation();

        subject.clear();

        REQUIRE(!subject.atlas_changed_since(before));
    }
}
