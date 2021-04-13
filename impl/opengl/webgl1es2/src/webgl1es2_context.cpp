// © 2019 Joseph Cameron - All Rights Reserved

#include <stdexcept>

#include <gdk/webgl1es2_screen_camera.h>
#include <gdk/webgl1es2_context.h>
#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_material.h>
#include <gdk/webgl1es2_model.h>
#include <gdk/webgl1es2_scene.h>
#include <gdk/webgl1es2_shader_program.h>
#include <gdk/webgl1es2_texture_camera.h>

using namespace gdk;

webgl1es2_context::webgl1es2_context()
{}

std::shared_ptr<screen_camera> webgl1es2_context::make_camera() const 
{
    return std::shared_ptr<screen_camera>(new webgl1es2_screen_camera());
}

std::shared_ptr<screen_camera> webgl1es2_context::make_camera(const camera &other) const
{
    return std::shared_ptr<screen_camera>(
        new webgl1es2_screen_camera(static_cast<const webgl1es2_screen_camera &>(other)));
}

std::shared_ptr<texture_camera> webgl1es2_context::make_texture_camera() const
{
    return std::shared_ptr<texture_camera>(new webgl1es2_texture_camera());
}

graphics::context::entity_ptr_type webgl1es2_context::make_entity(gdk::graphics::context::model_shared_ptr_type pModel, 
    gdk::graphics::context::material_shared_ptr_type pMaterial) const
{
    return graphics::context::entity_ptr_type(
        new webgl1es2_entity(
            std::static_pointer_cast<webgl1es2_model>(pModel),
            std::static_pointer_cast<webgl1es2_material>(pMaterial)));
}

graphics::context::entity_ptr_type webgl1es2_context::make_entity(const entity &other) const
{
    return graphics::context::entity_ptr_type(
        new webgl1es2_entity(
            static_cast<const webgl1es2_entity &>(other)));
}

graphics::context::material_ptr_type webgl1es2_context::make_material(gdk::graphics::context::shader_program_shared_ptr_type pShader) const 
{
    return graphics::context::material_ptr_type(
        new webgl1es2_material(
            std::static_pointer_cast<webgl1es2_shader_program>(pShader)));
}

graphics::context::shader_program_ptr_type webgl1es2_context::make_shader(const std::string &aVertexGLSL, const std::string &aFragGLSL) const 
{
    return graphics::context::shader_program_ptr_type(
        new webgl1es2_shader_program(aVertexGLSL, aFragGLSL));
}

graphics::context::built_in_shader_ptr_type webgl1es2_context::get_alpha_cutoff_shader() const
{
    return std::static_pointer_cast<shader_program>(
        static_cast<std::shared_ptr<webgl1es2_shader_program>>(
            webgl1es2_shader_program::AlphaCutOff));
}

graphics::context::built_in_shader_ptr_type webgl1es2_context::get_pink_shader_of_death() const
{
    return std::static_pointer_cast<shader_program>(
        static_cast<std::shared_ptr<webgl1es2_shader_program>>(
            webgl1es2_shader_program::PinkShaderOfDeath));
}

graphics::context::built_in_model_ptr_type webgl1es2_context::get_cube_model() const
{
    return std::static_pointer_cast<model>(
        std::shared_ptr<webgl1es2_model>(webgl1es2_model::Cube));
}

graphics::context::built_in_model_ptr_type webgl1es2_context::get_quad_model() const
{
    return std::static_pointer_cast<model>(
        std::shared_ptr<webgl1es2_model>(webgl1es2_model::Quad));
}

graphics::context::texture_ptr_type webgl1es2_context::make_texture(const texture::image_data_2d_view &imageView) const
{
    webgl1es2_texture::webgl1es2_texture_2d_data_view_type data;
    data.width = imageView.width;
    data.height = imageView.height;

    switch(imageView.format)
    {
        case texture::data_format::rgb: data.format = webgl1es2_texture::format::rgb; break;

        case texture::data_format::rgba: data.format = webgl1es2_texture::format::rgba; break;

        default: throw std::invalid_argument("webgl1es2 context does not support provided image format");
    }

    data.data = imageView.data;
    
    return graphics::context::texture_ptr_type(new webgl1es2_texture(data));
}

graphics::context::texture_ptr_type webgl1es2_context::make_texture(const std::vector<std::underlying_type<std::byte>::type>& aRGBA32PNGData) const
{
	return std::make_unique<gdk::webgl1es2_texture>(webgl1es2_texture::make_from_png_rgba32(aRGBA32PNGData));
}

static webgl1es2_model::Type VertexDataViewUsageHintToType(vertex_data_view::UsageHint usageHint)
{
    switch(usageHint)
    {
        case vertex_data_view::UsageHint::Dynamic: return webgl1es2_model::Type::Dynamic;
        case vertex_data_view::UsageHint::Static: return webgl1es2_model::Type::Static;
        case vertex_data_view::UsageHint::Streaming: return webgl1es2_model::Type::Stream;
    }

    throw std::invalid_argument("unhandled usageHint");
}

//TODO remove copy. This is a memory wasteful adapter between public context api and webgles model ctor. 
//How? mody ctor then in ctor do not interleave, instead append to back of vertexbuffer data. 
//additional advantage of removing format and attribute abstraction from web1gles2 impl. 
//this work needs to be moved into the initing functor for m_VertexHandle or whatver within the model impl, 
//then simplify this method, passthrough params of this method to the ctor
graphics::context::model_ptr_type webgl1es2_context::make_model(const vertex_data_view &vertexDataView) const
{
    if (!vertexDataView.m_AttributeData.size()) 
		throw std::invalid_argument("vertex data view must contain at least one attribute data view");

    auto usageType = VertexDataViewUsageHintToType(vertexDataView.m_Usage);

    std::vector<webgl1es2_vertex_attribute> attributeFormats;

    size_t vertexCount(
        vertexDataView.m_AttributeData.begin()->second.m_DataLength /
        vertexDataView.m_AttributeData.begin()->second.m_ComponentCount);

    if (!vertexCount) throw std::invalid_argument("vertex attribute data must have data");

    for (const auto &[current_name, current_attribute_data_view] : vertexDataView.m_AttributeData)
    {
        attributeFormats.push_back({
            current_name, 
            static_cast<short unsigned int>(current_attribute_data_view.m_ComponentCount)});

        auto currentVertexCount = current_attribute_data_view.m_DataLength /
            current_attribute_data_view.m_ComponentCount;

        if (currentVertexCount != vertexCount) 
			throw std::invalid_argument("attribute data arrays must contribute to the same number of vertexes");
    }

    std::vector<attribute_data_view::attribute_component_type> data;

    const size_t vertexcount = 
        vertexDataView.m_AttributeData.begin()->second.m_DataLength / 
        vertexDataView.m_AttributeData.begin()->second.m_ComponentCount;

    //Interleaver. Adapter required to deal with diff between make_model and model ctor. Replace this
    for (size_t vertexcounter(0); vertexcounter < vertexcount; ++vertexcounter) 
    {
        for (const auto &[current_name, current_attribute_data_view] : vertexDataView.m_AttributeData)
        {
            for (size_t i(0), s(current_attribute_data_view.m_ComponentCount); i < s; ++i)
            {
                data.push_back(*(current_attribute_data_view.m_pData + i + (vertexcounter * s)));
            }
        }
    }

    webgl1es2_vertex_format vertexFormat(attributeFormats);

    return graphics::context::model_ptr_type(new gdk::webgl1es2_model(
        gdk::webgl1es2_model::Type::Static, //Why constant?
        vertexFormat,
        data));
}

graphics::context::scene_ptr_type webgl1es2_context::make_scene() const
{
    return graphics::context::scene_ptr_type(
        new gdk::webgl1es2_scene());
}
