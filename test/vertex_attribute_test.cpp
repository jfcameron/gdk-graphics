// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/vertex_attribute.h>

using namespace gdk;

TEST_CASE("vertexattribute constructors and assignment operators", "[vertexattribute]")
{
    constexpr auto NAME = "uv";
    constexpr decltype(vertex_attribute::size) SIZE(0);

    const vertex_attribute a(NAME, SIZE);

    SECTION("standard ctor")
    {
        REQUIRE(a.name == NAME);
        REQUIRE(a.size == SIZE);
    }

    SECTION("copy ctor")
    {
        vertex_attribute b(a);
        
        REQUIRE(b.name == NAME);
        REQUIRE(b.size == SIZE);
    }
    
    SECTION("move ctor")
    {
        vertex_attribute b(a);
        
        REQUIRE(b.name == NAME);
        REQUIRE(b.size == SIZE);
    }
    
    SECTION("copy operator")
    {
        vertex_attribute b = a;
        
        REQUIRE(b.name == NAME);
        REQUIRE(b.size == SIZE);
    }
    
    SECTION("move operator")
    {
        vertex_attribute b(a);

        vertex_attribute c = std::move(b);
        
        REQUIRE(c.name == NAME);
        REQUIRE(c.size == SIZE);
    }
}

