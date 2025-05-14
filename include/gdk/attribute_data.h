// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_ATTRIBUTE_DATA_H
#define GDK_GFX_ATTRIBUTE_DATA_H

#include <gdk/graphics_types.h>

#include <vector>

namespace gdk {
    /// \brief type of a single vertex attribute component.
    using component_type = graphics_floating_point_type; 

    /// \brief attribute_data represents one of the lists of components that comprise a model_data object
    class attribute_data final {
    public:
        //! number of components that per attribute
        /// example: a list of components storing 3d position data would have a component_count of 3
        size_t number_of_components_per_attribute() const;

        size_t number_of_attributes_in_component_data() const;

        //! const access to the list of components in this attribute
        const std::vector<component_type> &components() const;

        //! nonconst access to the list of components in this attribute
        std::vector<component_type> &components();

        void push_back(const attribute_data &rhs);

        attribute_data &operator+=(const attribute_data &rhs);

        void overwrite(size_t vertexOffset, const attribute_data &other);

        //! support move semantics
        attribute_data &operator=(attribute_data &&a) = default;
        //! support move semantics
        attribute_data(attribute_data &&) = default;

        //! support copy semantics
        attribute_data &operator=(const attribute_data &a) = default;
        //! support copy semantics
        attribute_data(const attribute_data &) = default;

        attribute_data(const std::vector<component_type> &aComponents, const size_t aComponentCount);
        attribute_data() = default;
        ~attribute_data() = default;

    private:
        size_t m_NumberOfComponentsPerAttribute{};
        std::vector<component_type> m_Components{};
    };
}

#endif

