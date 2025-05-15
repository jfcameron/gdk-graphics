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

model_data make_quad() {
    const model_data quadData({
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
        model_data batchModelVertexData;
        return std::make_tuple(
            pContext->make_model(model::usage_hint::upload_once_, batchModelVertexData),
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
        p->set_clear_color(color::dark_green);
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

        const model_data userdefined_quad_model_data({
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

        return pContext->make_model(model::usage_hint::upload_once_, 
            userdefined_quad_model_data);
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
        view.data = &imageData.front();
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
        pEntity->set_transform(vector3<float>{2., 0., -11.}, quaternion<float>());
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
            view.data = &textureData.front();
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
        pEntity->set_transform({2., 0., -12.5}, {{0, 0, 0}}, {1.0, 1.0, 1});
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
        pEntity->set_transform({2., 0., -14.5}, {{0, 2, 0.6}}, {6.5, 0.5, 3});
        pScene->add(pEntity);

        return pEntity;
    }();
    
    game_loop(60, [&](const float time, const float deltaTime) {
        glfwPollEvents();

        pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        graphics_matrix4x4_type matCamera;
        matCamera.set_translation({0, 0, -10});
        matCamera.set_rotation({{0,0,0}});
        pCamera->set_transform(matCamera);

        pEntity->set_transform( {std::cos(time), -0., -11.}, {{0, 4 * ( 1/ 2), 4}});
        pEntity2->set_transform( {2., std::sin(time) * 2.f, -12.5}, {{time *0.9f, time *0.5f, 0}}, {1.0, 1.0, 1});
        
        pTextureCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        pTextureCamera->set_transform({std::sinf(time), 0, -10}, {});

        model_data newData = make_quad();
        newData.transform("a_Position", 
            {0.5,0,(float)sin(time)*0.5f}, {{0,0,0}}, {0.5});

        batchModelVertexData.clear();
        auto quad = make_quad();
        batchModelVertexData.push_back(quad);

        graphics_matrix4x4_type quadMat; {
            graphics_vector3_type tran(std::cos(time*0.25)*0.5,0.0,0); quadMat.set_translation({tran});
            graphics_vector3_type rot(time,0,0); 
            graphics_vector3_type sca(0.5);
            quadMat.set_rotation({rot}, sca);
            quad.transform("a_Position", quadMat);
        }

        quad.transform("a_UV", {0.2f, 0}, 0, {2.f, 2.f});
        batchModelVertexData.push_back(quad);
        batchModelVertexData.sort_by_nearest_triangle( {0,0,-20}, graphics_matrix4x4_type::identity);
        pBatchModel->upload(model::usage_hint::streaming, batchModelVertexData);

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

