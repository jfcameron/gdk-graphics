// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/webgl1es2_gl_state.h>
#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/graphics/exception.h>
#include <gdk/graphics/color.h>
#include <gdk/graphics/types.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdk/graphics/webgl1es2_texture.h>

#include <memory>
#include <string>
#include <type_traits>
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
    using program = webgl1es2_shader_program;

    [[nodiscard]] std::shared_ptr<program> alpha_cutoff() {
        return program::make_alpha_cutoff();
    }

    template <typename value_type>
    [[nodiscard]] bool set_array(const program &aProgram, const std::string &aName,
        const value_type &aValue) {
        return aProgram.try_set_uniform(aName, aValue)
            || aProgram.try_set_uniform(aName + "[0]", aValue);
    }

    [[nodiscard]] GLuint current_program() {
        GLint handle = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &handle);

        return static_cast<GLuint>(handle);
    }

    [[nodiscard]] std::vector<GLfloat> read_floats(const std::string &aName, const std::size_t aCount) {
        std::vector<GLfloat> out(aCount, -1.0f);
        glGetUniformfv(current_program(), glGetUniformLocation(current_program(), aName.c_str()),
            &out.front());

        return out;
    }

    [[nodiscard]] std::vector<GLint> read_ints(const std::string &aName, const std::size_t aCount) {
        std::vector<GLint> out(aCount, -1);
        glGetUniformiv(current_program(), glGetUniformLocation(current_program(), aName.c_str()),
            &out.front());

        return out;
    }

    [[nodiscard]] program every_uniform_type() {
        const std::string vertexSource(R"V0G0N(
        uniform mat4 _Mat4;
        uniform float _Float;
        uniform vec2 _Vec2;
        uniform vec3 _Vec3;
        uniform vec4 _Vec4;
        uniform int _Int;
        uniform ivec2 _IVec2;
        uniform ivec3 _IVec3;
        uniform ivec4 _IVec4;

        uniform float _FloatArray[3];
        uniform vec2 _Vec2Array[3];
        uniform vec3 _Vec3Array[3];
        uniform vec4 _Vec4Array[3];
        uniform int _IntArray[3];
        uniform ivec2 _IVec2Array[3];
        uniform ivec3 _IVec3Array[3];
        uniform ivec4 _IVec4Array[3];
        uniform mat4 _Mat4Array[2];
        uniform mat3 _Mat3;
        uniform mat3 _Mat3Array[2];

        uniform float _NeverUsed;

        attribute vec3 a_Position;

        void main() {
            vec4 p = _Mat4 * vec4(a_Position, 1.0);
            p += _Mat4Array[0] * vec4(a_Position, 1.0);
            p += _Mat4Array[1] * vec4(a_Position, 1.0);

            p.xyz += _Mat3 * a_Position;
            p.xyz += _Mat3Array[0] * a_Position;
            p.xyz += _Mat3Array[1] * a_Position;

            p.x += _Float + _Vec2.x + _Vec3.x + _Vec4.x;
            p.y += float(_Int) + float(_IVec2.x) + float(_IVec3.x) + float(_IVec4.x);

            p.z += _FloatArray[0] + _FloatArray[1] + _FloatArray[2];
            p.z += _Vec2Array[0].x + _Vec2Array[1].x + _Vec2Array[2].x;
            p.z += _Vec3Array[0].x + _Vec3Array[1].x + _Vec3Array[2].x;
            p.z += _Vec4Array[0].x + _Vec4Array[1].x + _Vec4Array[2].x;

            p.w += float(_IntArray[0] + _IntArray[1] + _IntArray[2]);
            p.w += float(_IVec2Array[0].x + _IVec2Array[1].x + _IVec2Array[2].x);
            p.w += float(_IVec3Array[0].x + _IVec3Array[1].x + _IVec3Array[2].x);
            p.w += float(_IVec4Array[0].x + _IVec4Array[1].x + _IVec4Array[2].x);

            gl_Position = p;
        }
        )V0G0N");

        const std::string fragmentSource(R"V0G0N(
        uniform sampler2D _Texture;

        void main() {
            gl_FragColor = texture2D(_Texture, vec2(0.5, 0.5));
        }
        )V0G0N");

        return program(vertexSource, fragmentSource);
    }
}

TEST_CASE("gdk::webgl1es2_shader_program provided shaders", "[gdk::webgl1es2_shader_program]")
{
    initGL();

    SECTION("both provided shaders are live and shared")
    {
        const auto pAlpha = alpha_cutoff();
        const auto pPink = program::make_pink_shader_of_death();

        REQUIRE(pAlpha);
        REQUIRE(pPink);
        REQUIRE(!jfc::glGetError());

        REQUIRE(program::make_alpha_cutoff().get() != pAlpha.get());
        REQUIRE(*pAlpha != *pPink);
    }

    SECTION("the alpha cutoff shader declares the attributes a model must supply")
    {
        const auto pAlpha = alpha_cutoff();

        const auto position = pAlpha->tryGetActiveAttribute("a_Position");
        REQUIRE(position.has_value());
        REQUIRE(position->count == 1);

        REQUIRE(pAlpha->tryGetActiveAttribute("a_UV").has_value());
        REQUIRE_FALSE(pAlpha->tryGetActiveAttribute("a_NoSuchAttribute").has_value());
    }

    SECTION("an attribute reports its location, not its enumeration index")
    {
        const program subject(
            "attribute mat4 a_Instance;\n"
            "attribute vec3 a_Position;\n"
            "attribute vec2 a_UV;\n"
            "void main() { gl_Position = a_Instance * vec4(a_Position, a_UV.x); }",
            "void main() { gl_FragColor = vec4(1.0); }");

        subject.useProgram(test_gl_state());

        for (const auto &name : {"a_Instance", "a_Position", "a_UV"}) {
            const auto reported = subject.tryGetActiveAttribute(name);

            REQUIRE(reported.has_value());
            REQUIRE(reported->location == glGetAttribLocation(current_program(), name));
        }

        REQUIRE(!jfc::glGetError());
    }
}

TEST_CASE("gdk::webgl1es2_shader_program construction", "[gdk::webgl1es2_shader_program]")
{
    initGL();

    SECTION("a program is built from vertex and fragment source")
    {
        const auto built = every_uniform_type();

        built.useProgram(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }

    SECTION("source that does not compile throws")
    {
        REQUIRE_THROWS_AS(program("void main() { this is not glsl }",
            "void main() { gl_FragColor = vec4(1.0); }"), exception);

        REQUIRE_THROWS_AS(program("void main() { gl_Position = vec4(0.0); }",
            "void main() { still not glsl }"), exception);
    }

    SECTION("two programs built from the same source are distinct")
    {
        const auto first = every_uniform_type();
        const auto second = every_uniform_type();

        REQUIRE(first != second);
        REQUIRE(first == first);
    }

    SECTION("it is move only, and has no default")
    {
        REQUIRE(std::is_move_constructible<program>::value);
        REQUIRE(std::is_move_assignable<program>::value);
        REQUIRE_FALSE(std::is_copy_constructible<program>::value);
        REQUIRE_FALSE(std::is_copy_assignable<program>::value);
        REQUIRE_FALSE(std::is_default_constructible<program>::value);
    }
}

TEST_CASE("gdk::webgl1es2_shader_program uniforms", "[gdk::webgl1es2_shader_program]")
{
    initGL();

    const auto subject = every_uniform_type();

    subject.useProgram(test_gl_state());

    SECTION("every scalar and vector overload reaches gl cleanly")
    {
        REQUIRE(subject.try_set_uniform("_Float", 0.5f));
        REQUIRE(subject.try_set_uniform("_Vec2", vector2_type(1, 2)));
        REQUIRE(subject.try_set_uniform("_Vec3", vector3_type(1, 2, 3)));
        REQUIRE(subject.try_set_uniform("_Vec4", vector4_type(1, 2, 3, 4)));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("every integer overload reaches gl cleanly")
    {
        REQUIRE(subject.try_set_uniform("_Int", GLint(1)));
        REQUIRE(subject.try_set_uniform("_IVec2", program::integer2_uniform_type(1, 2)));
        REQUIRE(subject.try_set_uniform("_IVec3", program::integer3_uniform_type(1, 2, 3)));
        REQUIRE(subject.try_set_uniform("_IVec4", program::integer4_uniform_type(1, 2, 3, 4)));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("**a colour maps to rgba in that order**")
    {
        REQUIRE(subject.try_set_uniform("_Vec4", color(0.125f, 0.25f, 0.5f, 0.75f)));

        const auto uploaded = read_floats("_Vec4", 4);

        REQUIRE(uploaded.at(0) == Approx(0.125f));   // r -> x
        REQUIRE(uploaded.at(1) == Approx(0.25f));    // g -> y
        REQUIRE(uploaded.at(2) == Approx(0.5f));     // b -> z
        REQUIRE(uploaded.at(3) == Approx(0.75f));    // a -> w

        REQUIRE(!jfc::glGetError());
    }

    SECTION("a colour and the equivalent vector4 upload identically")
    {
        REQUIRE(subject.try_set_uniform("_Vec4", color(0.1f, 0.2f, 0.3f, 0.4f)));
        const auto viaColour = read_floats("_Vec4", 4);

        REQUIRE(subject.try_set_uniform("_Vec4", vector4_type(0.1f, 0.2f, 0.3f, 0.4f)));
        const auto viaVector = read_floats("_Vec4", 4);

        REQUIRE(viaColour == viaVector);
    }

    SECTION("a colour named at an absent uniform returns false, like every other overload")
    {
        REQUIRE_FALSE(subject.try_set_uniform("_NoSuchUniformAnywhere", color::red));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("the matrix overloads reach gl cleanly")
    {
        REQUIRE(subject.try_set_uniform("_Mat4", matrix4x4_type::identity));
        REQUIRE(!jfc::glGetError());

        REQUIRE(subject.try_set_uniform("_Mat3", mat3x3_type::identity));
        REQUIRE(!jfc::glGetError());
    }

    SECTION("**an array is addressable by the name the shader declared, without a [0] suffix**")
    {
        REQUIRE(subject.try_set_uniform("_FloatArray", std::vector<GLfloat>{1, 2, 3}));
        REQUIRE(subject.try_set_uniform("_Vec2Array",
            std::vector<vector2_type>{{1, 2}, {3, 4}, {5, 6}}));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("and by the [0] spelling gl itself reports, which names the same location")
    {
        REQUIRE(subject.try_set_uniform("_FloatArray[0]", std::vector<GLfloat>{1, 2, 3}));

        REQUIRE(read_floats("_FloatArray[0]", 1).at(0) == Approx(1.0f));
        REQUIRE(read_floats("_FloatArray[2]", 1).at(0) == Approx(3.0f));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("every array overload reaches gl cleanly")
    {
        REQUIRE(set_array(subject, "_FloatArray", std::vector<GLfloat>{1, 2, 3}));
        REQUIRE(!jfc::glGetError());

        REQUIRE(set_array(subject, "_Vec2Array",
            std::vector<vector2_type>{{1, 2}, {3, 4}, {5, 6}}));
        REQUIRE(!jfc::glGetError());

        REQUIRE(set_array(subject, "_Vec3Array",
            std::vector<vector3_type>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}));
        REQUIRE(!jfc::glGetError());

        REQUIRE(set_array(subject, "_Vec4Array",
            std::vector<vector4_type>{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 1, 2, 3}}));
        REQUIRE(!jfc::glGetError());

        REQUIRE(set_array(subject, "_IntArray", std::vector<GLint>{1, 2, 3}));
        REQUIRE(!jfc::glGetError());

        REQUIRE(set_array(subject, "_IVec2Array",
            std::vector<program::integer2_uniform_type>{{1, 2}, {3, 4}, {5, 6}}));
        REQUIRE(!jfc::glGetError());

        REQUIRE(set_array(subject, "_IVec3Array",
            std::vector<program::integer3_uniform_type>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}));
        REQUIRE(!jfc::glGetError());

        REQUIRE(set_array(subject, "_IVec4Array",
            std::vector<program::integer4_uniform_type>{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 1, 2, 3}}));
        REQUIRE(!jfc::glGetError());

        REQUIRE(set_array(subject, "_Mat4Array", std::vector<matrix4x4_type>{
            matrix4x4_type::identity, matrix4x4_type::identity}));
        REQUIRE(!jfc::glGetError());

        REQUIRE(set_array(subject, "_Mat3Array", std::vector<mat3x3_type>{
            mat3x3_type::identity, mat3x3_type::identity}));
        REQUIRE(!jfc::glGetError());
    }

    SECTION("a texture overload binds and assigns cleanly")
    {
        REQUIRE(subject.try_set_uniform("_Texture", *webgl1es2_texture::make_checkerboard_of_death(), test_gl_state()));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("naming a uniform the program does not have returns false")
    {
        REQUIRE_FALSE(subject.try_set_uniform("_NoSuchUniformAnywhere", 1.0f));
        REQUIRE_FALSE(subject.try_set_uniform("_AlsoAbsent", vector3_type(1, 2, 3)));
        REQUIRE_FALSE(subject.try_set_uniform("_NotATextureSlot", *webgl1es2_texture::make_checkerboard_of_death(), test_gl_state()));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("a uniform declared but never used is not active")
    {
        REQUIRE_FALSE(subject.try_set_uniform("_NeverUsed", 1.0f));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("the return value is a question about the name, not about the type")
    {
        REQUIRE(subject.try_set_uniform("_Mat4", 1.0f));
        REQUIRE(jfc::glGetError(true));
    }
}

TEST_CASE("gdk::webgl1es2_shader_program uniform values arrive", "[gdk::webgl1es2_shader_program]")
{
    initGL();

    const auto subject = every_uniform_type();
    subject.useProgram(test_gl_state());

    SECTION("a scalar and a vector land on the uniform that was named")
    {
        REQUIRE(subject.try_set_uniform("_Float", 0.25f));
        REQUIRE(read_floats("_Float", 1).at(0) == Approx(0.25f));

        REQUIRE(subject.try_set_uniform("_Vec3", vector3_type(1, 2, 3)));

        const auto vec3 = read_floats("_Vec3", 3);
        REQUIRE(vec3.at(0) == Approx(1.0f));
        REQUIRE(vec3.at(1) == Approx(2.0f));
        REQUIRE(vec3.at(2) == Approx(3.0f));

        REQUIRE(subject.try_set_uniform("_IVec4", program::integer4_uniform_type(4, 5, 6, 7)));

        const auto ivec4 = read_ints("_IVec4", 4);
        REQUIRE(ivec4.at(0) == 4);
        REQUIRE(ivec4.at(3) == 7);

        REQUIRE(!jfc::glGetError());
    }

    SECTION("an array assignment reaches its last element, not just its first")
    {
        REQUIRE(set_array(subject, "_FloatArray", std::vector<GLfloat>{7, 8, 9}));

        REQUIRE(read_floats("_FloatArray[0]", 1).at(0) == Approx(7.0f));
        REQUIRE(read_floats("_FloatArray[2]", 1).at(0) == Approx(9.0f));

        REQUIRE(set_array(subject, "_IVec4Array",
            std::vector<program::integer4_uniform_type>{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}}));

        const auto last = read_ints("_IVec4Array[2]", 4);
        REQUIRE(last.at(0) == 9);
        REQUIRE(last.at(3) == 12);   

        REQUIRE(!jfc::glGetError());
    }

    SECTION("a mat3 array uploads every matrix, in the storage order gl expects")
    {
        auto second = mat3x3_type::identity;
        second.set(2, 0, 7.0f);   

        REQUIRE(set_array(subject, "_Mat3Array", std::vector<mat3x3_type>{
            mat3x3_type::identity, second}));

        const auto first = read_floats("_Mat3Array[0]", 9);
        for (std::size_t k = 0; k < 9; ++k)
            REQUIRE(first.at(k) == Approx(k % 4 == 0 ? 1.0f : 0.0f));   

        const auto uploaded = read_floats("_Mat3Array[1]", 9);
        REQUIRE(uploaded.at(6) == Approx(7.0f));   
        REQUIRE(uploaded.at(0) == Approx(1.0f));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("a matrix array uploads every matrix, in the storage order gl expects")
    {
        auto second = matrix4x4_type::identity;
        second.set(3, 0, 7.0f);   

        REQUIRE(set_array(subject, "_Mat4Array",
            std::vector<matrix4x4_type>{matrix4x4_type::identity, second}));

        const auto first = read_floats("_Mat4Array[0]", 16);
        for (std::size_t k = 0; k < 16; ++k)
            REQUIRE(first.at(k) == Approx(k % 5 == 0 ? 1.0f : 0.0f));   

        const auto uploaded = read_floats("_Mat4Array[1]", 16);
        REQUIRE(uploaded.at(12) == Approx(7.0f));   
        REQUIRE(uploaded.at(0) == Approx(1.0f));

        REQUIRE(!jfc::glGetError());
    }
}

TEST_CASE("gdk::webgl1es2_shader_program installation", "[gdk::webgl1es2_shader_program]")
{
    initGL();

    SECTION("installing twice in a row is harmless")
    {
        const auto pAlpha = alpha_cutoff();

        pAlpha->useProgram(test_gl_state());
        pAlpha->useProgram(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }

    SECTION("switching between programs is clean in both directions")
    {
        const auto pAlpha = alpha_cutoff();
        const auto pPink = program::make_pink_shader_of_death();

        pAlpha->useProgram(test_gl_state());
        pPink->useProgram(test_gl_state());
        pAlpha->useProgram(test_gl_state());

        REQUIRE(!jfc::glGetError());
    }
}

namespace {
    [[nodiscard]] program samplers(const std::size_t aCount) {
        std::string uniforms, sum;

        for (std::size_t i = 0; i < aCount; ++i) {
            uniforms += "uniform sampler2D _Texture" + std::to_string(i) + ";\n";
            sum += (i ? " + " : "") + std::string("texture2D(_Texture") + std::to_string(i)
                + ", vec2(0.5, 0.5))";
        }

        return program("attribute vec3 a_Position;\n"
            "void main() { gl_Position = vec4(a_Position, 1.0); }",
            uniforms + "void main() { gl_FragColor = " + sum + "; }");
    }

    struct texture_units_restored final {
        ~texture_units_restored() {
            program::make_pink_shader_of_death()->useProgram(test_gl_state());
            alpha_cutoff()->useProgram(test_gl_state());
        }
    };
}

TEST_CASE("gdk::webgl1es2_shader_program texture units", "[gdk::webgl1es2_shader_program]")
{
    initGL();

    const texture_units_restored restore;

    const auto limit = program::MAX_TEXTURE_UNITS();

    GLint deviceUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &deviceUnits);

    if (static_cast<std::size_t>(deviceUnits) <= limit) return;

    const auto subject = samplers(limit + 1);
    subject.useProgram(test_gl_state());

    SECTION("every unit up to the limit can be assigned")
    {
        for (std::size_t i = 0; i < limit; ++i)
            REQUIRE(subject.try_set_uniform("_Texture" + std::to_string(i),
                *webgl1es2_texture::make_checkerboard_of_death(), test_gl_state()));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("one more than the limit throws")
    {
        for (std::size_t i = 0; i < limit; ++i)
            subject.try_set_uniform("_Texture" + std::to_string(i),
                *webgl1es2_texture::make_checkerboard_of_death(), test_gl_state());

        REQUIRE_THROWS_AS(subject.try_set_uniform("_Texture" + std::to_string(limit),
            *webgl1es2_texture::make_checkerboard_of_death(), test_gl_state()), exception);
    }

    SECTION("reassigning a uniform that already holds a unit never throws")
    {
        for (std::size_t i = 0; i < limit; ++i)
            subject.try_set_uniform("_Texture" + std::to_string(i),
                *webgl1es2_texture::make_checkerboard_of_death(), test_gl_state());

        REQUIRE(subject.try_set_uniform("_Texture0", *webgl1es2_texture::make_checkerboard_of_death(), test_gl_state()));

        REQUIRE(!jfc::glGetError());
    }
}

TEST_CASE("gdk::webgl1es2_shader_program device limits", "[gdk::webgl1es2_shader_program]")
{
    initGL();

    SECTION("the texture unit count is the guaranteed minimum, not this device's maximum")
    {
        REQUIRE(program::MAX_TEXTURE_UNITS() == 8);
    }

    SECTION("the uniform vector limits come back at or above the es2 minimums")
    {
        REQUIRE(program::MAX_VERTEX_SHADER_UNIFORM_VECTORS() >= 128);
        REQUIRE(program::MAX_FRAGMENT_SHADER_UNIFORM_VECTORS() >= 16);

        REQUIRE(!jfc::glGetError());
    }
}


TEST_CASE("the standard matrices are set by cached location", "[gdk::webgl1es2_shader_program]")
{
    initGL();

    const auto pShader = program::make_alpha_cutoff();

    pShader->useProgram(test_gl_state());

    matrix4x4_type model, view, projection, mvp;
    model.set_to_identity();
    view.set_to_identity();
    projection.set_to_identity();
    mvp.set_to_identity();
    model.set_translation({1, 2, 3});

    SECTION("only _MVP is actually active on the built-in shaders")
    {
        REQUIRE(pShader->try_set_uniform("_MVP", mvp));

        REQUIRE_FALSE(pShader->try_set_uniform("_Model", model));
        REQUIRE_FALSE(pShader->try_set_uniform("_View", view));
        REQUIRE_FALSE(pShader->try_set_uniform("_Projection", projection));
    }

    SECTION("setting them all is equivalent, and errors on none of it")
    {
        REQUIRE_NOTHROW(pShader->set_standard_matrices(model, view, projection, mvp));

        REQUIRE_FALSE(jfc::glGetError());
    }

    SECTION("and a shader that declares only some of them is not an error")
    {
        const auto pMinimal = program::make_pink_shader_of_death();

        pMinimal->useProgram(test_gl_state());

        REQUIRE_NOTHROW(pMinimal->set_standard_matrices(model, view, projection, mvp));

        REQUIRE_FALSE(jfc::glGetError());
    }
}
