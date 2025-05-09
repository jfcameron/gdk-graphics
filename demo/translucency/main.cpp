// © Joseph Cameron - All Rights Reserved

#include <gdk/game_loop.h>
#include <gdk/graphics_context.h>
#include <gdk/scene.h>
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

vertex_data make_quad() {
    const vertex_data quadData({
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
    return quadData;
}

int main(int argc, char **argv) {
    glfw_window window("per entity and per triangle translucency");

    auto pContext = webgl1es2_context::make();
    auto pScene = pContext->make_scene();

    auto [pBatchModel, batchModelVertexData] = [&]() {
        vertex_data batchModelVertexData;
        return std::make_tuple(
            pContext->make_model(model::usage_hint::write_once, batchModelVertexData),
            std::move(batchModelVertexData)
        );
    }();

    auto pTextureCamera = [&]() {
        auto p(pContext->make_texture_camera());
        p->set_clear_color({1,0.1,0.1,1});
        pScene->add(p);
        return p;
    }();

    auto pCamera = [&]() {
        auto p(pContext->make_camera());
        p->set_clear_color(color::DarkGreen);
        pScene->add(p);
        return p;
    }();

    auto pAlpha = pContext->get_alpha_cutoff_shader();

    auto pUserModel = [&]() {
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

        return pContext->make_model(model::usage_hint::write_once, 
            userdefined_quad_vertex_data);
    }();

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
        return pContext->make_texture(view);
    }();

    auto pMaterial = [&]() {
        auto pMaterial = pContext->make_material(pAlpha, material::render_mode::transparent);
        pMaterial->setTexture("_Texture", pTextureCamera->get_color_texture());
        pMaterial->setVector2("_UVScale", {1, 1});
        pMaterial->setVector2("_UVOffset", {0, 0});
        return pMaterial;
    }();

    auto pEntity = [&]() {
        auto pEntity = pContext->make_entity(pBatchModel, pMaterial);
        pEntity->set_model_matrix(Vector3<float>{2., 0., -11.}, Quaternion<float>());
        pScene->add(pEntity);
        return pEntity;
    }();

    auto pEntity2 = [&]() {
        auto pTexture = [&]() {
            std::vector<std::underlying_type<std::byte>::type> textureData({
                0x55, 0xff, 0xff, 0xff,
                0xff, 0x00, 0xff, 0xff,
                0xff, 0xff, 0x00, 0xff,
                0x00, 0x00, 0x44, 0xff
            });
            texture_data::view view;
            view.width = 2;
            view.height = 2;
            view.format = texture::format::rgba;
            view.data = reinterpret_cast<std::byte *>(&textureData.front());
            return pContext->make_texture(view);
        }();

        auto pMaterial = [&]() {
            auto p = pContext->make_material(pAlpha, material::render_mode::transparent);
            p->setTexture("_Texture", pTexture);
            p->setVector2("_UVScale", {1, 1});
            p->setVector2("_UVOffset", {0, 0});
            return p;
        }();

        auto pEntity = pContext->make_entity(pContext->get_cube_model(), pMaterial);
        pEntity->set_model_matrix({2., 0., -12.5}, {{0, 0, 0}}, {1.0, 1.0, 1});
        pScene->add(pEntity);
        return pEntity;
    }();

    auto pEntity3 = [&]() {    
        auto pMaterial = [&]() {
            auto pMaterial = pContext->make_material(pAlpha);
            pMaterial->setTexture("_Texture", pTexture);
            pMaterial->setVector2("_UVScale", {1, 1});
            pMaterial->setVector2("_UVOffset", {0, 0});
            return pMaterial;
        }();

        auto pEntity(pContext->make_entity(pContext->get_cube_model(), pMaterial));
        pEntity->set_model_matrix({2., 0., -14.5}, {{0, 2, 0.6}}, {6.5, 0.5, 3});
        pScene->add(pEntity);

        return pEntity;
    }();
    
    game_loop(60, [&](const float time, const float deltaTime) {
        glfwPollEvents();

        pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        graphics_mat4x4_type matCamera;
        matCamera.translate({0, 0, -10});
        matCamera.rotate({{0,0,0}});
        pCamera->set_world_matrix(matCamera);

        pEntity->set_model_matrix( {std::cos(time), -0., -11.}, {{0, 4 * ( 1/ 2), 4}});
        pEntity2->set_model_matrix( {2., 0., -12.5}, {{time *0.9f, time *0.5f, 0}}, {1.0, 1.0, 1});
        
        pTextureCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        pTextureCamera->set_world_matrix({std::sin(time), 0, -10}, {});

        vertex_data newData = make_quad();
        newData.transform_position({0.5,0,(float)sin(time)*0.5f},{{0,0,0}},{0.5});

        batchModelVertexData.clear();
        auto quad = make_quad();
        batchModelVertexData.push_back(quad);
        quad.transform_uv({0.2f, 0}, {2.f, 2.f});
        quad.transform_position({0.5,0.5,(float)cos(time)*0.5f},{{0,0,0}},{0.5});
        batchModelVertexData.push_back(quad);
        batchModelVertexData.sort_by_nearest_triangle( {0,0,-20}, graphics_mat4x4_type::Identity);
        pBatchModel->update_vertex_data(model::usage_hint::streaming, batchModelVertexData);

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

