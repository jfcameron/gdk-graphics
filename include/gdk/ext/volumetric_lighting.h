// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VOLUMETRIC_LIGHTING_H
#define GDK_GFX_VOLUMETRIC_LIGHTING_H

#include <gdk/color.h>
#include <gdk/graphics_constraints.h>
#include <gdk/texture_data.h>
#include <jfc/cube_array.h>

#include <algorithm>
#include <array>

namespace gdk {
    /// \brief cubic volume of diffuse lighting data
    ///
    template<size_t size_param>
    class volumetric_lighting final {
    public:
        static constexpr size_t size = size_param;
        static constexpr size_t size_1d = size * size * size;
        static constexpr size_t size_2d = std::sqrt(size_1d);

        static_assert(is_power_of_two(size));
        static_assert(is_power_of_two(size_1d));
        static_assert(is_power_of_two(size_2d));

        static constexpr size_t CHANNELS_PER_COLOR = 3;

        using texture_data_type = std::array<gdk::texture_data::channel_type, size_1d * CHANNELS_PER_COLOR>;
        using texture_data_view_pair = std::pair<gdk::texture_data::view, std::shared_ptr<texture_data_type>>;
        using local_space_component_type = int;

    private:
        jfc::cube_array<gdk::color, size> m_Data;

        void add(const local_space_component_type aX, const local_space_component_type aY, const local_space_component_type aZ, 
            const gdk::color &aColor) {
            if (aX < 0 || aX >= size) return;
            if (aY < 0 || aY >= size) return;
            if (aZ < 0 || aZ >= size) return;

            auto &light = m_Data.at(aX, aY, aZ);
            light += aColor;
            light.clamp();
        }

    public:
        // \brief applies a uniform light to the whole volume
        void add_global(const gdk::color &aColor) {
            for (size_t x(0); x < size_param; ++x) 
                for (size_t y(0); y < size_param; ++y) 
                    for (size_t z(0); z < size_param; ++z) 
                        add(x, y, z, aColor);
        }

        /// \brief add a pointlight to the light volume
        void add_point_light(graphics_intvector3_type aLightPosition, const float aSize, const gdk::color &aColor) {
            const gdk::vector3<float> CENTRE(aSize/2.f); 
            const auto HALF(aSize/2.f);

            aLightPosition.x -= HALF;
            aLightPosition.y -= HALF;
            aLightPosition.z -= HALF;

            for (int x(0); x < aSize; ++x) for (int y(0); y < aSize; ++y) for (int z(0); z < aSize; ++z) {
                float distanceFromCentre = CENTRE.distance(gdk::vector3<float>(x,y,z));
                float normalizedHalfDistanceFromCentre = distanceFromCentre / HALF; 
                float intensity = (1.0f / std::sqrt(normalizedHalfDistanceFromCentre)) - 1.0f; 
                intensity = std::clamp(intensity, 0.0f, 1.0f);

                auto color(aColor);
                color.r *= intensity;
                color.g *= intensity;
                color.b *= intensity;

                add(aLightPosition.x + x, aLightPosition.y + y, aLightPosition.z + z, color);   
            }
        }

        // \brief Writes the lighting data to a 2d texture, for use with shaders etc
        texture_data_view_pair to_texture_data() const {
            texture_data_type textureData;

            size_t i(0);
            for (size_t j(0); j < size_1d; ++j) {
                textureData[i + 0] = m_Data.data()[j].r * 255; //Converting normalized floating-point light values
                textureData[i + 1] = m_Data.data()[j].g * 255; //to unsigned char values 
                textureData[i + 2] = m_Data.data()[j].b * 255; 
                i += CHANNELS_PER_COLOR;
            }

            auto pTextureData = std::make_shared<texture_data_type>(std::move(textureData));

            texture_data::view view;
            view.width = size_2d;
            view.height = size_2d;
            view.format = texture::format::rgb;
            view.data = &(pTextureData->front());

            return { view, pTextureData };
        }
    };
}

#endif

