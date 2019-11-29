// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/scene.h>

using namespace gdk;

TEST_CASE("gdk::scene", "[gdk::scene]")
{
    scene a;

    SECTION("blar")
    {
        REQUIRE(true);
    }
}

