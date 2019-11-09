// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_MESH_H
#define GDK_GFX_MESH_H

#include <gdk/vertex_format.h>
#include <jfc/lazy_ptr.h>

#include <iosfwd>
#include <string>

namespace gdk
{
    /// \brief Vertex data representing a 3D graphical object
    class vertex_data final
    {
        friend std::ostream &operator<< (std::ostream &, const vertex_data &);
            
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
        std::string m_Name; //!< Human friendly identifier for the resource
            
        GLuint m_IndexBufferHandle = {0}; //!< Handle to the (optional) index buffer in the context
        GLsizei m_IndexCount =       {0}; //!< total number of indicies
            
        GLuint m_VertexBufferHandle = {0}; //!< Handle to the vertex buffer in the context
        GLsizei m_VertexCount =       {0}; //!< total number of vertexes

        vertex_format m_vertex_format = vertex_format::Pos3uv2; //!< Format of the vertex data
        
        PrimitiveMode m_PrimitiveMode = PrimitiveMode::Triangles; //!< The primitive type to be generated using the vertex data
        
    public:
        std::string const &getName() const;
        GLuint getHandle() const;

        //! Invokes the pipline on this vertex data
        void draw(const GLuint ashader_programHandle) const;

        //! replace current data in the vbo and ibo with new data
        void updatevertex_data(const std::vector<GLfloat> &aNewvertex_data, const vertex_format &aNewvertex_format,
                              const std::vector<GLushort> &aIndexData = std::vector<GLushort>(), const vertex_data::Type &aNewType = Type::Dynamic);
            
        vertex_data &operator=(const vertex_data &other) = default;
            
        vertex_data &operator=(vertex_data &&) = delete;
      
        vertex_data(const std::string &aName, const vertex_data::Type &aType, const vertex_format &avertex_format, const std::vector<GLfloat> &avertex_data,
                   const std::vector<GLushort> &aIndexData = std::vector<GLushort>(), const PrimitiveMode &aPrimitiveMode = PrimitiveMode::Triangles);
        
        vertex_data() = delete;
        vertex_data(const vertex_data &) = delete;
        vertex_data(vertex_data &&);
        ~vertex_data();

        static const jfc::lazy_ptr<gdk::vertex_data> Quad; //!< a quad with format pos3uv2
        static const jfc::lazy_ptr<gdk::vertex_data> Cube; //!< a cube with format ps3uv2norm3
    };

    std::ostream &operator<< (std::ostream &, const vertex_data &);
}

#endif
