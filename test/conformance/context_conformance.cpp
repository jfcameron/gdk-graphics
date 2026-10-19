// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include "conformance.h"

#include <gdk/graphics/camera.h>
#include <gdk/graphics/entity.h>
#include <gdk/graphics/material.h>
#include <gdk/graphics/model.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/scene.h>
#include <gdk/graphics/screen_camera.h>
#include <gdk/graphics/texture.h>
#include <gdk/graphics/texture_camera.h>
#include <gdk/graphics/texture_data.h>

#include <vector>

using namespace gdk::graphics;
using gdk::graphics::conformance::implementation;
using gdk::graphics::conformance::make_context;

namespace {
    //! four texels by four, opaque white, for a texture 
    const std::vector<texture_data::channel_type> WHITE(4 * 4 * 4, 0xFF);

    [[nodiscard]] texture_data::view white_view() {
        return {.width = 4, .height = 4, .format = texture::format::rgba, .data = WHITE.data()};
    }
}

TEST_CASE("conformance, every factory makes what it promises", "[conformance]") {
    INFO("implementation " << implementation());

    const auto pContext = make_context();

    REQUIRE(pContext);

    REQUIRE(pContext->make_scene());
    REQUIRE(pContext->make_camera());
    REQUIRE(pContext->make_model());
    REQUIRE(pContext->make_model(model::usage_hint::upload_once, model_data::make_quad()));
    REQUIRE(pContext->make_texture());
    REQUIRE(pContext->make_texture(white_view()));

    const auto pShader = pContext->make_alpha_cutoff_shader();

    REQUIRE(pShader);
    REQUIRE(pContext->make_alpha_blend_shader());

    const auto pMaterial = pContext->make_material(pShader, material::render_mode::transparent,
        material::face_culling_mode::back);

    REQUIRE(pMaterial);
    REQUIRE(pContext->make_material(pMaterial));

    REQUIRE(pContext->make_cube_model());
    REQUIRE(pContext->make_sphere_model());

    REQUIRE(pContext->make_entity(pContext->make_cube_model(), pMaterial));

    SECTION("a texture camera has a colour texture to be drawn from, at its default size and at one given") {
        const auto pDefault = pContext->make_texture_camera();

        REQUIRE(pDefault);
        REQUIRE(pDefault->get_color_texture());

        const auto pSized = pContext->make_texture_camera({64, 32});

        REQUIRE(pSized);
        REQUIRE(pSized->get_color_texture());
    }
}

TEST_CASE("conformance, an entity is hidden and shown again", "[conformance][entity]") {
    INFO("implementation " << implementation());

    const auto pContext = make_context();

    const auto pEntity = pContext->make_entity(pContext->make_cube_model(),
        pContext->make_material(pContext->make_alpha_cutoff_shader()));

    REQUIRE_FALSE(pEntity->is_hidden());

    pEntity->hide();

    REQUIRE(pEntity->is_hidden());

    pEntity->show();

    REQUIRE_FALSE(pEntity->is_hidden());

    SECTION("and placed, and given bounds, whether shown or not") {
        pEntity->set_transform({1, 2, 3}, quaternion_type::identity, {1, 1, 1});
        pEntity->set_bounds({0, 0, 0}, 2);
        pEntity->hide();
        pEntity->set_transform({4, 5, 6}, quaternion_type::identity, {2, 2, 2});

        REQUIRE(pEntity->is_hidden());
    }
}

TEST_CASE("conformance, what is made is filled in again", "[conformance]") {
    INFO("implementation " << implementation());

    const auto pContext = make_context();

    SECTION("a model takes new vertices, indexed or not") {
        const auto pModel = pContext->make_model();

        pModel->upload(model::usage_hint::dynamic, model_data::make_quad());

        auto indexed = model_data::make_quad();

        indexed.set_indexes({0, 1, 2});

        pModel->upload(model::usage_hint::dynamic, indexed);
    }

    SECTION("a texture takes new texels, whole and a rectangle at a time") {
        const auto pTexture = pContext->make_texture(white_view(), texture::wrap_mode::clamped,
            texture::wrap_mode::clamped);

        pTexture->update_data(white_view());

        const std::vector<texture_data::channel_type> corner(2 * 2 * 4, 0x80);

        pTexture->update_data({.width = 2, .height = 2, .format = texture::format::rgba, .data = corner.data()}, 2, 2);
    }

    SECTION("a material takes every kind of uniform, and a texture") {
        const auto pMaterial = pContext->make_material(pContext->make_alpha_blend_shader());

        pMaterial->set_texture("_Texture", pContext->make_texture(white_view()));
        pMaterial->set_float("_Float", 1);
        pMaterial->set_vector2("_Vector2", {1, 2});
        pMaterial->set_vector3("_Vector3", {1, 2, 3});
        pMaterial->set_vector4("_Color", vector4_type{1, 1, 1, 0.5f});
        pMaterial->set_integer("_Integer", 1);
        pMaterial->set_integer2("_Integer2", 1, 2);
        pMaterial->set_integer3("_Integer3", 1, 2, 3);
        pMaterial->set_integer4("_Integer4", 1, 2, 3, 4);
    }
}

TEST_CASE("conformance, a scene draws what it holds, and stops when it is taken away", "[conformance][scene]") {
    INFO("implementation " << implementation());

    const auto pContext = make_context();
    const auto pScene = pContext->make_scene();

    const auto pCamera = pContext->make_camera();

    pCamera->set_perspective_projection(90, 0.01f, 100, 1);
    pCamera->set_transform({0, 0, 5}, quaternion_type::identity);

    const auto pMaterial = pContext->make_material(pContext->make_alpha_cutoff_shader());

    pMaterial->set_texture("_Texture", pContext->make_texture(white_view()));

    const auto pEntity = pContext->make_entity(pContext->make_cube_model(), pMaterial);

    pScene->add(pCamera);
    pScene->add(pEntity);

    pScene->set_float("_Time", 1);

    pScene->draw({64, 64});

    SECTION("a hidden entity, and the scene again") {
        pEntity->hide();

        pScene->draw({64, 64});
    }

    SECTION("taken away, entity and camera, and the scene again") {
        pScene->remove(pEntity);
        pScene->remove(pCamera);

        pScene->draw({64, 64});
    }

    SECTION("a texture camera beside the screen's") {
        const auto pTarget = pContext->make_texture_camera({32, 32});

        pScene->add(pTarget);
        pScene->draw({64, 64});
        pScene->remove(pTarget);
    }
}
