// © Joseph Cameron - All Rights Reserved

#include "voxel_modeler.h"

#include <gdk/game_loop.h>
#include <gdk/graphics_context.h>
#include <gdk/graphics_exception.h>
#include <gdk/scene.h>
#include <gdk/texture_data.h>
#include <gdk/webgl1es2_context.h>
#include <gdk/webgl1es2_texture.h>
#include <jfc/glfw_window.h>

#include <GLFW/glfw3.h>

#include <algorithm>
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
#include <vector>

using namespace gdk;
using namespace gdk::graphics::ext;

// 16^3 = 4096 & sqrt(4096) = 64, so a 16^3 volume fits perfectly into a 64^2 area
constexpr size_t LIGHT_TOTAL_VOXELS(4096);
constexpr size_t LIGHT_2D_TEXTURE_LENGTH(std::sqrt(LIGHT_TOTAL_VOXELS)); 
constexpr size_t LIGHT_3D_TEXTURE_LENGTH(std::cbrt(LIGHT_TOTAL_VOXELS)); 
//im not 100% sure these restrictions are needed. maybe as long as the 2dtexture has enough texels to store the
//3d data then its fine, they dont all have to be power of 2? possibly 2d restriction is just being a whole number

constexpr bool is_power_of_two(const size_t aNumber) {
    return aNumber > 0 && pow(2, static_cast<int>(log2(aNumber))) == aNumber;
}
static_assert(is_power_of_two(LIGHT_TOTAL_VOXELS));
static_assert(is_power_of_two(LIGHT_3D_TEXTURE_LENGTH));
static_assert(is_power_of_two(LIGHT_2D_TEXTURE_LENGTH));

std::array<std::array<std::array<gdk::color, LIGHT_3D_TEXTURE_LENGTH>, LIGHT_3D_TEXTURE_LENGTH>, 
    LIGHT_3D_TEXTURE_LENGTH> mData{};

void setLighting(int aX, int aY, int aZ, gdk::color aColor) {
    if (aX < 0 || aX >= 16) return;
    if (aY < 0 || aY >= 16) return;
    if (aZ < 0 || aZ >= 16) return;

    mData[aX][aY][aZ] = aColor;
}

void clearLighting() {
    mData = {};
}

gdk::color getLighting(const int aX, const int aY, const int aZ) {
    if (aX < 0 || aX >= 16) throw graphics_exception("parameter out of range");
    if (aY < 0 || aY >= 16) throw graphics_exception("parameter out of range");
    if (aZ < 0 || aZ >= 16) throw graphics_exception("parameter out of range");

    return {mData[aX][aY][aZ]};
}

void addLighting(int aX, int aY, int aZ, gdk::color aColor) {
    if (aX < 0 || aX >= 16) return;
    if (aY < 0 || aY >= 16) return;
    if (aZ < 0 || aZ >= 16) return;

    auto lightVoxel = getLighting(aX, aY, aZ);

    lightVoxel.r += aColor.r;
    lightVoxel.g += aColor.g;
    lightVoxel.b += aColor.b;

    lightVoxel.clamp();

    setLighting(aX, aY, aZ, lightVoxel);
}

// Applies a uniform light to the whole 3d array
void addGlobalLight(const gdk::color &aColor) {
    const auto size(mData.size());
    using size_type = decltype(mData)::size_type;
    for (size_type x(0); x < size; ++x) 
        for (size_type y(0); y < size; ++y) 
            for (size_type z(0); z < size; ++z) 
                addLighting( x, y, z, aColor);   
}

// Does not do any occlusion checking (light will pass through walls)
//TODO: think about how to do this. 
//  1 create a line segment from light's origin point to the current light voxel
//  2 check for any solid objects in each voxel that the line intersects
//  3 if a solid object is found, do not add any color to the current voxel, continue to next 
//  - lighting with occulsion will require a grid or a bsp or something passed as a const ref, to perform the check
//  - should start treating all position values as world space positions. currently its all local space
void addPointLight(int aX, int aY, int aZ, const float aSize, const gdk::color aColor) {
    const gdk::vector3<float> CENTRE(aSize/2.f); 
    const auto HALF(aSize/2.f);

    aX -= HALF;
    aY -= HALF;
    aZ -= HALF;

    for (int x(0); x < aSize; ++x) 
        for (int y(0); y < aSize; ++y) 
            for (int z(0); z < aSize; ++z) {
                float distanceFromCentre = CENTRE.distance(gdk::vector3<float>(x,y,z));
                float normalizedHalfDistanceFromCentre = distanceFromCentre / HALF; 
                float intensity = (1.0f / std::sqrt(normalizedHalfDistanceFromCentre)) - 1.0f; 
                intensity = std::clamp(intensity, 0.0f, 1.0f);

                auto color(aColor);
                color.r *= intensity;
                color.g *= intensity;
                color.b *= intensity;

                addLighting(aX + x, aY + y, aZ + z, color);   
            }    
};

//TODO: think about how to integrate the 3d/2d packing code into the GL implementation of gdk_graphics
void updateLightingTexture(std::shared_ptr<texture> aTexture) {
    //multiplied by 3 because this array is raw r,g,b,... channel data
    std::vector<std::underlying_type<std::byte>::type> imageData(LIGHT_TOTAL_VOXELS * 3, {});

    size_t i = 0;
    for (size_t x(0); x < LIGHT_3D_TEXTURE_LENGTH; ++x)
        for (size_t y(0); y < LIGHT_3D_TEXTURE_LENGTH; ++y)
            for (size_t z(0); z < LIGHT_3D_TEXTURE_LENGTH; ++z) {
                imageData[i + 0] = mData[x][y][z].r * 255;
                imageData[i + 1] = mData[x][y][z].g * 255;
                imageData[i + 2] = mData[x][y][z].b * 255; //converting from 0-1 to 00-ff

                i = i + 3;
            }

    texture_data::view view;
    view.width = LIGHT_2D_TEXTURE_LENGTH;
    view.height = LIGHT_2D_TEXTURE_LENGTH;
    view.format = texture::format::rgb;
    view.data = &imageData.front();

    aTexture->update_data(view);
}

int main(int argc, char **argv) {
    glfw_window window("Voxel rendering with lighting");

    const auto pGraphics = webgl1es2_context::make();

    const auto pScene = pGraphics->make_scene();

    auto pCamera = [&]() {
        auto pCamera = pGraphics->make_camera();
        pScene->add(pCamera);
        return pCamera;
    }();

    auto pTexture = ([&]() {
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
        view.data = &imageData.front();

        return pGraphics->make_texture(view);
    }());

    auto pLightingTexture = pGraphics->make_texture();

    auto pShader = [&]() {
        const std::string vertexShaderSource(R"V0G0N(
        uniform mat4 _MVP;

        attribute highp vec3 a_Position;
        attribute highp vec3 a_VoxelPosition;
        attribute lowp vec2 a_UV;
       
        varying highp vec3 v_Position;
        varying highp vec3 v_VoxelPosition;
        varying mediump vec2 v_UV;

        void main () {
            gl_Position = _MVP * vec4(a_Position,1.0);

            v_Position = a_Position;
            v_UV = a_UV;
            v_VoxelPosition = a_VoxelPosition;
        }
        )V0G0N");

        const std::string fragmentShaderSource(R"V0G0N(
        uniform sampler2D _LightTexture;
        uniform sampler2D _Texture;
        uniform vec2 _UVOffset;
        uniform vec2 _UVScale; 

        varying highp vec3 v_Position;
        varying highp vec3 v_VoxelPosition;
        varying lowp vec2 v_UV;

        lowp vec3 getLightVoxel(ivec3 aPosition) {
            highp float oneDimensionIndex = float(aPosition.x) + 
                (float(aPosition.y) * 16.) + 
                (float(aPosition.z) * 16. * 16.);

            lowp vec2 normalizedLightVoxelCoordinates = vec2(
                mod(oneDimensionIndex, 64.) / 64.,
                floor(oneDimensionIndex / 64.) / 64.
            );

            return texture2D(_LightTexture, normalizedLightVoxelCoordinates).xyz;
        }

        void main() {
            lowp vec2 uv = v_UV + _UVOffset * _UVScale;
            vec4 texel = texture2D(_Texture, uv);
            if (texel[3] < 1.0) discard;

            vec3 voxelPosition = v_VoxelPosition;
            /* 
                //visually confirms v_VoxelPosition values are reaching frag shader correctly
                voxelPosition = v_VoxelPosition;
                voxelPosition.xyz /= 16.;
                gl_FragColor.rgb = vec3(voxelPosition);
            */

            vec3 position = v_Position;
            /* 
                //visually confirms v_Position values are reaching frag shader correctly
                position = floor(position); 
                position.xyz /=16.;
                gl_FragColor.rgb = vec3(position);
            */
     
            highp vec3 thisLight =   getLightVoxel(ivec3(voxelPosition) + ivec3( 0, 0, 0));
            highp vec3 topLight =    getLightVoxel(ivec3(voxelPosition) + ivec3( 0, 1, 0));
            highp vec3 bottomLight = getLightVoxel(ivec3(voxelPosition) + ivec3( 0,-1, 0));
            highp vec3 northLight =  getLightVoxel(ivec3(voxelPosition) + ivec3( 0, 0, 1));
            highp vec3 southLight =  getLightVoxel(ivec3(voxelPosition) + ivec3( 0, 0,-1));
            highp vec3 eastLight =   getLightVoxel(ivec3(voxelPosition) + ivec3(-1, 0, 0));
            highp vec3 westLight =   getLightVoxel(ivec3(voxelPosition) + ivec3( 1, 0, 0));

// -------------------------------------------------------------------------------------------------------------------------------------
            //Sampling section is incomplete, currently only uses thislight.
            //TODO: need to think about how to blend this light with neighbours correctly  
            highp vec3 subVoxelPosition = mod(v_Position, 1.);
            highp vec3 bias = subVoxelPosition; 
            //centering it so 0,0,0 is center of voxel
            //bias.x -= 0.5;
            //bias.y -= 0.5;
            //bias.z -= 0.5;

            highp float mag = length(bias);

            highp vec3 light;
            light = thisLight * 1.0;

            //light += thisLight * (1.0 - mag);

            /*light += thisLight * (1.0 - bias.z) + (northLight * bias.z);
            light += thisLight * (1.0 - bias.z) + (southLight * bias.z);
            light += thisLight * (1.0 - bias.x) + (eastLight * bias.x);
            light += thisLight * (1.0 - bias.x) + (westLight * bias.x);
            light += thisLight * (1.0 - bias.y) + (eastLight * bias.y);
            light += thisLight * (1.0 - bias.y) + (westLight * bias.y);*/

            gl_FragColor = vec4(texel.xyz * light.xyz, 1.); //need to blend. currently only uses current voxel so the effect is very aliasy
            //gl_FragColor = vec4(light.xyz, 1.); //enable this line to see just lighting data
            //gl_FragColor = vec4(bias.x, bias.x, bias.x, 1.); //see bias values on fragments
// -------------------------------------------------------------------------------------------------------------------------------------
        }
        )V0G0N");

        return std::static_pointer_cast<webgl1es2_context>(pGraphics)->make_shader(vertexShaderSource,fragmentShaderSource);
    }();
    
    auto pMaterial = [&]() {
        auto pMaterial = pGraphics->make_material(pShader);
        pMaterial->setTexture("_LightTexture", pLightingTexture);
        pMaterial->setTexture("_Texture", pTexture);
        pMaterial->setVector2("_UVOffset", {0, 0});
        pMaterial->setVector2("_UVScale", {1, 1});
        return pMaterial;
    }();

    voxel_modeler voxelModeler = [&]() {
        voxel_modeler voxelModeler;
        
        for (size_t x(0); x < 16; ++x) for (size_t y(0); y < 16; ++y) {
            voxelModeler.set_voxel_data(x,0,y,1);
        }

        for (size_t x(1); x < 14; ++x) for (size_t z(1); z < 12; ++z) {
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
        voxelModeler.set_voxel_data(8,3,8,1);

        voxelModeler.update_model_data(); 

        return voxelModeler;
    }();

    auto pVoxelModel = [&]() {
        auto pVoxelModel(pGraphics->make_model());
        pVoxelModel->upload(model::usage_hint::streaming, voxelModeler.model_data());
        return pVoxelModel;
    }();
   
    auto pVoxelEntity = [&]() {
        auto pVoxelEntity = pGraphics->make_entity(pVoxelModel, pMaterial);
        pScene->add(pVoxelEntity);
        return pVoxelEntity;
    }();
 
    auto pSkyboxShader = [&]() {
        const std::string vertexShaderSource(R"V0G0N(
        uniform mat4 _MVP;
        attribute highp vec3 a_Position;
        varying highp vec3 v_Position;

        void main () {
            gl_Position = _MVP * vec4(a_Position,1.0);
            v_Position = a_Position;
        }
        )V0G0N");

        const std::string fragmentShaderSource(R"V0G0N(
        varying highp vec3 v_Position;

        const vec3 spaceColor = vec3(0.8,1.0,1.0);
        const vec3 horizonColor = vec3(0.4, 0.5, 0.8);

        float dist(vec3 a, vec3 b) {
            return sqrt(dot(a, b));
        }

        void main() {
            vec3 color = mix(horizonColor, spaceColor, abs(normalize(v_Position).y)); 
            gl_FragColor.xyz = vec3(color);
        }
        )V0G0N");

        return std::static_pointer_cast<webgl1es2_context>(pGraphics)->make_shader(vertexShaderSource,fragmentShaderSource);
    }();   

    auto pSkyboxMaterial = pGraphics->make_material(pSkyboxShader);

    auto pSkyboxModel(pGraphics->get_cube_model());

    auto pSkyboxEntity = [&]() {
        auto pSkyboxEntity = pGraphics->make_entity(pSkyboxModel, pSkyboxMaterial);
        pScene->add(pSkyboxEntity);
        pSkyboxEntity->set_transform({0, 5, 0}, {}, {30, 30, 30});
        return pSkyboxEntity; 
    }();

    game_loop(60, [&](const float time, const float deltaTime) {
        glfwPollEvents();

        clearLighting();
        addGlobalLight({0.1f, 0.1f, 0.1f});

        addPointLight(0, 2, 0, 20, {0.0, 0.0, 1.0});
        addPointLight(14, 2, 14, 10, {1.0, 0.0, 1.0});
        addPointLight(0, 2, 14, 10, {0.0, 1.0, 0.0});

        addPointLight(8, 1, 8, std::abs(std::sin(time)) * 10, {1.0, 1.0, 1.0});

        static int x = 8;
        static int z = 8;
        addPointLight(std::sin(time) * 8 + x, -2, std::cos(time) * 8 + z, 10, {1.0, 1.0, 1.0});

        updateLightingTexture(pLightingTexture);

        graphics_matrix4x4_type root({0,0,0}, {{0, time*0.5f, 0}});
        graphics_matrix4x4_type chunkMatrix({-8, 0, -8}, {{0, 0, 0}});
        pVoxelEntity->set_transform(root * chunkMatrix);

        pCamera->set_perspective_projection(90, 0.01, 35, window.getAspectRatio());
        pCamera->set_transform({0, +6, +9}, {{+0.4f, 0, 0}});

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

