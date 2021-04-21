// © Joseph Cameron - All Rights Reserved

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <unordered_map>

#include <jfc/glfw_window.h>

#include <gdk/graphics_context.h>
#include <gdk/scene.h>
#include <gdk/entity_owner.h>

#include <GLFW/glfw3.h>

using namespace gdk;

// Raw texture data (image these are loaded from file or compiled into the binary from e.g a "resource" header)
std::vector<std::underlying_type<std::byte>::type> imageData2({
    0x55, 0xff, 0xff, 0xff,
    0xff, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0xff,
    0x00, 0x00, 0x44, 0xff});

std::vector<std::underlying_type<std::byte>::type> imageData({
    0x00, 0xff, 0xff, 0xff,                                    
    0xff, 0xff, 0xff, 0xff,                                    
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0xff});

class tile_renderer : public gdk::entity_owner
{
    std::shared_ptr<material> m_pMaterial;

    std::shared_ptr<model> m_pModel;

    std::shared_ptr<entity> m_pEntity;
public:
    virtual std::vector<std::shared_ptr<const entity>> get_entities() const override;

    tile_renderer(const std::shared_ptr<graphics::context> &pContext);

    ~tile_renderer() = default;
};

tile_renderer::tile_renderer(const std::shared_ptr<graphics::context> &pContext)
: m_pMaterial(pContext->make_material(pContext->get_alpha_cutoff_shader()))
, m_pModel(pContext->make_model())
, m_pEntity(pContext->make_entity(m_pModel, m_pMaterial))
{
    float size = 1;
    decltype(size) hsize = size/2.;

    std::vector<float> posData({
        size -hsize, size -hsize, 0.0f,
        0.0f -hsize, size -hsize, 0.0f,
        0.0f -hsize, 0.0f -hsize, 0.0f,
        size -hsize, size -hsize, 0.0f,
        0.0f -hsize, 0.0f -hsize, 0.0f,
        size -hsize, 0.0f -hsize, 0.0f});

    std::vector<float> uvData({
        1, 0,
        0, 0,
        0, 1,
        1, 0,
        0, 1,
        1, 1});

    m_pModel->update_vertex_data({vertex_data::UsageHint::Dynamic,
    {
        { 
            "a_Position",
            {
                &posData.front(),
                posData.size(),
                3
            }
        },
        { 
            "a_UV",
            {
                &uvData.front(),
                uvData.size(),
                2
            }
        }
    }});

    m_pEntity->set_model_matrix(Vector3<float>{-2, 0., -12}, 
        Quaternion<float>{{0, 0, 0}});
}

std::vector<std::shared_ptr<const entity>> tile_renderer::get_entities() const
{
    return {m_pEntity};
}

int main(int argc, char **argv)
{
    // Separate lib, used to init GL and get a window ready for rendering on Linux/Mac/Windows
    glfw_window window("basic rendering demo");

    // Init a gles2/webgl1 context
    auto pContext = graphics::context::make(
        graphics::context::implementation::opengl_webgl1_gles2);

    // Setting up the main scene
    auto pScene = pContext->make_scene();

    tile_renderer tileRenderer(pContext);
    pScene->add(tileRenderer);

    auto pTextureCamera = pContext->make_texture_camera();
    pTextureCamera->set_clear_color({1,0.1,0.1,1});
    pScene->add(pTextureCamera);

    auto pCamera = pContext->make_camera();
    //pCamera->set_viewport(0, 0, 0.5, 0.5);
    pScene->add(pCamera);

    /*auto pCamera2 = pContext->make_camera();
    //pCamera2->set_viewport(0.5, 0.5, 0.5, 0.5);
    pCamera2->set_clear_color(color::DarkGreen);
    pScene->add(pCamera2);*/

    auto pAlpha = pContext->get_alpha_cutoff_shader();

    using vertex_attribute_type = float;
    using vertex_attribute_array_type = std::vector<vertex_attribute_type>;

    vertex_attribute_type size = 1;
    decltype(size) hsize = size/2.;

    vertex_attribute_array_type posData({ //Quad data: vertex positon  
        size -hsize, size -hsize, 0.0f,
        0.0f -hsize, size -hsize, 0.0f,
        0.0f -hsize, 0.0f -hsize, 0.0f,
        size -hsize, size -hsize, 0.0f,
        0.0f -hsize, 0.0f -hsize, 0.0f,
        size -hsize, 0.0f -hsize, 0.0f});

    vertex_attribute_array_type uvData({ //Quad data: uvs
        1, 0,
        0, 0,
        0, 1,
        1, 0,
        0, 1,
        1, 1});

    auto pUserModel = pContext->make_model({vertex_data::UsageHint::Static,
    {
        { 
            "a_Position",
            {
                &posData.front(),
                posData.size(),
                3
            }
        },
        { 
            "a_UV",
            {
                &uvData.front(),
                uvData.size(),
                2
            }
        }
    }});

    pUserModel->update_vertex_data({vertex_data::UsageHint::Static,
    {
        { 
            "a_Position",
            {
                &posData.front(),
                posData.size(),
                3
            }
        },
        { 
            "a_UV",
            {
                &uvData.front(),
                uvData.size(),
                2
            }
        }
    }});

    texture::image_data_2d_view view;
    view.width = 2;
    view.height = 2;
    view.format = texture::data_format::rgba;
    view.data = reinterpret_cast<std::byte *>(&imageData.front());

    auto pTexture = pContext->make_texture(view);
    auto pMaterial = pContext->make_material(pAlpha, material::render_mode::transparent);
    pMaterial->setTexture("_Texture", pTextureCamera->get_color_texture());
    pMaterial->setVector2("_UVScale", {1, 1});
    pMaterial->setVector2("_UVOffset", {0, 0});

    auto pEntity = pContext->make_entity(pUserModel, pMaterial);
    pEntity->set_model_matrix(Vector3<float>{2., 0., -11.}, Quaternion<float>());
    pScene->add(pEntity);

    texture::image_data_2d_view view2;
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
    pScene->add(pEntity2);

    auto pMaterial3 = pContext->make_material(pAlpha);
    pMaterial3->setTexture("_Texture", pTexture);
    pMaterial3->setVector2("_UVScale", {1, 1});
    pMaterial3->setVector2("_UVOffset", {0, 0});
    auto pEntity3 = std::shared_ptr<entity>(
        pContext->make_entity(pContext->get_cube_model(), pMaterial3));
    pScene->add(pEntity3);
    pEntity3->set_model_matrix(Vector3<float>{2., 0., -14.5}, 
        Quaternion<float>{{0, 2, 0.6}},
        {6.5, 0.5, 3});

    // Main loop
    float time = 0;

    for (float deltaTime(0); !window.shouldClose();)
    {
        using namespace std::chrono;

        steady_clock::time_point t1(steady_clock::now());

        glfwPollEvents();

        pEntity->set_model_matrix(Vector3<float>{std::cos(time), -0., -11.}, Quaternion<float>{ {time, 4 * (time / 2), 4}});
        pEntity2->set_model_matrix(Vector3<float>{0., 0., -12.5}, Quaternion<float>{{time, 2 * (time / 2), 4}});

        pCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        pCamera->set_view_matrix({std::sin(time), 0, -10}, {});
        
        pTextureCamera->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        pTextureCamera->set_view_matrix({std::sin(time), 0, -10}, {});

        //pCamera2->set_perspective_projection(90, 0.01, 20, window.getAspectRatio());
        //pCamera2->set_view_matrix({ std::sin(time), 0, -10 }, {});

        //pBackgroundScene->draw(window.getWindowSize());

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 

        time += 0.01;

        while (true)
        {
            std::this_thread::sleep_for(1us);
    
            steady_clock::time_point t2(steady_clock::now());

            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

            if (deltaTime = time_span.count(); deltaTime > 0.01666667) break;
        }
    }

    return EXIT_SUCCESS;
}

