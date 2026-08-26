// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_ATTRIBUTE_DATA_H
#define GDK_GFX_ATTRIBUTE_DATA_H

#include <gdk/graphics/types.h>

#include <vector>
#include <initializer_list>
#include <span>

namespace gdk::graphics {
    /// \brief type of a single vertex attribute component.
    using component_type = floating_point_type; 

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

        /// Span rather than `const std::vector &`: the components are copied into this object, so
        /// what the caller held them in does not matter -- an array or a subrange no longer has to be
        /// copied into a vector first. `components()` still returns the vector this owns.
        attribute_data(std::span<const component_type> aComponents, const size_t aComponentCount);

        /// \brief construct from a braced list of components
        ///
        /// A span cannot bind a braced list in C++20 -- there is nothing for it to point at -- and the
        /// mesh literals throughout this library are written that way. Both overloads exist for the
        /// same reason every standard container has an initializer_list constructor beside its
        /// iterator one.
        attribute_data(std::initializer_list<component_type> aComponents, const size_t aComponentCount);
        attribute_data() = default;
        ~attribute_data() = default;

    private:
        size_t m_NumberOfComponentsPerAttribute{};
        std::vector<component_type> m_Components{};
    };
}

#endif

