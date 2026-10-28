// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/graphics/null_context.h>
#include <gdk/graphics/profile.h>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/texture_data.h>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    //! a small target's profile, written here: the interface's tests hold contexts to one without naming an API
    [[nodiscard]] profile small_gpu() {
        profile out{"small_gpu", {}};

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

    struct pixels final {
        std::vector<texture_data::channel_type> data;
        texture_data::view view;

        pixels(const std::size_t aWidth, const std::size_t aHeight)
        : data(aWidth * aHeight * 4, 255)
        , view{.width = aWidth, .height = aHeight, .format = texture::format::rgba, .data = data.data()}
        {}
    };

    [[nodiscard]] texture_ptr_type texture_of(const std::shared_ptr<null_context> &pContext,
        const std::size_t aWidth, const std::size_t aHeight) {
        const pixels made(aWidth, aHeight);

        return pContext->make_texture(made.view, texture::wrap_mode::repeat, texture::wrap_mode::repeat);
    }

    [[nodiscard]] std::string vertex_with(std::string_view aMore) {
        return std::string(aMore) + "\nattribute vec3 a_Position;\nvoid main() { gl_Position = vec4(a_Position, 1.0); }\n";
    }

    [[nodiscard]] std::string fragment_with(std::string_view aMore) {
        return "precision mediump float;\n" + std::string(aMore) + "\nvoid main() { gl_FragColor = vec4(1.0); }\n";
    }

    [[nodiscard]] std::string repeated(std::string_view aDeclaration, const int aTimes) {
        std::string out;

        for (int i = 0; i < aTimes; ++i) out += std::string(aDeclaration) + std::to_string(i) + ";\n";

        return out;
    }
}

TEST_CASE("**limits are unrestricted until a profile says otherwise**", "[profile]") {
    REQUIRE(limits{}.texture_size == limits::UNRESTRICTED);
    REQUIRE(limits{}.non_power_of_two_textures);
}

TEST_CASE("**a context made without a profile checks nothing past its own**", "[profile][null_context]") {
    const auto pContext = null_context::make();

    REQUIRE(texture_of(pContext, 1000, 3));
    REQUIRE(pContext->make_shader(vertex_with(repeated("uniform mat4 u_", 100)), fragment_with("")));

    REQUIRE(pContext->limits().texture_size == 8192);
    REQUIRE(pContext->limits().fragment_uniform_vectors == limits::UNRESTRICTED);
}

TEST_CASE("**a context made with a profile says the narrower limits**", "[profile][null_context]") {
    REQUIRE(null_context::make(small_gpu())->limits() == small_gpu().most);

    profile wide{"wide", {}};
    wide.most.texture_size = 1 << 20;

    REQUIRE(null_context::make(wide)->limits().texture_size == 8192);
}

TEST_CASE("**a texture is held to the profile, when made and when given new data**", "[profile][texture]") {
    const auto pContext = null_context::make(small_gpu());

    REQUIRE(texture_of(pContext, 64, 32));

    SECTION("**one not a power of two is refused, saying so**") {
        REQUIRE_THROWS_WITH(texture_of(pContext, 48, 32),
            "small_gpu: a texture 48 wide is not a power of two, which small_gpu requires");
        REQUIRE_THROWS_WITH(texture_of(pContext, 32, 12), Catch::Contains("12 tall is not a power of two"));
    }

    SECTION("**one past the largest is refused, saying the limit**") {
        REQUIRE_THROWS_WITH(texture_of(pContext, 128, 32),
            "small_gpu: a texture 128 wide, past small_gpu's largest texture of 64");
    }

    SECTION("**an empty one is a texture still to be given its data**, and is held to it then") {
        const auto pTexture = pContext->make_texture();
        const pixels npot(48, 64);

        REQUIRE_THROWS_WITH(pTexture->update_data(npot.view), Catch::Contains("48 wide is not a power of two"));

        const pixels fine(16, 16);

        REQUIRE_NOTHROW(pTexture->update_data(fine.view));
    }
}

TEST_CASE("**a profile is anybody's to write**", "[profile]") {
    profile tiny{"tiny_gpu", {}};
    tiny.most.texture_size = 16;

    const auto pContext = null_context::make(tiny);

    REQUIRE(texture_of(pContext, 12, 16));
    REQUIRE_THROWS_WITH(texture_of(pContext, 32, 16), Catch::Contains("tiny_gpu's largest texture of 16"));
}

TEST_CASE("**a shader is checked by the check the context was given, against its profile**", "[profile][shader]") {
    struct seen final {
        std::string profile_name;
        std::string vertex;
        std::string fragment;
    };

    const auto pSeen = std::make_shared<std::vector<seen>>();

    const null_context::shader_check_type check = [pSeen](const profile &aProfile, std::string_view aVertex,
        std::string_view aFragment) {
        pSeen->push_back({aProfile.name, std::string(aVertex), std::string(aFragment)});

        if (aFragment == "too much") throw exception(aProfile.name + ": too much");
    };

    SECTION("**given both, each shader is checked as it is made**, and one refused is not made") {
        const auto pContext = null_context::make(small_gpu(), check);

        REQUIRE(pContext->make_shader("the vertex stage", "the fragment stage"));
        REQUIRE(pSeen->size() == 1);
        REQUIRE(pSeen->front().profile_name == "small_gpu");
        REQUIRE(pSeen->front().vertex == "the vertex stage");
        REQUIRE(pSeen->front().fragment == "the fragment stage");
        REQUIRE(pContext->resources_made() == 1);

        REQUIRE_THROWS_WITH(pContext->make_shader("the vertex stage", "too much"), "small_gpu: too much");
        REQUIRE(pContext->resources_made() == 1);
    }

    SECTION("**given no profile, there is nothing to check against**") {
        REQUIRE(null_context::make({}, check)->make_shader("the vertex stage", "too much"));
        REQUIRE(pSeen->empty());
    }

    SECTION("**given no check, a profile counts nothing in a shader**") {
        REQUIRE(null_context::make(small_gpu())->make_shader(vertex_with(repeated("uniform mat4 u_", 100)),
            fragment_with(repeated("uniform sampler2D t_", 9))));
    }
}

TEST_CASE("**a model is held to the profile's attributes, made and uploaded**", "[profile][model]") {
    profile one{"one_attribute", {}};
    one.most.vertex_attributes = 1;

    const auto pContext = null_context::make(one);
    const auto quad = model_data::make_quad();

    REQUIRE(quad.attributes().size() > 1);

    REQUIRE_THROWS_WITH(pContext->make_model(model::usage_hint::upload_once, quad),
        Catch::Contains("a model's attributes " + std::to_string(quad.attributes().size())));

    const auto pEmpty = pContext->make_model();

    REQUIRE_THROWS_AS(pEmpty->upload(model::usage_hint::upload_once, quad), exception);
}
