// © Joseph Cameron - All Rights Reserved

#include "./voxel_modeler.h"

#include <gdk/graphics_context.h>
#include <gdk/scene.h>

#include <gdk/ext/vertex_batch.h>

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

#include <iostream>

using namespace gdk;

using namespace gdk::graphics::ext;

#include <jfc/contiguous_view.h>
#include<array>
#include<vector>

int main(int argc, char **argv)
{
    std::vector<int> data{1,2,3,4,5,6,7,8};

    jfc::contiguous_view<const int> data_view(data);

    for (auto value : data_view) std::cout << value << "\n";

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

    float time(0);

    voxel_modeler voxelModeler(pContext);
    
    auto pVoxelEntity = pContext->make_entity(voxelModeler.model(), pMaterial3);
    pScene->add(pVoxelEntity);
    
    for (size_t i(0); i < 8; ++i)
    {
        voxelModeler.set_voxel_data(i,0,0,1);
        voxelModeler.set_voxel_data(0,i,0,1);
        voxelModeler.set_voxel_data(0,0,i,1);
        
        voxelModeler.set_voxel_data(i,i,i,1);
    }

    voxelModeler.update_model();

    for (float deltaTime(0); !window.shouldClose();)
    {
        using namespace std::chrono;

        steady_clock::time_point t1(steady_clock::now());

        glfwPollEvents();

        pVoxelEntity->set_model_matrix(Vector3<float>{-0., 0., -15}, Quaternion<float>{{time, 2 * (time / 2), 4}});

        pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        pCamera->set_view_matrix({std::sin(time), 0, -10}, {});

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        time += 0.01;

        while (true)
        {
            std::this_thread::sleep_for(10ms);
    
            steady_clock::time_point t2(steady_clock::now());

            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

            if (deltaTime = time_span.count(); deltaTime > 0.01666667) break;
        }
    }

    return EXIT_SUCCESS;
}

