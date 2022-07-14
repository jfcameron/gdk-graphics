// © Joseph Cameron - All Rights Reserved

#ifndef GDK_TEXT_RENDERER_H
#define GDK_TEXT_RENDERER_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <gdk/graphics_context.h>
#include <gdk/material.h>

/// \brief very simple ASCII text modeler.
///
/// glyph rasters and character map are hardcoded for simplicity
class text_renderer final
{
public:
    /// \brief constructs a new instance
    text_renderer(std::shared_ptr<gdk::graphics::context> pContext);
   
    /// \brief renders new text
    void set_text(const std::string &string);

    /// \brief gets the material used by text_renderers. shared among all text_renderers
    std::shared_ptr<gdk::material> material() const;

    /// \brief gets the vertex data for the text
    const gdk::vertex_data &vertex_data() const;

private:
    gdk::vertex_data m_VertexBuffer;
   
    std::shared_ptr<gdk::material> m_pMaterial;
};

#endif

