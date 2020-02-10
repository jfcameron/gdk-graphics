#include <stdio.h>
#include <string.h>
#include <math.h>
#include <codecvt>
#include <locale>
#include <stdexcept>

#include <ft2build.h>
#include FT_FREETYPE_H

#define WIDTH   800//640
#define HEIGHT  480

#include <jfc/unique_handle.h>

#include <jfc/glfw_window.h>

#include <gdk/graphics_context.h>
#include <gdk/webgl1es2_entity.h>
#include <gdk/webgl1es2_material.h>
#include <gdk/webgl1es2_model.h>
#include <gdk/webgl1es2_texture.h>

#include <GLFW/glfw3.h>

using namespace gdk;

/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];

/* Replace this function with something useful. */
void draw_bitmap(FT_Bitmap *bitmap,
    FT_Int x,
    FT_Int y)
{
    FT_Int  i, j, p, q;
    FT_Int  x_max = x + bitmap->width;
    FT_Int  y_max = y + bitmap->rows;

    /* for simplicity, we assume that `bitmap->pixel_mode' */
    /* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */
    for ( i = x, p = 0; i < x_max; i++, p++ )
    {
        for ( j = y, q = 0; j < y_max; j++, q++ )
        {
            if ( i < 0 || j < 0 || i >= WIDTH || j >= HEIGHT ) continue;

            image[j][i] |= bitmap->buffer[q * bitmap->width + p];
        }
    }
}

int main(int argc, char **argv)
{
    if (argc !=2)
    {
        fprintf ( stderr, "usage: %s font sample-text\n", argv[0] );
        exit(1);
    }

    ////////////////

    jfc::unique_handle<FT_Library> hLibrary([]()
        {
            FT_Library library;

            if (auto error = FT_Init_FreeType( &library ))
            {
                throw std::invalid_argument("FreeType could not initialize");
            }
            
            return library;
        }(),
        [](const FT_Library library)
        {
            FT_Done_FreeType(library);
        }
    );

    static constexpr auto GLYPH_RASTER_SIZE(32); // Magic num (for now) deciding how large a (monospaced) char will be.

    jfc::unique_handle<FT_Face> hFace([argv, &hLibrary]()
        {
            char *filename = argv[1];

            FT_Face face;

            //TODO load from memory buffer. do not use porcelain
            if (auto error = FT_New_Face( hLibrary.get(), filename, 0, &face ))
            {
                //TODO make this more robust
                throw std::invalid_argument("Could not load font file");
            }

            if (auto error = FT_Set_Char_Size(face, 
                GLYPH_RASTER_SIZE * 64, //char width. 64 converst from fixed-point subpixel val
                0, //char height  (zero defaults to w size)
                0, //horizontal res
                0)) //vertical res
            {
                throw std::invalid_argument("Could not set charsize");
            }

            return face;
        }(),
        [](const FT_Face face)
        {
            FT_Done_Face(face);
        }
    );
    
    /* cmap selection omitted;                                        */
    /* for simplicity we assume that the font contains a Unicode cmap */
    FT_GlyphSlot slot = hFace.get()->glyph;

    FT_Matrix matrix;

    matrix.xx = 1 * 0x10000L; //Transforms are possible before rasterization
    matrix.xy = 0 * 0x10000L; //but I just want an identity mat
    matrix.yx = 0 * 0x10000L;
    matrix.yy = 1 * 0x10000L;

    FT_Vector pen;

    int target_height;
    target_height = HEIGHT;

    pen.x = 64 * GLYPH_RASTER_SIZE * 0; //perfect alginment
    pen.y = 64 * GLYPH_RASTER_SIZE * 0; //seems to write over the edge of the texture a bit :/

    // ======================================================================================================
    //! glyph rasters can fit together in a single atlas texture.
    ///
    /// entire atlas is rendered once in ctor; no bookkeeping required
    struct static_texture_atlas
    {

    };

    //! glyph raster cannot fit within a single atlas texture
    struct dynamic_texture_atlas
    {

    };

    ///// messing around with glyphs. Foundation of the atlas ctor code.
    // calc expected tex req, if < provided, do dynamic atlas, else do static atlas.
    [](const std::vector<FT_Face> &fontFaces)
    {
        static constexpr auto ATLAS_TEXTURE_MAX_PIXEL_SIZE(1024);

        const auto atlasMaxGlyphCapacity = std::pow((ATLAS_TEXTURE_MAX_PIXEL_SIZE / GLYPH_RASTER_SIZE), 2);

        auto glyphCount([&fontFaces]()
        {
            size_t count(0);

            for (const auto &current_face : fontFaces) count += current_face->num_glyphs; 

            return count;
        }());

        std::cout << "atlastCapacity: " << atlasMaxGlyphCapacity << ", glyph count: " << glyphCount << ", font pix size: " << GLYPH_RASTER_SIZE << "\n";
        
        if (glyphCount > atlasMaxGlyphCapacity)
        {
            std::cout << "All Glyph rasters cannot fit in the atlas; The atlas will be dynamic\n";

            //throw std::invalid_argument("dynamic atlas unimplemented.");
        }
        else
        {
            std::cout << "All Glyph rasters can fit in the atlas; The atlas will be static\n";

            //iterate glyphs. render to texture atlas.
        }
    }({hFace.get()});

    // ======================================================================================================

    // u8 data and conversion
    std::string narrowJapanese(/*"世界お早うございます"*/"Good morning world"); //u8 data
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convertWindows;
    std::wstring text = convertWindows.from_bytes(narrowJapanese);  //u16, needed by the library.

    for (const auto &current_char : text)
    {
        FT_Set_Transform(hFace.get(), &matrix, &pen);

        if (const auto error = FT_Load_Char(hFace.get(), current_char, FT_LOAD_RENDER)) continue; // ignore errors

        /* now, draw to our target surface (convert position) */
        draw_bitmap(&slot->bitmap,
            slot->bitmap_left,
            target_height - slot->bitmap_top);

        /* increment pen position */
        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }

    //////////////////////////////////////////

    glfw_window window("cool demo");
    
    auto pContext = graphics::context::make(graphics::context::implementation::opengl_webgl1_gles2);    

    gdk::webgl1es2_texture::webgl1es2_texture_2d_data_view_type view;
    view.width = WIDTH;
    view.height = HEIGHT;
    view.format = webgl1es2_texture::format::luminance;

    std::vector<std::underlying_type<std::byte>::type> imageData({});
    
    for (int i(0); i < HEIGHT; ++i) for (int j(0); j < WIDTH; ++j)
    {
        imageData.push_back(image[i][j]);
    }

    view.data = reinterpret_cast<std::byte *>(&imageData.front()); 

    std::shared_ptr<webgl1es2_texture> pTexture(new webgl1es2_texture(view));

    auto pModel = std::shared_ptr<webgl1es2_model>(webgl1es2_model::Quad);
    
    auto pMaterial = std::shared_ptr<webgl1es2_material>(new webgl1es2_material(webgl1es2_shader_program::AlphaCutOff));
    
    pMaterial->setTexture("_Texture", pTexture);

    auto pEntity = std::shared_ptr<entity>(std::move(pContext->make_entity(pModel, pMaterial)));

    auto pCamera = std::shared_ptr<camera>(std::move(pContext->make_camera()));
    pCamera->setProjection(90, 0.001, 20, 1);

    auto pScene = pContext->make_scene();
    
    pScene->add_camera(pCamera);
    pScene->add_entity(pEntity);

    //////////////////////////////////////////

    while(!window.shouldClose())
    {
        glfwPollEvents();

        pEntity->set_model_matrix(Vector3<float>{-0.25, -0.25, -0.5}, Quaternion<float>{});

        pScene->draw(window.getWindowSize());

        window.swapBuffer(); 
    }

    return 0;
}

