// © Joseph Cameron - All Rights Reserved

#include "voxel_modeler.h"

#include <gdk/game_loop.h>
#include <gdk/graphics_context.h>
#include <gdk/scene.h>
#include <gdk/texture_data.h>
#include <gdk/webgl1es2_context.h>
#include <gdk/webgl1es2_texture.h>

#include <jfc/glfw_window.h>

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
    glfw_window window("Voxel rendering");

    auto pGraphics = webgl1es2_context::make();

    std::cout << "max texture size: " << webgl1es2_texture::getMaxTextureSize() << std::endl;

    auto pScene = pGraphics->make_scene();

    auto pCamera = pGraphics->make_camera();
    pScene->add(pCamera);

    auto pAlpha = pGraphics->get_alpha_cutoff_shader();

    std::vector<std::underlying_type<std::byte>::type> imageData({
        0x00, 0xff, 0xff, 0xff,                                    
        0xff, 0xff, 0xff, 0xff,                                    
        0xff, 0xff, 0xff, 0xff,
        0x00, 0x00, 0x00, 0xff,
    });
    texture_data::view view;
    view.width = 2;
    view.height = 2;
    view.format = texture::format::rgba;
    view.data = reinterpret_cast<std::byte *>(&imageData.front());

    auto pTexture = pGraphics->make_texture(view);
    
    auto pMaterial3 = pGraphics->make_material(pAlpha);
    pMaterial3->setTexture("_Texture", pTexture);
    pMaterial3->setVector2("_UVScale", {1, 1});
    pMaterial3->setVector2("_UVOffset", {0, 0});

    voxel_modeler voxelModeler(pGraphics);

    auto pVoxelModel(pGraphics->make_model());
    
    auto pVoxelEntity = pGraphics->make_entity(pVoxelModel, pMaterial3);
    pScene->add(pVoxelEntity);
    
    for (size_t x(0); x < 8; ++x) for (size_t y(0); y < 8; ++y) {
        voxelModeler.set_voxel_data(x,0,y,1);
    }

    for (size_t x(1); x < 6; ++x) for (size_t z(1); z < 6; ++z) {
        voxelModeler.set_voxel_data(x,1,z,1);
    }

    voxelModeler.set_voxel_data(1,7,0,1);
    voxelModeler.set_voxel_data(1,6,0,1);
    voxelModeler.set_voxel_data(3,7,0,1);
    voxelModeler.set_voxel_data(3,6,0,1);
    voxelModeler.set_voxel_data(0,4,0,1);
    voxelModeler.set_voxel_data(1,3,0,1);
    voxelModeler.set_voxel_data(2,3,0,1);
    voxelModeler.set_voxel_data(3,3,0,1);
    voxelModeler.set_voxel_data(4,4,0,1);

    voxelModeler.set_voxel_data(7,7,7,1);

    voxelModeler.update_vertex_data(); 

    pVoxelModel->update_vertex_data(model::usage_hint::write_once, voxelModeler.vertex_data());

    game_loop(60, [&](const float time, const float deltaTime)
    {
        glfwPollEvents();

        graphics_mat4x4_type root({0,0,0},Quaternion<float>({0,time,0}), {1});
        graphics_mat4x4_type chunkMatrix({-4,0,-4},Quaternion<float>({0,0,0}), {1});
        pVoxelEntity->set_model_matrix(root * chunkMatrix);

        pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        pCamera->set_world_matrix({0, 6, +10}, {0.1,0,0,1});

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

