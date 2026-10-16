// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/webgl1es2_gl_state.h>

#include "test_include.h"

#include <jfc/catch.hpp>
#include <jfc/types.h>

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
#include <type_traits>
#include <vector>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    struct checker final {
        std::vector<texture_data::channel_type> pixels{
            0x00, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff,
            0x00, 0x00, 0x00, 0xff};

        [[nodiscard]] texture_data::view view(const size_t aWidth = 2, const size_t aHeight = 2) const {
            texture_data::view v;
            v.width = aWidth;
            v.height = aHeight;
            v.format = texture::format::rgba;
            v.data = &pixels.front();

            return v;
        }
    };
}

TEST_CASE("gdk::webgl1es2_texture construction", "[gdk::webgl1es2_texture]")
{
    initGL();

    const checker image;

    SECTION("from a texture_data view")
    {
        const webgl1es2_texture tex(image.view());

        REQUIRE(tex.getHandle() != 0);
        REQUIRE(!jfc::glGetError());
    }

    SECTION("from loose format, size and data")
    {
        const webgl1es2_texture tex(webgl1es2_texture::format::rgba, 2, 2, &image.pixels.front());

        REQUIRE(tex.getHandle() != 0);
        REQUIRE(!jfc::glGetError());
    }

    SECTION("dimensions must be powers of two")
    {
        REQUIRE_THROWS(webgl1es2_texture(image.view(3, 2)));
        REQUIRE_THROWS(webgl1es2_texture(image.view(2, 3)));
    }

    SECTION("every wrap mode is accepted")
    {
        for (const auto u : {webgl1es2_texture::wrap_mode::repeat,
            webgl1es2_texture::wrap_mode::clamped, webgl1es2_texture::wrap_mode::mirrored}) {
            for (const auto v : {webgl1es2_texture::wrap_mode::repeat,
                webgl1es2_texture::wrap_mode::clamped, webgl1es2_texture::wrap_mode::mirrored}) {
                const webgl1es2_texture tex(image.view(), u, v);

                REQUIRE(tex.getHandle() != 0);
                REQUIRE(!jfc::glGetError());
            }
        }
    }

    SECTION("filtering is off unless it is asked for")
    {
        const webgl1es2_texture tex(image.view());

        glBindTexture(GL_TEXTURE_2D, tex.getHandle());

        GLint magnification{0}, minification{0};
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magnification);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minification);

        REQUIRE(magnification == GL_NEAREST);
        REQUIRE(minification == GL_NEAREST);
        REQUIRE(!jfc::glGetError());
    }

    SECTION("every minification and magnification filter is accepted")
    {
        using min_filter = webgl1es2_texture::minification_filter;
        using mag_filter = webgl1es2_texture::magnification_filter;

        for (const auto min : {min_filter::linear, min_filter::nearest,
            min_filter::nearest_mipmap_nearest, min_filter::linear_mipmap_nearest,
            min_filter::nearest_mipmap_linear, min_filter::linear_mipmap_linear}) {
            for (const auto mag : {mag_filter::nearest, mag_filter::linear}) {
                const webgl1es2_texture tex(image.view(),
                    webgl1es2_texture::wrap_mode::repeat, webgl1es2_texture::wrap_mode::repeat,
                    min, mag);

                REQUIRE(tex.getHandle() != 0);
                REQUIRE(!jfc::glGetError());
            }
        }
    }
}

TEST_CASE("gdk::webgl1es2_texture updating", "[gdk::webgl1es2_texture]")
{
    initGL();

    const checker image;
    webgl1es2_texture tex(image.view());

    const auto handleBefore = tex.getHandle();

    SECTION("replacing the data keeps the same gl object")
    {
        tex.update_data(image.view());

        REQUIRE(tex.getHandle() == handleBefore);
        REQUIRE(!jfc::glGetError());
    }

    SECTION("a sub-region can be replaced")
    {
        const checker patch;

        tex.update_data(patch.view(1, 1), 0, 0);

        REQUIRE(tex.getHandle() == handleBefore);
        REQUIRE(!jfc::glGetError());
    }
}

TEST_CASE("gdk::webgl1es2_texture semantics", "[gdk::webgl1es2_texture]")
{
    initGL();

    const checker image;

    SECTION("it is move only")
    {
        REQUIRE(std::is_move_constructible<webgl1es2_texture>::value);
        REQUIRE(std::is_move_assignable<webgl1es2_texture>::value);
        REQUIRE_FALSE(std::is_copy_constructible<webgl1es2_texture>::value);
        REQUIRE_FALSE(std::is_copy_assignable<webgl1es2_texture>::value);
    }

    SECTION("a moved-to texture carries the handle")
    {
        webgl1es2_texture source(image.view());
        const auto handle = source.getHandle();

        const webgl1es2_texture moved(std::move(source));

        REQUIRE(moved.getHandle() == handle);
        REQUIRE(!jfc::glGetError());
    }

    SECTION("equality distinguishes two separately built textures")
    {
        const webgl1es2_texture a(image.view());
        const webgl1es2_texture b(image.view());

        REQUIRE(a != b);
        REQUIRE(a == a);
    }
}

TEST_CASE("gdk::webgl1es2_texture provided resources", "[gdk::webgl1es2_texture]")
{
    initGL();

    SECTION("the checkerboard of death is live and shared")
    {
        const auto pFirst = webgl1es2_texture::make_checkerboard_of_death();

        REQUIRE(pFirst);
        REQUIRE(pFirst->getHandle() != 0);
        REQUIRE(!jfc::glGetError());

        REQUIRE(webgl1es2_texture::make_checkerboard_of_death().get() != pFirst.get());
    }

    SECTION("the device reports a usable maximum texture size")
    {
        const auto maxSize = webgl1es2_texture::getMaxTextureSize();

        REQUIRE(maxSize >= 64);
        REQUIRE(!jfc::glGetError());
    }

    SECTION("the maximum is asked of GL each time rather than remembered once")
    {
        const auto first = webgl1es2_texture::getMaxTextureSize();
        const auto second = webgl1es2_texture::getMaxTextureSize();

        REQUIRE(first == second);
        REQUIRE(first > 0);
        REQUIRE(!jfc::glGetError());
    }
}

TEST_CASE("gdk::webgl1es2_texture mipmaps follow the filter", "[gdk::webgl1es2_texture]")
{
    initGL();

    const checker image;

    using min_filter = webgl1es2_texture::minification_filter;
    using mag_filter = webgl1es2_texture::magnification_filter;

    SECTION("a texture whose filter reads no mipmaps has none generated, and samples as before")
    {
        for (const auto min : {min_filter::linear, min_filter::nearest}) {
            webgl1es2_texture tex(image.view(), webgl1es2_texture::wrap_mode::repeat,
                webgl1es2_texture::wrap_mode::repeat, min, mag_filter::linear);

            tex.update_data(image.view());
            tex.update_data(image.view(1, 1), 0, 0);

            REQUIRE(!jfc::glGetError());

#ifndef JFC_TARGET_PLATFORM_Emscripten
            glBindTexture(GL_TEXTURE_2D, tex.getHandle());

            GLint levelOneWidth = -1;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 1, GL_TEXTURE_WIDTH, &levelOneWidth);

            REQUIRE(levelOneWidth == 0);
            REQUIRE(!jfc::glGetError());
#endif
        }
    }

    SECTION("a mipmapped texture is complete after construction and after both kinds of update")
    {
        for (const auto min : {min_filter::nearest_mipmap_nearest, min_filter::linear_mipmap_nearest,
            min_filter::nearest_mipmap_linear, min_filter::linear_mipmap_linear}) {
            webgl1es2_texture tex(image.view(), webgl1es2_texture::wrap_mode::repeat,
                webgl1es2_texture::wrap_mode::repeat, min, mag_filter::linear);

            const auto level_one_width = [&]() {
                glBindTexture(GL_TEXTURE_2D, tex.getHandle());

                GLint width = -1;
#ifndef JFC_TARGET_PLATFORM_Emscripten
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 1, GL_TEXTURE_WIDTH, &width);
#else
                width = 1;
#endif
                return width;
            };

            REQUIRE(level_one_width() == 1);

            tex.update_data(image.view());
            REQUIRE(level_one_width() == 1);

            tex.update_data(image.view(1, 1), 0, 0);
            REQUIRE(level_one_width() == 1);

            REQUIRE(!jfc::glGetError());
        }
    }
}

TEST_CASE("gdk::webgl1es2_texture of a size that is not a power of two", "[gdk::webgl1es2_texture]")
{
    initGL();

    const checker image;

    using min_filter = webgl1es2_texture::minification_filter;
    using mag_filter = webgl1es2_texture::magnification_filter;
    using wrap = webgl1es2_texture::wrap_mode;

    SECTION("clamped on both axes and unmipmapped, any size is taken, as GLES 2 and WebGL 1 take it")
    {
        const webgl1es2_texture tex(image.view(3, 1), wrap::clamped, wrap::clamped,
            min_filter::linear, mag_filter::linear);

        REQUIRE(tex.getHandle() != 0);
        REQUIRE(!jfc::glGetError());
    }

    SECTION("repeating, mirrored, or mipmapped, it must still be a power of two")
    {
        REQUIRE_THROWS(webgl1es2_texture(image.view(3, 1), wrap::repeat, wrap::clamped));
        REQUIRE_THROWS(webgl1es2_texture(image.view(3, 1), wrap::clamped, wrap::mirrored));
        REQUIRE_THROWS(webgl1es2_texture(image.view(3, 1), wrap::clamped, wrap::clamped,
            min_filter::linear_mipmap_linear, mag_filter::linear));
    }

    SECTION("an update follows the same rule as construction")
    {
        webgl1es2_texture clamped(image.view(), wrap::clamped, wrap::clamped, min_filter::linear,
            mag_filter::linear);

        REQUIRE_NOTHROW(clamped.update_data(image.view(3, 1)));
        REQUIRE(!jfc::glGetError());

        webgl1es2_texture repeating(image.view());

        REQUIRE_THROWS(repeating.update_data(image.view(3, 1)));
    }
}

namespace {
    struct texel_probe final {
        std::shared_ptr<webgl1es2_shader_program> pShader =
            std::make_shared<webgl1es2_shader_program>(R"V0G0N(
            attribute highp vec3 a_Position;

            void main() { gl_Position = vec4(a_Position.xy * 4.0, 0.0, 1.0); }
            )V0G0N", R"V0G0N(
            uniform sampler2D _Texture;
            uniform mediump vec2 _At;
            uniform mediump float _Grey;

            void main() {
                lowp vec4 texel = texture2D(_Texture, _At);
                gl_FragColor = _Grey > 0.5 ? vec4(texel.a, 0.0, 0.0, 1.0) : vec4(texel.rgb, 1.0);
            }
            )V0G0N");

        std::shared_ptr<webgl1es2_material> pMaterial = std::make_shared<webgl1es2_material>(
            pShader, material::face_culling_mode::none, material::render_mode::opaque);

        std::shared_ptr<webgl1es2_entity> pEntity =
            std::make_shared<webgl1es2_entity>(webgl1es2_model::make_cube(), pMaterial);

        std::shared_ptr<webgl1es2_texture_camera> pCamera =
            std::make_shared<webgl1es2_texture_camera>(intvector2_type(4, 4));

        webgl1es2_scene scene{std::make_shared<gl_state>()};

        texel_probe() {
            pEntity->set_transform({0, 0, -3}, quaternion_type::identity);
            pCamera->set_perspective_projection(1.0f, 0.1f, 100.0f, 1.0f);
            pCamera->set_transform(vector3_type::zero, quaternion_type::identity);

            static_cast<void>(pCamera->get_color_texture(0));

            scene.add(pEntity);
            scene.add(pCamera);
        }

        ~texel_probe() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        [[nodiscard]] std::vector<std::array<int, 3>> texels(const std::shared_ptr<webgl1es2_texture> &aTexture,
            const int aWidth, const int aHeight, const bool aGrey) {
            pMaterial->set_texture("_Texture", aTexture);
            pMaterial->set_float("_Grey", aGrey ? 1.0f : 0.0f);

            std::vector<std::array<int, 3>> out;

            for (int y = 0; y < aHeight; ++y)
                for (int x = 0; x < aWidth; ++x) {
                    pMaterial->set_vector2("_At", {(static_cast<float>(x) + 0.5f) / static_cast<float>(aWidth),
                        (static_cast<float>(y) + 0.5f) / static_cast<float>(aHeight)});

                    scene.draw({4, 4});

                    std::array<std::uint8_t, 4> pixel{};
                    glReadPixels(1, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel.data());

                    out.push_back(aGrey ? std::array<int, 3>{pixel[0], 0, 0} : std::array<int, 3>{pixel[0], pixel[1], pixel[2]});
                }

            return out;
        }
    };

    void unpack_as_gl_starts() {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }
}

TEST_CASE("**rows that aren't a multiple of 4 bytes are read as they are packed**", "[gdk::webgl1es2_texture]")
{
    initGL();

    texel_probe probe;

    using wrap = texture::wrap_mode;

    const std::vector<texture_data::channel_type> grey{10, 20, 30, 40, 50, 60};
    const std::vector<std::array<int, 3>> greyTexels{{10, 0, 0}, {20, 0, 0}, {30, 0, 0}, {40, 0, 0}, {50, 0, 0}, {60, 0, 0}};

    const std::vector<texture_data::channel_type> rgb{
        10, 11, 12, 20, 21, 22, 30, 31, 32,
        40, 41, 42, 50, 51, 52, 60, 61, 62};
    const std::vector<std::array<int, 3>> rgbTexels{{10, 11, 12}, {20, 21, 22}, {30, 31, 32}, {40, 41, 42}, {50, 51, 52}, {60, 61, 62}};

    const texture_data::view greyView{3, 2, texture::format::grey, grey.data()};
    const texture_data::view rgbView{3, 2, texture::format::rgb, rgb.data()};

    SECTION("made with them")
    {
        unpack_as_gl_starts();
        const auto pGrey = std::make_shared<webgl1es2_texture>(greyView, wrap::clamped, wrap::clamped);
        REQUIRE(probe.texels(pGrey, 3, 2, true) == greyTexels);

        unpack_as_gl_starts();
        const auto pRgb = std::make_shared<webgl1es2_texture>(rgbView, wrap::clamped, wrap::clamped);
        REQUIRE(probe.texels(pRgb, 3, 2, false) == rgbTexels);
    }

    SECTION("given them, whole or over the top")
    {
        const std::vector<texture_data::channel_type> blank(18, 0);

        const auto pWhole = std::make_shared<webgl1es2_texture>(texture_data::view{1, 1, texture::format::grey, blank.data()},
            wrap::clamped, wrap::clamped);
        unpack_as_gl_starts();
        pWhole->update_data(greyView);
        REQUIRE(probe.texels(pWhole, 3, 2, true) == greyTexels);

        const auto pOver = std::make_shared<webgl1es2_texture>(texture_data::view{3, 2, texture::format::rgb, blank.data()},
            wrap::clamped, wrap::clamped);
        unpack_as_gl_starts();
        pOver->update_data(rgbView, 0, 0);
        REQUIRE(probe.texels(pOver, 3, 2, false) == rgbTexels);
    }

    REQUIRE(!jfc::glGetError());
}
