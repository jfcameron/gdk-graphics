// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/color.h>

#include <nlohmann/json.hpp>

#include <iostream>

namespace gdk
{
    const color color::Black      (0.0f,0.0f,0.0f,1.0f);
    const color color::White      (1.0f,1.0f,1.0f,1.0f);
    const color color::Red        (1.0f,0.0f,0.0f,1.0f);
    const color color::Green      (0.0f,1.0f,0.0f,1.0f);
    const color color::DarkGreen  (0.0f,0.6f,0.0f,1.0f);
    const color color::Blue       (0.0f,0.0f,1.0f,1.0f);
    const color color::DeathlyPink(1.0f,0.2f,0.8f,1.0f);

    const color color::CornflowerBlue(
        0.3921568627450980392156862745098,
        0.58431372549019607843137254901961,
        0.92941176470588235294117647058824,
        1.
    );

    static constexpr char TAG[] = "color";

    std::ostream &operator<<(std::ostream &s, const color &a)
    {
        return s << nlohmann::json
        {
            {"Type", TAG}, 
            {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
                {}
            },
            
            {"r", a.r},
            {"g", a.g},
            {"b", a.b},
            {"a", a.a},
        }
        .dump();
    }

    bool color::operator==(const color &aOther) const
    {
        return
            r == aOther.r &&
            g == aOther.g &&
            b == aOther.b &&
            a == aOther.a;
    }

    color::color()
        : r(0)
        , g(0)
        , b(0)
        , a(1)
    {}

    color::color(const channel_type aR, const channel_type aG, const channel_type aB, const channel_type aA)
        : r(aR)
        , g(aG)
        , b(aB)
        , a(aA)
    {}
}

