// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/opengl.h>
#include <gdk/vertex_data.h>

#include <iostream>
#include <stdexcept>

using namespace gdk;

static constexpr char TAG[] = "vertex_data";

const jfc::lazy_ptr<gdk::vertex_data> vertex_data::Quad([]()
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

    return new gdk::vertex_data(gdk::vertex_data::Type::Static, gdk::vertex_format::Pos3uv2, data);
});

const jfc::lazy_ptr<gdk::vertex_data> vertex_data::Cube([]()
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

    return new gdk::vertex_data(gdk::vertex_data::Type::Static, gdk::vertex_format::Pos3uv2Norm3, data);
});

static inline GLenum vertex_dataTypeToOpenGLDrawType(const vertex_data::Type aType)
{
    switch (aType)
    {
        case vertex_data::Type::Dynamic: return GL_DYNAMIC_DRAW;        
        case vertex_data::Type::Static: return GL_STATIC_DRAW;
        case vertex_data::Type::Stream: return GL_STREAM_DRAW;        
    }

    throw std::invalid_argument("unhandled vertex data type");
}

static inline GLenum PrimitiveModeToOpenGLPrimitiveType(const vertex_data::PrimitiveMode aPrimitiveMode)
{
    switch (aPrimitiveMode)
    {
        case vertex_data::PrimitiveMode::Points: return GL_POINTS;
        case vertex_data::PrimitiveMode::Lines: return GL_LINES;
        case vertex_data::PrimitiveMode::LineStrip: return GL_LINE_STRIP; 
        case vertex_data::PrimitiveMode::LineLoop: return GL_LINE_LOOP; 
        case vertex_data::PrimitiveMode::Triangles: return GL_TRIANGLES;            
        case vertex_data::PrimitiveMode::TriangleStrip: return GL_TRIANGLE_STRIP;
        case vertex_data::PrimitiveMode::TriangleFan: return GL_TRIANGLE_FAN;
    }
    
    throw std::invalid_argument("unhandled vertex primitive mode");
}

bool vertex_data::operator==(const vertex_data &that)
{
    return
        m_IndexBufferHandle == that.m_IndexBufferHandle
        && m_VertexBufferHandle == that.m_VertexBufferHandle;
}

bool vertex_data::operator!=(const vertex_data &that)
{
    return !(*this == that);
}

void vertex_data::bind(const GLuint currentShaderProgramHandle) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferHandle.get());
    
    m_vertex_format.enableAttributes(currentShaderProgramHandle);
}

void vertex_data::draw(const GLuint ashader_programHandle) const
{
    bind(ashader_programHandle); //TODO: move this call to "batch", will grealty reduce frequency of gl state changes.

    GLenum primitiveMode = PrimitiveModeToOpenGLPrimitiveType(m_PrimitiveMode);

    // This part is fine
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

void vertex_data::updatevertex_data(const std::vector<GLfloat> &aNewvertex_data
    , const vertex_format &aNewvertex_format
    , const std::vector<GLushort> &aIndexData
    , const vertex_data::Type &aNewType)
{
    //VBO
    m_vertex_format = aNewvertex_format;
    m_VertexCount  = static_cast<GLsizei>(aNewvertex_data.size() / aNewvertex_format.getSumOfAttributeComponents());
    GLint type = vertex_dataTypeToOpenGLDrawType(aNewType);
    
    glBindBuffer (GL_ARRAY_BUFFER, m_VertexBufferHandle.get());
    glBufferData (GL_ARRAY_BUFFER, sizeof(GLfloat) * aNewvertex_data.size(), &aNewvertex_data[0], type);
    //glBindBuffer (GL_ARRAY_BUFFER,0);

    //IBO    
    if (m_IndexCount > 0)
    {
        //glGenBuffers(1, &m_IndexBufferHandle.get());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle.get());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * aIndexData.size(), &aIndexData[0], vertex_dataTypeToOpenGLDrawType(aNewType));
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    
        std::string errorCode;
        
        if (glh::GetError(&errorCode)) std::runtime_error(std::string(TAG).append(errorCode));
    }
}

vertex_data::vertex_data(const vertex_data::Type &aType, 
    const vertex_format &avertex_format,
    const std::vector<GLfloat> &avertex_data, 
    const std::vector<GLushort> &aIndexData,
    const PrimitiveMode &aPrimitiveMode)
: m_IndexBufferHandle([&aIndexData, &aType]() -> GLuint
{
    GLuint ibo = 0;
    
    if (aIndexData.size() > 0)
    {
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * aIndexData.size(), &aIndexData[0], vertex_dataTypeToOpenGLDrawType(aType));
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
, m_VertexBufferHandle([&avertex_data, &aType]() -> GLuint
{
    if (avertex_data.size() == 0) std::invalid_argument(std::string(TAG).append("no vertex data to upload!"));
    
    GLuint vbo = 0;
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * avertex_data.size(), &avertex_data[0], vertex_dataTypeToOpenGLDrawType(aType));
    glBindBuffer(GL_ARRAY_BUFFER,0);
    
    std::string errorCode;

    if (glh::GetError(&errorCode)) std::runtime_error(std::string(TAG).append(errorCode));
    
    return vbo;
}(),
[](const GLuint handle)
{
    glDeleteBuffers(1, &handle);
})
, m_VertexCount((int)avertex_data.size()/avertex_format.getSumOfAttributeComponents())
, m_vertex_format(avertex_format)
, m_PrimitiveMode(aPrimitiveMode)
{}

