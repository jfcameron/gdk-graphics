// © Joseph Cameron - All Rights Reserved

#ifndef GDK_SPRITE_ANIMATION_H
#define GDK_SPRITE_ANIMATION_H

#include <gdk/graphics_types.h>

#include <map>

namespace gdk {
    /// \brief non-interpolated 2d sprite animation
    ///
    class sprite_animation final {
    public:
        using normalized_texture_coordinate_type = float;
        using normalized_time_type = float;
        using texel_coordinate_type = int;
        /// \brief animation frame position and size in texels
        struct frame {
            texel_coordinate_type x;
            texel_coordinate_type y;
            texel_coordinate_type w;
            texel_coordinate_type h;
        };
        /// \brief animation frame position and size in normalized texture coordinate space
        struct normalized_frame {
            normalized_texture_coordinate_type u;
            normalized_texture_coordinate_type v;
            normalized_texture_coordinate_type w;
            normalized_texture_coordinate_type h;
        };
        using frame_collection_type = std::map<normalized_time_type, frame>;

        /// \brief gets the active frame at the given time in normalized texture coordinates
        normalized_frame at(const normalized_time_type aNormalizedTime, 
            const texel_coordinate_type aTextureWidth, 
            const texel_coordinate_type aTextureHeight) const;

        sprite_animation(frame_collection_type aFrameData);

    private:
        std::map<normalized_time_type, frame> m_Frames;
    };
}

#endif

