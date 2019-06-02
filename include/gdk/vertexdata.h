// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MESH_H
#define GDK_GFX_MESH_H

#include <gdk/vertexformat.h>
#include <gdk/lazy_ptr.h>

#include <iosfwd>
#include <string>

namespace gdk
{
    /// \brief Vertex data representing a 3D graphical object
    class VertexData final
    {
        friend std::ostream &operator<< (std::ostream &, const VertexData &);
            
    public:
        
        /// \brief Hint to the graphics device about how the vertex data will be used.
        ///
        /// \detailed Generally, dynamic data (data that is likely to be frequently rewritten) will be placed
        /// in video memory with fast read write speeds while static will be placed in slower (and more plentiful)
        /// video memory. Exact behaviours are implementation specific.        
        enum class Type {Static, Dynamic};
        
        //! Determines the primitive type used at the primitive assembly stage.        
        enum class PrimitiveMode {Triangles, Lines, Points};
            
    private:
        std::string m_Name = {}; //!< Human friendly identifier for the resource
            
        GLuint m_IndexBufferHandle = {0}; //!< Handle to the (optional) index buffer in the context
        GLsizei m_IndexCount =       {0}; //!< total number of indicies
            
        GLuint m_VertexBufferHandle = {0}; //!< Handle to the vertex buffer in the context
        GLsizei m_VertexCount =       {0}; //!< total number of vertexes

        VertexFormat m_VertexFormat = VertexFormat::Pos3uv2; //!< Format of the vertex data
        
        PrimitiveMode m_PrimitiveMode = PrimitiveMode::Triangles; //!< The primitive type to be generated using the vertex data
        
    public:
        std::string const &getName() const;
        GLuint getHandle() const;

        //! Invokes the pipline on this vertex data
        void draw(const GLuint aShaderProgramHandle) const;

        //! replace current data in the vbo and ibo with new data
        void updateVertexData(const std::vector<GLfloat> &aNewVertexData, const VertexFormat &aNewVertexFormat,
                              const std::vector<GLushort> &aIndexData = std::vector<GLushort>(), const VertexData::Type &aNewType = Type::Dynamic);
            
        VertexData &operator=(const VertexData &other) = default;
            
        VertexData &operator=(VertexData &&) = delete;
      
        VertexData(const std::string &aName, const VertexData::Type &aType, const VertexFormat &aVertexFormat, const std::vector<GLfloat> &aVertexData,
                   const std::vector<GLushort> &aIndexData = std::vector<GLushort>(), const PrimitiveMode &aPrimitiveMode = PrimitiveMode::Triangles);
        
        VertexData() = delete;
        VertexData(const VertexData &) = delete;
        VertexData(VertexData &&);
        ~VertexData();

        static const gdk::lazy_ptr<gdk::VertexData> Quad; //!< a quad with format pos3uv2
        static const gdk::lazy_ptr<gdk::VertexData> Cube; //!< a cube with format ps3uv2norm3
    };

    std::ostream &operator<< (std::ostream &, const VertexData &);
}

#endif
