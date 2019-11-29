// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/opengl.h>
#include <gdk/model.h>

#include <iostream>
#include <stdexcept>

using namespace gdk;

static constexpr char TAG[] = "model";

const jfc::lazy_ptr<gdk::model> model::Quad([]()
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

    return new gdk::model(gdk::model::Type::Static, gdk::vertex_format::Pos3uv2, data);
});

const jfc::lazy_ptr<gdk::model> model::Cube([]()
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

    return new gdk::model(gdk::model::Type::Static, gdk::vertex_format::Pos3uv2Norm3, data);
});

static inline GLenum modelTypeToOpenGLDrawType(const model::Type aType)
{
    switch (aType)
    {
        case model::Type::Dynamic: return GL_DYNAMIC_DRAW;        
        case model::Type::Static: return GL_STATIC_DRAW;
        case model::Type::Stream: return GL_STREAM_DRAW;        
    }

    throw std::invalid_argument("unhandled vertex data type");
}

static inline GLenum PrimitiveModeToOpenGLPrimitiveType(const model::PrimitiveMode aPrimitiveMode)
{
    switch (aPrimitiveMode)
    {
        case model::PrimitiveMode::Points: return GL_POINTS;
        case model::PrimitiveMode::Lines: return GL_LINES;
        case model::PrimitiveMode::LineStrip: return GL_LINE_STRIP; 
        case model::PrimitiveMode::LineLoop: return GL_LINE_LOOP; 
        case model::PrimitiveMode::Triangles: return GL_TRIANGLES;            
        case model::PrimitiveMode::TriangleStrip: return GL_TRIANGLE_STRIP;
        case model::PrimitiveMode::TriangleFan: return GL_TRIANGLE_FAN;
    }
    
    throw std::invalid_argument("unhandled vertex primitive mode");
}

bool model::operator==(const model &that)
{
    return
        m_IndexBufferHandle == that.m_IndexBufferHandle
        && m_VertexBufferHandle == that.m_VertexBufferHandle;
}

bool model::operator!=(const model &that)
{
    return !(*this == that);
}

void model::bind(const shader_program &aShaderProgram) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferHandle.get());
    
    m_vertex_format.enableAttributes(aShaderProgram);
}

void model::draw() const
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

void model::updatemodel(const std::vector<GLfloat> &aNewmodel
    , const vertex_format &aNewvertex_format
    , const std::vector<GLushort> &aIndexData
    , const model::Type &aNewType)
{
    //VBO
    m_vertex_format = aNewvertex_format;
    m_VertexCount  = static_cast<GLsizei>(aNewmodel.size() / aNewvertex_format.getSumOfAttributeComponents());
    GLint type = modelTypeToOpenGLDrawType(aNewType);
    
    glBindBuffer (GL_ARRAY_BUFFER, m_VertexBufferHandle.get());
    glBufferData (GL_ARRAY_BUFFER, sizeof(GLfloat) * aNewmodel.size(), &aNewmodel[0], type);
    //glBindBuffer (GL_ARRAY_BUFFER,0);

    //IBO    
    if (m_IndexCount > 0)
    {
        //glGenBuffers(1, &m_IndexBufferHandle.get());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle.get());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * aIndexData.size(), &aIndexData[0], modelTypeToOpenGLDrawType(aNewType));
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    
        std::string errorCode;
        
        if (glh::GetError(&errorCode)) std::runtime_error(std::string(TAG).append(errorCode));
    }
}

model::model(const model::Type &aType, 
    const vertex_format &avertex_format,
    const std::vector<GLfloat> &amodel, 
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
            modelTypeToOpenGLDrawType(aType));

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
, m_VertexBufferHandle([&amodel, &aType]()
{
    if (!amodel.size()) std::invalid_argument(std::string(TAG).append("no vertex data to upload!"));
    
    GLuint vbo(0);
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * amodel.size(), &amodel[0], modelTypeToOpenGLDrawType(aType));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    std::string errorCode;

    if (glh::GetError(&errorCode)) std::runtime_error(std::string(TAG).append(errorCode));
    
    return vbo;
}(),
[](const GLuint handle)
{
    glDeleteBuffers(1, &handle);
})
, m_VertexCount(static_cast<GLsizei>(amodel.size())/avertex_format.getSumOfAttributeComponents())
, m_vertex_format(avertex_format)
, m_PrimitiveMode(aPrimitiveMode)
{}

