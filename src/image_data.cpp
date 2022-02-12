// © Joseph Cameron - All Rights Reserved

#include <gdk/image_data.h>

#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <stb/stb_image.h>

using namespace gdk;
    
decode_image::image_data_and_view_2d_pair decode_image::decode_png_rgba32bit(const byte_type *aDataStart, size_t aLength)
{
    int width, height, components;

    if (std::unique_ptr<byte_type, std::function<void(byte_type *)>> decodedData(
        stbi_load_from_memory(aDataStart
            , static_cast<int>(aLength)
            , &width
            , &height
            , &components
            , STBI_rgb_alpha)
        , [](byte_type *p)
        {
            stbi_image_free(p);
        }); decodedData)
    {
        image_data_2d_view data;
        data.width = width;
        data.height = height;
        data.format = texture::data_format::rgba;

        image_data_and_view_2d_pair imageData;
        imageData.second = std::make_shared<decoded_data>(decodedData.get(), decodedData.get() + (width * height * 4));
        imageData.first = data;
        imageData.first.data = reinterpret_cast<std::byte *>(&imageData.second->front());
        
        return imageData;
    }
    
    throw std::runtime_error(std::string("decoder").append(": could not decode RGBA32bit data provided"));
}

decode_image::image_data_and_view_2d_pair decode_image::decode_png_rgba32bit(
    const std::vector<byte_type> &aPNGBuffer)
{
    return decode_png_rgba32bit(&aPNGBuffer.front(), aPNGBuffer.size());
}

