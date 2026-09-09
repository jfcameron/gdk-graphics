// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include <gdk/graphics/null_context.h>

#include <gdk/graphics/model_data.h>
#include <gdk/graphics/texture_data.h>

using namespace gdk;
using namespace gdk::graphics;

TEST_CASE("the null context needs no window", "[null_context]")
{
    const auto pContext = null_context::make();

    REQUIRE(pContext != nullptr);
    REQUIRE(pContext->resources_made() == 0);

    SECTION("and it hands out every resource the interface promises")
    {
        REQUIRE(pContext->make_scene() != nullptr);
        REQUIRE(pContext->make_camera() != nullptr);
        REQUIRE(pContext->make_texture_camera() != nullptr);
        REQUIRE(pContext->make_model() != nullptr);
        REQUIRE(pContext->make_texture() != nullptr);
        REQUIRE(pContext->make_material(pContext->make_alpha_cutoff_shader(),
            material::render_mode::opaque, material::face_culling_mode::none) != nullptr);

        REQUIRE(pContext->resources_made() == 6);
    }

    SECTION("each call hands back a distinct object, as the interface requires")
    {
        REQUIRE(pContext->make_camera() != pContext->make_camera());
        REQUIRE(pContext->make_scene() != pContext->make_scene());
        REQUIRE(pContext->make_cube_model() != pContext->make_cube_model());
    }
}

TEST_CASE("a null scene remembers what is in it", "[null_context][scene]")
{
    const auto pContext = null_context::make();

    const auto pScene = std::static_pointer_cast<null_scene>(pContext->make_scene());
    const auto pCamera = pContext->make_camera();

    const auto pEntity = pContext->make_entity(pContext->make_model(),
        pContext->make_material(pContext->make_alpha_cutoff_shader(),
            material::render_mode::opaque, material::face_culling_mode::none));

    REQUIRE(pScene->entity_count() == 0);
    REQUIRE(pScene->camera_count() == 0);

    SECTION("adding and removing an entity")
    {
        pScene->add(pEntity);

        REQUIRE(pScene->entity_count() == 1);
        REQUIRE(pScene->contains(pEntity));

        pScene->remove(pEntity);

        REQUIRE(pScene->entity_count() == 0);
        REQUIRE_FALSE(pScene->contains(pEntity));
    }

    SECTION("adding the same entity twice does not double count it")
    {
        pScene->add(pEntity);
        pScene->add(pEntity);

        REQUIRE(pScene->entity_count() == 1);
    }

    SECTION("cameras are counted apart from entities")
    {
        pScene->add(pCamera);
        pScene->add(pContext->make_texture_camera());

        REQUIRE(pScene->camera_count() == 2);
        REQUIRE(pScene->entity_count() == 0);
    }

    SECTION("**drawing is counted, since it is the only sign a frame happened**")
    {
        REQUIRE(pScene->draw_count() == 0);

        pScene->draw({640, 480});

        REQUIRE(pScene->draw_count() == 1);
        REQUIRE(pScene->last_frame_buffer_size().x == 640);
        REQUIRE(pScene->last_frame_buffer_size().y == 480);

        pScene->draw({800, 600});

        REQUIRE(pScene->draw_count() == 2);
        REQUIRE(pScene->last_frame_buffer_size().x == 800);
    }
}

TEST_CASE("a null camera remembers its projection and where it was put", "[null_context][camera]")
{
    const auto pContext = null_context::make();

    const auto pCamera = std::static_pointer_cast<null_camera>(pContext->make_camera());

    SECTION("a perspective projection is kept as it was given")
    {
        pCamera->set_perspective_projection(1.5f, 0.1f, 500.0f, 1.75f);

        REQUIRE(pCamera->field_of_view() == Approx(1.5f));
        REQUIRE(pCamera->near_clip() == Approx(0.1f));
        REQUIRE(pCamera->far_clip() == Approx(500.0f));
        REQUIRE(pCamera->aspect_ratio() == Approx(1.75f));
    }

    SECTION("position and rotation come back exactly")
    {
        const auto rotation = quaternion_type::from_euler({0.25f, 0.5f, 0});

        pCamera->set_transform({1, 2, 3}, rotation);

        REQUIRE(pCamera->position().x == Approx(1));
        REQUIRE(pCamera->position().y == Approx(2));
        REQUIRE(pCamera->position().z == Approx(3));

        REQUIRE(pCamera->rotation().x == Approx(rotation.x));
        REQUIRE(pCamera->rotation().w == Approx(rotation.w));
    }

    SECTION("the clear colour is kept")
    {
        pCamera->set_clear_color(color::cornflower_blue);

        REQUIRE(pCamera->clear_color().r == Approx(color::cornflower_blue.r));

        pCamera->set_clear_mode(camera::clear_mode::depth_only);

        REQUIRE(pCamera->mode() == camera::clear_mode::depth_only);
    }
}

TEST_CASE("a null model remembers the shape of what was uploaded", "[null_context][model]")
{
    const auto pContext = null_context::make();

    model_data data = model_data::make_quad();

    const auto pModel = std::static_pointer_cast<null_model>(
        pContext->make_model(model::usage_hint::upload_once, data));

    REQUIRE(pModel->vertex_count() == data.vertex_count());
    REQUIRE(pModel->upload_count() == 1);

    SECTION("an indexed upload reports its indices, an unindexed one reports none")
    {
        REQUIRE(pModel->index_count() == 0);

        model_data indexed = model_data::make_quad();
        indexed.set_indexes({0, 1, 2});

        pModel->upload(model::usage_hint::upload_once, indexed);

        REQUIRE(pModel->index_count() == 3);
        REQUIRE(pModel->upload_count() == 2);
    }

    SECTION("the built in shapes are empty here, deliberately")
    {
        const auto pCube = std::static_pointer_cast<null_model>(pContext->make_cube_model());

        REQUIRE(pCube->vertex_count() == 0);
    }
}

TEST_CASE("a null material remembers the uniforms it was set", "[null_context][material]")
{
    const auto pContext = null_context::make();

    const auto pMaterial = std::static_pointer_cast<null_material>(
        pContext->make_material(pContext->make_alpha_cutoff_shader(),
            material::render_mode::transparent, material::face_culling_mode::back));

    REQUIRE(pMaterial->uniforms().empty());
    REQUIRE(pMaterial->mode() == material::render_mode::transparent);

    SECTION("a float comes back as a float")
    {
        pMaterial->set_float("_Cutoff", 0.5f);

        REQUIRE(pMaterial->float_at("_Cutoff").has_value());
        REQUIRE(pMaterial->float_at("_Cutoff").value() == Approx(0.5f));
        REQUIRE_FALSE(pMaterial->float_at("_NotSet").has_value());
    }

    SECTION("**a colour is stored in rgba order, matching the real backend**")
    {
        pMaterial->set_vector4("_Tint", color(0.125f, 0.25f, 0.5f, 0.75f));

        const auto stored = pMaterial->vector4_at("_Tint");

        REQUIRE(stored.has_value());
        REQUIRE(stored.value().x == Approx(0.125f));
        REQUIRE(stored.value().y == Approx(0.25f));
        REQUIRE(stored.value().z == Approx(0.5f));
        REQUIRE(stored.value().w == Approx(0.75f));
    }

    SECTION("a texture comes back as the same object")
    {
        const auto pTexture = pContext->make_texture();

        pMaterial->set_texture("_Texture", pTexture);

        REQUIRE(pMaterial->texture_at("_Texture") == pTexture);
        REQUIRE(pMaterial->texture_at("_Absent") == nullptr);
    }

    SECTION("every name that was set is listed, once each")
    {
        pMaterial->set_float("_A", 1);
        pMaterial->set_vector2("_B", {1, 2});
        pMaterial->set_float("_A", 2);

        const auto names = pMaterial->uniforms();

        REQUIRE(names.size() == 2);
        REQUIRE(names.at(0) == "_A");
        REQUIRE(names.at(1) == "_B");
    }
}

TEST_CASE("a null entity remembers its transform and visibility", "[null_context][entity]")
{
    const auto pContext = null_context::make();

    const auto pEntity = std::static_pointer_cast<null_entity>(
        pContext->make_entity(pContext->make_model(),
            pContext->make_material(pContext->make_alpha_cutoff_shader(),
                material::render_mode::opaque, material::face_culling_mode::none)));

    REQUIRE_FALSE(pEntity->is_hidden());

    pEntity->hide();

    REQUIRE(pEntity->is_hidden());

    pEntity->show();

    REQUIRE_FALSE(pEntity->is_hidden());

    SECTION("a transform set as a matrix comes back as that matrix")
    {
        matrix4x4_type expected;
        expected.set_translation({4, 5, 6});

        pEntity->set_transform(expected);

        REQUIRE(pEntity->transform() == expected);
    }

    SECTION("and one set as position, rotation and scale is composed in that order")
    {
        pEntity->set_transform({1, 0, 0}, quaternion_type::identity, vector3_type::one);

        matrix4x4_type expected;
        expected.set_translation({1, 0, 0});

        REQUIRE(pEntity->transform() == expected);
    }
}

TEST_CASE("a null texture remembers its size", "[null_context][texture]")
{
    const auto pContext = null_context::make();

    const std::vector<texture_data::channel_type> pixels(4 * 4 * 4, 0);

    const texture_data::view view{
        .width = 4, .height = 4, .format = texture::format::rgba, .data = pixels.data()};

    const auto pTexture = std::static_pointer_cast<null_texture>(
        pContext->make_texture(view, texture::wrap_mode::repeat, texture::wrap_mode::repeat));

    REQUIRE(pTexture->width() == 4);
    REQUIRE(pTexture->height() == 4);
    REQUIRE(pTexture->upload_count() == 1);

    SECTION("a full update changes the size")
    {
        const std::vector<texture_data::channel_type> bigger(8 * 8 * 4, 0);

        pTexture->update_data({
            .width = 8, .height = 8, .format = texture::format::rgba, .data = bigger.data()});

        REQUIRE(pTexture->width() == 8);
        REQUIRE(pTexture->upload_count() == 2);
    }

    SECTION("a partial update does not")
    {
        pTexture->update_data(view, 1, 1);

        REQUIRE(pTexture->width() == 4);
        REQUIRE(pTexture->upload_count() == 2);
    }
}

TEST_CASE("a camera clips to its viewport until it is told to clip to something else",
    "[null][scissor]") {
    auto pContext = gdk::graphics::null_context::make();

    auto pCamera = std::static_pointer_cast<gdk::graphics::null_camera>(pContext->make_camera());

    REQUIRE(pCamera != nullptr);

    SECTION("by default it is the whole window, which is the whole viewport")
    {
        REQUIRE(pCamera->scissor() == std::array<float, 4>{0, 0, 1, 1});
    }

    SECTION("a viewport is what it clips to, without anything being asked for")
    {
        pCamera->set_viewport(0.5f, 0.0f, 0.5f, 1.0f);

        REQUIRE(pCamera->scissor() == std::array<float, 4>{0.5f, 0, 0.5f, 1});
    }

    SECTION("and a scissor overrides that without moving the viewport")
    {
        pCamera->set_viewport(0.0f, 0.0f, 1.0f, 1.0f);
        pCamera->set_scissor(0.25f, 0.25f, 0.5f, 0.5f);

        REQUIRE(pCamera->scissor() == std::array<float, 4>{0.25f, 0.25f, 0.5f, 0.5f});
    }

    SECTION("clearing it goes back to the viewport rather than to the whole window")
    {
        pCamera->set_viewport(0.0f, 0.5f, 1.0f, 0.5f);
        pCamera->set_scissor(0.0f, 0.0f, 0.1f, 0.1f);

        pCamera->clear_scissor();

        REQUIRE(pCamera->scissor() == std::array<float, 4>{0, 0.5f, 1, 0.5f});
    }
}
