// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/webgl1es2_gl_state.h>
#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/graphics/types.h>
#include <gdk/graphics/webgl1es2_entity.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_shader_program.h>

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
    [[nodiscard]] std::shared_ptr<webgl1es2_material> a_material() {
        return std::make_shared<webgl1es2_material>(
            webgl1es2_shader_program::make_alpha_cutoff(),
            material::face_culling_mode::none,
            material::render_mode::opaque);
    }

    [[nodiscard]] std::shared_ptr<webgl1es2_model> a_model() {
        return webgl1es2_model::make_cube();
    }

    void as_the_scene_would(webgl1es2_material &aMaterial, const webgl1es2_model &aModel) {
        aMaterial.activate(test_gl_state());
        aModel.bind(*aMaterial.getShaderProgram());
    }

    [[nodiscard]] GLuint current_program() {
        GLint handle = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &handle);

        return static_cast<GLuint>(handle);
    }

    [[nodiscard]] std::vector<GLfloat> read_mat4(const std::string &aName) {
        std::vector<GLfloat> out(16, -1.0f);
        glGetUniformfv(current_program(), glGetUniformLocation(current_program(), aName.c_str()),
            &out.front());

        return out;
    }

    [[nodiscard]] matrix4x4_type a_view_matrix() {
        quaternion_type turn;
        turn.set_from_euler({0.1f, 0.5f, 0.2f});

        matrix4x4_type m;
        m.set_translation({0, -2, -8});
        m.set_rotation(turn);

        return m;
    }

    [[nodiscard]] matrix4x4_type a_projection_matrix() {
        matrix4x4_type m;
        m.set_to_identity();
        m.set(0, 0, 1.5f);
        m.set(1, 1, 1.5f);

        return m;
    }
}

TEST_CASE("gdk::webgl1es2_entity construction", "[gdk::webgl1es2_entity]")
{
    initGL();

    const auto pModel = a_model();
    const auto pMaterial = a_material();

    SECTION("it holds on to the model and material it was given")
    {
        const webgl1es2_entity a(pModel, pMaterial);

        REQUIRE(a.getModel().get() == pModel.get());
        REQUIRE(a.getMaterial().get() == pMaterial.get());
    }

    SECTION("two entities can share one model and one material")
    {
        const webgl1es2_entity a(pModel, pMaterial);
        const webgl1es2_entity b(pModel, pMaterial);

        REQUIRE(a.getModel().get() == b.getModel().get());
        REQUIRE(a.getMaterial().get() == b.getMaterial().get());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("it is copyable as well as movable")
    {
        REQUIRE(std::is_copy_constructible<webgl1es2_entity>::value);
        REQUIRE(std::is_copy_assignable<webgl1es2_entity>::value);
        REQUIRE(std::is_move_constructible<webgl1es2_entity>::value);
        REQUIRE(std::is_move_assignable<webgl1es2_entity>::value);
    }

    SECTION("a new entity starts visible, at the origin")
    {
        const webgl1es2_entity a(pModel, pMaterial);

        REQUIRE_FALSE(a.is_hidden());
        REQUIRE(a.getModelMatrix() == matrix4x4_type::identity);
    }
}

TEST_CASE("gdk::webgl1es2_entity transform", "[gdk::webgl1es2_entity]")
{
    initGL();

    webgl1es2_entity a(a_model(), a_material());

    SECTION("a position reaches the model matrix")
    {
        a.set_transform({4, 5, 6}, quaternion_type::identity);

        REQUIRE(a.getModelMatrix().translation() == vector3_type(4, 5, 6));
    }

    SECTION("a rotation does not discard the position")
    {
        quaternion_type turn;
        turn.set_from_euler({0.3f, 0.6f, 0.4f});

        a.set_transform({4, 5, 6}, turn, {2, 2, 2});

        const auto placed = a.getModelMatrix().translation();

        REQUIRE(placed.x == Approx(4.0f));
        REQUIRE(placed.y == Approx(5.0f));
        REQUIRE(placed.z == Approx(6.0f));
    }

    SECTION("the matrix overload stores exactly what it is handed")
    {
        quaternion_type turn;
        turn.set_from_euler({0.2f, -0.5f, 0.9f});

        matrix4x4_type transform;
        transform.set_translation({1, 2, 3});
        transform.set_rotation(turn);

        a.set_transform(transform);

        REQUIRE(a.getModelMatrix() == transform);
    }

    SECTION("scale reaches the matrix")
    {
        a.set_transform(vector3_type::zero, quaternion_type::identity, {2, 3, 4});

        REQUIRE(a.getModelMatrix().get(0, 0) == Approx(2.0f));
        REQUIRE(a.getModelMatrix().get(1, 1) == Approx(3.0f));
        REQUIRE(a.getModelMatrix().get(2, 2) == Approx(4.0f));
    }
}

TEST_CASE("gdk::webgl1es2_entity drawing", "[gdk::webgl1es2_entity]")
{
    initGL();

    const auto pModel = a_model();
    const auto pMaterial = a_material();

    webgl1es2_entity a(pModel, pMaterial);
    a.set_transform({1, 2, 3}, quaternion_type::identity);

    const auto view = a_view_matrix();
    const auto projection = a_projection_matrix();

    SECTION("a draw leaves gl in a good state")
    {
        as_the_scene_would(*pMaterial, *pModel);

        a.draw(view, projection, projection * view);

        REQUIRE(!jfc::glGetError());
    }

    SECTION("the mvp uniform receives projection * view * model")
    {
        as_the_scene_would(*pMaterial, *pModel);

        a.draw(view, projection, projection * view);

        const auto expected = projection * view * a.getModelMatrix();
        const auto uploaded = read_mat4("_MVP");

        for (std::size_t column = 0; column < matrix4x4_type::order; ++column)
            for (std::size_t row = 0; row < matrix4x4_type::order; ++row)
                REQUIRE(uploaded.at(column * matrix4x4_type::order + row)
                    == Approx(expected.get(column, row)).margin(1e-5f));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("moving the entity changes what is uploaded")
    {
        as_the_scene_would(*pMaterial, *pModel);

        a.draw(view, projection, projection * view);
        const auto before = read_mat4("_MVP");

        a.set_transform({-7, 0, 0}, quaternion_type::identity);
        a.draw(view, projection, projection * view);
        const auto after = read_mat4("_MVP");

        REQUIRE(before != after);
        REQUIRE(!jfc::glGetError());
    }

    SECTION("a hidden entity uploads nothing and draws nothing")
    {
        as_the_scene_would(*pMaterial, *pModel);

        a.draw(view, projection, projection * view);
        const auto whileVisible = read_mat4("_MVP");

        a.hide();
        REQUIRE(a.is_hidden());

        a.set_transform({100, 100, 100}, quaternion_type::identity);
        a.draw(view, projection, projection * view);

        REQUIRE(read_mat4("_MVP") == whileVisible);

        a.show();
        REQUIRE_FALSE(a.is_hidden());

        a.draw(view, projection, projection * view);
        REQUIRE(read_mat4("_MVP") != whileVisible);

        REQUIRE(!jfc::glGetError());
    }

    SECTION("the model's attributes are enabled at the locations the shader reports")
    {
        as_the_scene_would(*pMaterial, *pModel);

        for (const auto &[name, components] : std::vector<std::pair<std::string, GLint>>{
            {"a_Position", 3}, {"a_UV", 2}}) {
            const auto location = glGetAttribLocation(current_program(), name.c_str());
            REQUIRE(location >= 0);

            GLint enabled = 0;
            glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
            REQUIRE(enabled);

            GLint size = 0;
            glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
            REQUIRE(size == components);
        }

        REQUIRE(!jfc::glGetError());
    }
}
