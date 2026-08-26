// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_VOLUMETRIC_LIGHTING_H
#define GDK_GFX_VOLUMETRIC_LIGHTING_H

#include <gdk/graphics/color.h>
#include <gdk/graphics/constraints.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/cube_array.h>

#include <algorithm>
#include <array>

namespace gdk::graphics {
    /// \brief cubic volume of diffuse lighting data
    ///
    template<size_t size_param>
    class volumetric_lighting final {
    public:
        static constexpr size_t size = size_param;
        static constexpr size_t size_1d = size * size * size;
        static constexpr size_t size_2d = square_root(size_1d);

        static_assert(is_power_of_two(size));
        static_assert(is_power_of_two(size_1d));
        static_assert(is_power_of_two(size_2d));

        static constexpr size_t CHANNELS_PER_COLOR = 3;

        using texture_data_type = std::array<gdk::graphics::texture_data::channel_type, size_1d * CHANNELS_PER_COLOR>;
        using texture_data_view_pair = std::pair<gdk::graphics::texture_data::view, std::shared_ptr<texture_data_type>>;
        using local_space_component_type = int;

    private:
        gdk::graphics::cube_array<gdk::graphics::color, size> m_Data;

        void add(const local_space_component_type aX, const local_space_component_type aY, const local_space_component_type aZ, 
            const gdk::graphics::color &aColor) {
            // The cast is safe because the negative case has already returned; written out so the
            // signed/unsigned comparison is deliberate rather than accidental.
            if (aX < 0 || static_cast<std::size_t>(aX) >= size) return;
            if (aY < 0 || static_cast<std::size_t>(aY) >= size) return;
            if (aZ < 0 || static_cast<std::size_t>(aZ) >= size) return;

            auto &light = m_Data.at(aX, aY, aZ);
            light += aColor;
            light.clamp();
        }

    public:
        // \brief applies a uniform light to the whole volume
        void add_global(const gdk::graphics::color &aColor) {
            constexpr auto SIZE = static_cast<local_space_component_type>(size_param);

            for (local_space_component_type x(0); x < SIZE; ++x) 
                for (local_space_component_type y(0); y < SIZE; ++y) 
                    for (local_space_component_type z(0); z < SIZE; ++z) 
                        add(x, y, z, aColor);
        }

        /// \brief add a pointlight to the light volume
        void add_point_light(intvector3_type aLightPosition, const float aSize, const gdk::graphics::color &aColor) {
            const gdk::vector3<float> CENTRE(aSize/2.f); 
            const auto HALF(aSize/2.f);

            aLightPosition.x = static_cast<intvector3_type::component_type>(aLightPosition.x - HALF);
            aLightPosition.y = static_cast<intvector3_type::component_type>(aLightPosition.y - HALF);
            aLightPosition.z = static_cast<intvector3_type::component_type>(aLightPosition.z - HALF);

            for (int x(0); x < aSize; ++x) for (int y(0); y < aSize; ++y) for (int z(0); z < aSize; ++z) {
                float distanceFromCentre = CENTRE.distance_from(gdk::vector3<float>(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
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
                textureData[i + 0] = static_cast<gdk::graphics::texture_data::channel_type>(m_Data.data()[j].r * 255); //Converting normalized floating-point light values
                textureData[i + 1] = static_cast<gdk::graphics::texture_data::channel_type>(m_Data.data()[j].g * 255); //to unsigned char values 
                textureData[i + 2] = static_cast<gdk::graphics::texture_data::channel_type>(m_Data.data()[j].b * 255); 
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

