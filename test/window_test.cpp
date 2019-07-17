// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/window.h>

using namespace gdk;

TEST_CASE("window constructors", "[window]")
{
    SECTION("blar")
    {
        constexpr auto name = "Cool Window";

        SimpleGLFWWindow window(name);      

        REQUIRE(window.getPointerToGLFWWindow());
        REQUIRE(window.getName() == name);
    }
}

