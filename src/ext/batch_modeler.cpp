// © Joseph Cameron - All Rights Reserved

#include <gdk/ext/batch_modeler.h>

using namespace gdk;

batch_modeler::batch_modeler(graphics_context_ptr_type aGraphics, graphics_material_ptr_type aMaterial)
: m_pMaterial(aMaterial)
, m_pModel(aGraphics->make_model())
{}

const_graphics_model_ptr_type batch_modeler::model() const { return m_pModel; }

const_graphics_material_ptr_type batch_modeler::material() const { return m_pMaterial; }

void batch_modeler::push_back(const model_data &aData) { m_BatchData += aData; }

void batch_modeler::upload() {
    m_pModel->upload(model::usage_hint::streaming, m_BatchData);
    m_BatchData.clear();
}

size_t batch_modeler::vertex_count() const { return m_BatchData.vertex_count(); }

