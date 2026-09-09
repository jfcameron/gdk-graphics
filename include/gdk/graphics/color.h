// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_COLOR_H
#define GDK_GFX_COLOR_H

#include <gdk/graphics/types.h>

#include <iosfwd>

namespace gdk::graphics {
    /// \brief Represents a 4 channel color: {Red, Green, Blue, Alpha}.
    ///TODO: channel values should be limited to the range 0.0 - 1.0
    struct color final {
        using channel_type = float;

        channel_type r = 0, g = 0, b = 0, a = 1;

        /// \brief clamp each channel to the [0 - 1] range
        void clamp(); 

        /// \brief add two colors together.
        /// \warning does not clamp values within the normalized range
        void operator+=(const color &aOther);
        
        /// \brief equality semantics
        [[nodiscard]] constexpr bool operator==(const color &aOther) const = default;

        /// \brief copy semantics
        color(const color &) = default;
        /// \brief copy semantics
        color& operator=(const color &acolor) = default;
        
        /// \brief move semantics
        color(color &&) = default;
        /// \brief move semantics
        color& operator=(color &&acolor) = default;

        /// \brief constructs a color as opaque black
        color() = default;

        /// \brief constructs a color with provided channel values. 
        /// Alpha has a default opaque value
        constexpr color(const channel_type aR,
            const channel_type aG,
            const channel_type aB,
            const channel_type aA = 1)
        : r(aR), g(aG), b(aB), a(aA)
        {}

    /// \name special values
    ///@{
    //
        //! opaque black color
        static const color black;
        //! opaque white color
        static const color white;
        //! opaque red color
        static const color red;
        //! opaque green color
        static const color green;
        //! opaque dark green color
        static const color dark_green;
        //! opaque blue color
        static const color blue;
        //! opaque bright pink color, useful for indicating 
        /// color-related errors (stands out against most palettes)
        static const color deathly_pink;
        //! an opaque pastel blue color
static const color cornflower_blue;
    ///@}
    };
        
    std::ostream &operator<<(std::ostream &stream, const color &acolor);

    inline const color color::black{0.0f, 0.0f, 0.0f, 1.0f};
    inline const color color::white{1.0f, 1.0f, 1.0f, 1.0f};
    inline const color color::red{1.0f, 0.0f, 0.0f, 1.0f};
    inline const color color::green{0.0f, 1.0f, 0.0f, 1.0f};
    inline const color color::dark_green{0.0f, 0.6f, 0.0f, 1.0f};
    inline const color color::blue{0.0f, 0.0f, 1.0f, 1.0f};
    inline const color color::deathly_pink{1.0f, 0.2f, 0.8f, 1.0f};
    inline const color color::cornflower_blue{
        0.3921568627450980392156862745098f,
        0.58431372549019607843137254901961f,
        0.92941176470588235294117647058824f,
        1.0f
    };
}

#endif

