// © Joseph Cameron - All Rights Reserved

#include "skeleton_model.h"

#include <gdk/game_loop.h>
#include <gdk/graphics_context.h>
#include <gdk/scene.h>
#include <gdk/texture_data.h>
#include <gdk/webgl1es2_context.h>
#include <jfc/contiguous_view.h>
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

int main(int argc, char **argv)
{
    glfw_window window("skeleton");

    auto pContext = webgl1es2_context::make();

    auto pScene = pContext->make_scene();

    auto pCamera = pContext->make_camera();
    pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
    pCamera->set_transform({0, 0, 5}, {});
    pScene->add(pCamera);

    auto pAlpha = pContext->get_alpha_cutoff_shader();

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
    auto pTexture = pContext->make_texture(view);
    
    auto pMaterial = pContext->make_material(pAlpha);
    pMaterial->setTexture("_Texture", pTexture);
    pMaterial->setVector2("_UVScale", {1, 1});
    pMaterial->setVector2("_UVOffset", {0, 0});

    skeleton characterSkeleton({
        bone_data("head", 
            {{0,1,0},{{0,0,0}}}, 
            {}),
        bone_data("chest", 
            {{0,0,0},{{0,0,0}}}, 
            {"head", "armLeft", "armRight", "legLeft", "legRight"}),
        bone_data("armLeft", 
            {{1,0,0},{{0,0,0}}}, 
            {}),
        bone_data("armRight", 
            {{-1,0,0},{{0,0,0}}}, 
            {}),
        bone_data("legLeft", 
            {{0.5,-1,0},{{0,0,0}}}, 
            {}),
        bone_data("legRight", 
            {{-0.5,-1,0},{{0,0,0}}}, 
            {}),
    });
    
    std::unordered_map<std::string, std::shared_ptr<entity>> boneVisualizers;
    for (const auto &[name, bone] : characterSkeleton.boneMap)
    {
        auto pEntity = std::shared_ptr<entity>(
            pContext->make_entity(pContext->get_cube_model(), pMaterial));
        
        pScene->add(pEntity);

        pEntity->set_model_matrix(bone.transform);

        boneVisualizers[name] = pEntity;
    }

    animation walkAnimation({
        {
            0,
            skeleton({
                bone_data("head", 
                    {{0,1,0},{{0,0,0}}}, 
                    {}),
                bone_data("chest", 
                    {{0,0,0},{{0,0,0}}}, 
                    {"head", "armLeft", "armRight", "legLeft", "legRight"}),
                bone_data("armLeft", 
                    {{1,0,0},{{0,0,0}}}, 
                    {}),
                bone_data("armRight", 
                    {{-1,0,0},{{0,0,0}}}, 
                    {}),
                bone_data("legLeft", 
                    {{0.5,-1,0},{{0,0,0}}}, 
                    {}),
                bone_data("legRight", 
                    {{-0.5,-1,0},{{0,0,0}}}, 
                    {}),
            })
        },
        {
            2.1f,
            skeleton({
                bone_data("head", 
                    {{0,1,0},{{0,0,0}}}, 
                    {}),
                bone_data("chest", 
                    {{2,0,0},{{0,0,0}}}, 
                    {"head", "armLeft", "armRight", "legLeft", "legRight"}),
                bone_data("armLeft", 
                    {{1,0,0},{{0,0,0}}}, 
                    {}),
                bone_data("armRight", 
                    {{-1,0,0},{{0,0,0}}}, 
                    {}),
                bone_data("legLeft", 
                    {{0.5,-1,0},{{0,0,0}}}, 
                    {}),
                bone_data("legRight", 
                    {{-0.5,-1,0},{{0,0,0}}}, 
                    {}),
            })
        },
    });

    /*vector3<float> eulers(3.14,3.14/2.,3.14/3);
    quaternion<float> quat(eulers);
    auto eulers2 = quat.toEuler();

    std::cout << eulers << ", " << eulers2 << "\n";*/

    game_loop(60, [&](const float time, const float deltaTime)
    {
        glfwPollEvents();

        using namespace std;
        auto t = time;
        auto d = t*2;
        auto h = t/2;
        auto q = t/2;
        vector3<float> eulers(time/2,time/4,0);
        quaternion<float> quat(eulers);
        auto eulers2 = quat.toEuler();
        
        //25-04-22: matrix multiplication is working correctly 
        graphics_mat4x4_type m1({ 0,  0, 0},{{d,0,0}});
        graphics_mat4x4_type m2({ std::cos(t),  1, 0},{{0,q,0}});
        boneVisualizers["head"]->set_model_matrix(m1);
        boneVisualizers["chest"]->set_model_matrix(m1 * m2);

        /*characterSkeleton.set_local_transform("head",  {{0,1,0},{{time,time,0}},{1}});
        characterSkeleton.set_local_transform("chest", {{0,0,0},{{0,0,0}},{1}});*/
        
        /*graphics_mat4x4_type m3;
        //graphics_mat4x4_type m3({  1,  0,1.5},quaternion<float>({0,0,0}), {1});
        graphics_mat4x4_type m4({  1,  1,1.5},quaternion<float>({d,d,t}), {1});
        boneVisualizers["chest"]->set_model_matrix(m4);*/

        /*for (auto &[key, value] : boneVisualizers)
        {
            value->set_model_matrix(characterSkeleton.boneMap[key].transform);
        }*/

        pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        
        pScene->draw(window.getWindowSize());
        
        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

