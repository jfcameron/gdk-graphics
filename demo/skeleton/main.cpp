// © Joseph Cameron - All Rights Reserved

#include "skeleton_model.h"

#include <gdk/game_loop.h>
#include <gdk/graphics_context.h>
#include <gdk/scene.h>

#include <jfc/glfw_window.h>
#include <jfc/contiguous_view.h>

#include <GLFW/glfw3.h>

#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iostream>
#include <map>
#include <set>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace gdk;

int main(int argc, char **argv)
{
    glfw_window window("basic rendering demo");

    auto pContext = graphics::context::make(
        graphics::context::implementation::opengl_webgl1_gles2);

    auto pScene = pContext->make_scene();

    auto pCamera = pContext->make_camera();
    pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
    pCamera->set_world_matrix({0, 0, -10}, {});
    pScene->add(pCamera);

    auto pAlpha = pContext->get_alpha_cutoff_shader();

    std::vector<std::underlying_type<std::byte>::type> imageData({
        0x00, 0xff, 0xff, 0xff,                                    
        0xff, 0xff, 0xff, 0xff,                                    
        0xff, 0xff, 0xff, 0xff,
        0x00, 0x00, 0x00, 0xff,
    });
    image_data_2d_view view;
    view.width = 2;
    view.height = 2;
    view.format = texture::data_format::rgba;
    view.data = reinterpret_cast<std::byte *>(&imageData.front());
    auto pTexture = pContext->make_texture(view);
    
    auto pMaterial = pContext->make_material(pAlpha);
    pMaterial->setTexture("_Texture", pTexture);
    pMaterial->setVector2("_UVScale", {1, 1});
    pMaterial->setVector2("_UVOffset", {0, 0});

    auto pEntity = std::shared_ptr<entity>(
        pContext->make_entity(pContext->get_cube_model(), pMaterial));
    pEntity->set_model_matrix(Vector3<float>{2., 0., -12.5}, 
        Quaternion<float>{{0, 0, 0}},
        {1.0, 1.0, 1});
    pScene->add(pEntity);

    game_loop(60, [&](const float time, const float deltaTime)
    {
        glfwPollEvents();

        pScene->draw(window.getWindowSize());
        
        pEntity->set_model_matrix(Vector3<float>{2., 0., -12.5}, 
            Quaternion<float>{{time, 0, 0}},
            {1.0, 1.0, 1});
        
        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

