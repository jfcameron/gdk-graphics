// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VERTEX_DATA_H
#define GDK_GFX_VERTEX_DATA_H

#include <gdk/vertex_format.h>
#include <jfc/lazy_ptr.h>
#include <jfc/unique_handle.h>

#include <iosfwd>
#include <string>

namespace gdk
{
    /// \brief Vertex data representing a 3D graphical object
    // TODO: support multiple VBO objects, to allow using more appropriate Gl types for uvs, normals. Currently only supports interweaved GLBytes
    class model final
    {
    public:
        /// \brief Hint to the graphics device about how the vertex data will be used.
        enum class Type 
        {
            //! The data store contents will be modified repeatedly and used many times.
            Dynamic,

            //! The data store contents will be modified once and used many times.
            Static, 

            //! The data store contents will be modified once and used at most a few times.
            Stream
        };
        
        //! Determines the primitive type used at the primitive assembly stage.        
        enum class PrimitiveMode 
        {
            //! Every vertex is replaced with a point at the primitive assembly stage
            Points, 

            //! Every two verticies define a line
            Lines, 
            
            //! Same as lines except the last vertex of the last defined line serves as the 
            /// first vertex of the line currently being defined
            LineStrip, 
            
            //! Same as LineStrip except an additional line is defined between the last and first vertex
            LineLoop, 

            //! Every 3 verticies define a triangle
            Triangles,
            
            //! Every vertex after the first three define a new triangle
            TriangleStrip, 

            //! Same as Triangle strip except draws in a "fan shape" (???)
            TriangleFan
        };
            
    private:
        //! Handle to the (optional) index buffer in the context
        jfc::unique_handle<GLuint> m_IndexBufferHandle;

        //! total number of indicies
        GLsizei m_IndexCount = 0; 
        
        //! Handle to the vertex buffer in the context
        jfc::unique_handle<GLuint> m_VertexBufferHandle; 
        
        //! total number of vertexes
        GLsizei m_VertexCount = 0; 

        //! Format of the vertex data
        vertex_format m_vertex_format = vertex_format::Pos3uv2; 

        //! The primitive type to be generated using the vertex data
        PrimitiveMode m_PrimitiveMode = PrimitiveMode::Triangles; 
        
    public:
        //! Binds this vertex data to the pipeline, enables attributes on the currently used shaderprogram
        /// \brief strong association with draw. If draw is on this instance is not called after bind on this
        /// instance, behaviour is unintended. The bodies of these functions are separate because bind
        /// is expensive and does not need to be called nearly as frequently as draw. Bind is per unique vertex data
        /// draw is per unique entity (that vertex data projected out somewhere into the scene.)
        void bind(const shader_program &aShaderProgram) const;

        //! Invokes the pipline on the currently bound vertex data, with the expectation that
        /// the currently bound data is the data held by this instance
        /// \warn if bind() has not been called before draw, the behaviour will be unintended
        void draw() const;

        //! replace current data in the vbo and ibo with new data
        void updatemodel(const std::vector<GLfloat> &aNewmodel, 
            const vertex_format &aNewvertex_format,
            const std::vector<GLushort> &aIndexData = std::vector<GLushort>(), 
            const model::Type &aNewType = Type::Dynamic);
      
        //! equality semantics based on handle values
        bool operator==(const model &);
        //! equality semantics based on handle values
        bool operator!=(const model &);

        //! move semantics
        model &operator=(model &&) = default;
        //! move semantics
        model(model &&) = default;
           
        //! disable copy semantics
        model &operator=(const model &other) = delete;
        //! disable copy semantics
        model(const model &) = delete;
      
        model(const model::Type &aType, 
            const vertex_format &avertex_format, 
            const std::vector<GLfloat> &amodel,
            const std::vector<GLushort> &aIndexData = std::vector<GLushort>(), 
            const PrimitiveMode &aPrimitiveMode = PrimitiveMode::Triangles);
        
        static const jfc::lazy_ptr<gdk::model> Quad; //!< a quad with format pos3uv2
        static const jfc::lazy_ptr<gdk::model> Cube; //!< a cube with format ps3uv2norm3
    };
}

#endif
