// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VERTEXFORMAT_H
#define GDK_GFX_VERTEXFORMAT_H

#include <gdk/opengl.h>
#include <gdk/webgl1es2_shader_program.h>
#include <gdk/webgl1es2_vertex_attribute.h>

#include <string>
#include <vector>

namespace gdk
{
    /// \brief informs the gl context how to interpret the currently bound vertex data.
    ///
    /// \detailed Vertex data [in the context of OpenGL] is reprented as an array of floating-point values.
    /// The data and the format of that data is completely arbitrary and user defined and the GL context
    /// therefore has to be informed how to interpret it.
    /// An example of a typical format would be something like this: {Position3, UV2, Normal3, Tangent3}.
    /// In the above example, the context would need to know to interpret your vertex data (array of floats) like this:
    /// floats  0..2 will be uploaded to the position attribute, 3 & 4 to the UV attribute, 5..7 to Normal,
    /// 8..10 Tangent, then repeat (11-13 goes to Position etc...). Attributes, within the context of the graphics pipeline
    /// represents instanced data. In the context of the Vertex Shader stage, you will be able to access a set of attributes,
    /// representing a single full vertex.
    class webgl1es2_vertex_format final
    {
        //! name and # of floats of each attribute in the format
        std::vector<webgl1es2_vertex_attribute> m_Format;

        //! total number of floats in the entire format
        webgl1es2_vertex_attribute::size_type m_SumOfAttributeComponents = 0;       
            
    public:
        //! prepares gl context to draw vertex data formatted according to this vertex format
        void enableAttributes(const webgl1es2_shader_program &aShaderProgram) const;

        //! Total number of components (sum of length of attributes)
        int getSumOfAttributeComponents() const;

        //! copy semantics
        webgl1es2_vertex_format& operator=(const webgl1es2_vertex_format &) = default;
        //! copy semantics
        webgl1es2_vertex_format(const webgl1es2_vertex_format &) = default;

        //! move semantics
        webgl1es2_vertex_format& operator=(webgl1es2_vertex_format &&) = default;
        //! move semantics
        webgl1es2_vertex_format(webgl1es2_vertex_format &&) = default;
     
        //! construct a format using supplied attributes.
        webgl1es2_vertex_format(const std::vector<webgl1es2_vertex_attribute> &aAttributes);
        
		//! {3 position} format
		static const webgl1es2_vertex_format Pos3;

		//! {3 position, 2 uv} format
		static const webgl1es2_vertex_format Pos3uv2;

		//! {3 position, 2 uv, 3 normal} format
        static const webgl1es2_vertex_format Pos3uv2Norm3;
    };
}

#endif
