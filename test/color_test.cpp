// © Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/graphics/color.h>

using namespace gdk;
using namespace gdk::graphics;

TEST_CASE("gdk::color", "[gdk::color]")
{
    SECTION("default ctor produces opaque black color")
    {
        color color;

        REQUIRE(color == color::black);
    }

    SECTION("equality semantics work")
    {
        REQUIRE(color::blue == color::blue);

        REQUIRE(color::red != color::green);
    }

    SECTION("move semantics")
    {
        color a = color::cornflower_blue;
        {
            color b = std::move(a);

            REQUIRE(b == color::cornflower_blue);
        }
    }
    
    SECTION("copy semantics")
    {
        color a = color::white;

        auto b = a;

        REQUIRE(b == a);
    }
}

