// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/shader_program.h>

using namespace gdk;

TEST_CASE("gdk::shader_program", "[gdk::shader_program]")
{
    initGL();

    SECTION("AlphaCutOff shader initializes correctly")
    {
        auto a = static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff);

        REQUIRE(a->useProgram() != 0);
    }

    SECTION("equality semantics")
    {
        auto a = static_cast<std::shared_ptr<shader_program>>(shader_program::AlphaCutOff);
        auto b = a;

        REQUIRE(*a == *b);
    }
}

