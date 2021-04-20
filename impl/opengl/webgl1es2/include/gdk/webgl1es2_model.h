// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VERTEX_DATA_H
#define GDK_GFX_VERTEX_DATA_H

#include <gdk/model.h>
#include <gdk/webgl1es2_vertex_format.h>
#include <jfc/shared_proxy_ptr.h>
#include <jfc/unique_handle.h>

#include <iosfwd>
#include <string>

namespace gdk
{
    /// \brief Vertex data representing a 3D graphical object
    class webgl1es2_model final : public model
    {
    public:
        //! type that must be used to populate vertex data buffers. 
        /// All GLES2 attrib types have a float based component type. 
        /// (float, float vec2, float mat etc)
        using attribute_component_data_type = GLfloat; 

        //! type that must be used for index buffer data
        using index_data_type = GLushort; 

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
            /// TODO: find & document better explanation
            TriangleFan
        };
            
        //! Binds this vertex data to the pipeline, enables attributes on the currently 
        /// used shaderprogram
        /// \brief strong association with draw. If draw is on this instance is not called 
        /// after bind() has not been called before draw, the behaviour will be unintended
        void bind(const webgl1es2_shader_program &aShaderProgram) const;

        //! invokes pipeline on the data. data must be bound
        void draw() const;

        //! replace current data in the vbo and ibo with new data
        virtual void update_vertex_data(const vertex_data& vertexDataView) override;
      
        //! equality semantics based on handle values
        bool operator==(const webgl1es2_model &);
        //! equality semantics based on handle values
        bool operator!=(const webgl1es2_model &);

        //! support move semantics
        webgl1es2_model &operator=(webgl1es2_model &&) = default;
        //! support move semantics
        webgl1es2_model(webgl1es2_model &&) = default;
           
        //! disable copy semantics
        webgl1es2_model &operator=(const webgl1es2_model &other) = delete;
        //! disable copy semantics
        webgl1es2_model(const webgl1es2_model &) = delete;
      
        webgl1es2_model(const vertex_data &aData,
            const std::vector<GLushort> &aIndexData = std::vector<GLushort>(), 
            const PrimitiveMode &aPrimitiveMode = PrimitiveMode::Triangles);
        
        //! a quad with format pos3uv2
        static const jfc::shared_proxy_ptr<gdk::webgl1es2_model> Quad;
        //! a cube with format pos3uv2norm3
        static const jfc::shared_proxy_ptr<gdk::webgl1es2_model> Cube; 

    private:
        //! Handle to the (optional) index buffer in the context
        jfc::unique_handle<GLuint> m_IndexBufferHandle;

        //! total number of indicies
        GLsizei m_IndexCount = 0; 
        
        //! Handle to the vertex buffer in the context
        jfc::unique_handle<GLuint> m_VertexBufferHandle; 

        //! Format of the vertex data
        webgl1es2_vertex_format m_vertex_format = webgl1es2_vertex_format::Pos3uv2; 
        
        //! total number of vertexes
        GLsizei m_VertexCount = 0; 

        //! The primitive type to be generated using the vertex data
        PrimitiveMode m_PrimitiveMode = PrimitiveMode::Triangles; 
    };
}

#endif

