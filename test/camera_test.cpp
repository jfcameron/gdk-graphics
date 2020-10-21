// © 2019 Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/webgl1es2_camera.h>
#include <gdk/graphics_types.h>

#include <string>

using namespace gdk;

TEST_CASE("camera", "[camera]")
{
    initGL();

    webgl1es2_camera a;

    SECTION("default ctor worked")
    {
        REQUIRE(!jfc::glGetError());
    }
        
    a.setClearcolor(color::Blue);
    
    a.setViewportPosition(0.5, 0.0);
    
    a.setViewportSize(0.5, 1.0);

    a.set_view_matrix(graphics_vector3_type(0, 0, 0), graphics_quaternion_type());

    //a.setProjection(90, 0.001, 20, 1);

    SECTION("activate worked")
    {
        a.activate(graphics_intvector2_type());

        REQUIRE(!jfc::glGetError());
    }

    SECTION("copy semantics")
    {
        webgl1es2_camera b(a);

        auto c = b;

        c.activate(graphics_intvector2_type());

        REQUIRE(!jfc::glGetError());
    }

    SECTION("move semantics")
    {
        webgl1es2_camera b(std::move(a));

        auto c = b;

        c.activate({});

        REQUIRE(!jfc::glGetError());
    }
}

