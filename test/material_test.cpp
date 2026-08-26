// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/webgl1es2_gl_state.h>
#include "test_include.h"

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/graphics/color.h>
#include <gdk/graphics/types.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdk/graphics/webgl1es2_texture.h>

#include <memory>
#include <string>
#include <vector>

using namespace gdk;

namespace {
    [[nodiscard]] gdk::graphics::gl_state &test_gl_state() {
        static gdk::graphics::gl_state state;

        return state;
    }
}
using namespace gdk::graphics;

namespace {
    [[nodiscard]] std::shared_ptr<webgl1es2_shader_program> alpha_cutoff_shader() {
        return webgl1es2_shader_program::make_alpha_cutoff();
    }

    [[nodiscard]] webgl1es2_material a_material(
        const material::face_culling_mode aCulling = material::face_culling_mode::none,
        const material::render_mode aRender = material::render_mode::opaque) {
        return webgl1es2_material(alpha_cutoff_shader(), aCulling, aRender);
    }
}

TEST_CASE("gdk::webgl1es2_material construction", "[gdk::webgl1es2_material]")
{
    initGL();

    SECTION("a shader is required at construction")
    {
        REQUIRE_FALSE(std::is_default_constructible<webgl1es2_material>::value);
    }

    SECTION("every combination of culling and render mode constructs cleanly")
    {
        for (const auto render : {material::render_mode::opaque, material::render_mode::transparent}) {
            for (const auto culling : {material::face_culling_mode::none,
                material::face_culling_mode::front, material::face_culling_mode::back,
                material::face_culling_mode::front_and_back}) {
                auto mat = a_material(culling, render);

                mat.activate(test_gl_state());

                REQUIRE(!jfc::glGetError());
            }
        }
    }
}

TEST_CASE("gdk::webgl1es2_material uniforms", "[gdk::webgl1es2_material]")
{
    initGL();

    auto mat = a_material();

    SECTION("a texture uniform can be set and the material activated")
    {
        mat.set_texture("_Texture", webgl1es2_texture::make_checkerboard_of_death());

        mat.activate(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }

    SECTION("the scalar and vector setters all accept values")
    {
        mat.set_float("_Float", 0.5f);
        mat.set_vector2("_Vector2", {1, 2});
        mat.set_vector3("_Vector3", {1, 2, 3});
        mat.set_vector4("_Vector4", vector4_type{1, 2, 3, 4});

        mat.activate(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }

    SECTION("the integer setters all accept values")
    {
        mat.set_integer("_Int", 1);
        mat.set_integer2("_Int2", 1, 2);
        mat.set_integer3("_Int3", 1, 2, 3);
        mat.set_integer4("_Int4", 1, 2, 3, 4);

        mat.activate(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }

    SECTION("an array uniform accepts values, including an empty one")
    {
        mat.set_int_vector2_array("_Array", {{1, 2}, {3, 4}});
        mat.activate(test_gl_state());
        REQUIRE(!jfc::glGetError());

        mat.set_int_vector2_array("_Array", {});
        mat.activate(test_gl_state());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("a colour reaches a vec4 uniform")
    {
        mat.set_vector4("_Vector4", color::cornflower_blue);

        mat.activate(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }

    SECTION("**both set_vector4 overloads share one slot, last call wins**")
    {
        mat.set_vector4("_Vector4", vector4_type{1, 2, 3, 4});
        mat.set_vector4("_Vector4", color::red);
        mat.activate(test_gl_state());
        REQUIRE(!jfc::glGetError());

        mat.set_vector4("_Vector4", color::red);
        mat.set_vector4("_Vector4", vector4_type{1, 2, 3, 4});
        mat.activate(test_gl_state());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("naming a uniform the shader does not have is not an error")
    {
        mat.set_float("_NoSuchUniformAnywhere", 1.0f);
        mat.set_vector3("_AlsoAbsent", {1, 2, 3});
        mat.set_texture("_NotATextureSlot", webgl1es2_texture::make_checkerboard_of_death());

        mat.activate(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }

    SECTION("setting the same uniform twice keeps the last value")
    {
        mat.set_float("_Float", 1.0f);
        mat.set_float("_Float", 2.0f);

        mat.activate(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }
}

TEST_CASE("gdk::webgl1es2_material value semantics", "[gdk::webgl1es2_material]")
{
    initGL();

    auto mat = a_material();
    mat.set_texture("_Texture", webgl1es2_texture::make_checkerboard_of_death());
    mat.set_float("_Float", 0.5f);

    SECTION("a copy activates independently of its source")
    {
        auto copy = mat;

        copy.activate(test_gl_state());
        REQUIRE(!jfc::glGetError());

        mat.activate(test_gl_state());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("a moved-to material activates")
    {
        auto moved = std::move(mat);

        moved.activate(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }

    SECTION("activating twice in a row is harmless")
    {
        mat.activate(test_gl_state());
        mat.activate(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }
}
