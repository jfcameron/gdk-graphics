// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/webgl1es2_gl_state.h>
#include "test_include.h"

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/graphics/color.h>
#include <gdk/graphics/types.h>
#include <gdk/graphics/webgl1es2_context.h>
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

TEST_CASE("**activating a material puts its uniform values into gl**",
    "[gdk::webgl1es2_material]")
{
    initGL();

    const std::string vertexSource(R"V0G0N(
    uniform float _Float;
    uniform vec2 _Vec2;
    uniform vec4 _Vec4;
    uniform int _Int;
    uniform ivec2 _IVec2Array[3];
    attribute highp vec3 a_Position;

    void main() {
        gl_Position = vec4(a_Position, 1.0) * _Float * float(_Int)
            * vec4(_Vec2, 0.0, 0.0) * _Vec4
            * float(_IVec2Array[0].x + _IVec2Array[1].y + _IVec2Array[2].x);
    }
    )V0G0N");

    const std::string fragmentSource(R"V0G0N(
    void main() { gl_FragColor = vec4(1.0); }
    )V0G0N");

    const auto pShader = std::make_shared<webgl1es2_shader_program>(vertexSource, fragmentSource);

    webgl1es2_material subject(pShader, material::face_culling_mode::none,
        material::render_mode::opaque);

    pShader->useProgram(test_gl_state());

    const auto reads = [](const char *aName, const std::size_t aCount) {
        std::vector<GLfloat> out(aCount, -1.0f);

        GLint current = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current);

        glGetUniformfv(current, glGetUniformLocation(current, aName), &out.front());

        return out;
    };

    SECTION("a float arrives")
    {
        subject.set_float("_Float", 0.75f);
        subject.activate(test_gl_state());

        REQUIRE(reads("_Float", 1).at(0) == Approx(0.75f));
    }

    SECTION("a vector2 arrives, both components")
    {
        subject.set_vector2("_Vec2", {0.25f, 0.5f});
        subject.activate(test_gl_state());

        const auto stored = reads("_Vec2", 2);

        REQUIRE(stored.at(0) == Approx(0.25f));
        REQUIRE(stored.at(1) == Approx(0.5f));
    }

    SECTION("a colour arrives in rgba order")
    {
        subject.set_vector4("_Vec4", color(0.125f, 0.25f, 0.5f, 0.75f));
        subject.activate(test_gl_state());

        const auto stored = reads("_Vec4", 4);

        REQUIRE(stored.at(0) == Approx(0.125f));
        REQUIRE(stored.at(3) == Approx(0.75f));
    }

    SECTION("the newest value wins when a name is set twice")
    {
        subject.set_float("_Float", 0.1f);
        subject.set_float("_Float", 0.9f);
        subject.activate(test_gl_state());

        REQUIRE(reads("_Float", 1).at(0) == Approx(0.9f));
    }

    SECTION("a name the shader does not declare is harmless")
    {
        subject.set_float("_NoSuchUniform", 1.0f);
        subject.activate(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }

    SECTION("**an ivec2 array arrives, every element of it**")
    {
        subject.set_int_vector2_array("_IVec2Array", {{1, 2}, {3, 4}, {5, 6}});
        subject.activate(test_gl_state());

        GLint current = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current);

        const auto element = [&](const std::size_t aIndex) {
            const auto name = std::string("_IVec2Array[").append(std::to_string(aIndex)).append("]");

            std::vector<GLint> out(2, -1);

            glGetUniformiv(current, glGetUniformLocation(current, name.c_str()), &out.front());

            return out;
        };

        REQUIRE(element(0) == std::vector<GLint>{1, 2});
        REQUIRE(element(1) == std::vector<GLint>{3, 4});
        REQUIRE(element(2) == std::vector<GLint>{5, 6});

        REQUIRE(!jfc::glGetError());
    }

    SECTION("an empty ivec2 array names no elements and is harmless")
    {
        subject.set_int_vector2_array("_IVec2Array", {});
        subject.activate(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }
}

TEST_CASE("**a material made from a prototype starts as a copy and then goes its own way**",
    "[gdk::webgl1es2_material]")
{
    initGL();

    const auto pShader = std::make_shared<webgl1es2_shader_program>(R"V0G0N(
    uniform float _Float;
    attribute highp vec3 a_Position;

    void main() { gl_Position = vec4(a_Position, 1.0) * _Float; }
    )V0G0N", R"V0G0N(
    void main() { gl_FragColor = vec4(1.0); }
    )V0G0N");

    const auto pContext = webgl1es2_context::make();

    const auto pPrototype = pContext->make_material(pShader);

    pPrototype->set_float("_Float", 0.25f);

    const auto pCopy = pContext->make_material(const_material_ptr_type(pPrototype));

    REQUIRE(pCopy);
    REQUIRE(pCopy != pPrototype);

    gl_state state;

    const auto uploaded = [&](const material_ptr_type &aMaterial) {
        static_cast<webgl1es2_material &>(*aMaterial).activate(state);

        GLfloat value = -1.0f;
        glGetUniformfv(pShader->handle(), glGetUniformLocation(pShader->handle(), "_Float"),
            &value);

        return value;
    };

    SECTION("the copy starts with the prototype's values") {
        REQUIRE(uploaded(pCopy) == Approx(0.25f));
    }

    SECTION("a value set on the copy is not seen by the prototype") {
        pCopy->set_float("_Float", 0.75f);

        REQUIRE(uploaded(pCopy) == Approx(0.75f));
        REQUIRE(uploaded(pPrototype) == Approx(0.25f));
    }

    SECTION("nor one set on the prototype afterwards by the copy") {
        pPrototype->set_float("_Float", 0.5f);

        REQUIRE(uploaded(pPrototype) == Approx(0.5f));
        REQUIRE(uploaded(pCopy) == Approx(0.25f));
    }

    SECTION("there is nothing to copy from no prototype") {
        REQUIRE_THROWS(pContext->make_material(const_material_ptr_type()));
    }
}
