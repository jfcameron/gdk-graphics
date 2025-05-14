// © Joseph Cameron - All Rights Reserved

#ifndef JFC_TO_ARRAY_H
#define JFC_TO_ARRAY_H

#include <array>
#include <type_traits>
#include <utility>

namespace jfc {
    /// shim for c++20 stl's std::to_array template. requires c++17 language support.
    ///
    template <typename T, std::size_t N>
    constexpr std::array<T, N> to_array(const T (&arr)[N]) {
        std::array<T, N> result{};
        std::copy(std::begin(arr), std::end(arr), result.begin());
        return result;
    }

    template <typename T, typename... Args>
    constexpr std::array<T, sizeof...(Args)> to_array(Args&&... args) {
        return std::array<T, sizeof...(Args)>{std::forward<Args>(args)...};
    }
}

#endif

