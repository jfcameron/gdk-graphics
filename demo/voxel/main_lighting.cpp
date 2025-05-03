// © Joseph Cameron - All Rights Reserved

#include "voxel_modeler.h"

#include <gdk/game_loop.h>
#include <gdk/graphics_context.h>
#include <gdk/webgl1es2_context.h>
#include <gdk/webgl1es2_texture.h>
#include <gdk/scene.h>

#include <jfc/glfw_window.h>
#include <jfc/contiguous_view.h>

#include <GLFW/glfw3.h>

#include <algorithm>
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

// 16^3 = 4096 & sqrt(4096) = 64, so a 16^3 volume fits perfectly into a 64^2 area
constexpr size_t LIGHT_TOTAL_VOXELS(4096);
constexpr size_t LIGHT_3D_TEXTURE_LENGTH(std::cbrt(LIGHT_TOTAL_VOXELS)); 
constexpr size_t LIGHT_2D_TEXTURE_LENGTH(std::sqrt(LIGHT_TOTAL_VOXELS)); 
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
    if (aX < 0 || aX >= 16) throw std::invalid_argument("parameter out of range");
    if (aY < 0 || aY >= 16) throw std::invalid_argument("parameter out of range");
    if (aZ < 0 || aZ >= 16) throw std::invalid_argument("parameter out of range");

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

    lightVoxel.r = std::clamp<float>(lightVoxel.r, 0.f, 1.f);
    lightVoxel.g = std::clamp<float>(lightVoxel.g, 0.f, 1.f);
    lightVoxel.b = std::clamp<float>(lightVoxel.b, 0.f, 1.f);

    setLighting(aX, aY, aZ, lightVoxel);
}

// Applys a uniform light to the whole 3d array
void addGlobalLight(const gdk::color &aColor) {
    const auto size(mData.size());
    for (int x(0); x < size; ++x) 
        for (int y(0); y < size; ++y) 
            for (int z(0); z < size; ++z) 
                addLighting( x, y, z, aColor);   
}

// Does not do any occlusion checking (light will pass through walls)
void addPointLight(int aX, int aY, int aZ, float aSize, gdk::color aColor) {
    const float SIZE(aSize);
    const float HALF(SIZE/2.f);
    const gdk::Vector3<float> CENTRE(SIZE/2.f); 

    aX -= HALF;
    aY -= HALF;
    aZ -= HALF;

    for (int x(0); x < SIZE; ++x) 
        for (int y(0); y < SIZE; ++y) 
            for (int z(0); z < SIZE; ++z) {
                float distanceFromCentre = CENTRE.distance(gdk::Vector3<float>(x,y,z));
                float normalizedHalfDistanceFromCentre = distanceFromCentre / HALF; 
                float intensity = 1.0f;
                //Inverse of the square distance drop off
                intensity = (1.0f / std::sqrt(normalizedHalfDistanceFromCentre)) - 1.0f; 
                //Linear drop off
                //intensity = 1.0f - normalizedHalfDistanceFromCentre;
                
                intensity = std::clamp(intensity, 0.0f, 1.0f);

                auto color(aColor);
                color.r *= intensity;
                color.g *= intensity;
                color.b *= intensity;

                addLighting(aX + x, aY + y, aZ + z, color);   
            }    
};

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

    image_data_2d_view view;
    view.width = LIGHT_2D_TEXTURE_LENGTH;
    view.height = LIGHT_2D_TEXTURE_LENGTH;
    view.format = texture::data_format::rgb;
    view.data = reinterpret_cast<std::byte *>(&imageData.front());

    aTexture->update_data(view);
}

int main(int argc, char **argv)
{
    glfw_window window("Voxel rendering with lighting");

    auto pGraphics = webgl1es2_context::make();
    auto pScene = pGraphics->make_scene();
    //auto pShader = pGraphics->get_alpha_cutoff_shader();
    auto pCamera = pGraphics->make_camera();
    pScene->add(pCamera);

    auto pTexture = ([&]() {
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

        return pGraphics->make_texture(view);
    }());

    auto pLightingTexture = ([&]() {
        //points to the need to be able to instantiate textures without any data
        std::vector<std::underlying_type<std::byte>::type> imageData({
            0xff, 0xff, 0xff, 0xff, 
            0xff, 0xff, 0xff, 0xff,                                    
            0xff, 0xff, 0xff, 0xff,
            0x00, 0x00, 0x00, 0xff,
        });
        image_data_2d_view view;
        view.width = 2;
        view.height = 2;
        view.format = texture::data_format::rgba;
        view.data = reinterpret_cast<std::byte *>(&imageData.front());

        return pGraphics->make_texture(view);
    }());

    auto pShader = [&](){
        const std::string vertexShaderSource(R"V0G0N(
        //Uniforms
        uniform mat4 _MVP;
        uniform mat4 _Model;
        uniform mat4 _View;
        uniform mat4 _Projection;

        //VertexIn
        attribute highp vec3 a_Position;
        attribute mediump vec2 a_UV;
        attribute highp vec3 a_VoxelPosition;
       
        //FragmentIn
        varying mediump vec2 v_UV;
        varying highp vec3 v_Position;
        varying highp vec3 v_VoxelPosition;

        void main ()
        {
            gl_Position = _MVP * vec4(a_Position,1.0);

            v_UV = a_UV;
            v_Position = a_Position;
            v_VoxelPosition = a_VoxelPosition;
        }
        )V0G0N");

        clearLighting();

        addGlobalLight({0.1f,0.1f,0.1f});

        addPointLight(1,-0,2, 10, {1.0,1.0,1.0});
        addPointLight(7,-5,2, 10, {1.0,0.0,0.0});
        addPointLight(7,-5,7, 10, {0.0,1.0,0.0});
        addPointLight(0,-5,0, 10, {0.0,0.0,1.0});
        addPointLight(0,-5,7, 10, {1.0,0.0,1.0});

        updateLightingTexture(pLightingTexture);

        const std::string fragmentShaderSource(R"V0G0N(
        //Uniforms
        uniform sampler2D _Texture;
        uniform vec2 _UVOffset;
        uniform vec2 _UVScale; 

        uniform sampler2D _LightTexture;

        lowp vec3 getLightVoxel(highp vec3 aPosition) {
            highp float x = aPosition.x;
            highp float y = aPosition.y;
            highp float z = aPosition.z;
            highp float oneDimensionIndex = x + (y * 16.) + (z * 16. * 16.);

            lowp vec2 normalizedLightVoxelCoordinates;
            normalizedLightVoxelCoordinates.x = mod(oneDimensionIndex, 64.) /64.;
            normalizedLightVoxelCoordinates.y = floor(oneDimensionIndex /64.)/64.;

            return texture2D(_LightTexture, normalizedLightVoxelCoordinates).xyz;
        }

        //FragmentIn
        varying lowp vec2 v_UV;
        varying highp vec3 v_Position;
        varying highp vec3 v_VoxelPosition;

        void main()
        {
            lowp vec2 uv = v_UV;
            uv += _UVOffset;
            uv *= _UVScale;

            vec4 texel = texture2D(_Texture, uv);
            if (texel[3] < 1.0) discard;

            vec3 position = floor(v_Position); 
      
            //Using v_VoxelPosition (static value across voxel) works, using v_Position has a weird artifact.
            // Try to find a way to fix the artifact, that will reduce vertex data.
            // Possibly as simple as like if val > 0.1 then val -= 0.09 or something. 
            highp vec3 light = getLightVoxel(v_VoxelPosition);

            gl_FragColor = vec4(texel.xyz * light.xyz, 1.);
            //gl_FragColor = vec4(light.xyz, 1.);

            /*
            // Works correctly. colors a single voxel
            if (
                position.z == 2. 
                && position.y == 2.  
                && position.x == 3.
            ) {
                //gl_FragColor = vec4(0.,1.,1., 1.);
            }*/

            
            /*// voxel position, looks good
            highp vec3 voxelPosition;
            voxelPosition = v_VoxelPosition;
            voxelPosition.xyz /= 16.;
            //gl_FragColor = vec4(voxelPosition.xyz,1.);
            gl_FragColor.rgb = vec3(voxelPosition.x);*/
            
            /*
            // Per vertex. works
            position.xyz /=16.;
            gl_FragColor = vec4(position.xyz,1.);
            */
            
        }
        )V0G0N");

        return std::static_pointer_cast<webgl1es2_context>(pGraphics)->make_shader(vertexShaderSource,fragmentShaderSource);
    }();
    
    auto pMaterial = pGraphics->make_material(pShader);
    pMaterial->setTexture("_Texture", pTexture);
    pMaterial->setVector2("_UVScale", {1, 1});
    pMaterial->setVector2("_UVOffset", {0, 0});
    pMaterial->setTexture("_LightTexture", pLightingTexture);

    voxel_modeler voxelModeler(pGraphics);

    auto pVoxelModel(pGraphics->make_model());
    
    auto pVoxelEntity = pGraphics->make_entity(pVoxelModel, pMaterial);
    pScene->add(pVoxelEntity);
    
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

    voxelModeler.update_vertex_data(); 
    pVoxelModel->update_vertex_data(model::UsageHint::Streaming, voxelModeler.vertex_data());

    game_loop(60, [&](const float time, const float deltaTime)
    {
        glfwPollEvents();


        clearLighting();
        addGlobalLight({0.1f,0.1f,0.1f});
        static int x = 8;
        static int z = 8;
        addPointLight(std::sin(time) * 8 + x,-2,std::cos(time) * 8 + z, 10, {1.0,1.0,1.0});
        addPointLight(8,1,8, std::abs(std::sin(time)) * 10, {1.0,1.0,1.0});
        updateLightingTexture(pLightingTexture);

        graphics_mat4x4_type root({0,0,0},Quaternion<float>({0,time,0}), {1});
        graphics_mat4x4_type chunkMatrix({-8,0,-8},Quaternion<float>({0,0,0}), {1});
        pVoxelEntity->set_model_matrix(root * chunkMatrix);

        pCamera->set_perspective_projection(90, 0.01, 30, window.getAspectRatio());
        pCamera->set_world_matrix({0, 6, +10}, {0.1,0,0,1});

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        return window.shouldClose();
    });

    return EXIT_SUCCESS;
}

