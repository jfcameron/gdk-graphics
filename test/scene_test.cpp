// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_scene.h>

using namespace gdk;

TEST_CASE("gdk::webgl1es2_scene", "[gdk::webgl1es2_scene]")
{
    webgl1es2_scene a;

    SECTION("blar")
    {
        a.draw({0, 0});

        REQUIRE(true);
    }
}

