// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include <gdk/graphics/null_context.h>
#include <gdk/graphics/profile.h>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/texture_data.h>

#include <string>
#include <vector>

using namespace gdk;
using namespace gdk::graphics;

namespace {
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

    [[nodiscard]] std::string vertex_with(const std::string &aMore) {
        return aMore + "\nattribute vec3 a_Position;\nvoid main() { gl_Position = vec4(a_Position, 1.0); }\n";
    }

    [[nodiscard]] std::string fragment_with(const std::string &aMore) {
        return "precision mediump float;\n" + aMore + "\nvoid main() { gl_FragColor = vec4(1.0); }\n";
    }

    [[nodiscard]] std::string repeated(const std::string &aDeclaration, const int aTimes) {
        std::string out;

        for (int i = 0; i < aTimes; ++i) out += aDeclaration + std::to_string(i) + ";\n";

        return out;
    }
}

TEST_CASE("**the spec profiles are the specs' guaranteed minimums**", "[profile]") {
    const auto es2 = es2_minimum();

    REQUIRE(es2.name == "es2_minimum");
    REQUIRE(es2.most.texture_size == 64);
    REQUIRE_FALSE(es2.most.non_power_of_two_textures);
    REQUIRE(es2.most.vertex_uniform_vectors == 128);
    REQUIRE(es2.most.fragment_uniform_vectors == 16);
    REQUIRE(es2.most.texture_units == 8);
    REQUIRE(es2.most.vertex_texture_units == 0);
    REQUIRE(es2.most.vertex_attributes == 8);
    REQUIRE(es2.most.varying_vectors == 8);

    REQUIRE(webgl1_minimum().name == "webgl1_minimum");
    REQUIRE(webgl1_minimum().most == es2.most);

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
    REQUIRE(null_context::make(webgl1_minimum())->limits() == webgl1_minimum().most);

    profile wide{"wide", {}};
    wide.most.texture_size = 1 << 20;

    REQUIRE(null_context::make(wide)->limits().texture_size == 8192);
}

TEST_CASE("**a texture is held to the profile, when made and when given new data**", "[profile][texture]") {
    const auto pContext = null_context::make(webgl1_minimum());

    REQUIRE(texture_of(pContext, 64, 32));

    SECTION("**one not a power of two is refused, saying so**") {
        REQUIRE_THROWS_WITH(texture_of(pContext, 48, 32),
            "webgl1_minimum: a texture 48 wide is not a power of two, which webgl1_minimum requires");
        REQUIRE_THROWS_WITH(texture_of(pContext, 32, 12), Catch::Contains("12 tall is not a power of two"));
    }

    SECTION("**one past the largest is refused, saying the limit**") {
        REQUIRE_THROWS_WITH(texture_of(pContext, 128, 32),
            "webgl1_minimum: a texture 128 wide, past webgl1_minimum's largest texture of 64");
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

TEST_CASE("**a shader is counted from its declarations, a stage at a time**", "[profile][shader]") {
    const auto pContext = null_context::make(webgl1_minimum());

    REQUIRE(pContext->make_shader(vertex_with(""), fragment_with("")));

    SECTION("**fragment uniform vectors**, sixteen at most") {
        REQUIRE(pContext->make_shader(vertex_with(""), fragment_with(repeated("uniform vec4 u_", 16))));
        REQUIRE_THROWS_WITH(pContext->make_shader(vertex_with(""), fragment_with(repeated("uniform vec4 u_", 17))),
            "webgl1_minimum: a fragment shader's uniform vectors 17, past webgl1_minimum's fragment uniform vectors "
            "of 16");
    }

    SECTION("**a matrix is a vector a column, and an array its size times its type**") {
        REQUIRE(pContext->make_shader(vertex_with("uniform mat4 u_Bones[30];\nuniform mat3 u_Normal;\n"
            "uniform highp vec4 u_A, u_B;"), fragment_with("")));
        REQUIRE_THROWS_WITH(pContext->make_shader(vertex_with("uniform mat4 u_Bones[31];\nuniform mat3 u_Normal;\n"
            "uniform highp vec4 u_A, u_B;"), fragment_with("")), Catch::Contains("vertex shader's uniform vectors 129"));
    }

    SECTION("**an array sized by a #define or a const int is counted at that size**") {
        REQUIRE(pContext->make_shader(vertex_with("#define BONES 32\nuniform mat4 u_Bones[BONES];"), fragment_with("")));
        REQUIRE_THROWS_WITH(pContext->make_shader(vertex_with("#define BONES 33\nuniform mat4 u_Bones[BONES];"),
            fragment_with("")), Catch::Contains("vertex shader's uniform vectors 132"));
        REQUIRE_THROWS_WITH(pContext->make_shader(vertex_with("const int LIGHTS = 17;"),
            fragment_with("const int LIGHTS = 17;\nuniform vec4 u_Lights[LIGHTS];")),
            Catch::Contains("fragment shader's uniform vectors 17"));
    }

    SECTION("**a precision chosen by a #define is a precision**") {
        const std::string chosen = "#ifdef GL_FRAGMENT_PRECISION_HIGH\n#define HIGHP_IF_ANY highp\n#else\n"
            "#define HIGHP_IF_ANY mediump\n#endif\n";

        REQUIRE(pContext->make_shader(vertex_with(chosen + "uniform HIGHP_IF_ANY vec4 u_Across;"), fragment_with("")));
        REQUIRE_THROWS_WITH(pContext->make_shader(vertex_with(""),
            fragment_with(chosen + "uniform HIGHP_IF_ANY vec4 u_Across[17];")),
            Catch::Contains("fragment shader's uniform vectors 17"));
    }

    SECTION("**an array whose size it cannot read is refused, rather than guessed**") {
        REQUIRE_THROWS_WITH(pContext->make_shader(vertex_with("uniform vec4 u_Lights[COUNT];"), fragment_with("")),
            Catch::Contains("u_Lights[COUNT]"));
    }

    SECTION("**what is commented out is not declared**") {
        REQUIRE(pContext->make_shader(vertex_with(""),
            fragment_with("// " + repeated("uniform vec4 x_", 1) + "/*\n" + repeated("uniform vec4 y_", 20) + "*/")));
    }

    SECTION("**samplers against texture units**, and none in a vertex shader") {
        REQUIRE(pContext->make_shader(vertex_with(""), fragment_with(repeated("uniform sampler2D t_", 8))));
        REQUIRE_THROWS_WITH(pContext->make_shader(vertex_with(""), fragment_with(repeated("uniform sampler2D t_", 9))),
            Catch::Contains("samplers 9, past webgl1_minimum's texture units of 8"));
        REQUIRE_THROWS_WITH(pContext->make_shader(vertex_with("uniform sampler2D u_Height;"), fragment_with("")),
            Catch::Contains("vertex texture units of 0"));
    }

    SECTION("**attributes and varyings**, eight of each") {
        REQUIRE_THROWS_WITH(pContext->make_shader(vertex_with(repeated("attribute vec4 a_", 8)), fragment_with("")),
            Catch::Contains("attribute vectors 9"));
        REQUIRE_THROWS_WITH(pContext->make_shader(vertex_with(repeated("varying vec2 v_", 9)),
            fragment_with(repeated("varying vec2 v_", 9))), Catch::Contains("varying vectors 9"));
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
