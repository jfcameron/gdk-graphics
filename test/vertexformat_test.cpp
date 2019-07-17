// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/vertexformat.h>

using namespace gdk;

TEST_CASE("vertexformat constructors", "[vertexformat]")
{
    initGL();

    SECTION("vertexformat vertexformat")
    {
        auto format = VertexFormat::Pos3;
        
        REQUIRE(format.getSumOfAttributeComponents() == decltype(format.getSumOfAttributeComponents())(3));
    }
}

