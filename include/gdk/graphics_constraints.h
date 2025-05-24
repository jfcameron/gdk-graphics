// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CONSTRAINTS_H
#define GDK_GFX_CONSTRAINTS_H

#include <cmath>

namespace gdk {
    constexpr bool is_power_of_two(const std::size_t aNumber) {
        return aNumber != 0 && (aNumber & (aNumber - 1)) == 0;
    }
}

#endif

