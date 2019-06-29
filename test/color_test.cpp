// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/color.h>

using namespace gdk;

TEST_CASE("color constructors", "[color]" )
{
    SECTION("Default constructor produces an opaque black color")
    {
        Color color;

        REQUIRE(color == Color::Black);
    }
}

