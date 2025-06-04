// © Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/graphics_exception.h>
#include <gdk/opengl.h>
#include <gdk/webgl1es2_model.h>
#include <gdk/model_data.h>

#include <iostream>
#include <stdexcept>

using namespace gdk;

static constexpr auto TAG("webgl1es2_model");

const jfc::lazy_ptr<gdk::webgl1es2_model> webgl1es2_model::Cube([]() {
    model_data data = {{
        { "a_Position", {{
            1.0f, 1.0f, 0.0f, 
            0.0f, 0.0f, 0.0f, 
            0.0f, 1.0f, 0.0f, 
            1.0f, 1.0f, 0.0f, 
            1.0f, 0.0f, 0.0f, 
            0.0f, 0.0f, 0.0f, 

            1.0f, 1.0f, 1.0f, 
            0.0f, 1.0f, 1.0f, 
            0.0f, 0.0f, 1.0f, 
            1.0f, 1.0f, 1.0f, 
            0.0f, 0.0f, 1.0f, 
            1.0f, 0.0f, 1.0f, 

            0.0f, 1.0f, 1.0f, 
            0.0f, 1.0f, 0.0f, 
            0.0f, 0.0f, 0.0f, 
            0.0f, 1.0f, 1.0f, 
            0.0f, 0.0f, 0.0f, 
            0.0f, 0.0f, 1.0f, 

            1.0f, 1.0f, 1.0f, 
            1.0f, 0.0f, 0.0f, 
            1.0f, 1.0f, 0.0f, 
            1.0f, 1.0f, 1.0f, 
            1.0f, 0.0f, 1.0f, 
            1.0f, 0.0f, 0.0f, 

            1.0f, 0.0f, 0.0f, 
            0.0f, 0.0f, 1.0f, 
            0.0f, 0.0f, 0.0f, 
            1.0f, 0.0f, 0.0f, 
            1.0f, 0.0f, 1.0f, 
            0.0f, 0.0f, 1.0f, 

            1.0f, 1.0f, 0.0f, 
            0.0f, 1.0f, 0.0f, 
            0.0f, 1.0f, 1.0f, 
            1.0f, 1.0f, 0.0f, 
            0.0f, 1.0f, 1.0f, 
            1.0f, 1.0f, 1.0f, 
        }, 3 }},
        { "a_UV", {{
            0.0, 0.0, 
            1.0, 1.0, 
            1.0, 0.0, 
            0.0, 0.0, 
            0.0, 1.0, 
            1.0, 1.0, 
            
            1.0, 0.0, 
            0.0, 0.0, 
            0.0, 1.0, 
            1.0, 0.0, 
            0.0, 1.0, 
            1.0, 1.0, 
           
            1.0, 0.0, 
            0.0, 0.0, 
            0.0, 1.0, 
            1.0, 0.0, 
            0.0, 1.0, 
            1.0, 1.0, 
          
            0.0, 0.0, 
            1.0, 1.0, 
            1.0, 0.0, 
            0.0, 0.0, 
            0.0, 1.0, 
            1.0, 1.0, 
         
            1.0, 0.0, 
            0.0, 1.0, 
            0.0, 0.0, 
            1.0, 0.0, 
            1.0, 1.0, 
            0.0, 1.0, 
        
            1.0, 0.0, 
            0.0, 0.0, 
            0.0, 1.0, 
            1.0, 0.0, 
            0.0, 1.0, 
            1.0, 1.0, 
        }, 2 }},
        { "a_Normal", {{
            0.0, 0.0, -1.0, 
            0.0, 0.0, -1.0, 
            0.0, 0.0, -1.0, 
            0.0, 0.0, -1.0, 
            0.0, 0.0, -1.0, 
            0.0, 0.0, -1.0, 

            0.0, 0.0, 1.0, 
            0.0, 0.0, 1.0, 
            0.0, 0.0, 1.0, 
            0.0, 0.0, 1.0, 
            0.0, 0.0, 1.0, 
            0.0, 0.0, 1.0, 

            -1.0, 0.0, 0.0, 
            -1.0, 0.0, 0.0, 
            -1.0, 0.0, 0.0, 
            -1.0, 0.0, 0.0, 
            -1.0, 0.0, 0.0, 
            -1.0, 0.0, 0.0, 

            1.0, 0.0, 0.0, 
            1.0, 0.0, 0.0, 
            1.0, 0.0, 0.0, 
            1.0, 0.0, 0.0, 
            1.0, 0.0, 0.0, 
            1.0, 0.0, 0.0, 

            0.0, -1.0, 0.0, 
            0.0, -1.0, 0.0, 
            0.0, -1.0, 0.0, 
            0.0, -1.0, 0.0, 
            0.0, -1.0, 0.0, 
            0.0, -1.0, 0.0, 

            0.0, 1.0, 0.0, 
            0.0, 1.0, 0.0, 
            0.0, 1.0, 0.0, 
            0.0, 1.0, 0.0, 
            0.0, 1.0, 0.0, 
            0.0, 1.0, 0.0, 
        }, 3 }}
    }};
    data.transform("a_Position", {-0.5f,-0.5f,-0.5f});
    return new gdk::webgl1es2_model(model::usage_hint::upload_once, data);
});

static inline GLenum dataUsageToGLenum(const model::usage_hint aUsageHint) {
    switch (aUsageHint) {
        case model::usage_hint::dynamic: return GL_DYNAMIC_DRAW;
        case model::usage_hint::streaming: return GL_STREAM_DRAW;
        case model::usage_hint::upload_once: return GL_STATIC_DRAW;
    }
    throw graphics_exception("unhandled usage hint type");
}

static inline GLenum vertexDataPrimitiveMode_to_wegl1es2ModelPrimitiveMode(const model_data::primitive_mode aPrimitiveMode) {
    switch (aPrimitiveMode) {
        case model_data::primitive_mode::triangles: return GL_TRIANGLES;
    }
    throw graphics_exception("unhandled model_data::PrimitiveMode");
}

static inline void update_index_data(
    std::optional<jfc::unique_handle<GLuint>> &handle,
    size_t index_count, 
    const GLushort *pIndexBegin, 
    GLenum ausage_hint,
    GLsizei &m_IndexCount) {
    if (m_IndexCount = index_count > 0) {
        if (!handle.has_value()) {
            handle.emplace([&]() {
                GLuint ibo(0);
                
                glGenBuffers(1, &ibo);
                
                return ibo;
            }(),
            [](const GLuint handle) {
                glDeleteBuffers(1, &handle);
            });
        }

        GLuint ibo = handle.value().get();
            
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
            sizeof(GLushort) * index_count, 
            pIndexBegin, 
            ausage_hint);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

        if (const auto error = glh::GetError()) 
            throw graphics_exception(std::string(TAG).append(*error));
    }
    else handle.reset();
}

void webgl1es2_model::bind(const webgl1es2_shader_program &aShaderProgram) const {
    for (const auto &[name, current_attribute] : m_Attributes) {
        if (auto activeAttribute = aShaderProgram.tryGetActiveAttribute(name); 
            activeAttribute.has_value()) {
            glBindBuffer(GL_ARRAY_BUFFER, 
                m_VertexBufferHandles[current_attribute.buffer_handle_index].get());
            
            glEnableVertexAttribArray(activeAttribute->location);
        
            glVertexAttribPointer(
                activeAttribute->location,
                current_attribute.components,
                GL_FLOAT, //TODO: supporting smaller data types would be a good optimization for attributes that do not need the range or precision of floats
                GL_FALSE, 
                0, 
                0  
            );
        }
    }
}

void webgl1es2_model::draw() const {
    if (m_IndexBufferHandle.has_value()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle.value().get());

        glDrawElements(m_PrimitiveMode,
            m_IndexCount,
            GL_UNSIGNED_SHORT,
            static_cast<void *>(0));
    }
    else glDrawArrays(m_PrimitiveMode, 0, m_VertexCount);
}

void webgl1es2_model::upload(const usage_hint &aUsage,
    const model_data &aData) {
    m_PrimitiveMode = vertexDataPrimitiveMode_to_wegl1es2ModelPrimitiveMode(aData.get_primitive_mode());

    update_index_data(m_IndexBufferHandle, 
        aData.indexes().size(), 
        &aData.indexes().front(), 
        dataUsageToGLenum(aUsage),
        m_IndexCount
    );

    //Vertex buffer objects
    {
        const auto &newAttibuteData(aData.attributes());

        if (m_VertexBufferHandles.size() < newAttibuteData.size()) {
            m_VertexBufferHandles.reserve(newAttibuteData.size());

            while (m_VertexBufferHandles.size() < newAttibuteData.size()) {
                m_VertexBufferHandles.push_back({[&]() {
                    GLuint vbo(0);
                    glGenBuffers(1, &vbo);

                    if (const auto error = glh::GetError()) 
                        throw graphics_exception(std::string(TAG).append(*error));

                    return vbo;
                }(),
                [](const GLuint handle) {
                    glDeleteBuffers(1, &handle);
                }});
            }
        }
        else if (m_VertexBufferHandles.size() > newAttibuteData.size()) {
            while (m_VertexBufferHandles.size() > newAttibuteData.size())
                m_VertexBufferHandles.pop_back();
        }

        m_Attributes.clear();

        size_t i(0);
        for (const auto &[name, data] : newAttibuteData) {
            glBindBuffer (GL_ARRAY_BUFFER, m_VertexBufferHandles[i].get());

            glBufferData (GL_ARRAY_BUFFER, 
                sizeof(webgl1es2_model::attribute_component_data_type) * data.components().size(), 
                &data.components()[0], 
                dataUsageToGLenum(aUsage)
            );

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            attribute newAttribute = {
                .buffer_handle_index = i,
                .components = data.number_of_components_per_attribute(),
                .size = data.components().size()
            };

            m_Attributes[name] = newAttribute;

            m_VertexCount = newAttribute.size / newAttribute.components;

            ++i;
        }
    }
}

webgl1es2_model::webgl1es2_model(const usage_hint &aUsage,
    const model_data &aData)
: m_PrimitiveMode(vertexDataPrimitiveMode_to_wegl1es2ModelPrimitiveMode(aData.get_primitive_mode()))
, m_IndexCount((GLsizei)aData.indexes().size()) {
    upload(aUsage, aData);
}

