// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CONSTRAINTS_H
#define GDK_GFX_CONSTRAINTS_H

#include <cstddef>

namespace gdk::graphics {
    constexpr bool is_power_of_two(const std::size_t aNumber) {
        return aNumber != 0 && (aNumber & (aNumber - 1)) == 0;
    }

    /// \brief integer square root. rounded down
    constexpr std::size_t square_root(const std::size_t aNumber) {
        std::size_t root = 0;

        while ((root + 1) * (root + 1) <= aNumber) ++root;

        return root;
    }
}

#endif

