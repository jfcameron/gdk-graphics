// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VERTEX_DATA_H
#define GDK_GFX_VERTEX_DATA_H

#include <gdk/graphics_types.h>

#include <jfc/contiguous_view.h>

#include <array>
#include <functional>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace gdk
{
/// \brief A sequence of vertexes stored in system memory.
///
/// when given to a model, vertex data is used to create the polygonal surfaces of that model
///
/// interleaved data, best for data that does not need to be updated
///
class vertex_data final
{
public:
    /// \brief type of a single vertex attribute component.
    using component_type = graphics_floating_point_type;

    //TODO: separate this from vertex_data? maybe
    //TODO: unify atttribute_data and attribute_data_view: attribute_data::view.
    //TODO: create methods to convert them, to simplify impl
    class attribute_data
    {
    public:
        /// \brief view to attribute data owned by the user. Used when constructing a vertex_data instance
        /// 
        /// a view on to a contiguous sequence of component_type values representing the values of all 
        /// the components of a single attribute for a model + the size of the attribute
        /// 
        class view final
        {
        public:
            /// \brief build a view from a view to vertex component data and a attribute size
            view(jfc::contiguous_view<const component_type> aDataView, 
                const size_t aComponentCount);
            
            /// \brief [convenience] build a view from a const reference to component data and attribute size
            view(const std::vector<component_type> &aData, 
                const size_t aComponentCount);

            const component_type *begin() const;
            
            size_t data_size() const;
            
            size_t component_count() const;

        private:
            const component_type *m_pData; 

            size_t m_DataLength; 

            size_t m_ComponentCount; 
        };

        attribute_data() = default;

        attribute_data(const std::vector<component_type> &m_Components, 
            const size_t aComponentCount);
        
        attribute_data &operator+=(const attribute_data &rhs);
        
        //TODO: create from view
        //attribute_data(const attribute_data::view)

    //private://TODO: write getters
        //! collections of all the components for all of this attribute
        std::vector<component_type> m_Components;
        
        //! number of components in a single attribute
        size_t m_ComponentCount;

        //! number of vertexes this data provides components for
        //size_t m_VertexCount;
    };
    
    using attribute_collection_type = std::unordered_map<std::string, attribute_data>;
    
    using index_value_type = unsigned short;

    enum class PrimitiveMode
    {
        Triangles
    };

    using attribute_name_to_view_type = std::unordered_map<std::string, attribute_data::view>;

    /// \brief gets the primitive mode
    PrimitiveMode primitive_mode() const;

    size_t attribute_offset(const std::string &aName) const;

    //TODO: this returns a constant. need to modify vertex_data ctor to accept index data
    //TODO: change to view
    //TODO; rename "indicies"
    const std::vector<index_value_type> &getIndexData() const;

    //! clears all state from this vertex_data instance
    void clear();

    //! Convenience method, applies a transformation to a 3 component attribute
    //TODO: these should be able to work directly on uniform_data & nonconst uniform_data_view
    //So maybe this should be moved to a function not a method
    void transform_position(
        const graphics_vector3_type &aPos,
        const graphics_quaternion_type &aRot = {},
        const graphics_vector3_type &aSca = {1},
        const std::string &aPositionAttributeName = "a_Position");
        
    //! Convenience method, applies a transformation to a 2 component attribute
    //TODO: these should be able to work directly on uniform_data & nonconst uniform_data_view
    //So maybe this should be moved to a function not a method
    void transform_uv(
        const graphics_vector2_type &aPos,
        //TODO: float aRot,
        const graphics_vector2_type &aSca = {1},
        const std::string &aUVAttributeName = "a_UV");

    //void transform_scalar(

    //! Convenience method, sorts all attribute data by distance of an entity to an observer
    void sort_by_nearest(
        const graphics_vector3_type &aObserverWorldPosition,
        graphics_mat4x4_type aEntityInstanceWorldMatrix,
        const std::string &aPositionAttributeName = "a_Position");
    //TODO:
    // vector3 aObserverWorldPosition, mat4x4 aInstanceWorldMatrix
    // change args^

    const attribute_collection_type &data() const;

    /// \brief append a different vertex_data to this vertex_data
    ///
    /// returns the index to the start of the newly added data
    //TODO: allow new attribute_datas to be introduced, but be sure vertexcount is same etc.
    size_t push_back(const vertex_data &other);

    /// \brief overwrite a section of vertex data using another vertex data instance
    ///
    /// formats must match exactly, the incoming data cannot write past the end of this
    void overwrite(const size_t index, const vertex_data &other);

    /// \brief overwrite a section of a single attribute in the vertex data
    ///
    /// the vertex data must contain an attribute with the given name and that attribute must have
    /// the same # of components
    void overwrite(const std::string &aAttributeName, const size_t vertexOffset, const vertex_data &other);

    size_t vertex_count() const;

    //! create a new instance that is a concatenation of two separate datas
    vertex_data operator+(const vertex_data &aData);

    //! append a copy of different vertex_data to this vertex_data
    /// \warn must be same format
    vertex_data &operator+=(const vertex_data &other);

    //! assignment by const ref
    vertex_data &operator=(const vertex_data &other) = default;

    //! assignment by rvalue
    vertex_data &operator=(vertex_data &&other) = default;

    //! construct a vertex_data from attribute data
    vertex_data(const attribute_name_to_view_type &aAttributeData);

    vertex_data(const vertex_data &) = default;
    
    vertex_data(vertex_data &&) = default;

    //! default inited vertex_data is very useful for procedurally generated data
    vertex_data() = default;

private:
    size_t m_VertexCount = 0;

    //! primitive type to emit at primitive stage
    PrimitiveMode m_PrimitiveMode = PrimitiveMode::Triangles; 
    
    attribute_collection_type m_NonInterleavedData;
};
}
#endif

