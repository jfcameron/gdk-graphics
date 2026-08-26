// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/png.h>

#include <stb/stb_image.h>

#include <functional>
#include <iostream>
#include <memory>

using namespace gdk;
using namespace gdk::graphics;
using namespace gdk::graphics::texture_data;

static inline decltype(STBI_rgb_alpha) textureFormatToSTBImageFormat(const texture::format a) {
    switch(a) {
        case texture::format::rgba: return STBI_rgb_alpha;
        case texture::format::rgb:  return STBI_rgb;
        case texture::format::grey: return STBI_grey;
        default: break;
    }
    throw exception("unhandled texture format to STBI format case");
}

static inline char textureFormatToChannelCount(const texture::format a) {
    switch(a) {
        case texture::format::rgba: return 4;
        case texture::format::rgb:  return 3;
        case texture::format::grey: return 1;
        default: break;
    }
    throw exception("unhandled texture format to channel count case");
}
   
static inline std::pair<view, std::shared_ptr<channel_data>> decodePNG(
    const encoded_byte *aDataStart, const size_t aLength, const texture::format aFormat = texture::format::rgba) {
    int width, height, components;
    if (std::unique_ptr<channel_type, std::function<void(channel_type *)>> decodedData(
        stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(aDataStart)
            , static_cast<int>(aLength)
            , &width
            , &height
            , &components
            , textureFormatToSTBImageFormat(aFormat))
        , [](channel_type *p)
        {
            stbi_image_free(p);
        }); decodedData) {
        texture_data::view data;
        data.width = width;
        data.height = height;
        data.format = texture::format::rgba;

        std::pair<view, std::shared_ptr<channel_data>> imageData;
        imageData.second = std::make_shared<channel_data>(decodedData.get(), 
            decodedData.get() + (width * height * textureFormatToChannelCount(aFormat)));
        imageData.first = data;
        imageData.first.data = static_cast<channel_type *>(&imageData.second->front());
        
        return imageData;
    }
    
    throw exception("failed to decode PNG");
}

std::pair<view, std::shared_ptr<channel_data>> gdk::graphics::ext::make_from_png(
    std::span<const encoded_byte> aPNGData, const texture::format aFormat) {
    return decodePNG(aPNGData.data(), aPNGData.size(), aFormat);
}

