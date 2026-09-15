// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/webgl1es2_gl_state.h>

#include "test_include.h"

#include <jfc/catch.hpp>

#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/webgl1es2_entity.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_scene.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdk/graphics/webgl1es2_texture.h>
#include <gdk/graphics/webgl1es2_texture_camera.h>

#include <array>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    constexpr int LENGTH = 16;             //!< a power of four, as the packing needs
    constexpr int SIDE = 64;               //!< sqrt(16 cubed): the texture it packs into

    [[nodiscard]] constexpr int channel_for(const int aCoordinate) { return aCoordinate * 17; }

    [[nodiscard]] std::vector<texture_data::channel_type> a_volume_of_its_own_coordinates() {
        std::vector<texture_data::channel_type> out(SIDE * SIDE * 3);

        for (int z = 0; z < LENGTH; ++z)
            for (int y = 0; y < LENGTH; ++y)
                for (int x = 0; x < LENGTH; ++x) {
                    const int index = x + y * LENGTH + z * LENGTH * LENGTH;

                    for (const auto &[channel, value] : {std::pair(0, x), std::pair(1, y),
                        std::pair(2, z)})
                        out[static_cast<std::size_t>(index * 3 + channel)] =
                            static_cast<texture_data::channel_type>(channel_for(value));
                }

        return out;
    }

    struct probe final {
        std::vector<texture_data::channel_type> volume = a_volume_of_its_own_coordinates();

        std::shared_ptr<webgl1es2_texture> pVolume = std::make_shared<webgl1es2_texture>(
            texture_data::view{SIDE, SIDE, texture::format::rgb, volume.data()},
            texture::wrap_mode::clamped, texture::wrap_mode::clamped);

        std::shared_ptr<webgl1es2_shader_program> pShader =
            std::make_shared<webgl1es2_shader_program>(R"V0G0N(
            attribute highp vec3 a_Position;

            void main() { gl_Position = vec4(a_Position.xy * 4.0, 0.0, 1.0); }
            )V0G0N", R"V0G0N(
            uniform sampler2D _Volume;
            uniform mediump vec3 _Index;
            uniform mediump vec3 _Position;
            uniform mediump float _Blend;

            void main() {
                gl_FragColor = _Blend < 0.5
                    ? gdk_texture3D(_Volume, 16.0, vec2(0.5, 0.5), ivec3(_Index))
                    : gdk_texture3D_trilinear(_Volume, 16.0, _Position);
            }
            )V0G0N");

        std::shared_ptr<webgl1es2_material> pMaterial = std::make_shared<webgl1es2_material>(
            pShader, material::face_culling_mode::none, material::render_mode::opaque);

        std::shared_ptr<webgl1es2_entity> pEntity =
            std::make_shared<webgl1es2_entity>(webgl1es2_model::make_cube(), pMaterial);

        std::shared_ptr<webgl1es2_texture_camera> pCamera =
            std::make_shared<webgl1es2_texture_camera>(intvector2_type(4, 4));

        webgl1es2_scene scene{std::make_shared<gl_state>()};

        probe() {
            pMaterial->set_texture("_Volume", pVolume);

            pEntity->set_transform({0, 0, -3}, quaternion_type::identity);
            pCamera->set_perspective_projection(1.0f, 0.1f, 100.0f, 1.0f);
            pCamera->set_transform(vector3_type::zero, quaternion_type::identity);

            static_cast<void>(pCamera->get_color_texture(0));

            scene.add(pEntity);
            scene.add(pCamera);
        }

        ~probe() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        [[nodiscard]] std::array<int, 3> drawn() {
            scene.draw({4, 4});

            std::array<std::uint8_t, 4> pixel{};
            glReadPixels(1, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel.data());

            return {pixel[0], pixel[1], pixel[2]};
        }

        [[nodiscard]] std::array<int, 3> at_index(const int aX, const int aY, const int aZ) {
            pMaterial->set_float("_Blend", 0.0f);
            pMaterial->set_vector3("_Index", {static_cast<float>(aX), static_cast<float>(aY),
                static_cast<float>(aZ)});

            return drawn();
        }

        [[nodiscard]] std::array<int, 3> between(const float aX, const float aY, const float aZ) {
            pMaterial->set_float("_Blend", 1.0f);
            pMaterial->set_vector3("_Position", {aX, aY, aZ});

            return drawn();
        }
    };

    void require_near(const std::array<int, 3> &aGot, const std::array<float, 3> &aExpected) {
        for (std::size_t channel = 0; channel < 3; ++channel)
            REQUIRE(static_cast<float>(aGot[channel]) == Approx(aExpected[channel]).margin(1.0f));
    }
}

TEST_CASE("**gdk_texture3D reads the voxel it is asked for**", "[gdk_texture3D]")
{
    initGL();

    probe subject;

    SECTION("a voxel in the middle of the volume") {
        REQUIRE(subject.at_index(3, 7, 12) == std::array<int, 3>{51, 119, 204});
    }

    SECTION("the first and last voxels, the packing's two ends") {
        REQUIRE(subject.at_index(0, 0, 0) == std::array<int, 3>{0, 0, 0});
        REQUIRE(subject.at_index(15, 15, 15) == std::array<int, 3>{255, 255, 255});
    }

    SECTION("a voxel on every row of a texel line: y from 0 to 3 shares one line of the texture") {
        for (int y = 0; y < 4; ++y)
            REQUIRE(subject.at_index(9, y, 5)
                == std::array<int, 3>{channel_for(9), channel_for(y), channel_for(5)});
    }

    REQUIRE(!jfc::glGetError());
}

TEST_CASE("**an index outside the volume reads the nearest voxel at its edge**",
    "[gdk_texture3D]")
{
    initGL();

    probe subject;

    SECTION("past the high edge on one axis") {
        REQUIRE(subject.at_index(16, 5, 0) == std::array<int, 3>{255, channel_for(5), 0});
    }

    SECTION("past both edges on several axes at once") {
        REQUIRE(subject.at_index(20, -1, 16) == std::array<int, 3>{255, 0, 255});
        REQUIRE(subject.at_index(-3, 16, 5) == std::array<int, 3>{0, 255, channel_for(5)});
    }

    REQUIRE(!jfc::glGetError());
}

TEST_CASE("**gdk_texture3D_trilinear blends between voxel centres**", "[gdk_texture3D]")
{
    initGL();

    probe subject;

    SECTION("at a voxel's centre it is that voxel") {
        REQUIRE(subject.between(3.5f, 7.5f, 12.5f) == std::array<int, 3>{51, 119, 204});
    }

    SECTION("on the face between two voxels it is their average") {
        require_near(subject.between(4.0f, 7.5f, 12.5f), {17.0f * 3.5f, 119.0f, 204.0f});
    }

    SECTION("at the corner of eight it is the average of all of them, per axis") {
        require_near(subject.between(4.0f, 8.0f, 13.0f),
            {17.0f * 3.5f, 17.0f * 7.5f, 17.0f * 12.5f});
    }

    SECTION("a quarter of the way across, it is a quarter of the way") {
        require_near(subject.between(3.75f, 7.5f, 12.5f), {17.0f * 3.25f, 119.0f, 204.0f});
    }

    SECTION("past the edge of the volume it holds the edge's value") {
        require_near(subject.between(20.0f, 7.5f, 12.5f), {255.0f, 119.0f, 204.0f});
        require_near(subject.between(0.2f, 7.5f, 12.5f), {0.0f, 119.0f, 204.0f});
    }

    REQUIRE(!jfc::glGetError());
}
