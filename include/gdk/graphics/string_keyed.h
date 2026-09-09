// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_STRING_KEYED_H
#define GDK_GRAPHICS_STRING_KEYED_H

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace gdk::graphics {
    /// \brief a hash that will take anything a string_view can be built from
    struct transparent_string_hash final {
        using is_transparent = void;

        [[nodiscard]] std::size_t operator()(const std::string_view aName) const {
            return std::hash<std::string_view>{}(aName);
        }
    };

    /// \brief a map from a name to something, searchable without building a string to search with
    template<typename value_type>
    using string_keyed = std::unordered_map<std::string, value_type,
        transparent_string_hash, std::equal_to<>>;
}

#endif
