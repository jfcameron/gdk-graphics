// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/vertexattribute.h>

using namespace gdk;

TEST_CASE("vertexattribute constructors and assignment operators", "[vertexattribute]")
{
    constexpr auto NAME = "uv";
    constexpr decltype(VertexAttribute::size) SIZE(0);

    const VertexAttribute a(NAME, SIZE);

    SECTION("standard ctor")
    {
        REQUIRE(a.name == NAME);
        REQUIRE(a.size == SIZE);
    }

    SECTION("copy ctor")
    {
        VertexAttribute b(a);
        
        REQUIRE(b.name == NAME);
        REQUIRE(b.size == SIZE);
    }
    
    SECTION("move ctor")
    {
        VertexAttribute b(a);
        
        REQUIRE(b.name == NAME);
        REQUIRE(b.size == SIZE);
    }
    
    SECTION("copy operator")
    {
        VertexAttribute b = a;
        
        REQUIRE(b.name == NAME);
        REQUIRE(b.size == SIZE);
    }
    
    SECTION("move operator")
    {
        VertexAttribute b(a);

        VertexAttribute c = std::move(b);
        
        REQUIRE(c.name == NAME);
        REQUIRE(c.size == SIZE);
    }
}

