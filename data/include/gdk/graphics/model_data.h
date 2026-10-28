// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_MODEL_DATA_H
#define GDK_GRAPHICS_MODEL_DATA_H

#include <cstddef>
#include <gdk/graphics/string_keyed.h>
#include <gdk/graphics/attribute_data.h>
#include <gdk/graphics/data_types.h>

#include <array>
#include <functional>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace gdk::graphics {
    /// \brief vertex data stored in system memory. 
    ///
    /// vertex data is comprised of a collection of attributes, which in turn are a collection of components
    class model_data final {
    public:
        using attribute_collection_type = string_keyed<attribute_data>;
        using index_value_type = unsigned short;

        enum class primitive_mode {
            triangles
        };

        //! get the primitive mode
        primitive_mode get_primitive_mode() const;

        /// \brief the order the vertices are drawn in, empty if they are drawn in storage order
        ///
        /// A model with indices is drawn with glDrawElements and one without with glDrawArrays.
        /// Indexing lets a vertex shared by several triangles be stored once, which for a typical
        /// closed surface is about a third of the vertices a triangle soup needs.
        const std::vector<index_value_type> &indexes() const;

        //! does this model name its vertices in an explicit order?
        [[nodiscard]] bool indexed() const;

        /// \brief draw the vertices in this order rather than in the order they are stored
        ///
        /// \throws exception if an index is past the end of the vertex data. Out of range indices
        /// are undefined behaviour in gl: they usually draw garbage or take the driver down, and
        /// they never say why, so they are refused here where the cause is still visible.
        ///
        /// An empty list removes the indices, returning the model to storage order.
        void set_indexes(std::vector<index_value_type> aIndexes);

        //! get collection of all attribute data
        const attribute_collection_type &attributes() const;

        //! get attribute data by name, e.g: "a_Position"
        const attribute_data &get_attribute_data(std::string_view aAttributeName) const;
        //! get attribute data by name, e.g: "a_Position"
        attribute_data &get_attribute_data(std::string_view aAttributeName);

        //! clears all state from this model_data instance
        void clear();

        //! applies a transformation to a 3 component attribute
        void transform(const std::string &aPositionAttributeName, 
            const vector3_type &aPos,
            const quaternion_type &aRot = {},
            const vector3_type &aSca = {1, 1, 1});
        //! applies a transformation matrix to a 3 component attribute
        void transform(const std::string &aPositionAttributeName, const matrix4x4_type &aTransform);

        //! applies a transformation to a 2 component attribute
        void transform(const std::string &aUVAttributeName,
            const vector2_type &aPos,
            const float aRotation = 0,
            const vector2_type &aSca = {1, 1});

        //! sorts verticies from nearest to furthest based on the distance between the centroid 
        /// of the triangles they form and a given position in world space
        void sort_by_nearest_triangle(
            const vector3_type &aObserverWorldPosition,
            matrix4x4_type aEntityInstanceWorldMatrix,
            std::string_view aPositionAttributeName = "a_Position");
        
        //! sorts verticies from furthest to nearest based on the distance between the centroid 
        /// of the triangles they form and a given position in world space
        void sort_by_furthest_triangle(
            const vector3_type &aObserverWorldPosition,
            matrix4x4_type aEntityInstanceWorldMatrix,
            std::string_view aPositionAttributeName = "a_Position");

        /// \brief append a different model_data to this model_data
        ///
        /// Nothing changes when it throws. The appended indices are offset so that they still name the appended vertices. If only
        /// one of the two is indexed, the other is given the indices it implies, because the
        /// result has to be one thing or the other.
        void push_back(const model_data &other);

        /// \brief overwrite a section of vertex data using another vertex data instance
        ///
        /// formats must match exactly, the incoming data cannot write past the end of this
        void overwrite(const std::size_t index, const model_data &other);

        /// \brief overwrite a section of a single attribute in the vertex data
        ///
        /// the vertex data must contain an attribute with the given name and that attribute must have
        /// the same # of components
        void overwrite(std::string_view aAttributeName, const std::size_t vertexOffset, const model_data &other);

        //! get number of verticies in the data
        std::size_t vertex_count() const;

        //! create a new instance that is a concatenation of two separate datas
        model_data operator+(const model_data &aData);

        //! append a copy of different model_data to this model_data
        /// \warning must be same format
        model_data &operator+=(const model_data &other);

        //! support move semantics
        model_data &operator=(model_data &&other) = default;
        //! support move semantics
        model_data(model_data &&) = default;

        //! support copy semantics
        model_data(const model_data &) = default;
        //! support copy semantics
        model_data &operator=(const model_data &other) = default;

        //! default inited model_data is very useful for procedurally generated data
        model_data() = default;

        //! \throws exception if an attribute is not whole vertices, or the attributes count different numbers of them
        model_data(attribute_collection_type &&aAttributeData);

        //! \throws exception if an index is past the end of aAttributeData. \see set_indexes
        model_data(attribute_collection_type &&aAttributeData,
            std::vector<index_value_type> &&aIndexes);

        ~model_data() = default;

        //! creates model data for a unit sized quad with position and uv data
        static model_data make_quad();

    private:
        std::size_t m_VertexCount{};

        std::vector<index_value_type> m_Indexes{};
        attribute_collection_type m_Attributes{};
        primitive_mode m_PrimitiveMode = primitive_mode::triangles; 
    };
}

#endif

