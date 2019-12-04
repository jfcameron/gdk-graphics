// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/color.h>

using namespace gdk;

TEST_CASE("gdk::color", "[gdk::color]")
{
    SECTION("default ctor produces opaque black color")
    {
        color color;

        REQUIRE(color == color::Black);
    }

    SECTION("equality semantics work")
    {
        REQUIRE(color::Blue == color::Blue);

        REQUIRE(color::Red != color::Green);
    }

    SECTION("move semantics")
    {
        color a = color::CornflowerBlue;
        {
            color b = std::move(a);

            REQUIRE(b == color::CornflowerBlue);
        }
    }
    
    SECTION("copy semantics")
    {
        color a = color::White;

        auto b = a;

        REQUIRE(b == a);
    }
}

