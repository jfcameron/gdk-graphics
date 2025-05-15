// © Joseph Cameron - All Rights Reserved

#include <gdk/ext/sprite_animation.h>

#include <cmath>

using namespace gdk;

static inline const sprite_animation::frame &get_frame(const sprite_animation::frame_collection_type &aFrameCollection, 
    sprite_animation::normalized_time_type aNormalizedTime) {
    aNormalizedTime = aNormalizedTime - std::floor(aNormalizedTime);
    const auto it = aFrameCollection.upper_bound(aNormalizedTime); 
    return (it == aFrameCollection.begin()) ? it->second : std::prev(it)->second;
}

sprite_animation::normalized_frame sprite_animation::at(const normalized_time_type aNormalizedTime, 
    const texel_coordinate_type aTextureWidth, const texel_coordinate_type aTextureHeight) const {
    auto frame = get_frame(m_Frames, aNormalizedTime);

    return {
        static_cast<normalized_texture_coordinate_type>(frame.x) / aTextureWidth,
        static_cast<normalized_texture_coordinate_type>(frame.y) / aTextureHeight,
        static_cast<normalized_texture_coordinate_type>(frame.w) / aTextureWidth,
        static_cast<normalized_texture_coordinate_type>(frame.h) / aTextureHeight
    };
}

sprite_animation::sprite_animation(frame_collection_type aFrameData)
: m_Frames(std::move(aFrameData)) {}

