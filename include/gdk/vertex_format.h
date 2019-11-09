// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VERTEXFORMAT_H
#define GDK_GFX_VERTEXFORMAT_H

#include <gdk/opengl.h>
#include <gdk/vertex_attribute.h>

#include <iosfwd>
#include <map>
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
    class vertex_format final
    {
        friend std::ostream &operator<<(std::ostream &, const vertex_format &);
    
        std::vector<vertex_attribute> m_Format = {}; //!< name and # of floats of each attribute in the format
        int m_SumOfAttributeComponents = {0};       //!< total number of floats in the entire format
            
    public:
        //! prepares gl context to draw vertex data formatted according to this vertex format
        void enableAttributes(const GLuint ashader_programHandle) const;

        //!
        int getSumOfAttributeComponents() const;
            
        vertex_format& operator=(const vertex_format &) = default;
        vertex_format& operator=(vertex_format &&) = default;
      
        vertex_format(const std::vector<vertex_attribute> &aAttributes);
        vertex_format() = delete;
        vertex_format(const vertex_format &) = default;
        vertex_format(vertex_format &&) = default;
        ~vertex_format() = default;
            
        static const vertex_format Pos3uv2Norm3;
        static const vertex_format Pos3uv2;
        static const vertex_format Pos3;
    };

    std::ostream &operator<< (std::ostream &, const vertex_format &);
}

#endif
