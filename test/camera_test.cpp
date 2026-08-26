// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/graphics/types.h>
#include <gdk/graphics/webgl1es2_screen_camera.h>

#include <cmath>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    [[nodiscard]] vector3_type as_a_shader_would(const matrix4x4_type &aM,
        const vector3_type &aV) {
        const auto e = [&aM](const std::size_t aColumn, const std::size_t aRow) {
            return aM.get(aColumn, aRow);
        };

        return vector3_type{
            e(0, 0) * aV.x + e(1, 0) * aV.y + e(2, 0) * aV.z + e(3, 0),
            e(0, 1) * aV.x + e(1, 1) * aV.y + e(2, 1) * aV.z + e(3, 1),
            e(0, 2) * aV.x + e(1, 2) * aV.y + e(2, 2) * aV.z + e(3, 2)};
    }

    [[nodiscard]] float clip_depth(const matrix4x4_type &aProjection, const float aViewZ) {
        const auto z = aProjection.get(2, 2) * aViewZ + aProjection.get(3, 2);
        const auto w = aProjection.get(2, 3) * aViewZ + aProjection.get(3, 3);

        return w != 0 ? z / w : z;
    }
}

TEST_CASE("camera construction", "[camera]")
{
    initGL();

    webgl1es2_screen_camera a;

    SECTION("default ctor leaves gl in a good state")
    {
        REQUIRE(!jfc::glGetError());
    }

    SECTION("the matrices start as identity")
    {
        REQUIRE(a.get_world_matrix() == matrix4x4_type::identity);
        REQUIRE(a.get_view_matrix() == matrix4x4_type::identity);
        REQUIRE(a.get_projection_matrix() == matrix4x4_type::identity);
    }
}

TEST_CASE("camera transform", "[camera]")
{
    initGL();

    webgl1es2_screen_camera a;

    SECTION("the view matrix is the inverse of the world matrix")
    {
        quaternion_type turn;
        turn.set_from_euler({0.3f, 0.6f, 0.4f});

        a.set_transform({2, -3, 4}, turn);

        const auto shouldBeIdentity = a.get_view_matrix() * a.get_world_matrix();

        for (std::size_t column = 0; column < matrix4x4_type::order; ++column)
            for (std::size_t row = 0; row < matrix4x4_type::order; ++row)
                REQUIRE(shouldBeIdentity.get(column, row)
                    == Approx(matrix4x4_type::identity.get(column, row)).margin(1e-4f));
    }

    SECTION("a point at the camera's position lands at the view-space origin")
    {
        quaternion_type turn;
        turn.set_from_euler({0, 0.7f, 0});

        const vector3_type position{5, 6, -7};
        a.set_transform(position, turn);

        const auto viewed = as_a_shader_would(a.get_view_matrix(), position);

        REQUIRE(viewed.x == Approx(0.0f).margin(1e-4f));
        REQUIRE(viewed.y == Approx(0.0f).margin(1e-4f));
        REQUIRE(viewed.z == Approx(0.0f).margin(1e-4f));
    }

    SECTION("an unrotated camera sees what is in front of it down -Z")
    {
        a.set_transform(vector3_type::zero, quaternion_type::identity);

        const auto ahead = as_a_shader_would(a.get_view_matrix(),
            vector3_type::forward * 10.0f);

        REQUIRE(ahead.z == Approx(-10.0f).margin(1e-4f));
    }

    SECTION("the matrix overload agrees with the position and rotation one")
    {
        quaternion_type turn;
        turn.set_from_euler({0.2f, -0.5f, 0.9f});

        a.set_transform({1, 2, 3}, turn);
        const auto viaComponents = a.get_view_matrix();

        matrix4x4_type transform;
        transform.set_translation({1, 2, 3});
        transform.set_rotation(turn);

        webgl1es2_screen_camera b;
        b.set_transform(transform);

        const auto rebuilt = b.get_view_matrix();

        for (std::size_t column = 0; column < matrix4x4_type::order; ++column)
            for (std::size_t row = 0; row < matrix4x4_type::order; ++row)
                REQUIRE(rebuilt.get(column, row)
                    == Approx(viaComponents.get(column, row)).margin(1e-5f));
    }
}

TEST_CASE("camera projection", "[camera]")
{
    initGL();

    webgl1es2_screen_camera a;

    constexpr float NEAR = 0.1f;
    constexpr float FAR = 100.0f;

    SECTION("perspective maps the near and far planes to OpenGL's depth range")
    {
        a.set_perspective_projection(1.0f, NEAR, FAR, 1.0f);

        REQUIRE(clip_depth(a.get_projection_matrix(), -NEAR) == Approx(-1.0f).margin(1e-4f));
        REQUIRE(clip_depth(a.get_projection_matrix(), -FAR) == Approx(1.0f).margin(1e-3f));
    }

    SECTION("orthographic maps them the same way")
    {
        a.set_orthographic_projection({2, 2}, NEAR, FAR, 1.0f);

        REQUIRE(clip_depth(a.get_projection_matrix(), -NEAR) == Approx(-1.0f).margin(1e-4f));
        REQUIRE(clip_depth(a.get_projection_matrix(), -FAR) == Approx(1.0f).margin(1e-4f));
    }

    SECTION("field of view and aspect ratio reach the matrix")
    {
        a.set_perspective_projection(1.0f, NEAR, FAR, 1.0f);
        const auto narrow = a.get_projection_matrix().get(1, 1);

        a.set_perspective_projection(2.0f, NEAR, FAR, 1.0f);
        const auto wide = a.get_projection_matrix().get(1, 1);

        REQUIRE(wide < narrow);

        a.set_perspective_projection(1.0f, NEAR, FAR, 2.0f);
        const auto stretched = a.get_projection_matrix();

        REQUIRE(stretched.get(0, 0) < stretched.get(1, 1));   
    }

    SECTION("the caller never has to know the depth range")
    {
        a.set_perspective_projection(1.0f, NEAR, FAR, 1.0f);

        REQUIRE(!jfc::glGetError());
        REQUIRE(a.get_projection_matrix() != matrix4x4_type::identity);
    }
}

TEST_CASE("camera viewport and clearing", "[camera]")
{
    initGL();

    webgl1es2_screen_camera a;

    SECTION("setting a viewport and activating leaves gl in a good state")
    {
        a.set_viewport(0.5f, 0.0f, 0.5f, 1.0f);
        a.set_clear_color(color::blue);
        a.set_clear_mode(camera::clear_mode::color_and_depth);

        a.activate(intvector2_type(400, 300));

        REQUIRE(!jfc::glGetError());
    }

    SECTION("**a fractional viewport reaches gl as the right pixel rectangle**")
    {
        a.set_viewport(0.5f, 0.0f, 0.5f, 1.0f);
        a.activate(intvector2_type(400, 300));

        GLint viewport[4] = {-1, -1, -1, -1};
        glGetIntegerv(GL_VIEWPORT, viewport);

        REQUIRE(viewport[0] == 200);   
        REQUIRE(viewport[1] == 0);    
        REQUIRE(viewport[2] == 200); 
        REQUIRE(viewport[3] == 300);

        REQUIRE(!jfc::glGetError());
    }

    SECTION("the full viewport covers the whole framebuffer")
    {
        a.set_viewport(0.0f, 0.0f, 1.0f, 1.0f);
        a.activate(intvector2_type(400, 300));

        GLint viewport[4] = {-1, -1, -1, -1};
        glGetIntegerv(GL_VIEWPORT, viewport);

        REQUIRE(viewport[2] == 400);
        REQUIRE(viewport[3] == 300);
    }

    SECTION("every clear mode activates cleanly")
    {
        for (const auto mode : {camera::clear_mode::nothing,
            camera::clear_mode::color_and_depth, camera::clear_mode::depth_only}) {
            a.set_clear_mode(mode);
            a.activate(intvector2_type(400, 300));

            REQUIRE(!jfc::glGetError());
        }
    }
}

TEST_CASE("camera value semantics", "[camera]")
{
    initGL();

    webgl1es2_screen_camera a;
    a.set_perspective_projection(1.0f, 0.1f, 100.0f, 1.0f);
    a.set_transform({1, 2, 3}, quaternion_type::identity);

    SECTION("a copy carries the matrices and activates cleanly")
    {
        webgl1es2_screen_camera b(a);
        auto c = b;

        REQUIRE(c.get_view_matrix() == a.get_view_matrix());
        REQUIRE(c.get_projection_matrix() == a.get_projection_matrix());

        c.activate(intvector2_type(400, 300));
        REQUIRE(!jfc::glGetError());
    }

    SECTION("a moved-from camera's value is carried across")
    {
        const auto expectedView = a.get_view_matrix();

        webgl1es2_screen_camera b(std::move(a));

        REQUIRE(b.get_view_matrix() == expectedView);

        b.activate(intvector2_type(400, 300));
        REQUIRE(!jfc::glGetError());
    }
}
