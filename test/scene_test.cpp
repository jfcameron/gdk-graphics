// © Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/camera.h>
#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_scene.h>

#include "test_include.h"

using namespace gdk;

TEST_CASE("gdk::webgl1es2_scene", "[gdk::webgl1es2_scene]")
{
    webgl1es2_scene a;

    SECTION("can add camera, remove camera, and contains camera reports expected values")
    {
        auto pCamera = std::shared_ptr<camera>(new webgl1es2_camera());

        REQUIRE(!a.contains_camera(pCamera));

        a.add_camera(pCamera);
        
        REQUIRE(a.contains_camera(pCamera));

        a.remove_camera(pCamera);
        
        REQUIRE(!a.contains_camera(pCamera));
    }
   
    SECTION("draw does not cause gl errors")
    {
        a.draw({0, 0});

        REQUIRE(!jfc::glGetError());
    }

    SECTION("Entity methods")
    {
        //auto pEntity = std::shared_ptr<entity>(new webgl1es2_entity());
    }
}

