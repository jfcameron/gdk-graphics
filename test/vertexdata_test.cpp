// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/vertexdata.h>

using namespace gdk;

TEST_CASE("vertexdata constructors", "[vertexdata]")
{
    initGL();

    SECTION("Hardcoded vertex data is well formed")
    {
        auto pCube = static_cast<std::shared_ptr<VertexData>>(VertexData::Cube);

        REQUIRE(pCube->getName() == "Cube");
        REQUIRE(pCube->getHandle() >= 0);

        auto pQuad = static_cast<std::shared_ptr<VertexData>>(VertexData::Quad);

        REQUIRE(pQuad->getName() == "Quad");
        REQUIRE(pQuad->getHandle() >= 0);
    }
}

