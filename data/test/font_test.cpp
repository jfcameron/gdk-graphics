// © Joseph Cameron - MIT License

#include <cstddef>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

#include <jfc/catch.hpp>

#include <gdk/graphics/test/font_bytes.h>
#include <gdk/graphics/test/font_collection_bytes.h>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/font.h>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
    [[nodiscard]] const font::glyph &must_get(font &aFont, const char32_t aCodepoint) {
        const auto *const pGlyph = aFont.try_get(aCodepoint);

        REQUIRE(pGlyph);

        return *pGlyph;
    }

    [[nodiscard]] std::optional<std::vector<texture_data::encoded_byte>> load_a_font() {
        return std::vector<texture_data::encoded_byte>(std::begin(GDK_TEST_FONT_BYTES), std::end(GDK_TEST_FONT_BYTES));
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

        REQUIRE(coverage_after > coverage_before);
    }

    SECTION("the atlas is a single channel image of the requested size") {
        const auto view = subject.atlas();

        REQUIRE(view.width == 512);
        REQUIRE(view.height == 512);
        REQUIRE(view.format == texture_data::format::grey);
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
        return std::vector<texture_data::encoded_byte>(std::begin(GDK_TEST_FONT_COLLECTION_BYTES),
            std::end(GDK_TEST_FONT_COLLECTION_BYTES));
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

namespace {
    using font_bytes = std::vector<texture_data::encoded_byte>;

    [[nodiscard]] std::size_t u16_at(const font_bytes &aFont, const std::size_t aAt) {
        return (std::size_t{aFont[aAt]} << 8) | aFont[aAt + 1];
    }

    [[nodiscard]] std::size_t u32_at(const font_bytes &aFont, const std::size_t aAt) {
        return (u16_at(aFont, aAt) << 16) | u16_at(aFont, aAt + 2);
    }

    void put16(font_bytes &aFont, const std::size_t aAt, const std::size_t aValue) {
        aFont[aAt] = static_cast<texture_data::encoded_byte>(aValue >> 8);
        aFont[aAt + 1] = static_cast<texture_data::encoded_byte>(aValue);
    }

    void put32(font_bytes &aFont, const std::size_t aAt, const std::size_t aValue) {
        put16(aFont, aAt, aValue >> 16);
        put16(aFont, aAt + 2, aValue & 0xFFFF);
    }

    //! where a table's directory record is
    [[nodiscard]] std::size_t record_of(const font_bytes &aFont, const std::string &aTag) {
        for (std::size_t i = 0; i < u16_at(aFont, 4); ++i)
            if (std::string(aFont.begin() + 12 + i * 16, aFont.begin() + 16 + i * 16) == aTag) return 12 + i * 16;

        FAIL("the test font has no " << aTag);

        return 0;
    }

    [[nodiscard]] std::size_t table_at(const font_bytes &aFont, const std::string &aTag) {
        return u32_at(aFont, record_of(aFont, aTag) + 8);
    }
}

TEST_CASE("a font cut short anywhere is refused, never read past", "[gdk::graphics::ext::font]") {
    const auto data = load_a_font();

    REQUIRE(data);

    REQUIRE_THROWS_AS(font(font_bytes{}, 16.f), exception);
    REQUIRE_THROWS_AS(font(font_bytes(data->begin(), data->begin() + 4), 16.f), exception);

    for (std::size_t length = 8; length < data->size(); length += 97) {
        INFO("the first " << length << " bytes");

        REQUIRE_THROWS_AS(font(font_bytes(data->begin(), data->begin() + static_cast<std::ptrdiff_t>(length)), 16.f),
            exception);
    }
}

TEST_CASE("a font whose parts point where they should not is refused", "[gdk::graphics::ext::font]") {
    const auto data = load_a_font();

    REQUIRE(data);

    REQUIRE_NOTHROW(font(*data, 16.f));

    SECTION("a table reaching past the end of the font") {
        auto broken = *data;

        put32(broken, record_of(broken, "glyf") + 12, broken.size());

        REQUIRE_THROWS_AS(font(broken, 16.f), exception);
    }

    SECTION("a table at the start of the font, which stb_truetype takes for one it hasn't") {
        auto broken = *data;

        put32(broken, record_of(broken, "maxp") + 8, 0);

        REQUIRE_THROWS_AS(font(broken, 16.f), exception);
    }

    SECTION("a character map claiming more groups than it holds") {
        auto broken = *data;

        const auto cmap = table_at(broken, "cmap");
        const auto map = cmap + u32_at(broken, cmap + 8);

        put32(broken, map + 12, 0x0FFFFFFF);

        REQUIRE_THROWS_AS(font(broken, 16.f), exception);
    }

    SECTION("a composite glyph made of itself is refused; one of a glyph read from before the font is not drawn") {
        auto broken = *data;

        const auto loca = table_at(broken, "loca");
        const auto glyf = table_at(broken, "glyf");
        const auto longLocations = u16_at(broken, table_at(broken, "head") + 50) == 1;

        const auto location = [&](const std::size_t aGlyph) {
            return longLocations ? u32_at(broken, loca + aGlyph * 4) : u16_at(broken, loca + aGlyph * 2) * 2;
        };

        std::size_t glyph = 1;

        while (location(glyph + 1) - location(glyph) < 16) ++glyph;

        const auto at = glyf + location(glyph);

        put16(broken, at, 0xFFFF);
        put16(broken, at + 10, 0x0002);
        put16(broken, at + 12, glyph);

        REQUIRE_THROWS_AS(font(broken, 16.f), exception);

        put16(broken, at + 12, 0x8001);

        const auto *const pCodepoint = [&]() -> const char32_t * {
            static const char32_t ANY[] = {U'A', U'B', U'a', U'b', U'0', U'1'};

            return ANY;
        }();

        font subject(broken, 16.f);

        for (std::size_t i = 0; i < 6; ++i) REQUIRE_NOTHROW(subject.try_get(pCodepoint[i]));
    }
}

TEST_CASE("a font's broken kerning is left unread, and the font still reads", "[gdk::graphics::ext::font]") {
    const auto data = load_a_font();

    REQUIRE(data);

    auto broken = *data;

    put16(broken, table_at(broken, "kern") + 10, 0xFFFF);

    font subject(broken, 16.f);

    REQUIRE(subject.try_get(U'A'));
    REQUIRE(subject.kerning(U'A', U'V') == 0.0f);
}

TEST_CASE("a glyph whose box is inside out is drawn as nothing, not rasterised", "[gdk::graphics::ext::font]") {
    const auto data = load_a_font();

    REQUIRE(data);

    auto broken = *data;

    const auto loca = table_at(broken, "loca");
    const auto glyf = table_at(broken, "glyf");
    const auto longLocations = u16_at(broken, table_at(broken, "head") + 50) == 1;

    for (std::size_t glyph = 0; glyph < 4; ++glyph) {
        const auto at = glyf + (longLocations ? u32_at(broken, loca + glyph * 4) : u16_at(broken, loca + glyph * 2) * 2);

        put16(broken, at + 2, 0x7000);
        put16(broken, at + 6, 0x0010);
    }

    font subject(broken, 16.f);

    REQUIRE(subject.fallback().width == 0.0f);
}
