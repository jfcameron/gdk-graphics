// © Joseph Cameron - All Rights Reserved

#ifndef JFC_CONTIGUOUS_VIEW_H
#define JFC_CONTIGUOUS_VIEW_H

namespace jfc
{
/// \brief provides an interface to a contiguous sequence of values
/// without exposing the interface of the underlying collection and
/// without copying or moving its data.
///
/// - a view does not own the data it points to.
/// - a non-constant view allows the user to read and modify values 
///   so long as value_type_param is also a non-constant type.
/// - a constant view only allows the user to read values, even if 
///   value_type_param is a non-constant type.
/// - to construct a view from a constant collection, value_type_param must be const 
///   qualified regardless of whether or not the collection's value type is constant
///   since the view will only have access to the collection's constant interface.
///
template<class value_type_param>
class contiguous_view final
{
public:
    using size_type = size_t;
    using value_type = value_type_param;

    /// construct from a pointer to the head of a sequence 
    /// and the number of values in the sequence
    template<class Pointer>
    constexpr contiguous_view(Pointer *pBegin, const size_type aSize)
    : m_pBegin(pBegin)
    , m_pEnd(pBegin + aSize)
    , m_Size(aSize)
    {}

    /// construct from an iterator to the beginning of a sequence
    /// and the number of values in the sequence
    template<class Iterator>
    constexpr contiguous_view(Iterator iBegin, const size_type aSize)
    : contiguous_view(static_cast<value_type_param *>(&(*iBegin)), aSize)
    {}

    /// construct from a STL contiguous collection (array, vector, string)
    /// view will encompass the full sequence owned by the collection
    /// at the time the view was constructed
    template<class Collection>
    constexpr contiguous_view(Collection &aCollection)
    : contiguous_view(static_cast<value_type_param *>(&(*aCollection.begin())), aCollection.size())
    {}

    /// construct from a pair of pointers.
    /// - both pointers must point somewhere in the same sequence of data
    /// - pEnd must come after pBegin in the sequence. pEnd defines the end of the
    /// - sequence to be viewed, pEnd points to one element after the last
    template<class Pointer>
    contiguous_view(Pointer *pBegin, Pointer *pEnd)
    : contiguous_view(static_cast<value_type_param *>(&(*pBegin)), 
        [pBegin, pEnd]()
        {
            size_type size(0);

            for(auto iter(pBegin); iter != pEnd; ++iter) 
                ++size;

            return size;
        }())
    {}

    /// construct from a pair of iterators.
    template<class Iterator>
    contiguous_view(Iterator aBegin, Iterator aEnd)
    : contiguous_view(static_cast<value_type_param *>(&(*aBegin)), 
        static_cast<value_type_param *>(&(*aEnd)))
    {}

    /// \brief constructs an empty view
    constexpr contiguous_view() = default;

    /// \brief support copy semantics
    constexpr contiguous_view(const contiguous_view &) = default;

    /// \brief support copy semantics
    constexpr contiguous_view &operator=(const contiguous_view &rhv) = default;

    /// \brief support move semantics
    constexpr contiguous_view(contiguous_view &&) = default;
    
    /// \brief support move semantics
    constexpr contiguous_view &operator=(contiguous_view &&rhv) = default;

    /// \brief access an element by index
    [[nodiscard]] constexpr value_type &operator[](const size_type index)
    {
        return *(m_pBegin + index);
    }

    /// \brief access an element by index, constant
    [[nodiscard]] constexpr const value_type &operator[](const size_type index) const
    {
        return *(m_pBegin + index);
    }

    /// \brief pointer to the beginning of the data
    [[nodiscard]] constexpr value_type_param *begin() 
    {
        return m_pBegin;
    }
    
    /// \brief pointer to the beginning of the data, constant
    [[nodiscard]] constexpr value_type_param *begin() const  
    { 
        return static_cast<const value_type *>(m_pBegin);
    }

    /// \brief pointer to the element following the last
    [[nodiscard]] constexpr value_type_param *end() 
    {
        return m_pEnd;
    }

    /// \brief pointer to the element following the last, constant
    [[nodiscard]] constexpr value_type_param *end() const  
    { 
        return static_cast<const value_type *>(m_pEnd);
    }

    /// \brief number of elements accessible through the view
    [[nodiscard]] constexpr size_t size() const 
    {
        return m_Size;
    }

    /// \brief whether or not size is 0
    [[nodiscard]] constexpr bool empty() const
    {
        return m_Size == 0;
    }

private:
    value_type_param *m_pBegin = nullptr;
    
    value_type_param *m_pEnd = nullptr;
    
    size_type m_Size = 0;
};
}

#endif

