// © Joseph Cameron - All Rights Reserved

#include "text_modeler.h"

#include <gdk/game_loop.h>
#include <gdk/graphics_context.h>
#include <gdk/scene.h>
#include <gdk/texture_data.h>
#include <gdk/webgl1es2_context.h>

#include <jfc/glfw_window.h>
#include <jfc/to_array.h>
#include <jfc/event.h>

#include <GLFW/glfw3.h>

#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

using namespace gdk;

int main(int argc, char **argv) {
    glfw_window window("basic rendering demo");

    jfc::event<float, float> update_event;

    const auto pGraphics = webgl1es2_context::make();
    const auto pScene = pGraphics->make_scene();

    const auto pCamera = [&]() {
        auto pCamera = pGraphics->make_camera();
        pCamera->set_clear_color(color::black);
        pScene->add(pCamera);
        update_event.subscribe([pCamera, &window](float time, float deltaTime) {
            pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
            pCamera->set_transform({});
        });
        return pCamera;
    }();

    text_modeler textModeler(pGraphics);
    update_event.subscribe([&](float time, float deltaTime) {
        textModeler.set_text("this is not a test,\nthis is rock and roll!\n<blar>\ntime: " + std::to_string((int)time));
        textModeler.upload();
    });

    const auto [pBackgroundTextEntity] = [&]() {
        auto pBackgroundTextEntity(pGraphics->make_entity(textModeler.model(), textModeler.material()));
        pScene->add(pBackgroundTextEntity);
        update_event.subscribe([pBackgroundTextEntity](float time, float deltaTime) {
            pBackgroundTextEntity->set_transform({-0., +0., -3}, {{0, 0, 1.f / 2 * std::cos(time)}}, {0.2, 0.2, 0.2});
        });
        return std::tuple(pBackgroundTextEntity);
    }();

    auto pForegroundTextEntity = [&]() {
        auto pForegroundTextEntity(pGraphics->make_entity(textModeler.model(), textModeler.material()));
        pScene->add(pForegroundTextEntity);
        pForegroundTextEntity->set_transform({-2., +1., -1}, {{0, 0, 0.0}}, {0.2, 0.2, 0.2});
        return pForegroundTextEntity;
    }();

    [&]() {
        static const auto background = jfc::to_array<texture_data::channel_type>({
            0x2e, 0x2e, 0x2e, 0xff, 0x22, 0x22, 0x22, 0xff,                                    
            0x22, 0x22, 0x22, 0xff, 0x2e, 0x2e, 0x2e, 0xff, 
        });
        gdk::texture_data::view view;
        view.format = gdk::texture::format::rgba;
        view.height = 2;
        view.width = 2;
        view.data = &background.front();
        const auto pTexture = pGraphics->make_texture(view);

        const auto pMaterial = pGraphics->make_material(pGraphics->get_alpha_cutoff_shader());
        pMaterial->setTexture("_Texture", pTexture);
        pMaterial->setVector2("_UVScale", {4});
        pMaterial->setVector2("_UVOffset", {0.0, 0});

        const auto pModel = pGraphics->get_quad_model(); //TODO: remove get_quad_model, move it to model_data

        const auto pEntity = pGraphics->make_entity(pModel, pMaterial);
        pEntity->set_transform({0,0,-4},{{0,0,0}},{25});
        pScene->add(pEntity);

        update_event.subscribe([pMaterial](float time, float deltaTime){
            pMaterial->setVector2("_UVOffset", {time * 1.f / 12, time * 1.f / 24});
        });
    }();

    game_loop(60, [&](const float time, const float deltaTime) {
        glfwPollEvents();
        update_event.notify(time, deltaTime);
        pScene->draw(window.getWindowSize());
        window.swapBuffer(); 
        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

