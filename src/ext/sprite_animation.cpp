// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/sprite_animation.h>

#include <cmath>
#include <string>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    constexpr const char *TAG = "gdk::graphics::sprite_animation: ";
}

sprite_animation::normalized_frame sprite_animation::at(time_type aTime) const {
    if (m_PlayMode == play_mode::once) {
        if (aTime >= mAnimationLength) return std::prev(m_Frames.end())->second;

        if (aTime < 0) aTime = 0;
    }
    else {
        aTime -= std::floor(aTime / mAnimationLength) * mAnimationLength;
    }

    return std::prev(m_Frames.upper_bound(aTime))->second;
}

bool sprite_animation::has_finished(const time_type aTime) const {
    return m_PlayMode == play_mode::once && aTime >= mAnimationLength;
}

sprite_animation::time_type sprite_animation::length() const { return mAnimationLength; }

size_t sprite_animation::frame_count() const { return m_Frames.size(); }

sprite_animation::sprite_animation(std::span<const frame> aFrameData,
    const texel_coordinate_type aTextureWidth,
    const texel_coordinate_type aTextureHeight,
    const play_mode aPlayMode)
: m_Frames([&]() {
    if (aFrameData.empty()) throw exception(std::string(TAG).append(
        "an animation needs at least one frame"));

    if (aTextureWidth <= 0 || aTextureHeight <= 0) throw exception(std::string(TAG).append(
        "the sheet's dimensions must be positive"));

    std::map<time_type, normalized_frame> out;

    time_type currentTime = 0;

    for (const auto &frame : aFrameData) {
        if (!(frame.length > 0)) throw exception(std::string(TAG).append(
            "every frame needs a length greater than zero"));

        if (frame.x < 0 || frame.y < 0 || frame.w <= 0 || frame.h <= 0 ||
            frame.x + frame.w > aTextureWidth || frame.y + frame.h > aTextureHeight)
            throw exception(std::string(TAG).append("a frame does not fit inside the sheet"));

        out[currentTime] = {
            .u = static_cast<normalized_texture_coordinate_type>(frame.x) / aTextureWidth,
            .v = static_cast<normalized_texture_coordinate_type>(frame.y) / aTextureHeight,
            .w = static_cast<normalized_texture_coordinate_type>(frame.w) / aTextureWidth,
            .h = static_cast<normalized_texture_coordinate_type>(frame.h) / aTextureHeight
        };

        currentTime += frame.length;
    }

    return out;
}())
, mAnimationLength([&]() {
    time_type totalTime = 0;

    for (const auto &frame : aFrameData) totalTime += frame.length;

    return totalTime;
}())
, m_PlayMode(aPlayMode)
{}
