// © Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/graphics/webgl1es2_context.h>
#include <gdk/graphics/webgl1es2_entity.h>
#include <gdk/graphics/webgl1es2_gl_state.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_scene.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdk/graphics/webgl1es2_texture_camera.h>

#include <array>
#include <cstdint>
#include <memory>

using namespace gdk;
using namespace gdk::graphics;

TEST_CASE("webgl1es2_model", "[gdk::webgl1es2_model]")
{
    initGL();

    SECTION("Hardcoded vertex data is well formed")
    {
        /*auto pCube = model::make_cube();

        REQUIRE(pCube->getHandle() >= 0);

        auto pQuad = static_cast<std::shared_ptr<model>>(model::Quad);

        REQUIRE(pQuad->getHandle() >= 0);*/
    }
}


TEST_CASE("**a model re-uploaded with different attributes rebinds correctly**",
    "[gdk::webgl1es2_model]")
{
    initGL();

    const auto pShader = webgl1es2_shader_program::make_alpha_cutoff();

    const auto uv = pShader->tryGetActiveAttribute("a_UV");

    REQUIRE(uv.has_value());

    const auto uv_array_enabled = [&] {
        GLint enabled = 0;

        glGetVertexAttribiv(static_cast<GLuint>(uv->location),
            GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);

        return enabled != 0;
    };

    const auto forget_previous_binds = [&] {
        glDisableVertexAttribArray(static_cast<GLuint>(uv->location));

        REQUIRE_FALSE(uv_array_enabled());
    };

    webgl1es2_model subject(model::usage_hint::streaming, model_data::make_quad());

    SECTION("a quad feeds the uv attribute")
    {
        forget_previous_binds();

        subject.bind(*pShader);

        REQUIRE(uv_array_enabled());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("and after re-uploading without uvs, it must stop feeding it")
    {
        subject.bind(*pShader);

        model_data positionsOnly({{"a_Position",
            attribute_data({0, 0, 0, 1, 0, 0, 0, 1, 0}, 3)}});

        subject.upload(model::usage_hint::streaming, positionsOnly);

        forget_previous_binds();

        subject.bind(*pShader);

        REQUIRE_FALSE(uv_array_enabled());

        subject.draw();

        REQUIRE(!jfc::glGetError());
    }

    SECTION("and uvs come back when a model that has them is uploaded again")
    {
        model_data positionsOnly({{"a_Position",
            attribute_data({0, 0, 0, 1, 0, 0, 0, 1, 0}, 3)}});

        subject.upload(model::usage_hint::streaming, positionsOnly);
        subject.bind(*pShader);

        subject.upload(model::usage_hint::streaming, model_data::make_quad());

        forget_previous_binds();

        subject.bind(*pShader);

        REQUIRE(uv_array_enabled());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("binding against a second program resolves separately")
    {
        const auto pOther = webgl1es2_shader_program::make_alpha_cutoff();

        subject.bind(*pShader);
        subject.bind(*pOther);
        subject.draw();

        REQUIRE(!jfc::glGetError());
    }
}

namespace {
    struct empty_model_probe final {
        std::shared_ptr<webgl1es2_shader_program> pShader =
            std::make_shared<webgl1es2_shader_program>(R"V0G0N(
            attribute highp vec3 a_Position;

            void main() { gl_Position = vec4(a_Position.xy * 4.0, 0.0, 1.0); }
            )V0G0N", R"V0G0N(
            uniform mediump vec4 _Colour;

            void main() { gl_FragColor = _Colour; }
            )V0G0N");

        std::shared_ptr<webgl1es2_texture_camera> pCamera =
            std::make_shared<webgl1es2_texture_camera>(intvector2_type(4, 4));

        std::shared_ptr<gl_state> pState = std::make_shared<gl_state>();

        empty_model_probe() {
            pCamera->set_perspective_projection(1.0f, 0.1f, 100.0f, 1.0f);
            pCamera->set_transform(vector3_type::zero, quaternion_type::identity);
            pCamera->set_clear_color(color(0, 0, 0, 1));

            static_cast<void>(pCamera->get_color_texture(0));
        }

        ~empty_model_probe() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

        [[nodiscard]] std::shared_ptr<webgl1es2_entity> entity(std::shared_ptr<webgl1es2_model> aModel,
            const color &aColour) {
            auto pMaterial = std::make_shared<webgl1es2_material>(pShader,
                material::face_culling_mode::none, material::render_mode::opaque);

            pMaterial->set_vector4("_Colour", aColour);

            auto pEntity = std::make_shared<webgl1es2_entity>(std::move(aModel), pMaterial);

            pEntity->set_transform({0, 0, -3}, quaternion_type::identity);

            return pEntity;
        }

        [[nodiscard]] int red_drawn_by(const std::shared_ptr<webgl1es2_entity> &aEntity) {
            webgl1es2_scene scene{pState};

            scene.add(aEntity);
            scene.add(pCamera);

            scene.draw({4, 4});

            std::array<std::uint8_t, 4> pixel{};
            glReadPixels(1, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel.data());

            return pixel[0];
        }
    };
}

TEST_CASE("**a model with no vertices draws nothing**", "[gdk::webgl1es2_model]") {
    initGL();

    empty_model_probe probe;

    const auto covering = [] {
        return std::make_shared<webgl1es2_model>(model::usage_hint::upload_once, model_data({
            {"a_Position", {{-0.25f, -0.25f, 0.0f,   0.75f, -0.25f, 0.0f,   -0.25f, 0.75f, 0.0f}, 3}}}));
    };

    const auto pCube = probe.entity(covering(), color(0, 0, 1, 1));

    SECTION("**one that had vertices, and was given none**") {
        auto pModel = covering();

        pModel->upload(model::usage_hint::streaming, model_data());

        const auto pEmptied = probe.entity(pModel, color(1, 0, 0, 1));

        static_cast<void>(probe.red_drawn_by(pCube));

        REQUIRE(probe.red_drawn_by(pEmptied) == 0);
    }

    SECTION("**one the context made empty**") {
        const auto pContext = webgl1es2_context::make();

        const auto pModel = std::static_pointer_cast<webgl1es2_model>(pContext->make_model());

        const auto pEmpty = probe.entity(pModel, color(1, 0, 0, 1));

        static_cast<void>(probe.red_drawn_by(pCube));

        REQUIRE(probe.red_drawn_by(pEmpty) == 0);
    }

    SECTION("**and the probe does see a model that has them**") {
        REQUIRE(probe.red_drawn_by(probe.entity(covering(), color(1, 0, 0, 1))) == 255);
    }
}

