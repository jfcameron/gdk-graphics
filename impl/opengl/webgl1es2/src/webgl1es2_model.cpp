// © Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/opengl.h>

#include <gdk/webgl1es2_model.h>

#include <iostream>
#include <stdexcept>

using namespace gdk;

static constexpr char TAG[] = "webgl1es2_model";

const jfc::shared_proxy_ptr<gdk::webgl1es2_model> webgl1es2_model::Quad([]()
{
    std::vector<webgl1es2_model::attribute_component_data_type> pos({
        // x,    y,    z
        1.0f, 1.0f, 0.0f, // 1--0
        0.0f, 1.0f, 0.0f, // | /
        0.0f, 0.0f, 0.0f, // 2
        1.0f, 1.0f, 0.0f, //    0
        0.0f, 0.0f, 0.0f, //  / |
        1.0f, 0.0f, 0.0f, // 1--2
    });
    
    // Center the quad
    for (size_t i(0); i < (pos.size()); i += 3)
    {
        pos[i + 0] -= 0.5f;
        pos[i + 1] -= 0.5f;
    }
    
    std::vector<webgl1es2_model::attribute_component_data_type> uv({
        // u,    v
        1.0f, 0.0f, // 1--0
        0.0f, 0.0f, // | /
        0.0f, 1.0f, // 2
        1.0f, 0.0f, //    0
        0.0f, 1.0f, //  / |
        1.0f, 1.0f, // 1--2
    });

    return new gdk::webgl1es2_model(model::UsageHint::Static, {
    {
        { 
            "a_Position",
            {
                &pos.front(),
                pos.size(),
                3
            }
        },
        { 
            "a_UV",
            {
                &uv.front(),
                uv.size(),
                2
            }
        }
    }});
});

const jfc::shared_proxy_ptr<gdk::webgl1es2_model> webgl1es2_model::Cube([]()
{
    float size(1.f);
    float hsize(size/2.f);

    std::vector<webgl1es2_model::attribute_component_data_type> pos({
        //        x,           y,      z,
        size -hsize, size -hsize, -hsize, // 2--0
        0.0f -hsize, 0.0f -hsize, -hsize, // | /
        0.0f -hsize, size -hsize, -hsize, // 1
        size -hsize, size -hsize, -hsize, //    0
        size -hsize, 0.0f -hsize, -hsize, //  / |
        0.0f -hsize, 0.0f -hsize, -hsize, // 2--1 
        //        x,           y,      z,
        size -hsize, size -hsize,  hsize, // 1--0
        0.0f -hsize, size -hsize,  hsize, // | /
        0.0f -hsize, 0.0f -hsize,  hsize, // 2
        size -hsize, size -hsize,  hsize, //    0
        0.0f -hsize, 0.0f -hsize,  hsize, //  / |
        size -hsize, 0.0f -hsize,  hsize, // 1--2 
        //        x,           y       z,
        0.0f -hsize, size -hsize,  hsize, // 2--0
        0.0f -hsize, size -hsize, -hsize, // | /
        0.0f -hsize, 0.0f -hsize, -hsize, // 1
        0.0f -hsize, size -hsize,  hsize, //    0
        0.0f -hsize, 0.0f -hsize, -hsize, //  / |
        0.0f -hsize, 0.0f -hsize,  hsize, // 2--1 
        //        x,           y,      z,
        size -hsize, size -hsize,  hsize, // 2--0
        size -hsize, 0.0f -hsize, -hsize, // | /
        size -hsize, size -hsize, -hsize, // 1
        size -hsize, size -hsize,  hsize, //    0
        size -hsize, 0.0f -hsize,  hsize, //  / |
        size -hsize, 0.0f -hsize, -hsize, // 2--1 
        //        x,           y,      z,
        size -hsize, 0.0f -hsize, -hsize, // 2--0
        0.0f -hsize, 0.0f -hsize,  hsize, // | /
        0.0f -hsize, 0.0f -hsize, -hsize, // 1
        size -hsize, 0.0f -hsize, -hsize, //    0
        size -hsize, 0.0f -hsize,  hsize, //  / |
        0.0f -hsize, 0.0f -hsize,  hsize, // 2--1 
        //        x,           y,      z,
        size -hsize, 1.0f -hsize, -hsize, // 1--0
        0.0f -hsize, 1.0f -hsize, -hsize, // | /
        0.0f -hsize, 1.0f -hsize,  hsize, // 2
        size -hsize, 1.0f -hsize, -hsize, //    0
        0.0f -hsize, 1.0f -hsize,  hsize, //  / |
        size -hsize, 1.0f -hsize,  hsize, // 1--2             
    });

    std::vector<webgl1es2_model::attribute_component_data_type> uv({
        //u,   v,
        0.0, 0.0, // 2--0
        1.0, 1.0, // | /
        1.0, 0.0, // 1
        0.0, 0.0, //    0
        0.0, 1.0, //  / |
        1.0, 1.0, // 2--1 
        //u,   v,
        1.0, 0.0, // 1--0
        0.0, 0.0, // | /
        0.0, 1.0, // 2
        1.0, 0.0, //    0
        0.0, 1.0, //  / |
        1.0, 1.0, // 1--2 
        //u,   v,
        1.0, 0.0, // 2--0
        0.0, 0.0, // | /
        0.0, 1.0, // 1
        1.0, 0.0, //    0
        0.0, 1.0, //  / |
        1.0, 1.0, // 2--1 
        //u,   v,
        0.0, 0.0, // 2--0
        1.0, 1.0, // | /
        1.0, 0.0, // 1
        0.0, 0.0, //    0
        0.0, 1.0, //  / |
        1.0, 1.0, // 2--1 
        //u,   v,
        1.0, 0.0, // 2--0
        0.0, 1.0, // | /
        0.0, 0.0, // 1
        1.0, 0.0, //    0
        1.0, 1.0, //  / |
        0.0, 1.0, // 2--1 
        //u,   v,
        1.0, 0.0, // 1--0
        0.0, 0.0, // | /
        0.0, 1.0, // 2
        1.0, 0.0, //    0
        0.0, 1.0, //  / |
        1.0, 1.0, // 1--2             
    });
    
    std::vector<webgl1es2_model::attribute_component_data_type> normal({
        //Nx,   Ny,   Nz, North
        +0.0, +0.0, -1.0, // 2--0
        +0.0, +0.0, -1.0, // | /
        +0.0, +0.0, -1.0, // 1
        +0.0, +0.0, -1.0, //    0
        +0.0, +0.0, -1.0, //  / |
        +0.0, +0.0, -1.0, // 2--1 
        //Nx,   Ny,   Nz, South
        +0.0, +0.0, +1.0, // 1--0
        +0.0, +0.0, +1.0, // | /
        +0.0, +0.0, +1.0, // 2
        +0.0, +0.0, +1.0, //    0
        +0.0, +0.0, +1.0, //  / |
        +0.0, +0.0, +1.0, // 1--2 
        //Nx,   Ny,   Nz, West
        -1.0, +0.0, +0.0, // 2--0
        -1.0, +0.0, +0.0, // | /
        -1.0, +0.0, +0.0, // 1
        -1.0, +0.0, +0.0, //    0
        -1.0, +0.0, +0.0, //  / |
        -1.0, +0.0, +0.0, // 2--1 
        //Nx,   Ny,   Nz, East
        +1.0, +0.0, +0.0, // 2--0
        +1.0, +0.0, +0.0, // | /
        +1.0, +0.0, +0.0, // 1
        +1.0, +0.0, +0.0, //    0
        +1.0, +0.0, +0.0, //  / |
        +1.0, +0.0, +0.0, // 2--1 
        //Nx,  Ny,    Nz, Down
        +0.0, -1.0, +0.0, // 2--0
        +0.0, -1.0, +0.0, // | /
        +0.0, -1.0, +0.0, // 1
        +0.0, -1.0, +0.0, //    0
        +0.0, -1.0, +0.0, //  / |
        +0.0, -1.0, +0.0, // 2--1 
        //Nx,   Ny,   Nz, Up
        +0.0, +1.0, +0.0, // 1--0
        +0.0, +1.0, +0.0, // | /
        +0.0, +1.0, +0.0, // 2
        +0.0, +1.0, +0.0, //    0
        +0.0, +1.0, +0.0, //  / |
        +0.0, +1.0, +0.0, // 1--2             
    });
    
    return new gdk::webgl1es2_model(model::UsageHint::Static, {
    {
        { 
            "a_Position",
            {
                &pos.front(),
                pos.size(),
                3
            }
        },
        { 
            "a_UV",
            {
                &uv.front(),
                uv.size(),
                2
            }
        },
        { 
            "a_Normal",
            {
                &uv.front(),
                uv.size(),
                2
            }
        }
    }});
});

static inline GLenum webgl1es2_modelTypeToOpenGLDrawType(const webgl1es2_model::Type aType)
{
    switch (aType)
    {
        case webgl1es2_model::Type::Dynamic: return GL_DYNAMIC_DRAW;        
        case webgl1es2_model::Type::Static: return GL_STATIC_DRAW;
        case webgl1es2_model::Type::Stream: return GL_STREAM_DRAW;        
    }

    throw std::invalid_argument("unhandled vertex data type");
}

static inline webgl1es2_model::Type vertexDataUsageHint_to_webgl1es2ModelType(const model::UsageHint aUsageHint)
{
    switch (aUsageHint)
    {
        case model::UsageHint::Static: return webgl1es2_model::Type::Dynamic;
        case model::UsageHint::Dynamic: return webgl1es2_model::Type::Static;
        case model::UsageHint::Streaming: return webgl1es2_model::Type::Stream;
    }
    
    throw std::invalid_argument("unhandled usage hint type");
}

static inline GLenum PrimitiveModeToOpenGLPrimitiveType(const webgl1es2_model::PrimitiveMode aPrimitiveMode)
{
    switch (aPrimitiveMode)
    {
        case webgl1es2_model::PrimitiveMode::Points: return GL_POINTS;
        case webgl1es2_model::PrimitiveMode::Lines: return GL_LINES;
        case webgl1es2_model::PrimitiveMode::LineStrip: return GL_LINE_STRIP; 
        case webgl1es2_model::PrimitiveMode::LineLoop: return GL_LINE_LOOP; 
        case webgl1es2_model::PrimitiveMode::Triangles: return GL_TRIANGLES;            
        case webgl1es2_model::PrimitiveMode::TriangleStrip: return GL_TRIANGLE_STRIP;
        case webgl1es2_model::PrimitiveMode::TriangleFan: return GL_TRIANGLE_FAN;
    }
    
    throw std::invalid_argument("unhandled vertex primitive mode");
}

static inline webgl1es2_model::PrimitiveMode vertexDataPrimitiveMode_to_wegl1es2ModelPrimitiveMode(
    const vertex_data::PrimitiveMode aPrimitiveMode)
{
    switch (aPrimitiveMode)
    {
        case vertex_data::PrimitiveMode::Triangles: 
            return webgl1es2_model::PrimitiveMode::Triangles;
    }
    
    throw std::invalid_argument("unhandled vertex_data::PrimitiveMode");
}

bool webgl1es2_model::operator==(const webgl1es2_model &that)
{
    return
        m_IndexBufferHandle  == that.m_IndexBufferHandle &&
        m_VertexBufferHandle == that.m_VertexBufferHandle;
}

bool webgl1es2_model::operator!=(const webgl1es2_model &that)
{
    return !(*this == that);
}

void webgl1es2_model::bind(const webgl1es2_shader_program &aShaderProgram) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferHandle.get());
    
    m_vertex_format.enableAttributes(aShaderProgram);
}

void webgl1es2_model::draw() const
{
    GLenum primitiveMode = PrimitiveModeToOpenGLPrimitiveType(m_PrimitiveMode);

    if (m_IndexBufferHandle.get() > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle.get());
        
        glDrawElements(primitiveMode,
            m_IndexCount,
            GL_UNSIGNED_SHORT,
            static_cast<void *>(0));
    }
    else glDrawArrays(primitiveMode, 0, m_VertexCount);
}

static webgl1es2_model::Type VertexDataViewUsageHintToType(model::UsageHint usageHint)
{
    switch (usageHint)
    {
        case model::UsageHint::Dynamic: return webgl1es2_model::Type::Dynamic;
        case model::UsageHint::Static: return webgl1es2_model::Type::Static;
        case model::UsageHint::Streaming: return webgl1es2_model::Type::Stream;
    }

    throw std::invalid_argument("unhandled usageHint");
}

void webgl1es2_model::update_vertex_data(const UsageHint &aUsage,
    const vertex_data& vertexDataView)
{
    auto aIndexData = vertexDataView.getIndexData();

    const PrimitiveMode& aPrimitiveMode = vertexDataPrimitiveMode_to_wegl1es2ModelPrimitiveMode(
        vertexDataView.getPrimitiveMode());

    std::vector<webgl1es2_vertex_attribute> attributeFormats;

    for (const auto &[current_name, current_attribute_component_count] : vertexDataView.attribute_format())
    {
        attributeFormats.push_back({current_name, 
            static_cast<short unsigned int>(current_attribute_component_count)});
    }

    webgl1es2_vertex_format vertexFormat(attributeFormats);

    const std::vector<webgl1es2_model::attribute_component_data_type>& aNewwebgl1es2_model(vertexDataView.getData());

    //VBO
    m_vertex_format = vertexFormat;
    m_VertexCount = m_vertex_format.getSumOfAttributeComponents() 
        ? static_cast<GLsizei>(
            aNewwebgl1es2_model.size() / m_vertex_format.getSumOfAttributeComponents())
        : 0;
    
    glBindBuffer (GL_ARRAY_BUFFER, m_VertexBufferHandle.get());

    glBufferData (GL_ARRAY_BUFFER, 
        sizeof(webgl1es2_model::attribute_component_data_type) * aNewwebgl1es2_model.size(), 
        &aNewwebgl1es2_model[0], 
        webgl1es2_modelTypeToOpenGLDrawType(
            vertexDataUsageHint_to_webgl1es2ModelType(aUsage)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_PrimitiveMode = aPrimitiveMode;

    //IBO    
    if (m_IndexCount = (GLsizei)aIndexData.size(); m_IndexCount > 0)
    {
        if (!m_IndexBufferHandle.get())
        {
            auto handle = m_IndexBufferHandle.get();

            glGenBuffers(1, &handle);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle.get());
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
            sizeof(GLushort) * aIndexData.size(), 
            &aIndexData[0], 
            webgl1es2_modelTypeToOpenGLDrawType(
                vertexDataUsageHint_to_webgl1es2ModelType(aUsage)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    
        if (std::string errorCode; glh::GetError(&errorCode)) 
            throw std::runtime_error(std::string(TAG).append(errorCode));
    }
}

webgl1es2_model::webgl1es2_model(const UsageHint &aUsage,
    const vertex_data &aData)
: m_IndexBufferHandle([&]()
{
    GLuint ibo(0);
    
    if (aData.getIndexData().size() > 0)
    {
        glGenBuffers(1, &ibo);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
            sizeof(GLushort) * aData.getIndexData().size(), 
            &aData.getIndexData()[0], 
            webgl1es2_modelTypeToOpenGLDrawType(
                vertexDataUsageHint_to_webgl1es2ModelType(aUsage)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    
        if (std::string errorCode; glh::GetError(&errorCode)) 
            throw std::runtime_error(std::string(TAG).append(errorCode));
    }
    
    return ibo;
}(),
[](const GLuint handle)
{
    glDeleteBuffers(1, &handle);
})
, m_IndexCount((GLsizei)aData.getIndexData().size())
, m_VertexBufferHandle([&]()
{
    GLuint vbo(0);
    glGenBuffers(1, &vbo);

    if (aData.getData().size())
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 
            sizeof(webgl1es2_model::attribute_component_data_type) * aData.getData().size(), 
            &aData.getData()[0], 
            webgl1es2_modelTypeToOpenGLDrawType(
                vertexDataUsageHint_to_webgl1es2ModelType(aUsage)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
	
    if (std::string errorCode; glh::GetError(&errorCode)) 
        throw std::runtime_error(std::string(TAG).append(errorCode));
    
    return vbo;
}(),
[](const GLuint handle)
{
    glDeleteBuffers(1, &handle);
})
, m_vertex_format([&]()
{
    std::vector<webgl1es2_vertex_attribute> attributeFormats;

    for (const auto &[current_name, current_attribute_component_count] : aData.attribute_format())
    {
        attributeFormats.push_back({current_name, 
            static_cast<short unsigned int>(current_attribute_component_count)});
    }

    return attributeFormats;
}())
, m_VertexCount(m_vertex_format.getSumOfAttributeComponents()
    ? static_cast<GLsizei>(aData.getData().size()) / m_vertex_format.getSumOfAttributeComponents()
    : 0)
, m_PrimitiveMode(vertexDataPrimitiveMode_to_wegl1es2ModelPrimitiveMode(aData.getPrimitiveMode()))
{}

