// © 2019 Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/camera.h>
#include <gdk/graphics_types.h>

#include <string>

using namespace gdk;

TEST_CASE("camera", "[camera]")
{
    initGL();

    camera a;

    SECTION("default ctor worked")
    {
        REQUIRE(!jfc::glGetError());
    }
        
    a.setClearColor(Color::Blue);
    
    a.setViewportPosition(0.5, 0.0);
    
    a.setViewportSize(0.5, 1.0);

    a.setViewMatrix(graphics_vector3_type(0, 0, 0), graphics_quaternion_type());

    a.setProjection(90, 0.001, 20, 1);

    SECTION("draw worked")
    {
        a.draw(0, 0, graphics_intvector2_type(), {});

        REQUIRE(!jfc::glGetError());
    }

    SECTION("copy semantics")
    {
        camera b(a);

        auto c = b;

        c.draw(0, 0, graphics_intvector2_type(), {});

        REQUIRE(!jfc::glGetError());
    }

    SECTION("move semantics")
    {
        camera b(std::move(a));

        auto c = b;

        c.draw({}, {}, {}, {});

        REQUIRE(!jfc::glGetError());
    }
}

