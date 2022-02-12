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

    return new gdk::webgl1es2_model(model::UsageHint::Static, {
    {
        { 
            "a_Position",
            {
                pos,
                3
            }
        },
        { 
            "a_UV",
            {
                {
                    // u,    v
                    1.0f, 0.0f, // 1--0
                    0.0f, 0.0f, // | /
                    0.0f, 1.0f, // 2
                    1.0f, 0.0f, //    0
                    0.0f, 1.0f, //  / |
                    1.0f, 1.0f, // 1--2
                },
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
                pos,
                3
            }
        },
        { 
            "a_UV",
            {
                uv,
                2
            }
        },
        { 
            "a_Normal",
            {
                normal,
                3
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

static inline webgl1es2_model::Type VertexDataViewUsageHintToType(model::UsageHint usageHint)
{
    switch (usageHint)
    {
        case model::UsageHint::Dynamic: return webgl1es2_model::Type::Dynamic;
        case model::UsageHint::Static: return webgl1es2_model::Type::Static;
        case model::UsageHint::Streaming: return webgl1es2_model::Type::Stream;
    }

    throw std::invalid_argument("unhandled usageHint");
}

static inline void update_index_data(
    std::optional<jfc::unique_handle<GLuint>> &handle,
    size_t index_count, 
    const GLushort *pIndexBegin, 
    GLenum aUsageHint,
    GLsizei &m_IndexCount
    )
{
    if (m_IndexCount = index_count > 0)
    {
        if (!handle.has_value())
        {
            handle.emplace([&]()
            {
                GLuint ibo(0);
                
                glGenBuffers(1, &ibo);
                
                return ibo;
            }(),
            [](const GLuint handle)
            {
                glDeleteBuffers(1, &handle);
            });
        }

        GLuint ibo = handle.value().get();
            
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
            sizeof(GLushort) * index_count, 
            pIndexBegin, 
            aUsageHint);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

        if (std::string errorCode; glh::GetError(&errorCode)) 
            throw std::runtime_error(std::string(TAG).append(errorCode));
    }
    else handle.reset();
}

void webgl1es2_model::bind(const webgl1es2_shader_program &aShaderProgram) const
{
    for (const auto &[name, current_attribute] : m_Attributes)
    {
        if (auto activeAttribute = aShaderProgram.tryGetActiveAttribute(name); 
            activeAttribute.has_value())
        {
            glBindBuffer(GL_ARRAY_BUFFER, 
                m_VertexBufferHandles[current_attribute.buffer_handle_index].get());
            
            glEnableVertexAttribArray(activeAttribute->location);
        
            glVertexAttribPointer(
                activeAttribute->location, //index of the shader program's matching attribute
                current_attribute.components, //# of components per individual attribute
                GL_FLOAT, //component type TODO: support smaller types
                GL_FALSE, //^normalize fixed-point values. fixed-point is one of the possible component types
                0, //stride: distance between consecutive attributes. Would only be nonzero if data is interleaved
                0  //offset to the first component. only nonzero if interleaved and not the first attribute.
            );
        }
    }
}

void webgl1es2_model::draw() const
{
    const GLenum primitiveMode(PrimitiveModeToOpenGLPrimitiveType(m_PrimitiveMode));

    if (m_IndexBufferHandle.has_value())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle.value().get());

        glDrawElements(primitiveMode,
            m_IndexCount,
            GL_UNSIGNED_SHORT,
            static_cast<void *>(0));
    }
    else glDrawArrays(primitiveMode, 0, m_VertexCount);
}

void webgl1es2_model::update_vertex_data(const UsageHint &aUsage,
    const vertex_data &aData)
{
    //Primitive mode
    {
        m_PrimitiveMode = vertexDataPrimitiveMode_to_wegl1es2ModelPrimitiveMode(
            aData.primitive_mode());
    }

    //Index buffer object (optional)
    {
        update_index_data(m_IndexBufferHandle, 
            aData.getIndexData().size(), 
            &aData.getIndexData()[0], 
            webgl1es2_modelTypeToOpenGLDrawType(
                vertexDataUsageHint_to_webgl1es2ModelType(aUsage)),
            m_IndexCount);
    }

    //Vertex buffer objects
    {
        const auto &newAttibuteData(aData.data());

        if (m_VertexBufferHandles.size() < newAttibuteData.size())
        {
            m_VertexBufferHandles.reserve(newAttibuteData.size());

            while (m_VertexBufferHandles.size() < newAttibuteData.size())
            {
                m_VertexBufferHandles.push_back({[&]()
                {
                    GLuint vbo(0);
                    glGenBuffers(1, &vbo);

                    if (std::string errorCode; glh::GetError(&errorCode)) 
                    throw std::runtime_error(std::string(TAG).append(errorCode));

                    return vbo;
                }(),
                [](const GLuint handle)
                {
                    glDeleteBuffers(1, &handle);
                }});
            }
        }
        else if (m_VertexBufferHandles.size() > newAttibuteData.size())
        {
            while (m_VertexBufferHandles.size() > newAttibuteData.size())
                m_VertexBufferHandles.pop_back();
        }

        m_Attributes.clear();

        size_t i(0);
        
        for (const auto &[name, data] : newAttibuteData)
        {
            glBindBuffer (GL_ARRAY_BUFFER, m_VertexBufferHandles[i].get());

            glBufferData (GL_ARRAY_BUFFER, 
                sizeof(webgl1es2_model::attribute_component_data_type) * data.m_Components.size(), 
                &data.m_Components[0], 
                webgl1es2_modelTypeToOpenGLDrawType(
                    vertexDataUsageHint_to_webgl1es2ModelType(aUsage)));

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            attribute newAttribute =
            {
                .buffer_handle_index = i,
                .components = data.m_ComponentCount,
                .size = data.m_Components.size()
            };

            m_Attributes[name] = newAttribute;

            i++;

            //TODO: likely vertexcount should come from the vertex_data object, which will
            // check that all attrib arrays have the same vertex count at construction/update time
            //m_VertexCount = aData.interleaved_data_size() / aData.vertex_size();
            m_VertexCount = newAttribute.size / newAttribute.components;
        }
    }
}

webgl1es2_model::webgl1es2_model(const UsageHint &aUsage,
    const vertex_data &aData)
: m_PrimitiveMode(vertexDataPrimitiveMode_to_wegl1es2ModelPrimitiveMode(aData.primitive_mode()))
, m_IndexCount((GLsizei)aData.getIndexData().size())
{
    update_vertex_data(aUsage, aData);
}

