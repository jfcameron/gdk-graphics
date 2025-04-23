// © Joseph Cameron - All Rights Reserved

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
    // Separate lib, used to init GL and get a window ready for rendering on Linux/Mac/Windows
    glfw_window window("per entity and per triangle translucency");

    // Init a gles2/webgl1 context
    auto pContext = webgl1es2_context::make();

    auto pScene = pContext->make_scene();

    const vertex_data batchVertexData({
        { 
            "a_Position",
            {
                {
                    1.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    1.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, 0.0f,
                },
                3
            }
        },
        { 
            "a_UV",
            {
                {
                    1, 0,
                    0, 0,
                    0, 1,
                    1, 0,
                    0, 1,
                    1, 1,
                },
                2
            }
        }
    });

    auto pTextureCamera = pContext->make_texture_camera();
    pTextureCamera->set_clear_color({1,0.1,0.1,1});
    pScene->add(pTextureCamera);

    auto pCamera = pContext->make_camera();
    pCamera->set_clear_color(color::DarkGreen);
    pScene->add(pCamera);

    auto pAlpha = pContext->get_alpha_cutoff_shader();

    float size(1);
    decltype(size) hsize = size/2.;

    const std::vector<float> posData({
        size -hsize, size -hsize, 0.0f,
        0.0f -hsize, size -hsize, 0.0f,
        0.0f -hsize, 0.0f -hsize, 0.0f,
        size -hsize, size -hsize, 0.0f,
        0.0f -hsize, 0.0f -hsize, 0.0f,
        size -hsize, 0.0f -hsize, 0.0f
    });

    const std::vector<float> uvData({
        1, 0,
        0, 0,
        0, 1,
        1, 0,
        0, 1,
        1, 1
    });

    const vertex_data userdefined_quad_vertex_data(
    {
        { 
            "a_Position",
            {
                posData,
                3
            }
        },
        { 
            "a_UV",
            {
                uvData,
                2
            }
        }
    });

    auto pUserModel = pContext->make_model(model::UsageHint::Static, userdefined_quad_vertex_data);

    std::vector<std::underlying_type<std::byte>::type> imageData({
        0x00, 0xff, 0xff, 0xff,                                    
        0xff, 0xff, 0xff, 0xff,                                    
        0xff, 0xff, 0xff, 0xff,
        0x00, 0x00, 0x00, 0xff
    });
    image_data_2d_view view;
    view.width = 2;
    view.height = 2;
    view.format = texture::data_format::rgba;
    view.data = reinterpret_cast<std::byte *>(&imageData.front());
    auto pTexture = pContext->make_texture(view);

    auto pMaterial = pContext->make_material(pAlpha, material::render_mode::transparent);
    pMaterial->setTexture("_Texture", pTextureCamera->get_color_texture());
    pMaterial->setVector2("_UVScale", {1, 1});
    pMaterial->setVector2("_UVOffset", {0, 0});

    auto pBatchModel = pContext->make_model(model::UsageHint::Static, batchVertexData);
    auto pEntity = pContext->make_entity(pBatchModel, pMaterial);
    pEntity->set_model_matrix(Vector3<float>{2., 0., -11.}, Quaternion<float>());
    pScene->add(pEntity);

    std::vector<std::underlying_type<std::byte>::type> imageData2({
        0x55, 0xff, 0xff, 0xff,
        0xff, 0x00, 0xff, 0xff,
        0xff, 0xff, 0x00, 0xff,
        0x00, 0x00, 0x44, 0xff
    });
    image_data_2d_view view2;
    view2.width = 2;
    view2.height = 2;
    view2.format = texture::data_format::rgba;
    view2.data = reinterpret_cast<std::byte *>(&imageData2.front());
    auto pTexture2 = pContext->make_texture(view2);

    auto pMaterial2 = pContext->make_material(pAlpha, material::render_mode::transparent);
    pMaterial2->setTexture("_Texture", pTexture2);
    pMaterial2->setVector2("_UVScale", {1, 1});
    pMaterial2->setVector2("_UVOffset", {0, 0});
    
    auto pEntity2 = std::shared_ptr<entity>(
        pContext->make_entity(pContext->get_cube_model(), pMaterial2));
    pEntity2->set_model_matrix(Vector3<float>{2., 0., -12.5}, 
        Quaternion<float>{{0, 0, 0}},
        {1.0, 1.0, 1});
    pScene->add(pEntity2);

    auto pMaterial3 = pContext->make_material(pAlpha);
    pMaterial3->setTexture("_Texture", pTexture);
    pMaterial3->setVector2("_UVScale", {1, 1});
    pMaterial3->setVector2("_UVOffset", {0, 0});
    
    auto pEntity3 = std::shared_ptr<entity>(
        pContext->make_entity(pContext->get_cube_model(), pMaterial3));
    pEntity3->set_model_matrix(Vector3<float>{2., 0., -14.5}, 
        Quaternion<float>{{0, 2, 0.6}},
        {6.5, 0.5, 3});
    pScene->add(pEntity3);
    
    vertex_data newData(batchVertexData);
    newData.transform_uv({0.2f, 0}, {2.f, 2.f});
    newData.transform_position({1,0,0},{},{1});
    vertex_data buffer(batchVertexData);
    auto indexToSpinner = buffer.push_back(newData);
    
    game_loop(60, [&](const float time, const float deltaTime)
    {
        glfwPollEvents();

        pEntity2->set_model_matrix(Vector3<float>{2., 0., -12.5}, Quaternion<float>{{time *0.9f, time *0.5f, 0}},
            {1.0, 1.0, 1});

        pEntity->set_model_matrix(Vector3<float>{std::cos(time), -0., -11.}, Quaternion<float>{ {0, 4 * ( 1/ 2), 4}});

        pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        pCamera->set_world_matrix({std::sin(time), 0, -10}, {});
        
        pTextureCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        pTextureCamera->set_world_matrix({std::sin(time), 0, -10}, {});

        vertex_data newNewData(newData);
        newNewData.transform_position({0,0,0},{{time,0,0}},{1});
        buffer.overwrite("a_Position", indexToSpinner, newNewData);
        pBatchModel->update_vertex_data(model::UsageHint::Streaming,
            buffer);

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

