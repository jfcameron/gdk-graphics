// © Joseph Cameron - All Rights Reserved

#include <embedded_font.h>

#include <gdk/graphics/ext/font.h>
#include <gdk/graphics/ext/text_modeler.h>

#include <gdk/timing/game_loop.h>
#include <gdk/windowing/impl_glfw_window.h>
#include <gdk/graphics/context.h>
#include <gdk/graphics/scene.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/webgl1es2_context.h>

#include <jfc/events/event.h>

#include <GLFW/glfw3.h>

#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <span>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

using namespace gdk;

using namespace gdk::graphics;
using namespace gdk::timing;

int main() {
    const auto pWindow = windowing::impl_glfw_window::make("basic rendering demo");

    jfc::events::event<float, float> update_event;

    const auto pGraphics = webgl1es2_context::make();
    const auto pScene = pGraphics->make_scene();

    const auto pCamera = [&]() {
        auto pCamera = pGraphics->make_camera();
        pCamera->set_clear_color(color::black);
        pScene->add(pCamera);
        update_event.subscribe([pCamera, &pWindow]([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) {
            pCamera->set_perspective_projection(to_radians(90.0f), 0.01f, 20, static_cast<floating_point_type>(pWindow->aspect_ratio()));
            pCamera->set_transform({});
        });
        return pCamera;
    }();

    auto pFont = std::make_shared<gdk::graphics::ext::font>(
        std::span<const gdk::graphics::texture_data::encoded_byte>(
            EMBEDDED_FONT, sizeof(EMBEDDED_FONT)),
        48.f);

    gdk::graphics::ext::text_modeler textModeler(pGraphics, pFont);
    update_event.subscribe([&]([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) {
        textModeler.set_text("This is NOT a test,\nthis is Rock & Roll!\ntime: "
            + std::to_string((int)time));
        textModeler.upload();
    });

    const auto [pBackgroundTextEntity] = [&]() {
        auto pBackgroundTextEntity(pGraphics->make_entity(textModeler.model(), textModeler.material()));
        pScene->add(pBackgroundTextEntity);
        update_event.subscribe([pBackgroundTextEntity]([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) {
            pBackgroundTextEntity->set_transform({-0., +0., -3},
                quaternion_type::from_euler({0, 0, 1.f / 2 * std::cos(time)}), {0.2f, 0.2f, 0.2f});
        });
        return std::tuple(pBackgroundTextEntity);
    }();

    auto pForegroundTextEntity = [&]() {
        auto pForegroundTextEntity(pGraphics->make_entity(textModeler.model(), textModeler.material()));
        pScene->add(pForegroundTextEntity);
        pForegroundTextEntity->set_transform({-2., +1., -1}, quaternion_type::identity,
            {0.2f, 0.2f, 0.2f});
        return pForegroundTextEntity;
    }();

    const auto pBackgroundEntity = [&]() {
        static const std::vector<texture_data::encoded_byte> background { 
            0x2e, 0x2e, 0x2e, 0xff, 0x22, 0x22, 0x22, 0xff,                                    
            0x22, 0x22, 0x22, 0xff, 0x2e, 0x2e, 0x2e, 0xff, 
        };
        gdk::graphics::texture_data::view view;
        view.format = gdk::graphics::texture::format::rgba;
        view.height = 2;
        view.width = 2;
        view.data = &background.front();
        const auto pTexture = pGraphics->make_texture(view);

        const auto pMaterial = pGraphics->make_material(pGraphics->make_alpha_cutoff_shader());
        pMaterial->set_texture("_Texture", pTexture);
        pMaterial->set_vector2("_UVScale", {4});
        pMaterial->set_vector2("_UVOffset", {0.0, 0});

        const auto pModel = [&]() {
            auto quadData = model_data::make_quad();
            quadData.transform("a_Position", {-0.5f, -0.5f, 0.f});
            return pGraphics->make_model(gdk::graphics::model::usage_hint::upload_once, quadData);
        }();

        const auto pEntity = pGraphics->make_entity(pModel, pMaterial);
        pEntity->set_transform({0, 0, -4}, quaternion_type::identity,
            vector3_type(25));
        pScene->add(pEntity);

        update_event.subscribe([pMaterial]([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) {
            pMaterial->set_vector2("_UVOffset", {time * 1.f / 12, time * 1.f / 24});
        });

        return pEntity;
    }();

    game_loop(frames_per_second{60}, [&](const game_loop::frame aFrame) {
        const auto time = static_cast<float>(aFrame.elapsed);
        const auto deltaTime = static_cast<float>(aFrame.delta);
        windowing::impl_glfw_window::poll_events();
        update_event.notify(time, deltaTime);
        pScene->draw(pWindow->window_size());
        pWindow->swap_buffers(); 
        return pWindow->should_close();
    }).run();

    return EXIT_SUCCESS;
}

