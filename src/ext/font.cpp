// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/font.h>

#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>

#include <algorithm>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
    constexpr int PADDING = 1;
    constexpr const char *TAG = "gdk::graphics::ext::font: ";
}

struct font::impl {
    std::vector<texture_data::encoded_byte> m_FontData;

    stbtt_fontinfo m_Info{};

    float m_Scale = 0;

    float m_Ascent = 0;
    float m_Descent = 0;
    float m_LineHeight = 0;

    size_t m_AtlasEdge = 0;

    size_t m_MaxAtlasEdge = 0;

    size_t m_InitialAtlasEdge = 0;

    struct placed {
        font::glyph glyph;

        size_t x = 0, y = 0, width = 0, height = 0;

        size_t generation = 0;
    };

    placed m_Fallback;

    texture_data::channel_data m_Atlas;

    size_t m_AtlasGeneration = 1;

    stbrp_context m_PackContext{};
    std::vector<stbrp_node> m_Nodes;

    std::unordered_map<char32_t, placed> m_Glyphs;

    size_t m_LayoutGeneration = 1;

    impl(std::span<const texture_data::encoded_byte> aData, const float aPixelSize,
        const font_settings &aSettings)
    : m_FontData(aData.begin(), aData.end())
    , m_AtlasEdge(aSettings.atlas_edge)
    , m_MaxAtlasEdge(std::max(aSettings.atlas_edge, aSettings.max_atlas_edge))
    , m_InitialAtlasEdge(aSettings.atlas_edge)
    , m_Atlas(aSettings.atlas_edge * aSettings.atlas_edge, 0)
    , m_Nodes(aSettings.atlas_edge)
    {
        if (aPixelSize <= 0) throw exception(std::string(TAG).append("pixel size must be positive"));

        if (aSettings.atlas_edge == 0)
            throw exception(std::string(TAG).append("atlas edge must be positive"));

        const auto faces = stbtt_GetNumberOfFonts(m_FontData.data());

        if (faces <= 0)
            throw exception(std::string(TAG).append("data does not begin a truetype font"));

        if (aSettings.face >= static_cast<size_t>(faces)) throw exception(std::string(TAG)
            .append("face ").append(std::to_string(aSettings.face))
            .append(" was asked for, and the data holds ").append(std::to_string(faces)));

        const auto offset = stbtt_GetFontOffsetForIndex(m_FontData.data(),
            static_cast<int>(aSettings.face));

        if (offset < 0) throw exception(std::string(TAG).append("data does not begin a truetype font"));

        if (!stbtt_InitFont(&m_Info, m_FontData.data(), offset))
            throw exception(std::string(TAG).append("truetype data could not be parsed"));

        m_Scale = aSettings.sizing == font_sizing::em
            ? stbtt_ScaleForMappingEmToPixels(&m_Info, aPixelSize)
            : stbtt_ScaleForPixelHeight(&m_Info, aPixelSize);

        int ascent(0), descent(0), lineGap(0);

        stbtt_GetFontVMetrics(&m_Info, &ascent, &descent, &lineGap);

        m_Ascent = static_cast<float>(ascent) * m_Scale;
        m_Descent = static_cast<float>(descent) * m_Scale;
        m_LineHeight = static_cast<float>(ascent - descent + lineGap) * m_Scale;

        reset_packing();

        ensure_fallback();
    }

    void reset_packing() {
        stbrp_init_target(&m_PackContext, static_cast<int>(m_AtlasEdge), static_cast<int>(m_AtlasEdge),
            m_Nodes.data(), static_cast<int>(m_Nodes.size()));
    }

    bool enlarge() {
        const auto edge = m_AtlasEdge * 2;

        if (edge > m_MaxAtlasEdge) return false;

        m_AtlasEdge = edge;
        m_Atlas.assign(edge * edge, 0);
        m_Nodes.assign(edge, {});

        reset_packing();

        ++m_AtlasGeneration;

        m_LayoutGeneration = m_AtlasGeneration;

        return true;
    }

    void ensure_fallback() {
        m_Fallback = placed();

        while (!pack_index(0, m_Fallback))
            if (!enlarge()) throw exception(std::string(TAG).append(
                "the atlas cannot be made large enough to hold this font's notdef glyph"));
    }

    bool grow() {
        std::vector<char32_t> existing;

        existing.reserve(m_Glyphs.size());

        for (const auto &pair : m_Glyphs) existing.push_back(pair.first);

        if (!enlarge()) return false;

        m_Glyphs.clear();

        ensure_fallback();

        for (const auto codepoint : existing)
            if (!pack(codepoint)) throw exception(std::string(TAG)
                .append("a glyph that fit the previous atlas did not fit a larger one"));

        ++m_AtlasGeneration;

        return true;
    }

    void clear() {
        m_AtlasEdge = m_InitialAtlasEdge;
        m_Atlas.assign(m_AtlasEdge * m_AtlasEdge, 0);
        m_Nodes.assign(m_AtlasEdge, {});

        reset_packing();

        ++m_AtlasGeneration;

        m_LayoutGeneration = m_AtlasGeneration;

        m_Glyphs.clear();

        ensure_fallback();

        ++m_AtlasGeneration;
    }

    bool pack(const char32_t aCodepoint) {
        placed placement;

        if (!pack_index(stbtt_FindGlyphIndex(&m_Info, static_cast<int>(aCodepoint)), placement))
            return false;

        m_Glyphs.emplace(aCodepoint, placement);

        return true;
    }

    bool pack_index(const int index, placed &aOut) {

        int advance(0), leftSideBearing(0);

        stbtt_GetGlyphHMetrics(&m_Info, index, &advance, &leftSideBearing);

        font::glyph glyph;

        glyph.advance = static_cast<float>(advance) * m_Scale;

        int width(0), height(0), xoff(0), yoff(0);

        unsigned char *pBitmap = stbtt_GetGlyphBitmap(&m_Info, m_Scale, m_Scale, index,
            &width, &height, &xoff, &yoff);

        if (pBitmap && width > 0 && height > 0) {
            stbrp_rect rect{};

            rect.w = static_cast<stbrp_coord>(width + PADDING * 2);
            rect.h = static_cast<stbrp_coord>(height + PADDING * 2);

            stbrp_pack_rects(&m_PackContext, &rect, 1);

            if (!rect.was_packed) {
                stbtt_FreeBitmap(pBitmap, nullptr);

                return false;
            }

            const auto x = rect.x + PADDING;
            const auto y = rect.y + PADDING;

            for (int row = 0; row < height; ++row)
                std::copy_n(pBitmap + row * width, width,
                    m_Atlas.begin() + (y + row) * static_cast<int>(m_AtlasEdge) + x);

            const auto edge = static_cast<float>(m_AtlasEdge);

            glyph.s0 = static_cast<float>(x) / edge;
            glyph.t0 = static_cast<float>(y) / edge;
            glyph.s1 = static_cast<float>(x + width) / edge;
            glyph.t1 = static_cast<float>(y + height) / edge;

            aOut.x = static_cast<size_t>(x);
            aOut.y = static_cast<size_t>(y);
            aOut.width = static_cast<size_t>(width);
            aOut.height = static_cast<size_t>(height);

            glyph.left = static_cast<float>(xoff);
            glyph.top = -static_cast<float>(yoff); 
            glyph.width = static_cast<float>(width);
            glyph.height = static_cast<float>(height);

            ++m_AtlasGeneration;
        }

        if (pBitmap) stbtt_FreeBitmap(pBitmap, nullptr);

        aOut.glyph = glyph;
        aOut.generation = m_AtlasGeneration;

        return true;
    }

    const font::glyph *rasterise(const char32_t aCodepoint) {
        while (!pack(aCodepoint)) if (!grow()) return nullptr;

        return &m_Glyphs.find(aCodepoint)->second.glyph;
    }
};

const font::glyph *font::try_get(const char32_t aCodepoint) {
    const auto found = m_pImpl->m_Glyphs.find(aCodepoint);

    if (found != m_pImpl->m_Glyphs.end()) return &found->second.glyph;

    return m_pImpl->rasterise(aCodepoint);
}

const font::glyph &font::fallback() const { return m_pImpl->m_Fallback.glyph; }

std::optional<font::atlas_region> font::atlas_changed_since(const size_t aGeneration) const {
    if (aGeneration < m_pImpl->m_LayoutGeneration) return {};

    if (aGeneration >= m_pImpl->m_AtlasGeneration) return atlas_region{0, 0, 0, 0};

    size_t x0 = m_pImpl->m_AtlasEdge, y0 = m_pImpl->m_AtlasEdge, x1 = 0, y1 = 0;

    const auto include = [&](const impl::placed &aPlaced) {
        if (aPlaced.generation <= aGeneration || aPlaced.width == 0) return;

        x0 = std::min(x0, aPlaced.x);
        y0 = std::min(y0, aPlaced.y);
        x1 = std::max(x1, aPlaced.x + aPlaced.width);
        y1 = std::max(y1, aPlaced.y + aPlaced.height);
    };

    include(m_pImpl->m_Fallback);

    for (const auto &pair : m_pImpl->m_Glyphs) include(pair.second);

    if (x1 <= x0 || y1 <= y0) return atlas_region{0, 0, 0, 0};

    return atlas_region{x0, y0, x1 - x0, y1 - y0};
}

void font::clear() { m_pImpl->clear(); }

float font::kerning(const char32_t aLeft, const char32_t aRight) const {
    return static_cast<float>(stbtt_GetCodepointKernAdvance(&m_pImpl->m_Info,
        static_cast<int>(aLeft), static_cast<int>(aRight))) * m_pImpl->m_Scale;
}

texture_data::view font::atlas() const {
    return {
        m_pImpl->m_AtlasEdge,
        m_pImpl->m_AtlasEdge,
        texture::format::grey,
        m_pImpl->m_Atlas.data()
    };
}

size_t font::atlas_generation() const { return m_pImpl->m_AtlasGeneration; }

float font::line_height() const { return m_pImpl->m_LineHeight; }

float font::ascent() const { return m_pImpl->m_Ascent; }

float font::descent() const { return m_pImpl->m_Descent; }

size_t font::atlas_edge() const { return m_pImpl->m_AtlasEdge; }

font::font(std::span<const texture_data::encoded_byte> aTrueTypeData,
    const float aPixelSize,
    const font_settings aSettings)
: m_pImpl(std::make_unique<impl>(aTrueTypeData, aPixelSize, aSettings))
{}

font::~font() = default;

font::font(font &&) noexcept = default;

font &font::operator=(font &&) noexcept = default;

