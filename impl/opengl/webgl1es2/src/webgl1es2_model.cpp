// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/opengl.h>

#include <gdk/webgl1es2_model.h>

#include <iostream>
#include <stdexcept>

using namespace gdk;

static constexpr char TAG[] = "model";

const jfc::lazy_ptr<gdk::webgl1es2_model> webgl1es2_model::Quad([]()
{
    float size  = 1.;
    float hsize = size/2.;

    std::vector<GLfloat> data(
    {
        //        x,           y,    z,    u,    v,
        size -hsize, size -hsize, 0.0f, 1.0f, 0.0f, // 1--0
        0.0f -hsize, size -hsize, 0.0f, 0.0f, 0.0f, // | /
        0.0f -hsize, 0.0f -hsize, 0.0f, 0.0f, 1.0f, // 2
        size -hsize, size -hsize, 0.0f, 1.0f, 0.0f, //    0
        0.0f -hsize, 0.0f -hsize, 0.0f, 0.0f, 1.0f, //  / |
        size -hsize, 0.0f -hsize, 0.0f, 1.0f, 1.0f, // 1--2
    });

    return new gdk::webgl1es2_model(gdk::webgl1es2_model::Type::Static, gdk::webgl1es2_vertex_format::Pos3uv2, data);
});

const jfc::lazy_ptr<gdk::webgl1es2_model> webgl1es2_model::Cube([]()
{
    float size  = 1.;
    float hsize = size/2.;
    
    std::vector<GLfloat> data(
    {
        //        x,           y,      z,   u,   v,  Nx,  Ny,    Nz, North
        size -hsize, size -hsize, -hsize, 0.0, 0.0,  0.0, 0.0, -1.0, // 2--0
        0.0f -hsize, 0.0f -hsize, -hsize, 1.0, 1.0,  0.0, 0.0, -1.0, // | /
        0.0f -hsize, size -hsize, -hsize, 1.0, 0.0,  0.0, 0.0, -1.0, // 1
        size -hsize, size -hsize, -hsize, 0.0, 0.0,  0.0, 0.0, -1.0, //    0
        size -hsize, 0.0f -hsize, -hsize, 0.0, 1.0,  0.0, 0.0, -1.0, //  / |
        0.0f -hsize, 0.0f -hsize, -hsize, 1.0, 1.0,  0.0, 0.0, -1.0, // 2--1 */
        //        x,           y,      z,   u,   v,   Nx,  Ny,   Nz, South
        size -hsize, size -hsize,  hsize, 1.0, 0.0,  0.0, 0.0, +1.0, // 1--0
        0.0f -hsize, size -hsize,  hsize, 0.0, 0.0,  0.0, 0.0, +1.0, // | /
        0.0f -hsize, 0.0f -hsize,  hsize, 0.0, 1.0,  0.0, 0.0, +1.0, // 2
        size -hsize, size -hsize,  hsize, 1.0, 0.0,  0.0, 0.0, +1.0, //    0
        0.0f -hsize, 0.0f -hsize,  hsize, 0.0, 1.0,  0.0, 0.0, +1.0, //  / |
        size -hsize, 0.0f -hsize,  hsize, 1.0, 1.0,  0.0, 0.0, +1.0, // 1--2 */
        //        x,           y       z,   u,   v,   Nx,  Ny,  Nz, West
        0.0f -hsize, size -hsize,  hsize, 1.0, 0.0, -1.0, 0.0, 0.0, // 2--0
        0.0f -hsize, size -hsize, -hsize, 0.0, 0.0, -1.0, 0.0, 0.0, // | /
        0.0f -hsize, 0.0f -hsize, -hsize, 0.0, 1.0, -1.0, 0.0, 0.0, // 1
        0.0f -hsize, size -hsize,  hsize, 1.0, 0.0, -1.0, 0.0, 0.0, //    0
        0.0f -hsize, 0.0f -hsize, -hsize, 0.0, 1.0, -1.0, 0.0, 0.0, //  / |
        0.0f -hsize, 0.0f -hsize,  hsize, 1.0, 1.0, -1.0, 0.0, 0.0, // 2--1 */
        //        x,           y,      z,   u,   v,   Nx,   Ny,  Nz, East
        size -hsize, size -hsize,  hsize, 0.0, 0.0, +1.0,  0.0, 0.0, // 2--0
        size -hsize, 0.0f -hsize, -hsize, 1.0, 1.0, +1.0,  0.0, 0.0, // | /
        size -hsize, size -hsize, -hsize, 1.0, 0.0, +1.0,  0.0, 0.0, // 1
        size -hsize, size -hsize,  hsize, 0.0, 0.0, +1.0,  0.0, 0.0, //    0
        size -hsize, 0.0f -hsize,  hsize, 0.0, 1.0, +1.0,  0.0, 0.0, //  / |
        size -hsize, 0.0f -hsize, -hsize, 1.0, 1.0, +1.0,  0.0, 0.0, // 2--1 */
        //        x,           y,      z,   u,   v,   Nx,  Ny,  Nz, Down
        size -hsize, 0.0f -hsize, -hsize, 1.0, 0.0,  0.0, -1.0, 0.0, // 2--0
        0.0f -hsize, 0.0f -hsize,  hsize, 0.0, 1.0,  0.0, -1.0, 0.0, // | /
        0.0f -hsize, 0.0f -hsize, -hsize, 0.0, 0.0,  0.0, -1.0, 0.0, // 1
        size -hsize, 0.0f -hsize, -hsize, 1.0, 0.0,  0.0, -1.0, 0.0, //    0
        size -hsize, 0.0f -hsize,  hsize, 1.0, 1.0,  0.0, -1.0, 0.0, //  / |
        0.0f -hsize, 0.0f -hsize,  hsize, 0.0, 1.0,  0.0, -1.0, 0.0, // 2--1 */
        //        x,           y,      z,   u,   v,   Nx,   Ny,  Nz, Up
        size -hsize, 1.0f -hsize, -hsize, 1.0, 0.0,  0.0, +1.0, 0.0, // 1--0
        0.0f -hsize, 1.0f -hsize, -hsize, 0.0, 0.0,  0.0, +1.0, 0.0, // | /
        0.0f -hsize, 1.0f -hsize,  hsize, 0.0, 1.0,  0.0, +1.0, 0.0, // 2
        size -hsize, 1.0f -hsize, -hsize, 1.0, 0.0,  0.0, +1.0, 0.0, //    0
        0.0f -hsize, 1.0f -hsize,  hsize, 0.0, 1.0,  0.0, +1.0, 0.0, //  / |
        size -hsize, 1.0f -hsize,  hsize, 1.0, 1.0,  0.0, +1.0, 0.0, // 1--2 */            
    });

    return new gdk::webgl1es2_model(gdk::webgl1es2_model::Type::Static, gdk::webgl1es2_vertex_format::Pos3uv2Norm3, data);
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

bool webgl1es2_model::operator==(const webgl1es2_model &that)
{
    return
        m_IndexBufferHandle == that.m_IndexBufferHandle
        && m_VertexBufferHandle == that.m_VertexBufferHandle;
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

void webgl1es2_model::updatewebgl1es2_model(const std::vector<GLfloat> &aNewwebgl1es2_model
    , const webgl1es2_vertex_format &aNewvertex_format
    , const std::vector<GLushort> &aIndexData
    , const webgl1es2_model::Type &aNewType)
{
    //VBO
    m_vertex_format = aNewvertex_format;
    m_VertexCount  = static_cast<GLsizei>(aNewwebgl1es2_model.size() / aNewvertex_format.getSumOfAttributeComponents());
    GLint type = webgl1es2_modelTypeToOpenGLDrawType(aNewType);
    
    glBindBuffer (GL_ARRAY_BUFFER, m_VertexBufferHandle.get());
    glBufferData (GL_ARRAY_BUFFER, sizeof(GLfloat) * aNewwebgl1es2_model.size(), &aNewwebgl1es2_model[0], type);
    //glBindBuffer (GL_ARRAY_BUFFER,0);

    //IBO    
    if (m_IndexCount > 0)
    {
        //glGenBuffers(1, &m_IndexBufferHandle.get());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle.get());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * aIndexData.size(), &aIndexData[0], webgl1es2_modelTypeToOpenGLDrawType(aNewType));
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    
        std::string errorCode;
        
        if (glh::GetError(&errorCode)) std::runtime_error(std::string(TAG).append(errorCode));
    }
}

webgl1es2_model::webgl1es2_model(const webgl1es2_model::Type &aType, 
    const webgl1es2_vertex_format &avertex_format,
    const std::vector<GLfloat> &awebgl1es2_model, 
    const std::vector<GLushort> &aIndexData,
    const PrimitiveMode &aPrimitiveMode)
: m_IndexBufferHandle([&aIndexData, &aType]()
{
    GLuint ibo(0);
    
    if (aIndexData.size() > 0)
    {
        glGenBuffers(1, &ibo);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
            sizeof(GLushort) * aIndexData.size(), 
            &aIndexData[0], 
            webgl1es2_modelTypeToOpenGLDrawType(aType));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    
        std::string errorCode;
        
        if (glh::GetError(&errorCode)) std::runtime_error(std::string(TAG).append(errorCode));
    }
    
    return ibo;
}(),
[](const GLuint handle)
{
    glDeleteBuffers(1, &handle);
})
, m_IndexCount((GLsizei)aIndexData.size())
, m_VertexBufferHandle([&awebgl1es2_model, &aType]()
{
    if (!awebgl1es2_model.size()) std::invalid_argument(std::string(TAG).append("no vertex data to upload!"));
    
    GLuint vbo(0);
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * awebgl1es2_model.size(), &awebgl1es2_model[0], webgl1es2_modelTypeToOpenGLDrawType(aType));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    std::string errorCode;

    if (glh::GetError(&errorCode)) std::runtime_error(std::string(TAG).append(errorCode));
    
    return vbo;
}(),
[](const GLuint handle)
{
    glDeleteBuffers(1, &handle);
})
, m_VertexCount(static_cast<GLsizei>(awebgl1es2_model.size())/avertex_format.getSumOfAttributeComponents())
, m_vertex_format(avertex_format)
, m_PrimitiveMode(aPrimitiveMode)
{}

