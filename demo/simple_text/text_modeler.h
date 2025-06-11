// © Joseph Cameron - All Rights Reserved

#ifndef GDK_TEXT_RENDERER_H
#define GDK_TEXT_RENDERER_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <gdk/graphics_context.h>
#include <gdk/material.h>
#include <gdk/ext/batch_modeler.h>

/// \brief very simple ASCII text modeler.
///
/// glyph rasters and character map are hardcoded for simplicity
/// TODO: generalize this into a rasterized text class, support for multiple resolutions.
/// TODO: think about font rasterizing. sibling class "font_modeler" or something.
/// logographic writing requires this (rasterizing glyphs just in time to a buffer) simply too many characters to prerasterize them all.
///
class text_modeler final {
public:
    //! get the model, contains the uploaded model data for the batch
    const gdk::const_graphics_model_ptr_type model() const;

    //! get the material, used to render the model
    const gdk::const_graphics_material_ptr_type material() const;

    //! upload the model data to the model and clear the data
    void upload();
   
    /// \brief renders new text
    void set_text(const std::string &string);

    /// \brief constructs a new instance
    text_modeler(gdk::graphics_context_ptr_type pContext, gdk::graphics_material_ptr_type aMaterial = {});

private:
    gdk::batch_modeler m_BatchModeler;
};

#endif

