// © Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/webgl1es2_vertex_format.h>

using namespace gdk;

TEST_CASE("webgl1es2_vertex_format", "[gdk::webgl1es2_vertex_format]")
{
    initGL();

    SECTION("webgl1es2_vertex_format webgl1es2_vertex_format")
    {
        auto format = webgl1es2_vertex_format::Pos3;
        
        REQUIRE(format.getSumOfAttributeComponents() == decltype(format.getSumOfAttributeComponents())(3));
    }
}

