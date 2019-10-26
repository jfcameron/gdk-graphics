// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/perspective_camera.h>

using namespace gdk;

TEST_CASE("camera constructors", "[camera]")
{
    initGL();

    SECTION("blarblarblarblar")
    {
        perspective_camera camera;

        camera.setViewportSize(0.5, 1.0);
        camera.setViewportPosition(0.5, 0.0);
        camera.setClearColor(Color::Blue);

        camera.draw({}, {}, {}, {});

        REQUIRE(!jfc::glGetError());
    }
}

