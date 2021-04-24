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

#include <array>
#include <type_traits>
#include <map>
class dynamic_vertex_batch : public gdk::entity_owner
{
public:
    using tile_creator_type = std::function<vertex_data(const size_t, const size_t)>;

    /*struct buffer_location
    {
        size_t x, y, z;
    };
    std::map<buffer_location, vertex_data> m_Buffer;*/

    //! update buffer data at a location
    //void update_buffer(size_t x, size_t y, size_t z);
    //! update buffer data over a range
    //void update_buffer(size_t xLow, size_t xHigh, size_t yLow, size_t yHigh);
    //void clear_buffer();
    //void update_model();

    virtual std::vector<std::shared_ptr<const entity>> get_entities() const override
    {
        return {m_pEntity};
    }

    dynamic_vertex_batch(const std::shared_ptr<graphics::context> &pContext)
    : m_pMaterial(pContext->make_material(pContext->get_alpha_cutoff_shader()))
    , m_pModel(pContext->make_model())
    , m_pEntity(pContext->make_entity(m_pModel, m_pMaterial))
    {
        m_pEntity->set_model_matrix(Vector3<float>{-2, 0., -12}, 
            Quaternion<float>{{0, 0, 0}},
            {0.5,0.5,0.5});

        vertex_data newdata({});

        for (size_t x(0); x < 10; ++x) for (size_t y(0); y < 10; ++y)
            newdata += m_TileCreator(x, y);

        m_pModel->update_vertex_data(model::UsageHint::Static, newdata);
    }

    ~dynamic_vertex_batch() = default;

private:
    std::shared_ptr<material> m_pMaterial;
    std::shared_ptr<model> m_pModel;
    std::shared_ptr<entity> m_pEntity;

    tile_creator_type m_TileCreator = [](const size_t x, const size_t y)
    {
        std::vector<float> posData({
            1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
        });

        for (size_t i(0); i < posData.size(); i += 3)
        {
            posData[i + 0] += x;
            posData[i + 1] += y;
            posData[i + 2] -= y;
        }

        std::vector<float> uvData({
            1, 0,
            0, 0,
            0, 1,
            1, 0,
            0, 1,
            1, 1,
        });

        if (x == 2 && y == 2)
        {
            return vertex_data({});
        }

        return vertex_data(
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
        });
    };
};

// \brief a batch model is a model whos vertex data is the composition of
/// the transformed vertex data of many separate objects.
/// these objects could be separate instances of the same model or
/// instances of many different models, as long as they use the same
/// material and the same primitive mode.
class batch_model
{
public:
    //! update buffer at location
    void generate_in_buffer(size_t vertexDataIndex, 
        const graphics_vector3_type &aPos, 
        const graphics_quaternion_type &aRot = {},
        const graphics_vector3_type &aScale = {});

    //! erase all vertex data in the current buffer
    void clear_buffer();

    //! overwrite the model's vertex data with the data in the buffer
    void update_model();
    
    //! throw is no v_Position
    //! throw is v_Postion does not have 3 components
    //! throw if ahh
    batch_model(std::shared_ptr<gdk::graphics::context> pContext, 
        std::vector<vertex_data> data);

private:
    //! model, the polygonal model rendered
    std::shared_ptr<gdk::model> m_pModel;

    //! vetex data buffer, modifed and uploaded to the model
    vertex_data m_Buffer = vertex_data({});

    std::vector<vertex_data> m_Inputs;
};

batch_model::batch_model(std::shared_ptr<gdk::graphics::context> pContext, 
    std::vector<vertex_data> data)
: m_pModel(pContext->make_model())
, m_Inputs(data)
{}

void batch_model::generate_in_buffer(size_t vertexDataIndex, 
    const graphics_vector3_type &aPos, 
    const graphics_quaternion_type &aRot,
    const graphics_vector3_type &aScale)
{
    if (vertexDataIndex >= m_Inputs.size()) 
        throw std::invalid_argument("index out of range");

    auto newData = m_Inputs[vertexDataIndex];

    auto view = newData.view_to_interleaved_data();
    auto pos_offset = newData.attribute_offset("a_Position");
    auto vertex_size = newData.vertex_size();
   
    auto *p = view.begin + pos_offset;

    for (size_t i(0); i < view.size; i+= vertex_size)
    {
        //TODO: scale

        //TODO: rotate

        //Offset
        *(p + 0) += aPos.x;
        *(p + 1) += aPos.y;
        *(p + 2) += aPos.z;

        p += vertex_size;
    }
}

void batch_model::clear_buffer()
{
    m_Buffer.clear();
}

void batch_model::update_model()
{
    m_pModel->update_vertex_data(model::UsageHint::Streaming,
        m_Buffer);
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

    dynamic_vertex_batch tileRenderer(pContext);
    pScene->add(tileRenderer);
    /*my_cool_batch tileRenderer(pContext);
    pScene->add(tileRenderer);*/

    batch_model batchModel(pContext, {});

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

    auto pUserModel = pContext->make_model(model::UsageHint::Static, {
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

    pUserModel->update_vertex_data(model::UsageHint::Static, {
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

