// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/nlohmann_json_util.h>
#include <gdk/opengl.h>
#include <gdk/vertexdata.h>

#include <nlohmann/json.hpp>

#include <iostream>
#include <stdexcept>

using namespace gdk;

static constexpr char TAG[] = "VertexData";

const gdk::lazy_ptr<gdk::VertexData> VertexData::Quad([](){
        float size  = 1.;
        float hsize = size/2.;
        std::vector<GLfloat> data({
                //        x,           y,    z,    u,    v,
                size -hsize, size -hsize, 0.0f, 1.0f, 0.0f, // 1--0
                    0.0f -hsize, size -hsize, 0.0f, 0.0f, 0.0f, // | /
                    0.0f -hsize, 0.0f -hsize, 0.0f, 0.0f, 1.0f, // 2
                    size -hsize, size -hsize, 0.0f, 1.0f, 0.0f, //    0
                    0.0f -hsize, 0.0f -hsize, 0.0f, 0.0f, 1.0f, //  / |
                    size -hsize, 0.0f -hsize, 0.0f, 1.0f, 1.0f, // 1--2
                    });
        return new gdk::VertexData("Quad", gdk::VertexData::Type::Static, gdk::VertexFormat::Pos3uv2, data);
    });

const gdk::lazy_ptr<gdk::VertexData> VertexData::Cube([](){
        float size  = 1.;
        float hsize = size/2.;
        std::vector<GLfloat> data({
                //        x,           y,      z,   u,   v,  Nx,  Ny,   Nz, North
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

        return new gdk::VertexData("Cube", gdk::VertexData::Type::Static, gdk::VertexFormat::Pos3uv2Norm3, data);
    });

std::ostream& gdk::operator<<(std::ostream &s, const VertexData &a)
{
    return s << nlohmann::json
    {
        {"Type", TAG}, 
        {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
            {}
        },
        
        {"m_Name", jfc::insertion_operator_to_nlohmann_json_object(a.m_Name)},
        {"m_VertexBufferHandle", jfc::insertion_operator_to_nlohmann_json_object(a.m_VertexBufferHandle)},
        {"m_VertexCount", jfc::insertion_operator_to_nlohmann_json_object(a.m_VertexCount)},
        {"m_VertexFormat", jfc::insertion_operator_to_nlohmann_json_object(a.m_VertexFormat)},
    }
    .dump();
}

static GLenum VertexDataTypeToOpenGLDrawType(const VertexData::Type &aType)
{
    switch (aType)
    {
        case VertexData::Type::Dynamic: return GL_DYNAMIC_DRAW;        
        case VertexData::Type::Static:  return GL_STATIC_DRAW;
    }
}

static GLenum PrimitiveModeToOpenGLPrimitiveType(const VertexData::PrimitiveMode &aPrimitiveMode)
{
    switch (aPrimitiveMode)
    {
        case VertexData::PrimitiveMode::Triangles: return GL_TRIANGLES;            
        case VertexData::PrimitiveMode::Lines:     return GL_LINES;        
        case VertexData::PrimitiveMode::Points:    return GL_POINTS;
    }
}

void VertexData::draw(const GLuint aShaderProgramHandle) const
{
    //std::string error;
    //while(glh::GetError(&error)) std::cout << TAG << ", glerror: " << error; // ???????????
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferHandle);
    
    m_VertexFormat.enableAttributes(aShaderProgramHandle);
    
    GLenum primitiveMode = PrimitiveModeToOpenGLPrimitiveType(m_PrimitiveMode);

    if (m_IndexBufferHandle > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle);
        
        glDrawElements
        (
            primitiveMode,
            m_IndexCount,
            GL_UNSIGNED_SHORT,
            static_cast<void *>(0)
        );
    }
    else glDrawArrays(primitiveMode, 0, m_VertexCount);

    // It may be necessary to unbind?
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexData::updateVertexData(
    const std::vector<GLfloat>  &aNewVertexData, const VertexFormat     &aNewVertexFormat,
    const std::vector<GLushort> &aIndexData,     const VertexData::Type &aNewType)
{
    //VBO
    m_VertexFormat = aNewVertexFormat;
    m_VertexCount  = static_cast<GLsizei>(aNewVertexData.size() / aNewVertexFormat.getSumOfAttributeComponents());
    GLint type = VertexDataTypeToOpenGLDrawType(aNewType);
    
    glBindBuffer (GL_ARRAY_BUFFER, m_VertexBufferHandle);
    glBufferData (GL_ARRAY_BUFFER, sizeof(GLfloat) * aNewVertexData.size(), &aNewVertexData[0], type);
    glBindBuffer (GL_ARRAY_BUFFER,0);

    //IBO    
    if (m_IndexCount > 0)
    {
        glGenBuffers(1, &m_IndexBufferHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * aIndexData.size(), &aIndexData[0], VertexDataTypeToOpenGLDrawType(aNewType));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    
        std::string errorCode;
        
        if (glh::GetError(&errorCode)) std::runtime_error(std::string(TAG).append(errorCode));
    }
}

VertexData::VertexData(const std::string &aName, const VertexData::Type &aType, const VertexFormat &aVertexFormat,
           const std::vector<GLfloat> &aVertexData, const std::vector<GLushort> &aIndexData,
           const PrimitiveMode &aPrimitiveMode)
: m_Name(aName)
, m_IndexBufferHandle([&aIndexData, &aType]() -> GLuint
{
    GLuint ibo = 0;
    
    if (aIndexData.size() > 0)
    {
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * aIndexData.size(), &aIndexData[0], VertexDataTypeToOpenGLDrawType(aType));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    
        std::string errorCode;
        
        if (glh::GetError(&errorCode)) std::runtime_error(std::string(TAG).append(errorCode));
    }
    
    return ibo;
}())
, m_IndexCount((GLsizei)aIndexData.size())
, m_VertexBufferHandle([&aVertexData, &aType]() -> GLuint
{
    if (aVertexData.size() <= 0) std::runtime_error(std::string(TAG).append("bad vertex data"));
    
    GLuint vbo = 0;
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * aVertexData.size(), &aVertexData[0], VertexDataTypeToOpenGLDrawType(aType));
    glBindBuffer(GL_ARRAY_BUFFER,0);
    
    std::string errorCode;

    if (glh::GetError(&errorCode)) std::runtime_error(std::string(TAG).append(errorCode));
    
    return vbo;
}())
, m_VertexCount((int)aVertexData.size()/aVertexFormat.getSumOfAttributeComponents())
, m_VertexFormat(aVertexFormat)
, m_PrimitiveMode(aPrimitiveMode)
{}

VertexData::~VertexData()
{
    if (m_VertexBufferHandle > 0)
        glDeleteBuffers(1, &m_VertexBufferHandle);
    
    if (m_IndexBufferHandle > 0)
        glDeleteBuffers(1, &m_IndexBufferHandle);
}

VertexData::VertexData(VertexData &&a)
{
    m_Name               = std::move(a.m_Name);
    m_IndexBufferHandle  = std::move(a.m_IndexBufferHandle);
    m_IndexCount         = std::move(a.m_IndexCount);
    m_VertexBufferHandle = std::move(a.m_VertexBufferHandle);
    m_VertexCount        = std::move(a.m_VertexCount);
    m_VertexFormat       = std::move(a.m_VertexFormat);
    m_PrimitiveMode      = std::move(a.m_PrimitiveMode);
    
    a.m_IndexBufferHandle  = 0;
    a.m_VertexBufferHandle = 0;
}

// Accessors
std::string const &VertexData::getName()const
{
    return m_Name;
}

GLuint VertexData::getHandle()const
{
    return m_VertexBufferHandle;
}
