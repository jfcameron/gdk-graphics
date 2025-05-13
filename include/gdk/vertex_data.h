// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VERTEX_DATA_H
#define GDK_GFX_VERTEX_DATA_H

#include <gdk/attribute_data.h>
#include <gdk/graphics_types.h>

#include <array>
#include <functional>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace gdk {
    /// \brief vertex data stored in system memory. 
    ///
    /// vertex data is comprised of a collection of attributes, which in turn are a collection of components
    class vertex_data final {
    public:
        using attribute_collection_type = std::unordered_map<std::string, attribute_data>;
        using index_value_type = unsigned short;

        enum class primitive_mode {
            triangles
        };

        //! get the primitive mode
        primitive_mode get_primitive_mode() const;

        //! get list of indexes
        const std::vector<index_value_type> &indexes() const;

        //! get collection of all attribute data
        const attribute_collection_type &attributes() const;

        //! get attribute data by name, e.g: "a_Position"
        const attribute_data &get_attribute_data(const std::string &aAttributeName) const;
        //! get attribute data by name, e.g: "a_Position"
        attribute_data &get_attribute_data(const std::string &aAttributeName);

        //! clears all state from this vertex_data instance
        void clear();

        //! applies a transformation to a 3 component attribute
        void transform(const std::string &aPositionAttributeName, 
            const graphics_vector3_type &aPos,
            const graphics_quaternion_type &aRot = {},
            const graphics_vector3_type &aSca = {1});
        //transform(mat4x4) ^
        void transform(const std::string &aPositionAttributeName, graphics_mat4x4_type &aTransform);

        //! applies a transformation to a 2 component attribute
        void transform(const std::string &aUVAttributeName,
            const graphics_vector2_type &aPos,
            //rotation float?
            const graphics_vector2_type &aSca = {1});
        //transform(mat3x3) ?

        //! sorts verticies from nearest to furthest based on the distance between the centroid 
        /// of the triangles they form and a given position in world space
        void sort_by_nearest_triangle(
            const graphics_vector3_type &aObserverWorldPosition,
            graphics_mat4x4_type aEntityInstanceWorldMatrix,
            const std::string &aPositionAttributeName = "a_Position");
        
        //! sorts verticies from furthest to nearest based on the distance between the centroid 
        /// of the triangles they form and a given position in world space
        void sort_by_furthest_triangle(
            const graphics_vector3_type &aObserverWorldPosition,
            graphics_mat4x4_type aEntityInstanceWorldMatrix,
            const std::string &aPositionAttributeName = "a_Position");

        /// \brief append a different vertex_data to this vertex_data
        void push_back(const vertex_data &other);

        /// \brief overwrite a section of vertex data using another vertex data instance
        ///
        /// formats must match exactly, the incoming data cannot write past the end of this
        void overwrite(const size_t index, const vertex_data &other);

        /// \brief overwrite a section of a single attribute in the vertex data
        ///
        /// the vertex data must contain an attribute with the given name and that attribute must have
        /// the same # of components
        void overwrite(const std::string &aAttributeName, const size_t vertexOffset, const vertex_data &other);

        //! get number of verticies in the data
        size_t vertex_count() const;

        //! create a new instance that is a concatenation of two separate datas
        vertex_data operator+(const vertex_data &aData);

        //! append a copy of different vertex_data to this vertex_data
        /// \warn must be same format
        vertex_data &operator+=(const vertex_data &other);

        //! support move semantics
        vertex_data &operator=(vertex_data &&other) = default;
        //! support move semantics
        vertex_data(vertex_data &&) = default;

        //! support copy semantics
        vertex_data(const vertex_data &) = default;
        //! support copy semantics
        vertex_data &operator=(const vertex_data &other) = default;

        //! default inited vertex_data is very useful for procedurally generated data
        vertex_data() = default;

        vertex_data(attribute_collection_type &&aAttributeData);

        ~vertex_data() = default;

    private:
        size_t m_VertexCount{};
        attribute_collection_type m_Attributes{};
        primitive_mode m_PrimitiveMode = primitive_mode::triangles; 
    };
}

#endif

