// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/graphics_context.h>

using namespace gdk;

TEST_CASE("graphics_context", "[gdk::graphics_context]")
{
    initGL();

    auto pContext = graphics::context::make(graphics::context::implementation::opengl_webgl1_gles2);

    REQUIRE(pContext);

    SECTION("build camera by default make & by copy")
    {
        auto pCamera = pContext->make_camera();

        REQUIRE(pCamera);
        REQUIRE(!jfc::glGetError());

        auto pCamera2 = pContext->make_camera(*pCamera);

        REQUIRE(pCamera2);
        REQUIRE(!jfc::glGetError());
    }

    SECTION("get a built-in shader")
    {
        auto pShader = pContext->get_alpha_cutoff_shader();

        REQUIRE(pShader);
        REQUIRE(!jfc::glGetError());

        SECTION("make a material with standard make")
        {
            auto pMaterial = std::shared_ptr<material>(std::move(pContext->make_material(pShader)));

            REQUIRE(pMaterial);
            REQUIRE(!jfc::glGetError());

            SECTION("get a buit-in model")
            {
                auto pModel = pContext->get_quad_model();
                
                REQUIRE(pModel);
                REQUIRE(!jfc::glGetError());

                SECTION("build entity with standard make & by copy")
                {
                    auto pEntity = pContext->make_entity(pModel, pMaterial);
                
                    REQUIRE(pEntity);
                    REQUIRE(!jfc::glGetError());
                }
            }
        }
    }

    SECTION("make a texture with standard make & by copy")
    {
        texture::image_data_2d_view view;
        view.width = 2;
        view.height = 2;
        view.format = texture::data_format::rgba;

        std::vector<std::underlying_type<std::byte>::type> imageData({
            0x00, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff,
            0x00, 0x00, 0x00, 0xff,
        });

        view.data = reinterpret_cast<std::byte *>(&imageData.front());

        auto pTexture = pContext->make_texture(view);

        REQUIRE(pTexture);
        REQUIRE(!jfc::glGetError());
        
    }

    SECTION("make a model using a vertex_data_view")
    {
        float size = 1;
        float hsize = size/2.;

        std::vector<float> posData({  //Quad data
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
    
        auto pModel = std::shared_ptr<model>(std::move(
        pContext->make_model({
            vertex_data_view::UsageHint::Static,
            {
                { "a_Position",
                    {
                        &posData.front(),
                        posData.size(),
                        3
                    }
                },
                { "a_UV",
                    {
                        &uvData.front(),
                        uvData.size(),
                        2
                    }
                }
            }
        })));

        REQUIRE(pModel);
    }
}

