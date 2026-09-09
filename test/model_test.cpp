// © Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/graphics/webgl1es2_model.h>

using namespace gdk;
using namespace gdk::graphics;

TEST_CASE("webgl1es2_model", "[gdk::webgl1es2_model]")
{
    initGL();

    SECTION("Hardcoded vertex data is well formed")
    {
        /*auto pCube = model::make_cube();

        REQUIRE(pCube->getHandle() >= 0);

        auto pQuad = static_cast<std::shared_ptr<model>>(model::Quad);

        REQUIRE(pQuad->getHandle() >= 0);*/
    }
}


TEST_CASE("**a model re-uploaded with different attributes rebinds correctly**",
    "[gdk::webgl1es2_model]")
{
    initGL();

    const auto pShader = webgl1es2_shader_program::make_alpha_cutoff();

    const auto uv = pShader->tryGetActiveAttribute("a_UV");

    REQUIRE(uv.has_value());

    const auto uv_array_enabled = [&] {
        GLint enabled = 0;

        glGetVertexAttribiv(static_cast<GLuint>(uv->location),
            GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);

        return enabled != 0;
    };

    const auto forget_previous_binds = [&] {
        glDisableVertexAttribArray(static_cast<GLuint>(uv->location));

        REQUIRE_FALSE(uv_array_enabled());
    };

    webgl1es2_model subject(model::usage_hint::streaming, model_data::make_quad());

    SECTION("a quad feeds the uv attribute")
    {
        forget_previous_binds();

        subject.bind(*pShader);

        REQUIRE(uv_array_enabled());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("and after re-uploading without uvs, it must stop feeding it")
    {
        subject.bind(*pShader);

        model_data positionsOnly({{"a_Position",
            attribute_data({0, 0, 0, 1, 0, 0, 0, 1, 0}, 3)}});

        subject.upload(model::usage_hint::streaming, positionsOnly);

        forget_previous_binds();

        subject.bind(*pShader);

        REQUIRE_FALSE(uv_array_enabled());

        subject.draw();

        REQUIRE(!jfc::glGetError());
    }

    SECTION("and uvs come back when a model that has them is uploaded again")
    {
        model_data positionsOnly({{"a_Position",
            attribute_data({0, 0, 0, 1, 0, 0, 0, 1, 0}, 3)}});

        subject.upload(model::usage_hint::streaming, positionsOnly);
        subject.bind(*pShader);

        subject.upload(model::usage_hint::streaming, model_data::make_quad());

        forget_previous_binds();

        subject.bind(*pShader);

        REQUIRE(uv_array_enabled());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("binding against a second program resolves separately")
    {
        const auto pOther = webgl1es2_shader_program::make_alpha_cutoff();

        subject.bind(*pShader);
        subject.bind(*pOther);
        subject.draw();

        REQUIRE(!jfc::glGetError());
    }
}
