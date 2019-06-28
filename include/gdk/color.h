// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_COLOR_H
#define GDK_GFX_COLOR_H

#include <iosfwd>

namespace gdk
{
    /// \brief Represents a 4 channel color: {Red, Green, Blue, Alpha}.
    struct Color final
    {
        using channel_type = float;

        channel_type r = 0, g = 0, b = 0, a = 1;
                        
        bool operator==(const Color &aOther) const;

        Color& operator=(const Color &aColor) = default;
        Color& operator=(Color &&aColor) = default;

        Color();
        Color(const channel_type aR, const channel_type aG, const channel_type aB, const channel_type aA = 1);
        Color(const Color &) = default;
        Color(Color &&) = default;
        ~Color() = default;

        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color DarkGreen;
        static const Color Blue;
        static const Color DeathlyPink;
        static const Color CornflowerBlue;
    };
        
    std::ostream &operator<<(std::ostream &stream, const Color &aColor);
}

#endif
