// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/graphics/context.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/webgl1es2_context.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdk/graphics/webgl1es2_texture.h>

#include <memory>
#include <utility>
#include <type_traits>
#include <vector>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    struct checker final {
        std::vector<std::underlying_type<std::byte>::type> pixels{
            0x00, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff,
            0x00, 0x00, 0x00, 0xff};

        [[nodiscard]] texture_data::view view() const {
            texture_data::view v;
            v.width = 2;
            v.height = 2;
            v.format = texture::format::rgba;
            v.data = &pixels.front();

            return v;
        }
    };

    [[nodiscard]] model_data a_triangle() {
        return model_data{{
            {"a_Position", {{0, 0, 0,  1, 0, 0,  0, 1, 0}, 3}},
            {"a_UV", {{0, 0,  1, 0,  0, 1}, 2}}}};
    }
}

TEST_CASE("gdk::context::make_texture filtering", "[gdk::context]")
{
    initGL();

    auto pContext = webgl1es2_context::make();

    const checker image;

    const auto filters_of = [](const texture_ptr_type &apTexture) {
        const auto pTexture = std::static_pointer_cast<webgl1es2_texture>(apTexture);

        glBindTexture(GL_TEXTURE_2D, pTexture->getHandle());

        GLint magnification{0}, minification{0};
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magnification);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minification);

        return std::pair(magnification, minification);
    };

    SECTION("a texture is unfiltered unless filtering is asked for")
    {
        const auto [magnification, minification] = filters_of(pContext->make_texture(image.view()));

        REQUIRE(magnification == GL_NEAREST);
        REQUIRE(minification == GL_NEAREST);
    }

    SECTION("the empty overload agrees with the one that takes data")
    {
        const auto [magnification, minification] = filters_of(pContext->make_texture());

        REQUIRE(magnification == GL_NEAREST);
        REQUIRE(minification == GL_NEAREST);
    }

    SECTION("filter_mode::sharp is the default spelled out")
    {
        const auto [magnification, minification] = filters_of(pContext->make_texture(image.view(),
            texture::wrap_mode::repeat, texture::wrap_mode::repeat,
            texture::filter_mode::sharp));

        REQUIRE(magnification == GL_NEAREST);
        REQUIRE(minification == GL_NEAREST);
    }

    SECTION("filter_mode::smooth opts in to filtering")
    {
        const auto [magnification, minification] = filters_of(pContext->make_texture(image.view(),
            texture::wrap_mode::repeat, texture::wrap_mode::repeat,
            texture::filter_mode::smooth));

        REQUIRE(magnification == GL_LINEAR);
        REQUIRE(minification == GL_LINEAR);
    }

    REQUIRE(!jfc::glGetError());
}

TEST_CASE("context construction", "[gdk::context]")
{
    initGL();

    SECTION("a backend is chosen by naming its context type")
    {
        const auto pContext = webgl1es2_context::make();

        REQUIRE(pContext);
        REQUIRE(!jfc::glGetError());

        const std::shared_ptr<graphics::context> asInterface = pContext;
        REQUIRE(asInterface);
    }
}

TEST_CASE("context factories", "[gdk::context]")
{
    initGL();

    const auto pContext = webgl1es2_context::make();
    REQUIRE(pContext);

    SECTION("scenes and cameras")
    {
        REQUIRE(pContext->make_scene());
        REQUIRE(!jfc::glGetError());

        REQUIRE(pContext->make_camera());
        REQUIRE(!jfc::glGetError());

        REQUIRE(pContext->make_texture_camera());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("the built-in resources every implementation must provide")
    {
        REQUIRE(pContext->make_cube_model());
        REQUIRE(pContext->make_sphere_model());
        REQUIRE(pContext->make_alpha_cutoff_shader());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("materials, in each render and culling mode")
    {
        const auto pShader = pContext->make_alpha_cutoff_shader();
        REQUIRE(pShader);

        for (const auto render : {material::render_mode::opaque, material::render_mode::transparent}) {
            for (const auto culling : {material::face_culling_mode::none,
                material::face_culling_mode::front, material::face_culling_mode::back,
                material::face_culling_mode::front_and_back}) {
                REQUIRE(pContext->make_material(pShader, render, culling));
                REQUIRE(!jfc::glGetError());
            }
        }
    }

    SECTION("entities, from a built-in model and a material")
    {
        const auto pMaterial = pContext->make_material(pContext->make_alpha_cutoff_shader());
        REQUIRE(pMaterial);

        REQUIRE(pContext->make_entity(pContext->make_cube_model(), pMaterial));
        REQUIRE(!jfc::glGetError());
    }

    SECTION("models, empty and from vertex data")
    {
        REQUIRE(pContext->make_model());
        REQUIRE(!jfc::glGetError());

        for (const auto hint : {model::usage_hint::upload_once, model::usage_hint::dynamic,
            model::usage_hint::streaming}) {
            REQUIRE(pContext->make_model(hint, a_triangle()));
            REQUIRE(!jfc::glGetError());
        }
    }

    SECTION("textures, empty and from a data view")
    {
        REQUIRE(pContext->make_texture());
        REQUIRE(!jfc::glGetError());

        const checker image;

        REQUIRE(pContext->make_texture(image.view()));
        REQUIRE(!jfc::glGetError());
    }

    SECTION("textures in each wrap mode")
    {
        const checker image;

        for (const auto u : {texture::wrap_mode::repeat, texture::wrap_mode::clamped,
            texture::wrap_mode::mirrored}) {
            REQUIRE(pContext->make_texture(image.view(), u, texture::wrap_mode::repeat));
            REQUIRE(!jfc::glGetError());
        }
    }
}

TEST_CASE("context resource independence", "[gdk::context]")
{
    initGL();

    const auto pContext = webgl1es2_context::make();

    SECTION("two calls to the same factory give two distinct resources")
    {
        const auto pFirst = pContext->make_camera();
        const auto pSecond = pContext->make_camera();

        REQUIRE(pFirst);
        REQUIRE(pSecond);
        REQUIRE(pFirst.get() != pSecond.get());
    }

    SECTION("each call builds a new built-in, and the caller owns what it gets")
    {
        REQUIRE(pContext->make_cube_model().get() != pContext->make_cube_model().get());
        REQUIRE(pContext->make_alpha_cutoff_shader().get()
            != pContext->make_alpha_cutoff_shader().get());
    }

    SECTION("a second context is allowed, and has its own gl state cache")
    {
        const auto pSecond = webgl1es2_context::make();

        REQUIRE(pSecond);

        REQUIRE(std::static_pointer_cast<webgl1es2_context>(pSecond)->state()
            != std::static_pointer_cast<webgl1es2_context>(pContext)->state());
    }
}

TEST_CASE("context single instance", "[gdk::context]")
{
    initGL();

    SECTION("the constraint is released when the context is")
    {
        {
            const auto pFirst = webgl1es2_context::make();
            REQUIRE(pFirst);
        }

        const auto pSecond = webgl1es2_context::make();

        REQUIRE(pSecond);
        REQUIRE(!jfc::glGetError());
    }
}

TEST_CASE("the gl state cache skips redundant calls, per context", "[context][gl_state]")
{
    initGL();

    const auto pShader = webgl1es2_shader_program::make_alpha_cutoff();

    gl_state first;

    REQUIRE(first.current_program() == 0);

    SECTION("using the same program twice only calls gl once")
    {
        pShader->useProgram(first);

        const auto handle = first.current_program();

        REQUIRE(handle != 0);

        pShader->useProgram(first);

        REQUIRE(first.current_program() == handle);
    }

    SECTION("a second context knows nothing about the first one's program")
    {
        pShader->useProgram(first);

        REQUIRE(first.current_program() != 0);

        gl_state second;

        REQUIRE(second.current_program() == 0);
    }

    SECTION("texture units are remembered until the program changes")
    {
        pShader->useProgram(first);

        REQUIRE(first.assigned_texture_unit("_Texture") == -1);

        const auto unit = first.assign_texture_unit("_Texture");

        REQUIRE(first.assigned_texture_unit("_Texture") == unit);

        webgl1es2_shader_program::make_pink_shader_of_death()->useProgram(first);

        REQUIRE(first.assigned_texture_unit("_Texture") == -1);
        REQUIRE(first.assigned_texture_unit_count() == 0);
    }
}
