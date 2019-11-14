// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/uniform_manager.h>

using namespace gdk;

TEST_CASE("gdk::uniform_manager", "[gdk::uniform_manager]")
{
    initGL();

    uniform_manager uniforms;

    SECTION("")
    {
        REQUIRE(true);
    }
}

