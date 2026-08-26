// © Joseph Cameron - All Rights Reserved

#include "test_include.h"

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/webgl1es2_texture.h>

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
}
