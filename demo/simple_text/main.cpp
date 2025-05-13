// © Joseph Cameron - All Rights Reserved

#include "text_renderer.h"

#include <gdk/game_loop.h>
#include <gdk/graphics_context.h>
#include <gdk/webgl1es2_context.h>
#include <gdk/scene.h>

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

int main(int argc, char **argv)
{
    glfw_window window("basic rendering demo");

    auto pContext = webgl1es2_context::make();

    auto pScene = pContext->make_scene();

    auto pCamera = pContext->make_camera();
    pCamera->set_clear_color(color::dark_green);
    pScene->add(pCamera);

    text_renderer textRenderer(pContext);
    auto pTextModel(pContext->make_model());
    
    auto pTextEntity(pContext->make_entity(pTextModel, textRenderer.material()));
    pScene->add(pTextEntity);
    pTextEntity->set_model_matrix(vector3<float>{-2., 0., -11}, 
        quaternion<float>{{0, 0, 0.0}},
        {0.2, 0.2, 0.2});

    auto pTextEntity2(pContext->make_entity(pTextModel, textRenderer.material()));
    pScene->add(pTextEntity2);
    pTextEntity2->set_model_matrix(vector3<float>{-2., 2., -12}, 
        quaternion<float>{{0, 0, 0.0}},
        {0.2, 0.2, 0.2});

    game_loop(60, [&](const float time, const float deltaTime)
    {
        glfwPollEvents();

        textRenderer.set_text("this is not a test,\n\tthis is rock and roll!\n____<blar>\ntime: " + std::to_string((int)time));
        pTextModel->upload_vertex_data(model::usage_hint::streaming, textRenderer.vertex_data());

        pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        pCamera->set_transform({std::sin(time), 0, -10}, {});
        
        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

