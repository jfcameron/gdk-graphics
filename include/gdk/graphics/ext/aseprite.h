// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_EXT_ASEPRITE_H
#define GDK_GRAPHICS_EXT_ASEPRITE_H

#include <gdk/graphics/ext/sprite_animation.h>

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gdk::graphics::ext {
    struct aseprite_frame final {
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;

        int offsetX = 0;
        int offsetY = 0;
        int sourceWidth = 0;
        int sourceHeight = 0;

        int milliseconds = 0;

        [[nodiscard]] bool operator==(const aseprite_frame &) const = default;
    };

    //! which way round a tag's frames play. \see aseprite_tag
    enum class aseprite_direction { 
        forward, 
        reverse, 
        ping_pong, 
        ping_pong_reverse 
    };

    /// \brief a tag is a run of frames with a name
    struct aseprite_tag final {
        std::string name;

        int from = 0;
        int to = 0;

        aseprite_direction direction = aseprite_direction::forward;

        int repeat = 0;

        [[nodiscard]] bool operator==(const aseprite_tag &) const = default;
    };

    /// \brief a slice is a named rectangle of a frame which may carry a pivot
    struct aseprite_slice final {
        std::string name;

        int frame = 0;

        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;

        std::optional<std::pair<int, int>> pivot;

        [[nodiscard]] bool operator==(const aseprite_slice &) const = default;
    };

    struct aseprite_sheet final {
        std::string image;

        int width = 0;
        int height = 0;

        std::vector<aseprite_frame> frames;

        std::vector<aseprite_tag> tags;

        std::vector<aseprite_slice> slices;

        //! the tag of that name
        [[nodiscard]] const aseprite_tag *tag(std::string_view aName) const;

        //! the first slice of that name
        [[nodiscard]] const aseprite_slice *slice(std::string_view aName) const;
    };

    /// \brief read an exported sheet's JSON
    [[nodiscard]] aseprite_sheet read_aseprite(std::span<const std::byte> aBytes);

    /// \brief a tag's frames in the order they play
    [[nodiscard]] std::vector<sprite_animation::frame> frames_of(
            const aseprite_sheet &aSheet, 
            const aseprite_tag &aTag);

    //! every frame of the sheet, for one that has no tags at all. \see frames_of
    [[nodiscard]] std::vector<sprite_animation::frame> frames_of(const aseprite_sheet &aSheet);

    /// \brief one tag of a sheet as an animation
    [[nodiscard]] sprite_animation animation_of(const aseprite_sheet &aSheet, std::string_view aTag);

    //! the whole sheet as one looping animation, for a sheet with no tags
    [[nodiscard]] sprite_animation animation_of(const aseprite_sheet &aSheet);
}

#endif
