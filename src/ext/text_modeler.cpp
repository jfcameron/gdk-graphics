// © Joseph Cameron - MIT License

#include <gdk/graphics/ext/text_modeler.h>
#include <gdk/graphics/ext/utf8.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/texture_data.h>

#include <algorithm>
#include <vector>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
    constexpr int SPACES_PER_TAB = 4;
}

const_model_ptr_type text_modeler::model() const { return m_pModel; }

const_material_ptr_type text_modeler::material() const { return m_pMaterial; }

vector2_type text_modeler::size() const { return m_Size; }

void text_modeler::set_text(const std::string_view aUTF8) {
    m_Text.assign(aUTF8);

    lay_out();
}

void text_modeler::lay_out() {
    const std::string_view text(m_Text);

    std::vector<char32_t> codepoints;

    for (size_t i = 0; i < text.size();) codepoints.push_back(decode_utf8(text, i));

    m_FallbackGlyphCount = 0;

    for (const auto codepoint : codepoints) {
        if (codepoint == U'\n' || codepoint == U'\r') continue;

        if (!m_pFont->try_get(codepoint == U'\t' ? U' ' : codepoint)) ++m_FallbackGlyphCount;
    }

    m_LaidOutAtlasGeneration = m_pFont->atlas_generation();

    m_Layout = model_data();

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

            m_Layout += data;
        }

        penX += glyph.advance;

        previous = codepoint;
    }

    widest = std::max(widest, penX);

    m_Size = {
        widest * m_Scale,
        (penY == 0 ? m_pFont->line_height() : m_pFont->line_height() - penY) * m_Scale
    };

    m_LayoutSent = false;
}

size_t text_modeler::uploaded_atlas_generation() const { return m_pFontTexture->uploaded_atlas_generation(); }

size_t text_modeler::fallback_glyph_count() const { return m_FallbackGlyphCount; }

void text_modeler::upload() {
    if (m_pFont->atlas_generation() != m_LaidOutAtlasGeneration
        && !m_pFont->atlas_changed_since(m_LaidOutAtlasGeneration)) lay_out();

    m_pFontTexture->upload();

    if (const auto pTexture = m_pFontTexture->texture(); pTexture != m_pBoundTexture) {
        m_pMaterial->set_texture("_Texture", pTexture);

        m_pBoundTexture = pTexture;
    }

    if (!m_LayoutSent) {
        m_pModel->upload(model::usage_hint::dynamic, m_Layout);

        m_LayoutSent = true;
    }
}

text_modeler::text_modeler(context_ptr_type aContext,
    std::shared_ptr<font> aFont,
    material_ptr_type aMaterial,
    const float aScale)
: text_modeler(aContext, std::make_shared<font_texture>(aContext, std::move(aFont)), std::move(aMaterial), aScale)
{}

text_modeler::text_modeler(context_ptr_type aContext,
    std::shared_ptr<font_texture> aFontTexture,
    material_ptr_type aMaterial,
    const float aScale)
: m_pFont(aFontTexture->typeface())
, m_pFontTexture(std::move(aFontTexture))
, m_pMaterial(aMaterial ? aMaterial : [&]() {
    auto pMaterial(aContext->make_material(
    aContext->make_alpha_blend_shader(),
        material::render_mode::transparent));

    pMaterial->set_vector2("_UVScale", {1, 1});
    pMaterial->set_vector2("_UVOffset", {0, 0});

    return pMaterial;
}())
, m_pModel(aContext->make_model())
, m_Scale(aScale > 0 ? aScale : m_pFont->line_height() > 0 ? 1.f / m_pFont->line_height() : 1.f)
{}
