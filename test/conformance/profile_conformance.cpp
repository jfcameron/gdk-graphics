// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include "conformance.h"

#include <gdk/graphics/exception.h>
#include <gdk/graphics/model.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/texture.h>
#include <gdk/graphics/texture_data.h>

#include <vector>

using namespace gdk::graphics;
using gdk::graphics::conformance::implementation;
using gdk::graphics::conformance::make_context;
using gdk::graphics::conformance::minimum_profile;

namespace {
    [[nodiscard]] profile small_target() {
        profile out{"small_target", {}};

        out.most.texture_size = 64;
        out.most.non_power_of_two_textures = false;
        out.most.vertex_uniform_vectors = 128;
        out.most.fragment_uniform_vectors = 16;
        out.most.texture_units = 8;
        out.most.vertex_texture_units = 0;
        out.most.vertex_attributes = 8;
        out.most.varying_vectors = 8;

        return out;
    }

    struct texels final {
        std::vector<texture_data::channel_type> rgba;
        std::size_t width, height;

        texels(const std::size_t aWidth, const std::size_t aHeight)
        : rgba(aWidth * aHeight * 4, 0xFF)
        , width(aWidth)
        , height(aHeight)
        {}

        [[nodiscard]] texture_data::view view() const {
            return {.width = width, .height = height, .format = texture::format::rgba, .data = rgba.data()};
        }
    };
}

TEST_CASE("conformance, a context's limits are within its profile", "[conformance][profile]") {
    INFO("implementation " << implementation());

    const auto profile = small_target();
    const auto has = make_context(profile)->limits();

    REQUIRE(has.texture_size <= profile.most.texture_size);
    REQUIRE((!has.non_power_of_two_textures || profile.most.non_power_of_two_textures));
    REQUIRE(has.vertex_uniform_vectors <= profile.most.vertex_uniform_vectors);
    REQUIRE(has.fragment_uniform_vectors <= profile.most.fragment_uniform_vectors);
    REQUIRE(has.texture_units <= profile.most.texture_units);
    REQUIRE(has.vertex_texture_units <= profile.most.vertex_texture_units);
    REQUIRE(has.vertex_attributes <= profile.most.vertex_attributes);
    REQUIRE(has.varying_vectors <= profile.most.varying_vectors);

    SECTION("and with none, it says something can be drawn") {
        const auto unheld = make_context()->limits();

        REQUIRE(unheld.texture_size >= 64);
        REQUIRE(unheld.vertex_attributes >= 8);
    }
}

TEST_CASE("conformance, textures past the profile are refused, and what it allows is not", "[conformance][profile]") {
    INFO("implementation " << implementation());

    const auto pContext = make_context(small_target());

    SECTION("the largest texture, and past it") {
        REQUIRE_NOTHROW(pContext->make_texture(texels(64, 64).view()));
        REQUIRE_THROWS_AS(pContext->make_texture(texels(128, 128).view()), exception);
    }

    SECTION("a size not a power of two repeating is refused, clamped on both axes is allowed") {
        REQUIRE_THROWS_AS(pContext->make_texture(texels(48, 48).view(), texture::wrap_mode::repeat,
            texture::wrap_mode::repeat), exception);
        REQUIRE_THROWS_AS(pContext->make_texture(texels(48, 48).view(), texture::wrap_mode::clamped,
            texture::wrap_mode::repeat), exception);
        REQUIRE_NOTHROW(pContext->make_texture(texels(48, 48).view(), texture::wrap_mode::clamped,
            texture::wrap_mode::clamped));
    }

    SECTION("a texture filled in again is held to the profile as well") {
        const auto pTexture = pContext->make_texture(texels(4, 4).view());

        REQUIRE_THROWS_AS(pTexture->update_data(texels(128, 128).view()), exception);
    }
}

TEST_CASE("conformance, a model past the profile's attributes is refused, made and uploaded", "[conformance][profile]") {
    INFO("implementation " << implementation());

    auto one = small_target();

    one.most.vertex_attributes = 1;

    const auto pContext = make_context(one);
    const auto quad = model_data::make_quad();

    REQUIRE(quad.attributes().size() > 1);

    REQUIRE_THROWS_AS(pContext->make_model(model::usage_hint::upload_once, quad), exception);

    const auto pModel = pContext->make_model();

    REQUIRE_THROWS_AS(pModel->upload(model::usage_hint::upload_once, quad), exception);
}

TEST_CASE("conformance, the built in shaders fit the implementation's own floor", "[conformance][profile]") {
    INFO("implementation " << implementation() << ", held to " << minimum_profile().name);

    const auto pContext = make_context(minimum_profile());

    REQUIRE_NOTHROW(pContext->make_alpha_cutoff_shader());
    REQUIRE_NOTHROW(pContext->make_alpha_blend_shader());
}
