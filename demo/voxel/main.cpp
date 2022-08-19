// © Joseph Cameron - All Rights Reserved

#include "voxel_modeler.h"

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
using namespace gdk::graphics::ext;

int main(int argc, char **argv)
{
    glfw_window window("basic rendering demo");

    auto pContext = graphics::context::make(
        graphics::context::implementation::opengl_webgl1_gles2);

    auto pScene = pContext->make_scene();

    auto pCamera = pContext->make_camera();
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
    
    auto pMaterial3 = pContext->make_material(pAlpha);
    pMaterial3->setTexture("_Texture", pTexture);
    pMaterial3->setVector2("_UVScale", {1, 1});
    pMaterial3->setVector2("_UVOffset", {0, 0});

    voxel_modeler voxelModeler(pContext);

    auto pVoxelModel(pContext->make_model());
    
    auto pVoxelEntity = pContext->make_entity(pVoxelModel, pMaterial3);
    pScene->add(pVoxelEntity);
    
    for (size_t i(0); i < 8; ++i)
    {
        voxelModeler.set_voxel_data(i,0,0,1);
        voxelModeler.set_voxel_data(0,i,0,1);
        voxelModeler.set_voxel_data(0,0,i,1);
        
        voxelModeler.set_voxel_data(i,i,i,1);
    }

    voxelModeler.update_vertex_data(); 

    pVoxelModel->update_vertex_data(model::UsageHint::Streaming, voxelModeler.vertex_data());

    game_loop(60, [&](const float time, const float deltaTime)
    {
        glfwPollEvents();

        pVoxelEntity->set_model_matrix(Vector3<float>{-0., 0., -20}, 
            Quaternion<float>{{time, 2 * (time / 2), 4}});

        pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        pCamera->set_world_matrix({std::sin(time), 0, -10}, {});

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

