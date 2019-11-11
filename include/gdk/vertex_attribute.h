// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VERTEXATTRIBUTE_H
#define GDK_GFX_VERTEXATTRIBUTE_H

#include <iosfwd>
#include <string>

namespace gdk
{
    /// \brief A vertex attribute is a component of a vertex. Typical definitions would include: position, uv, normal, color. Strictly speaking though they are entirely arbitrary.
    ///
    /// \detailed Vertex attributes are made up of an arbitrary set of components.
    /// In the context of OpenGL, a component is a floating-point value.
    struct vertex_attribute final
    {
        using size_type = unsigned short;

        //! name of the vertex attribute, used to access its value within a programmable shader stage. e.g: "a_uv"
        std::string name;

        //! number of components in the attribute. TODO: consider renaming to count? size is a bit confusing
        size_type size = 0;
       
        /// \brief equality semantics
        bool operator==(const vertex_attribute &) const;
        /// \brief equality semantics
        bool operator!=(const vertex_attribute &) const;

        /// \brief copy semantics
        vertex_attribute(const vertex_attribute &) = default;
        /// \brief copy semantics
        vertex_attribute &operator=(const vertex_attribute &) = default;

        /// \brief move semantics
        vertex_attribute(vertex_attribute &&) = default;
        /// \brief move semantics
        vertex_attribute &operator=(vertex_attribute &&) = default;
        
        /// \brief constructs an attribute with a given name and number of components
        vertex_attribute(const std::string &aName, const unsigned short &aSize);
    };
}

#endif
