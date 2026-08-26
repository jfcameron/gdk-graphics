// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/graphics/types.h>

#include <cmath>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    using vec = vector3_type;
    using mat = matrix4x4_type;
    using quat = quaternion_type;

    constexpr float QUARTER_TURN = 1.5707963267948966f;

    [[nodiscard]] vec as_a_shader_would(const mat &aM, const vec &aV) {
        const auto e = [&aM](const std::size_t aColumn, const std::size_t aRow) {
            return aM.get(aColumn, aRow);
        };

        return vec{
            e(0, 0) * aV.x + e(1, 0) * aV.y + e(2, 0) * aV.z + e(3, 0),
            e(0, 1) * aV.x + e(1, 1) * aV.y + e(2, 1) * aV.z + e(3, 1),
            e(0, 2) * aV.x + e(1, 2) * aV.y + e(2, 2) * aV.z + e(3, 2)};
    }
}

TEST_CASE("gdk-graphics convention: handedness", "[convention]")
{
    SECTION("the basis is right handed, so X cross Y points toward the viewer")
    {
        REQUIRE(vec::right.cross_product(vec::up) == vec(0, 0, 1));
        REQUIRE(vec::up.cross_product(vec(0, 0, 1)) == vec::right);
        REQUIRE(vec(0, 0, 1).cross_product(vec::right) == vec::up);
    }

    SECTION("forward is -Z, because that is what right handedness means here")
    {
        REQUIRE(vec::forward == vec(0, 0, -1));
        REQUIRE(vec::backward == vec(0, 0, 1));
        REQUIRE(vec::right.cross_product(vec::up) == vec::backward);

        REQUIRE(vec::forward.dot_product(vec::backward) == Approx(-1.0f));
    }

    SECTION("rotations follow the right hand rule")
    {
        quat q;
        q.set_from_euler({0, 0, QUARTER_TURN});

        mat m;
        m.set_rotation(q);

        const auto turnedX = as_a_shader_would(m, vec::right);
        REQUIRE(turnedX.x == Approx(0.0f).margin(1e-5f));
        REQUIRE(turnedX.y == Approx(1.0f).margin(1e-5f));

        const auto turnedY = as_a_shader_would(m, vec::up);
        REQUIRE(turnedY.x == Approx(-1.0f).margin(1e-5f));
        REQUIRE(turnedY.y == Approx(0.0f).margin(1e-5f));
    }
}

TEST_CASE("gdk-graphics convention: matrix storage is column major", "[convention]")
{
    SECTION("set and get take the column first, the row second")
    {
        mat m;
        m.set_to_identity();
        m.set(2, 1, 7.0f);   

        REQUIRE(m.get(2, 1) == Approx(7.0f));
        REQUIRE(m.get(1, 2) == Approx(0.0f));   
    }

    SECTION("the raw array is what glUniformMatrix4fv expects with transpose = GL_FALSE")
    {
        mat m;
        m.set_to_identity();

        for (std::size_t column = 0; column < mat::order; ++column)
            for (std::size_t row = 0; row < mat::order; ++row)
                m.set(column, row, static_cast<float>(column * mat::order + row));

        const float *const raw = &m.front();

        for (std::size_t k = 0; k < mat::order * mat::order; ++k) {
            const auto column = k / mat::order;
            const auto row = k % mat::order;

            REQUIRE(raw[k] == Approx(m.get(column, row)));
            REQUIRE(raw[k] == Approx(static_cast<float>(k)));
        }
    }

    SECTION("translation lives in column 3")
    {
        mat m;
        m.set_to_identity();
        m.set_translation({4, 5, 6});

        REQUIRE(m.get(3, 0) == Approx(4.0f));
        REQUIRE(m.get(3, 1) == Approx(5.0f));
        REQUIRE(m.get(3, 2) == Approx(6.0f));
        REQUIRE(m.translation() == vec(4, 5, 6));
    }
}

TEST_CASE("gdk-graphics convention: vectors are columns", "[convention]")
{
    SECTION("a transform is applied as M * v")
    {
        quat q;
        q.set_from_euler({0, 0, QUARTER_TURN});

        mat m;
        m.set_translation({10, 0, 0});
        m.set_rotation(q);

        const auto placed = as_a_shader_would(m, vec::right);

        REQUIRE(placed.x == Approx(10.0f).margin(1e-4f));
        REQUIRE(placed.y == Approx(1.0f).margin(1e-4f));
    }

    SECTION("composition applies the rightmost factor first")
    {
        quat aboutZ, aboutX;
        aboutZ.set_from_euler({0, 0, QUARTER_TURN});
        aboutX.set_from_euler({QUARTER_TURN, 0, 0});

        mat first, second;
        first.set_rotation(aboutZ);
        second.set_rotation(aboutX);

        const auto probe = vec::right;
        const auto composed = as_a_shader_would(second * first, probe);
        const auto sequential = as_a_shader_would(second, as_a_shader_would(first, probe));

        REQUIRE(composed.x == Approx(sequential.x).margin(1e-5f));
        REQUIRE(composed.y == Approx(sequential.y).margin(1e-5f));
        REQUIRE(composed.z == Approx(sequential.z).margin(1e-5f));

        const auto reversed = as_a_shader_would(first * second, probe);
        REQUIRE(std::abs(reversed.z - composed.z) > 0.5f);
    }

    SECTION("the model-view-projection order the renderer uses composes correctly")
    {
        quat turn;
        turn.set_from_euler({0.3f, 0.6f, 0.4f});

        mat model, view, projection;
        model.set_translation({1, 2, 3});
        model.set_rotation(turn);
        view.set_translation({-4, 0, 2});
        projection.set_to_identity();
        projection.set(0, 0, 1.5f);   

        const vec point{0.5f, -0.25f, 0.75f};

        const auto viaMvp = as_a_shader_would(projection * view * model, point);
        const auto viaStages = as_a_shader_would(projection,
            as_a_shader_would(view, as_a_shader_would(model, point)));

        REQUIRE(viaMvp.x == Approx(viaStages.x).margin(1e-4f));
        REQUIRE(viaMvp.y == Approx(viaStages.y).margin(1e-4f));
        REQUIRE(viaMvp.z == Approx(viaStages.z).margin(1e-4f));
    }
}
