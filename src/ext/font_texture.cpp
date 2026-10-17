// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/font_texture.h>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
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

font_texture::font_texture(context_ptr_type aContext, std::shared_ptr<font> aFont)
: m_pContext(std::move(aContext))
, m_pFont(std::move(aFont))
{}

void font_texture::upload() {
    const auto generation = m_pFont->atlas_generation();

    if (m_pTexture && generation == m_UploadedAtlasGeneration) return;

    const auto coverage = m_pFont->atlas();

    const auto changed = m_pFont->atlas_changed_since(m_UploadedAtlasGeneration);

    if (changed && m_pTexture) {
        if (changed->width > 0 && changed->height > 0) {
            coverage_to_rgba(coverage, changed->x, changed->y, changed->width, changed->height, m_Rgba);

            m_pTexture->update_data(
                { changed->width, changed->height, texture::format::rgba, m_Rgba.data() },
                changed->x, changed->y);
        }
    }
    else {
        coverage_to_rgba(coverage, 0, 0, coverage.width, coverage.height, m_Rgba);

        m_pTexture = m_pContext->make_texture(
            { coverage.width, coverage.height, texture::format::rgba, m_Rgba.data() },
            texture::wrap_mode::repeat, texture::wrap_mode::repeat,
            texture::filter_mode::smooth);

        m_Rgba.clear();
        m_Rgba.shrink_to_fit();
    }

    m_UploadedAtlasGeneration = generation;
}

texture_ptr_type font_texture::texture() const { return m_pTexture; }

size_t font_texture::uploaded_atlas_generation() const { return m_UploadedAtlasGeneration; }

const std::shared_ptr<font> &font_texture::typeface() const { return m_pFont; }
