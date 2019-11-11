// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/vertex_attribute.h>

using namespace gdk;

TEST_CASE("gdk::vertexattribute", "[gdk::vertexattribute]")
{
    constexpr auto NAME = "uv";
    constexpr decltype(vertex_attribute::size) SIZE(2);

    const vertex_attribute a(NAME, SIZE);

    SECTION("equality semantics")
    {
        auto a = vertex_attribute("a_normal", 3);
        auto b = vertex_attribute("a_uv", 2);

        REQUIRE(a != b); 
    }
    SECTION("copy semantics")
    {
        vertex_attribute b = a;
        
        REQUIRE(b.name == NAME);
        REQUIRE(b.size == SIZE);
    }
    
    SECTION("move semantics")
    {
        vertex_attribute b(a);

        vertex_attribute c = std::move(b);
        
        REQUIRE(c.name == NAME);
        REQUIRE(c.size == SIZE);
    }

    SECTION("standard ctor")
    {
        REQUIRE(a.name == NAME);
        REQUIRE(a.size == SIZE);
    }
}

