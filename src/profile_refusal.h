// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_PROFILE_REFUSAL_H
#define GDK_GRAPHICS_PROFILE_REFUSAL_H

#include <gdk/graphics/profile.h>

#include <cstddef>
#include <string>
#include <string_view>

namespace gdk::graphics::detail {
    [[nodiscard]] inline std::string past(const profile &aProfile, std::string_view aWhat, const std::size_t aHas,
        std::string_view aLimit, const std::size_t aMost) {
        return aProfile.name + ": " + std::string(aWhat) + " " + std::to_string(aHas) + ", past " + aProfile.name + "'s "
            + std::string(aLimit) + " of " + std::to_string(aMost);
    }
}

#endif
