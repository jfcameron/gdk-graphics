// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/text_modeler.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/texture_data.h>

#include <algorithm>
#include <vector>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
    constexpr int SPACES_PER_TAB = 4;
    constexpr char32_t REPLACEMENT_CHARACTER = 0xFFFD;

    [[nodiscard]] char32_t decode_utf8(const std::string_view aText, size_t &aIndex) {
        const auto remaining = aText.size() - aIndex;

        const auto first = static_cast<unsigned char>(aText[aIndex]);

        const auto continuation = [&](const size_t aOffset) {
            return (static_cast<unsigned char>(aText[aIndex + aOffset]) & 0xC0) == 0x80;
        };

        if (first < 0x80) {
            aIndex += 1;

            return first;
        }
        else if ((first & 0xE0) == 0xC0 && remaining >= 2 && continuation(1)) {
            const char32_t value = ((first & 0x1Fu) << 6)
                | (static_cast<unsigned char>(aText[aIndex + 1]) & 0x3Fu);

            aIndex += 2;

            return value < 0x80 ? REPLACEMENT_CHARACTER : value; 
        }
        else if ((first & 0xF0) == 0xE0 && remaining >= 3 && continuation(1) && continuation(2)) {
            const char32_t value = ((first & 0x0Fu) << 12)
                | ((static_cast<unsigned char>(aText[aIndex + 1]) & 0x3Fu) << 6)
                | (static_cast<unsigned char>(aText[aIndex + 2]) & 0x3Fu);

            aIndex += 3;

            return value < 0x800 ? REPLACEMENT_CHARACTER : value;
        }
        else if ((first & 0xF8) == 0xF0 && remaining >= 4 && continuation(1) && continuation(2) && continuation(3)) {
            const char32_t value = ((first & 0x07u) << 18)
                | ((static_cast<unsigned char>(aText[aIndex + 1]) & 0x3Fu) << 12)
                | ((static_cast<unsigned char>(aText[aIndex + 2]) & 0x3Fu) << 6)
                | (static_cast<unsigned char>(aText[aIndex + 3]) & 0x3Fu);

            aIndex += 4;

            return (value < 0x10000 || value > 0x10FFFF) ? REPLACEMENT_CHARACTER : value;
        }

        aIndex += 1;

        return REPLACEMENT_CHARACTER;
    }

    void coverage_to_rgba(const texture_data::view aCoverage,
        const size_t aX, const size_t aY, const size_t aWidth, const size_t aHeight,
        texture_data::channel_data &aOut) {
        aOut.assign(aWidth * aHeight * 4, 0xFF);

        for (size_t row = 0; row < aHeight; ++row)
            for (size_t column = 0; column < aWidth; ++column) {
                aOut[(row * aWidth + column) * 4 + 3] =
                    aCoverage.data[(aY + row) * aCoverage.width + aX + column];
            }
    }
}

const_model_ptr_type text_modeler::model() const { return m_BatchModeler.model(); }

const_material_ptr_type text_modeler::material() const { return m_BatchModeler.material(); }

vector2_type text_modeler::size() const { return m_Size; }

void text_modeler::set_text(const std::string_view aUTF8) {
    std::vector<char32_t> codepoints;

    for (size_t i = 0; i < aUTF8.size();) codepoints.push_back(decode_utf8(aUTF8, i));

    m_FallbackGlyphCount = 0;

    for (const auto codepoint : codepoints) {
        if (codepoint == U'\n' || codepoint == U'\r') continue;

        if (!m_pFont->try_get(codepoint == U'\t' ? U' ' : codepoint)) ++m_FallbackGlyphCount;
    }

    float penX(0), penY(0), widest(0);

    char32_t previous(0);

    for (const auto codepoint : codepoints) {

        if (codepoint == U'\n') {
            widest = std::max(widest, penX);

            penX = 0;
            penY -= m_pFont->line_height();

            previous = 0;

            continue;
        }

        if (codepoint == U'\r') continue;

        if (codepoint == U'\t') {
            const auto *const pSpace = m_pFont->try_get(U' ');

            penX += (pSpace ? *pSpace : m_pFont->fallback()).advance * SPACES_PER_TAB;

            previous = 0;

            continue;
        }

        if (previous) penX += m_pFont->kerning(previous, codepoint);

        const auto *const pGlyph = m_pFont->try_get(codepoint);

        const auto &glyph = pGlyph ? *pGlyph : m_pFont->fallback();

        if (glyph.width > 0 && glyph.height > 0) {
            const auto left = (penX + glyph.left) * m_Scale;
            const auto right = left + glyph.width * m_Scale;
            const auto top = (penY + glyph.top) * m_Scale;
            const auto bottom = top - glyph.height * m_Scale;

            model_data data({
                { "a_Position", {{
                    right, top,    0.0f,
                    left,  top,    0.0f,
                    left,  bottom, 0.0f,
                    right, top,    0.0f,
                    left,  bottom, 0.0f,
                    right, bottom, 0.0f,
                }, 3 }},
                { "a_UV", {{
                    glyph.s1, glyph.t0,
                    glyph.s0, glyph.t0,
                    glyph.s0, glyph.t1,
                    glyph.s1, glyph.t0,
                    glyph.s0, glyph.t1,
                    glyph.s1, glyph.t1,
                }, 2 }}
            });

            m_BatchModeler.push_back(data);
        }

        penX += glyph.advance;

        previous = codepoint;
    }

    widest = std::max(widest, penX);

    m_Size = {
        widest * m_Scale,
        (penY == 0 ? m_pFont->line_height() : m_pFont->line_height() - penY) * m_Scale
    };
}

size_t text_modeler::uploaded_atlas_generation() const { return m_UploadedAtlasGeneration; }

size_t text_modeler::fallback_glyph_count() const { return m_FallbackGlyphCount; }

void text_modeler::upload() {
    if (const auto generation = m_pFont->atlas_generation(); generation != m_UploadedAtlasGeneration) {
        const auto coverage = m_pFont->atlas();

        const auto changed = m_pFont->atlas_changed_since(m_UploadedAtlasGeneration);

        if (changed && m_pAtlasTexture) {
            if (changed->width > 0 && changed->height > 0) {
                coverage_to_rgba(coverage, changed->x, changed->y, changed->width, changed->height,
                    m_Rgba);

                m_pAtlasTexture->update_data(
                    { changed->width, changed->height, texture::format::rgba, m_Rgba.data() },
                    changed->x, changed->y);
            }
        }
        else {
            coverage_to_rgba(coverage, 0, 0, coverage.width, coverage.height, m_Rgba);

            m_pAtlasTexture = m_pContext->make_texture(
                { coverage.width, coverage.height, texture::format::rgba, m_Rgba.data() },
                texture::wrap_mode::repeat, texture::wrap_mode::repeat,
                texture::filter_mode::smooth);

            m_pMaterial->set_texture("_Texture", m_pAtlasTexture);
        }

        m_UploadedAtlasGeneration = generation;
    }

    m_BatchModeler.upload();
}

text_modeler::text_modeler(context_ptr_type aContext,
    std::shared_ptr<font> aFont,
    material_ptr_type aMaterial,
    const float aScale)
: m_pFont(std::move(aFont))
, m_pContext(aContext)
, m_pMaterial(aMaterial ? aMaterial : [&]() {
    auto pMaterial(aContext->make_material(
    aContext->make_alpha_blend_shader(),
        material::render_mode::transparent));

    pMaterial->set_vector2("_UVScale", {1, 1});
    pMaterial->set_vector2("_UVOffset", {0, 0});

    return pMaterial;
}())
, m_BatchModeler(aContext, m_pMaterial)
, m_Scale(aScale > 0 
        ? aScale 
        : 1.f / m_pFont->line_height())
{}

