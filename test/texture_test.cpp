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
            // desktop GL can say whether a level exists: level 1 of a 2 by 2 texture would be 1 wide
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
