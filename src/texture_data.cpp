// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics_exception.h>
#include <gdk/texture_data.h>

#include <stb/stb_image.h>

#include <functional>
#include <iostream>
#include <memory>

using namespace gdk;
using namespace gdk::texture_data;

static inline decltype(STBI_rgb_alpha) textureFormatToSTBImageFormat(const texture::format a) {
    switch(a) {
        case texture::format::rgba: return STBI_rgb_alpha;
        case texture::format::rgb:  return STBI_rgb;
        case texture::format::grey: return STBI_grey;
        default: break;
    }
    throw graphics_exception("unhandled format type");
}
   
static inline std::pair<view, std::shared_ptr<decoded_data>> decodePNG(
    const byte_underlying_type *aDataStart, const size_t aLength, const texture::format aFormat = texture::format::rgba) {
    int width, height, components;
    if (std::unique_ptr<byte_underlying_type, std::function<void(byte_underlying_type *)>> decodedData(
        stbi_load_from_memory(aDataStart
            , static_cast<int>(aLength)
            , &width
            , &height
            , &components
            , textureFormatToSTBImageFormat(aFormat))
        , [](byte_underlying_type *p)
        {
            stbi_image_free(p);
        }); decodedData)
    {
        texture_data::view data;
        data.width = width;
        data.height = height;
        data.format = texture::format::rgba;

        std::pair<view, std::shared_ptr<decoded_data>> imageData;
        imageData.second = std::make_shared<decoded_data>(decodedData.get(), decodedData.get() + (width * height * 4));
        imageData.first = data;
        imageData.first.data = reinterpret_cast<std::byte *>(&imageData.second->front());
        
        return imageData;
    }
    
    throw graphics_exception("failed to decode provided PNG");
}

std::pair<view, std::shared_ptr<decoded_data>> gdk::texture_data::decode_from_png(const byte_underlying_type *aDataStart, const size_t aLength, 
    const texture::format aFormat) {
    return decodePNG(aDataStart, aLength, aFormat);
}

std::pair<view, std::shared_ptr<decoded_data>> gdk::texture_data::decode_from_png(const std::vector<byte_underlying_type> &aPNGBuffer, 
    const texture::format aFormat) {
    return gdk::texture_data::decode_from_png(&aPNGBuffer.front(), aPNGBuffer.size(), aFormat);
}

