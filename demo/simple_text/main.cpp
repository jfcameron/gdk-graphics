// © Joseph Cameron - All Rights Reserved

#include "text_modeler.h"

#include <gdk/game_loop.h>
#include <gdk/graphics_context.h>
#include <gdk/scene.h>
#include <gdk/webgl1es2_context.h>

#include <jfc/glfw_window.h>

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

    auto pGraphics = webgl1es2_context::make();

    auto pScene = pGraphics->make_scene();

    auto pCamera = [&]() {
        auto pCamera = pGraphics->make_camera();
        pCamera->set_clear_color(color::dark_green);
        pScene->add(pCamera);
        return pCamera;
    }();

    text_modeler textModeler(pGraphics);

    const auto pTextEntity = [&]() {
        auto pTextEntity(pGraphics->make_entity(textModeler.model(), textModeler.material()));
        pScene->add(pTextEntity);
        pTextEntity->set_transform({-2., 0., -11}, {{0, 0, 0.0}}, {0.2, 0.2, 0.2});
        return pTextEntity;
    }();

    auto pTextEntity2 = [&]() {
        auto pTextEntity2(pGraphics->make_entity(textModeler.model(), textModeler.material()));
        pScene->add(pTextEntity2);
        pTextEntity2->set_transform({-2., 2., -12}, {{0, 0, 0.0}}, {0.2, 0.2, 0.2});
        return pTextEntity2;
    }();

    game_loop(60, [&](const float time, const float deltaTime) {
        glfwPollEvents();

        textModeler.set_text("this is not a test,\n\tthis is rock and roll!\n<blar>\ntime: " + std::to_string((int)time));
        textModeler.upload();

        pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        pCamera->set_transform({std::sin(time), 0, -10}, {});
        
        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

