// © Joseph Cameron - All Rights Reserved

#include <gdk/game_loop.h>
#include <gdk/graphics_context.h>
#include <gdk/texture_data.h>
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

    auto pContext = webgl1es2_context::make();
    auto pScene = pContext->make_scene();

    const auto pTextureCamera = [&]() {
        auto p(pContext->make_texture_camera());
        p->set_clear_color({1,0.1,0.1,1});
        pScene->add(p);
        return p;
    }();

    auto pCamera = [&]() {
        auto p(pContext->make_camera());
        p->set_clear_color(color::CornflowerBlue);
        pScene->add(p);
        return p;
    }();

    auto pShader = pContext->get_alpha_cutoff_shader();

    auto pUserModel = pContext->make_model();

    auto pTexture = [&]() {
        std::vector<std::underlying_type<std::byte>::type> imageData({
            0x00, 0xff, 0xff, 0xff,                                    
            0xff, 0xff, 0xff, 0xff,                                    
            0xff, 0xff, 0xff, 0xff,
            0x00, 0x00, 0x00, 0xff
        });
        texture_data::view view;
        view.width = 2;
        view.height = 2;
        view.format = texture::format::rgba;
        view.data = reinterpret_cast<std::byte *>(&imageData.front());
        auto p = pContext->make_texture(view);
        return p;
    }();

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

    auto pMaterial = [&]() {
        auto p(pContext->make_material(pShader, material::render_mode::transparent));
        p->setTexture("_Texture", pTextureCamera->get_color_texture());
        p->setVector2("_UVScale", {1, 1});
        p->setVector2("_UVOffset", {0, 0});
        return p;
    }();

    auto pBatchModel = pContext->make_model(model::usage_hint::write_once, batchVertexData);
    auto pEntity = pContext->make_entity(pBatchModel, pMaterial);
    pEntity->set_model_matrix(Vector3<float>{2., 0., -11.}, Quaternion<float>());
    pScene->add(pEntity);

    vertex_data newData(batchVertexData);
    newData.transform_uv({0.2f, 0}, {2.f, 2.f});
    newData.transform_position({1,0,0},{},{1});
    vertex_data buffer(batchVertexData);
    auto indexToSpinner = buffer.push_back(newData);
    
    game_loop(60, [&](const float time, const float deltaTime) {
        glfwPollEvents();

        pEntity->set_model_matrix(Vector3<float>{std::cos(time), -0., -11.}, 
            Quaternion<float>{ {0, 4 * ( 1/ 2), 4}});

        pCamera->set_perspective_projection(90, 0.01, 40, window.getAspectRatio());
        pCamera->set_world_matrix({std::sin(time*0.2f), 0, -10}, {});
        
        vertex_data newNewData(newData);
        newNewData.transform_position({0,0,0},{{time,0,0}},{1});
        buffer.overwrite("a_Position", indexToSpinner, newNewData);
        pBatchModel->update_vertex_data(model::usage_hint::streaming, buffer);

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

