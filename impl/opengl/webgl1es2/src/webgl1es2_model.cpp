// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/glh.h>
#include <gdk/graphics/exception.h>
#include <gdk/graphics/opengl.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/model_data.h>
#include <gdk/math_constants.h>

#include <iostream>
#include <stdexcept>

using namespace gdk;
using namespace gdk::graphics;

static constexpr auto TAG("webgl1es2_model");

std::shared_ptr<webgl1es2_model> webgl1es2_model::make_sphere() {
    constexpr int latitudeBands = 8;
    constexpr int longitudeBands = 8;
    constexpr float radius = 0.5f;

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> uvs;

    auto get_vertex = [&](int lat, int lon) {
        const float theta = lat * numbers::pi_f / latitudeBands;
        const float phi = lon * 2.0f * numbers::pi_f / longitudeBands;

        const float sinTheta = std::sin(theta);
        const float cosTheta = std::cos(theta);
        const float sinPhi = std::sin(phi);
        const float cosPhi = std::cos(phi);

        const float x = cosPhi * sinTheta;
        const float y = cosTheta;
        const float z = sinPhi * sinTheta;

        const float u = 1.0f - (float(lon) / longitudeBands);
        const float v = 1.0f - (float(lat) / latitudeBands);

        positions.push_back(radius * x);
        positions.push_back(radius * y);
        positions.push_back(radius * z);

        normals.push_back(x);
        normals.push_back(y);
        normals.push_back(z);

        uvs.push_back(u);
        uvs.push_back(v);
    };

    for (int lat = 0; lat < latitudeBands; ++lat) {
        for (int lon = 0; lon < longitudeBands; ++lon) {
            // Triangle 1
            get_vertex(lat, lon);
            get_vertex(lat + 1, lon);
            get_vertex(lat, lon + 1);

            // Triangle 2
            get_vertex(lat + 1, lon);
            get_vertex(lat + 1, lon + 1);
            get_vertex(lat, lon + 1);
        }
    }

    model_data data = {{
        { "a_Position", {positions, 3}},
        { "a_Normal", {normals, 3}},
        { "a_UV", {uvs, 2}}
    }};
    return std::make_shared<webgl1es2_model>(model::usage_hint::upload_once, data);
}

std::shared_ptr<webgl1es2_model> webgl1es2_model::make_cube() {
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
    return std::make_shared<webgl1es2_model>(model::usage_hint::upload_once, data);
}

static inline GLenum dataUsageToGLenum(const model::usage_hint aUsageHint) {
    switch (aUsageHint) {
        case model::usage_hint::dynamic: return GL_DYNAMIC_DRAW;
        case model::usage_hint::streaming: return GL_STREAM_DRAW;
        case model::usage_hint::upload_once: return GL_STATIC_DRAW;
    }
    throw exception("unhandled usage hint type");
}

static inline GLenum vertexDataPrimitiveMode_to_wegl1es2ModelPrimitiveMode(const model_data::primitive_mode aPrimitiveMode) {
    switch (aPrimitiveMode) {
        case model_data::primitive_mode::triangles: return GL_TRIANGLES;
    }
    throw exception("unhandled model_data::PrimitiveMode");
}

static inline void update_index_data(
    std::optional<jfc::unique_handle<GLuint>> &handle,
    size_t index_count, 
    const GLushort *pIndexBegin, 
    GLenum ausage_hint,
    GLsizei &m_IndexCount) {
    m_IndexCount = static_cast<GLsizei>(index_count);

    if (m_IndexCount > 0) {
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
            throw exception(std::string(TAG).append(*error));
    }
    else handle.reset();
}

void webgl1es2_model::bind(const webgl1es2_shader_program &aShaderProgram) const {
    const auto cached = m_BindingCache.find(aShaderProgram.handle());

    const auto &bindings = cached != m_BindingCache.end()
        ? cached->second
        : [&]() -> const std::vector<bound_attribute> & {
            std::vector<bound_attribute> resolved;

            for (const auto &[name, current_attribute] : m_Attributes) {
                const auto active = aShaderProgram.tryGetActiveAttribute(name);

                if (!active.has_value()) continue;

                resolved.push_back({
                    m_VertexBufferHandles[current_attribute.buffer_handle_index].get(),
                    active->location,
                    static_cast<GLint>(current_attribute.components)});
            }

            return m_BindingCache.emplace(aShaderProgram.handle(), std::move(resolved))
                .first->second;
        }();

    for (const auto &each : bindings) {
        glBindBuffer(GL_ARRAY_BUFFER, each.buffer);

        glEnableVertexAttribArray(each.location);

        glVertexAttribPointer(
            each.location,
            each.components,
            GL_FLOAT, //TODO: supporting smaller data types would be a good optimization for attributes that do not need the range or precision of floats
            GL_FALSE,
            0,
            0
        );
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

static void compute_bounds(const model_data &aData, vector3_type &aCentreOut,
    floating_point_type &aRadiusOut) {
    aCentreOut = {0, 0, 0};
    aRadiusOut = 0;

    const auto found = aData.attributes().find("a_Position");

    if (found == aData.attributes().end()) return;

    const auto &components = found->second.components();
    const auto stride = found->second.number_of_components_per_attribute();

    if (stride < 3 || components.size() < 3) return;

    auto minimum = vector3_type{components[0], components[1], components[2]};
    auto maximum = minimum;

    for (std::size_t i = 0; i + 2 < components.size(); i += stride) {
        minimum.x = std::min(minimum.x, components[i + 0]);
        minimum.y = std::min(minimum.y, components[i + 1]);
        minimum.z = std::min(minimum.z, components[i + 2]);

        maximum.x = std::max(maximum.x, components[i + 0]);
        maximum.y = std::max(maximum.y, components[i + 1]);
        maximum.z = std::max(maximum.z, components[i + 2]);
    }

    aCentreOut = {(minimum.x + maximum.x) * 0.5f, (minimum.y + maximum.y) * 0.5f,
        (minimum.z + maximum.z) * 0.5f};

    for (std::size_t i = 0; i + 2 < components.size(); i += stride) {
        const auto dx = components[i + 0] - aCentreOut.x;
        const auto dy = components[i + 1] - aCentreOut.y;
        const auto dz = components[i + 2] - aCentreOut.z;

        aRadiusOut = std::max(aRadiusOut, std::sqrt(dx * dx + dy * dy + dz * dz));
    }
}

void webgl1es2_model::upload(const usage_hint &aUsage,
    const model_data &aData) {
    m_BindingCache.clear();

    compute_bounds(aData, m_BoundsCentre, m_BoundsRadius);

    m_PrimitiveMode = vertexDataPrimitiveMode_to_wegl1es2ModelPrimitiveMode(aData.get_primitive_mode());

    update_index_data(m_IndexBufferHandle,
        aData.indexes().size(),
        aData.indexes().empty() ? nullptr : &aData.indexes().front(),
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
                        throw exception(std::string(TAG).append(*error));

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

            const attribute newAttribute{
                .buffer_handle_index = i,
                .components = data.number_of_components_per_attribute(),
                .size = data.components().size()};

            m_Attributes[name] = newAttribute;

            m_VertexCount = static_cast<GLsizei>(newAttribute.size / newAttribute.components);

            ++i;
        }
    }
}

webgl1es2_model::webgl1es2_model(const usage_hint &aUsage,
    const model_data &aData)
: m_IndexCount(static_cast<GLsizei>(aData.indexes().size()))
, m_PrimitiveMode(vertexDataPrimitiveMode_to_wegl1es2ModelPrimitiveMode(aData.get_primitive_mode())) {
    upload(aUsage, aData);
}

