// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEB1GLES2_MODEL_H
#define GDK_GFX_WEB1GLES2_MODEL_H

#include <gdk/graphics/string_keyed.h>
#include <gdk/graphics/model.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <jfc/lazy_ptr.h>

#include <memory>
#include <jfc/unique_handle.h>

#include <iosfwd>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace gdk::graphics {
    class model_data;

    /// \brief Vertex data representing a 3D graphical object
    class webgl1es2_model final : public model {
    public:
        //! type that must be used to populate vertex data buffers. 
        /// All GLES2 attrib types have a float based component type. 
        /// (float, float vec2, float mat etc)
        using attribute_component_data_type = GLfloat; 

        //! type that must be used for index buffer data
        using index_data_type = GLushort; 
        
        //! Binds this vertex data to the pipeline, enables attributes on the currently 
        /// used shaderprogram
        /// \brief strong association with draw. If draw is on this instance is not called 
        /// after bind() has not been called before draw, the behaviour will be unintended
        void bind(const webgl1es2_shader_program &aShaderProgram) const;

        //! invokes pipeline on the data. data must be bound
        void draw() const;

        //! replace current data in the vbo and ibo with new data
        //TODO: optional vector to selectively update data,
        //TODO if vertex_count is different in selective mode, throw
        virtual void upload(const usage_hint &, const model_data& vertexDataView) override;

        //! centre of the model's bounding sphere, in its own space. \see m_BoundsCentre
        [[nodiscard]] const vector3_type &bounds_centre() const { return m_BoundsCentre; }

        //! radius of the model's bounding sphere, in its own space
        [[nodiscard]] floating_point_type bounds_radius() const { return m_BoundsRadius; }

        //! support move semantics
        webgl1es2_model &operator=(webgl1es2_model &&) = default;
        //! support move semantics
        webgl1es2_model(webgl1es2_model &&) = default;
           
        //! disable copy semantics
        webgl1es2_model &operator=(const webgl1es2_model &other) = delete;
        //! disable copy semantics
        webgl1es2_model(const webgl1es2_model &) = delete;
      
        webgl1es2_model(const usage_hint &aUsage, const model_data &aData);
        
        //! a cube with format pos3uv2norm3
        /// \brief a 1x1x1 cube: vec3 position, vec2 uv, vec3 normal
        [[nodiscard]] static std::shared_ptr<webgl1es2_model> make_cube(); 

        //! a sphere with format pos3uv2norm3
        //! a unit sphere. \see make_cube
        [[nodiscard]] static std::shared_ptr<webgl1es2_model> make_sphere(); 

    private:
        //! stores component data for a single attribute
        struct attribute {
            size_t buffer_handle_index;
            size_t components;
            size_t size;
        };

        vector3_type m_BoundsCentre{0, 0, 0};
        floating_point_type m_BoundsRadius = 0;

        GLsizei m_IndexCount = 0; 
        GLsizei m_VertexCount = 0; 

        std::optional<jfc::unique_handle<GLuint>> m_IndexBufferHandle;
        std::vector<jfc::unique_handle<GLuint>> m_VertexBufferHandles;
        
        GLenum m_PrimitiveMode;

        string_keyed<attribute> m_Attributes;

        /// \brief the attribute locations this model resolved for one shader program
        struct bound_attribute final {
            GLuint buffer;
            GLint location;
            GLint components;
        };
        //! keyed by program handle, because one model is drawn with more than one shader
        mutable std::unordered_map<GLuint, std::vector<bound_attribute>> m_BindingCache;
    };
}

#endif

