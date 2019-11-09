// © 2019 Joseph Cameron - All Rights Reserved

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
    struct vertex_attribute final
    {
        std::string name = "attribute"; //!< name of the attribute
        unsigned short size = {0};      //!< number of components in the attribute
            
        vertex_attribute &operator=(const vertex_attribute &) = default;
        vertex_attribute &operator=(vertex_attribute &&) = default;
      
        vertex_attribute() = delete;
        vertex_attribute(const std::string &aName, const unsigned short &aSize);
        vertex_attribute(const vertex_attribute &) = default;
        vertex_attribute(vertex_attribute &&) = default;
        ~vertex_attribute() = default;
    };

    std::ostream &operator<< (std::ostream &, const vertex_attribute &);
}

#endif
