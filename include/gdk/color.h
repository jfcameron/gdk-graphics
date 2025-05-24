// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_COLOR_H
#define GDK_GFX_COLOR_H

#include <iosfwd>

namespace gdk {
    /// \brief Represents a 4 channel color: {Red, Green, Blue, Alpha}.
    ///TODO: channel values should be limited to the range 0.0 - 1.0
    struct color final {
        using channel_type = float;

        channel_type r = 0, g = 0, b = 0, a = 1;

        /// \brief clamp each channel to the [0 - 1] range
        void clamp(); 

        /// \brief add two colors together.
        /// \warn does not clamp values within the normalized range
        void operator+=(const color &aOther);
        
        /// \brief equality semantics
        bool operator==(const color &aOther) const;
        /// \brief equality semantics
        bool operator!=(const color &aOther) const;

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
        color(const channel_type aR, 
            const channel_type aG, 
            const channel_type aB, 
            const channel_type aA = 1);

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
}

#endif

