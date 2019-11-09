// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_COLOR_H
#define GDK_GFX_COLOR_H

#include <iosfwd>

namespace gdk
{
    /// \brief Represents a 4 channel color: {Red, Green, Blue, Alpha}.
    struct color final
    {
        using channel_type = float;

        channel_type r = 0, g = 0, b = 0, a = 1;
                        
        bool operator==(const color &aOther) const;

        color& operator=(const color &acolor) = default;
        color& operator=(color &&acolor) = default;

        color();
        color(const channel_type aR, const channel_type aG, const channel_type aB, const channel_type aA = 1);
        color(const color &) = default;
        color(color &&) = default;
        ~color() = default;

        static const color Black;
        static const color White;
        static const color Red;
        static const color Green;
        static const color DarkGreen;
        static const color Blue;
        static const color DeathlyPink;
        static const color CornflowerBlue;
    };
        
    std::ostream &operator<<(std::ostream &stream, const color &acolor);
}

#endif
