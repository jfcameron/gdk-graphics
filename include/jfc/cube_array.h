// © Joseph Cameron - All Rights Reserved

#ifndef JFC_CUBE_ARRAY_H
#define JFC_CUBE_ARRAY_H

#include <array>

namespace jfc {
    /// \brief 3d array where its length width and height are equal
    ///
    template<typename value_type, size_t size_param>
    class cube_array final {
    public:
        static constexpr size_t size = size_param;
        static constexpr size_t size_1d = size * size * size;
    private:
        std::array<value_type, size_1d> m_Data;
    public:
        const value_type &at(const size_t aX, const size_t aY, const size_t aZ) const {
            return m_Data[index(aX, aY, aZ)];
        }

        const decltype(m_Data) &data() const  {
            return m_Data;
        }

        value_type &at(const size_t aX, const size_t aY, const size_t aZ) {
            return m_Data[index(aX, aY, aZ)];
        }

        decltype(m_Data) &data() {
            return m_Data;
        }

        void clear() { 
            m_Data = {};
        }
    private:
        constexpr size_t index(const size_t aX, const size_t aY, const size_t aZ) const noexcept {
            return aX + size * aY + size * size * aZ;
        }
    };
}

#endif

