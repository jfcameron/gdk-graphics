// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VERTEXATTRIBUTE_H
#define GDK_GFX_VERTEXATTRIBUTE_H

#include <iosfwd>
#include <string>

namespace gdk
{
    /// \brief A vertex attribute is a component of a vertex (eg: position, uv, normal, color).
    ///
    /// \detailed Vertex attributes are made up of an arbitrary set of components.
    /// In the context of OpenGL, a component is a floating-point value.
    struct VertexAttribute final
    {
        std::string name = "attribute"; //!< name of the attribute
        unsigned short size = {0};      //!< number of components in the attribute
            
        VertexAttribute &operator=(const VertexAttribute &) = default;
        VertexAttribute &operator=(VertexAttribute &&) = default;
      
        VertexAttribute() = delete;
        VertexAttribute(const std::string &aName, const unsigned short &aSize);
        VertexAttribute(const VertexAttribute &) = default;
        VertexAttribute(VertexAttribute &&) = default;
        ~VertexAttribute() = default;
    };

    std::ostream &operator<< (std::ostream &, const VertexAttribute &);
}

#endif
