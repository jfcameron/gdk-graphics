// © Joseph Cameron - All Rights Reserved

#include "text_modeler.h"

#include <gdk/texture_data.h>

using namespace gdk;

static const std::vector<texture_data::encoded_byte> GLYPH_PNG_DATA { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x73, 0x7a, 0x7a, 0xf4, 0x00, 0x00, 0x00,
    0x06, 0x62, 0x4b, 0x47, 0x44, 0x00, 0x00, 0x00, 0xa2, 0x00, 0xe8, 0x8d,
    0x6e, 0x58, 0x3b, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00,
    0x00, 0x12, 0x74, 0x00, 0x00, 0x12, 0x74, 0x01, 0xde, 0x66, 0x1f, 0x78,
    0x00, 0x00, 0x00, 0x07, 0x74, 0x49, 0x4d, 0x45, 0x07, 0xe5, 0x0b, 0x09,
    0x14, 0x28, 0x2c, 0xf4, 0x2f, 0x97, 0xef, 0x00, 0x00, 0x01, 0xbe, 0x49,
    0x44, 0x41, 0x54, 0x58, 0xc3, 0xed, 0x56, 0xbb, 0xad, 0x84, 0x30, 0x10,
    0x1c, 0x4b, 0xee, 0xc0, 0x09, 0x01, 0x21, 0x65, 0x50, 0x0b, 0x85, 0x50,
    0xc0, 0x2b, 0x84, 0x5a, 0x5e, 0x19, 0x84, 0x04, 0x24, 0xd4, 0xe0, 0x17,
    0x9c, 0x0f, 0xd6, 0xfb, 0xb1, 0x0d, 0xba, 0xec, 0x9e, 0xa5, 0x93, 0x38,
    0xef, 0x32, 0x5e, 0xcf, 0xec, 0x07, 0x07, 0x00, 0x47, 0x3f, 0x44, 0x00,
    0x08, 0xdb, 0xea, 0xb0, 0xec, 0x11, 0x53, 0xe7, 0x40, 0xd6, 0xdb, 0x0e,
    0x00, 0xe1, 0xe7, 0xf7, 0x32, 0x24, 0xbf, 0xcc, 0xfe, 0xc6, 0xa0, 0x3e,
    0xe4, 0xff, 0x31, 0x8f, 0x2f, 0x1f, 0x0d, 0xfc, 0xe8, 0x87, 0x88, 0x65,
    0xcf, 0x7e, 0xd4, 0x0e, 0x00, 0x19, 0x38, 0xb3, 0x9f, 0xef, 0x5b, 0xfe,
    0xec, 0x3c, 0x00, 0xf0, 0xda, 0xe6, 0xc9, 0x00, 0x07, 0x2b, 0xac, 0xa3,
    0x1f, 0x62, 0xba, 0xd9, 0x15, 0x04, 0x63, 0x12, 0xcb, 0x1e, 0x31, 0x8f,
    0xd9, 0x79, 0xfe, 0xa4, 0x8d, 0x06, 0x42, 0x0e, 0x0e, 0xdb, 0xea, 0x32,
    0x8a, 0xa9, 0x7d, 0xea, 0x5c, 0x20, 0xef, 0x1d, 0xfd, 0x10, 0x03, 0x3f,
    0x98, 0x49, 0xa0, 0x9e, 0xc7, 0x69, 0xa4, 0xbf, 0x1a, 0x85, 0x45, 0xff,
    0xf4, 0x7c, 0xda, 0xe8, 0x7e, 0x0b, 0xbb, 0x1a, 0xa0, 0x15, 0x80, 0xc8,
    0xa1, 0x96, 0x1c, 0x59, 0xf6, 0xe8, 0x99, 0x7e, 0xfa, 0xa2, 0x9a, 0xf2,
    0x7c, 0xd1, 0x96, 0x42, 0xbb, 0x85, 0xe7, 0xd5, 0x5b, 0xe7, 0x5a, 0x45,
    0x9a, 0x94, 0x61, 0xea, 0x1c, 0x67, 0x41, 0x0d, 0x3e, 0x05, 0x41, 0x73,
    0x48, 0xe0, 0x69, 0x34, 0xf2, 0x32, 0xe4, 0x7b, 0x35, 0xcd, 0x6b, 0x94,
    0x73, 0xbb, 0xa7, 0xf4, 0x8b, 0x2c, 0xa6, 0x41, 0xa4, 0xbd, 0x20, 0x25,
    0xab, 0x4b, 0x62, 0x54, 0x55, 0xd8, 0xd6, 0x4b, 0x02, 0x91, 0xa5, 0xbc,
    0x9c, 0xac, 0xda, 0x6e, 0xe9, 0x0b, 0x2d, 0xce, 0x66, 0x19, 0x71, 0x39,
    0xb4, 0xc6, 0x75, 0xc3, 0xce, 0x25, 0x70, 0x42, 0x02, 0xda, 0xeb, 0xd3,
    0x8d, 0xc5, 0xac, 0xa8, 0xcd, 0x81, 0x42, 0x27, 0x15, 0xb3, 0x40, 0x6d,
    0x14, 0xac, 0x89, 0x98, 0xb7, 0xd2, 0x30, 0x2a, 0x0c, 0x66, 0x6c, 0x17,
    0x29, 0xb3, 0xca, 0xb3, 0x50, 0xdf, 0xda, 0x30, 0xe3, 0xb8, 0x42, 0x02,
    0xd1, 0x88, 0xf8, 0xcd, 0xe6, 0x31, 0xc3, 0xe0, 0x12, 0x51, 0xbb, 0xa0,
    0x96, 0x07, 0xc0, 0x64, 0x7b, 0xf9, 0x17, 0x68, 0x6a, 0x49, 0x32, 0xf5,
    0xf9, 0xc6, 0x14, 0x55, 0xe7, 0x7b, 0xcb, 0x70, 0xa9, 0x4a, 0x54, 0xcb,
    0x21, 0x2b, 0x18, 0xf3, 0x46, 0x85, 0x09, 0x56, 0x03, 0x2f, 0x4e, 0x5b,
    0x2b, 0xf3, 0x55, 0x50, 0xa3, 0x96, 0x4b, 0x81, 0xdc, 0x63, 0xe0, 0xae,
    0x6c, 0x86, 0x24, 0x77, 0x0e, 0xf3, 0x95, 0x36, 0x8a, 0xa6, 0xc9, 0xf7,
    0xc9, 0x65, 0xeb, 0x73, 0x0f, 0xa3, 0xd5, 0xc7, 0x8b, 0xef, 0xb5, 0xd4,
    0x7a, 0x3f, 0xc9, 0x00, 0xff, 0xe0, 0x11, 0x1f, 0x40, 0x6a, 0xdd, 0x17,
    0xb2, 0xf8, 0x51, 0x89, 0x5b, 0xfb, 0x34, 0x4b, 0x5b, 0xc1, 0xad, 0xa4,
    0x33, 0x2f, 0x61, 0x3d, 0x3f, 0x8a, 0xbe, 0xa5, 0xcc, 0x4a, 0x07, 0x5a,
    0xb8, 0xad, 0x12, 0x3c, 0x0a, 0xbc, 0x35, 0x88, 0xff, 0xf5, 0x95, 0xeb,
    0x0f, 0xc7, 0x9a, 0xa1, 0x14, 0x92, 0xeb, 0x41, 0xeb, 0x00, 0x00, 0x00,
    0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82,
};

text_modeler::text_modeler(gdk::graphics_context_ptr_type pContext, gdk::graphics_material_ptr_type aMaterial) 
: m_BatchModeler(pContext, aMaterial ? aMaterial : [&]() {
    auto pTexture = pContext->make_texture(texture_data::make_from_png(GLYPH_PNG_DATA).first);
    auto pMaterial(pContext->make_material(pContext->get_alpha_cutoff_shader(), material::render_mode::opaque));
    pMaterial->set_texture("_Texture", pTexture);
    pMaterial->set_vector2("_UVScale", {1, 1});
    pMaterial->set_vector2("_UVOffset", {0, 0});
    return pMaterial;
}()) {}

void text_modeler::set_text(const std::string &string) {
    static const auto GLYPH_PER_ROW_OR_COLUMN(8);
    static const auto GLYPH_UV_SIZE(1/8.f);
    static const std::unordered_map<char, graphics_vector2_type> CHAR_TO_UV({
        {'!', {GLYPH_UV_SIZE * 3, GLYPH_UV_SIZE * 4}},
        {'"', {GLYPH_UV_SIZE * 3, GLYPH_UV_SIZE * 5}},
        {'%', {GLYPH_UV_SIZE * 4, GLYPH_UV_SIZE * 4}},
        {'(', {GLYPH_UV_SIZE * 7, GLYPH_UV_SIZE * 6}}, 
        {')', {-GLYPH_UV_SIZE * 8, GLYPH_UV_SIZE * 6}},
        {'*', {GLYPH_UV_SIZE * 6, GLYPH_UV_SIZE * 4}},
        {'+', {GLYPH_UV_SIZE * 1, GLYPH_UV_SIZE * 5}},
        {',', {GLYPH_UV_SIZE * 7, GLYPH_UV_SIZE * 5}},
        {'-', {GLYPH_UV_SIZE * 7, GLYPH_UV_SIZE * 4}},
        {'.', {GLYPH_UV_SIZE * 6, GLYPH_UV_SIZE * 5}},
        {'/', {GLYPH_UV_SIZE * 5, GLYPH_UV_SIZE * 6}}, 
        {'0', {GLYPH_UV_SIZE * 6, GLYPH_UV_SIZE * 1}},
        {'1', {GLYPH_UV_SIZE * 2, GLYPH_UV_SIZE * 3}},
        {'2', {GLYPH_UV_SIZE * 3, GLYPH_UV_SIZE * 3}},
        {'3', {GLYPH_UV_SIZE * 4, GLYPH_UV_SIZE * 3}},
        {'4', {GLYPH_UV_SIZE * 5, GLYPH_UV_SIZE * 3}},
        {'5', {GLYPH_UV_SIZE * 6, GLYPH_UV_SIZE * 3}},
        {'6', {GLYPH_UV_SIZE * 7, GLYPH_UV_SIZE * 3}},
        {'7', {GLYPH_UV_SIZE * 0, GLYPH_UV_SIZE * 4}},
        {'8', {GLYPH_UV_SIZE * 1, GLYPH_UV_SIZE * 4}},
        {'9', {GLYPH_UV_SIZE * 2, GLYPH_UV_SIZE * 4}},
        {':', {GLYPH_UV_SIZE * 2, GLYPH_UV_SIZE * 6}},
        {';', {GLYPH_UV_SIZE * 1, GLYPH_UV_SIZE * 6}},
        {'<', {GLYPH_UV_SIZE * 6, GLYPH_UV_SIZE * 6}}, 
        {'=', {GLYPH_UV_SIZE * 2, GLYPH_UV_SIZE * 5}},
        {'>', {-GLYPH_UV_SIZE * 7, GLYPH_UV_SIZE * 6}},
        {'?', {GLYPH_UV_SIZE * 0, GLYPH_UV_SIZE * 6}},
        {'\'',{GLYPH_UV_SIZE * 4, GLYPH_UV_SIZE * 5}},
        {'\\',{-GLYPH_UV_SIZE * 6, GLYPH_UV_SIZE * 6}},
        {'^', {GLYPH_UV_SIZE * 5, GLYPH_UV_SIZE * 4}},
        {'_', {GLYPH_UV_SIZE * 0, GLYPH_UV_SIZE * 5}},
        {'`', {GLYPH_UV_SIZE * 3, GLYPH_UV_SIZE * 6}},
        {'a', {GLYPH_UV_SIZE * 0, GLYPH_UV_SIZE * 0}},
        {'b', {GLYPH_UV_SIZE * 1, GLYPH_UV_SIZE * 0}},
        {'c', {GLYPH_UV_SIZE * 2, GLYPH_UV_SIZE * 0}},
        {'d', {GLYPH_UV_SIZE * 3, GLYPH_UV_SIZE * 0}},
        {'e', {GLYPH_UV_SIZE * 4, GLYPH_UV_SIZE * 0}},
        {'f', {GLYPH_UV_SIZE * 5, GLYPH_UV_SIZE * 0}},
        {'g', {GLYPH_UV_SIZE * 6, GLYPH_UV_SIZE * 0}},
        {'h', {GLYPH_UV_SIZE * 7, GLYPH_UV_SIZE * 0}},
        {'i', {GLYPH_UV_SIZE * 0, GLYPH_UV_SIZE * 1}},
        {'j', {GLYPH_UV_SIZE * 1, GLYPH_UV_SIZE * 1}},
        {'k', {GLYPH_UV_SIZE * 2, GLYPH_UV_SIZE * 1}},
        {'l', {GLYPH_UV_SIZE * 3, GLYPH_UV_SIZE * 1}},
        {'m', {GLYPH_UV_SIZE * 4, GLYPH_UV_SIZE * 1}},
        {'n', {GLYPH_UV_SIZE * 5, GLYPH_UV_SIZE * 1}},
        {'o', {GLYPH_UV_SIZE * 6, GLYPH_UV_SIZE * 1}}, 
        {'p', {GLYPH_UV_SIZE * 7, GLYPH_UV_SIZE * 1}},
        {'q', {GLYPH_UV_SIZE * 0, GLYPH_UV_SIZE * 2}},
        {'r', {GLYPH_UV_SIZE * 1, GLYPH_UV_SIZE * 2}},
        {'s', {GLYPH_UV_SIZE * 2, GLYPH_UV_SIZE * 2}},
        {'t', {GLYPH_UV_SIZE * 3, GLYPH_UV_SIZE * 2}},
        {'u', {GLYPH_UV_SIZE * 4, GLYPH_UV_SIZE * 2}},
        {'v', {GLYPH_UV_SIZE * 5, GLYPH_UV_SIZE * 2}},
        {'w', {GLYPH_UV_SIZE * 6, GLYPH_UV_SIZE * 2}},
        {'x', {GLYPH_UV_SIZE * 7, GLYPH_UV_SIZE * 2}},
        {'y', {GLYPH_UV_SIZE * 0, GLYPH_UV_SIZE * 3}},
        {'z', {GLYPH_UV_SIZE * 1, GLYPH_UV_SIZE * 3}},
        {'|', {GLYPH_UV_SIZE * 5, GLYPH_UV_SIZE * 5}},
        {'~', {GLYPH_UV_SIZE * 4, GLYPH_UV_SIZE * 6}},
    });

    float x(0), y(0), z(0);

    for (const auto character : string) {
        if (character == '\r' || character == '\n') {
            x = 0;
            y++;
        }
        else if (character == ' ') {
            x++;
        }
        else if (character == '\t') {
            x += 2;
        }
        else {
            float uvMarginLow = 0.01f;
            float uvMarginHigh = 1 - uvMarginLow;

            gdk::model_data data({
                { "a_Position", {{
                    1.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    1.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, 0.0f,
                }, 3 }},
                { "a_UV", {{
                    uvMarginHigh, uvMarginLow, 
                    uvMarginLow,  uvMarginLow,
                    uvMarginLow,  uvMarginHigh, 
                    uvMarginHigh, uvMarginLow,
                    uvMarginLow,  uvMarginHigh,
                    uvMarginHigh, uvMarginHigh,
                }, 2 }}
            });
        
            data.transform("a_Position", {x * 1.1f, -y * 1.1f, z});
            data.transform("a_UV", CHAR_TO_UV.at(character), 0, {GLYPH_UV_SIZE});
            
            m_BatchModeler.push_back(data);

            x++;
        }
    }
}

const gdk::const_graphics_model_ptr_type text_modeler::model() const { return m_BatchModeler.model(); }
const gdk::const_graphics_material_ptr_type text_modeler::material() const { return m_BatchModeler.material(); }
void text_modeler::upload() { m_BatchModeler.upload(); }

