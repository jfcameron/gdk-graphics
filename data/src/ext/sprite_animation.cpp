// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/sprite_animation.h>

#include <algorithm>
#include <cmath>
#include <string>

using namespace gdk;
using namespace gdk::graphics;

namespace {
    constexpr const char *TAG = "gdk::graphics::sprite_animation: ";
}

sprite_animation::normalized_frame sprite_animation::at(const time_type aTime) const {
    const double length = m_Length;

    double time = aTime;

    if (m_PlayMode == play_mode::once) {
        if (time >= length) return m_Frames.back();
    }
    else time -= std::floor(time / length) * length;

    if (!(time > 0)) return m_Frames.front();

    const auto after = std::upper_bound(m_Starts.begin(), m_Starts.end(), time);

    return m_Frames[static_cast<std::size_t>(std::max<std::ptrdiff_t>(0, after - m_Starts.begin() - 1))];
}

bool sprite_animation::has_finished(const time_type aTime) const {
    return m_PlayMode == play_mode::once && aTime >= m_Length;
}

sprite_animation::time_type sprite_animation::length() const { return static_cast<time_type>(m_Length); }

size_t sprite_animation::frame_count() const { return m_Frames.size(); }

sprite_animation::sprite_animation(std::span<const frame> aFrameData,
    const texel_coordinate_type aTextureWidth,
    const texel_coordinate_type aTextureHeight,
    const play_mode aPlayMode)
: m_PlayMode(aPlayMode) {
    if (aFrameData.empty()) throw exception(std::string(TAG).append("an animation needs at least one frame"));

    if (aTextureWidth <= 0 || aTextureHeight <= 0)
        throw exception(std::string(TAG).append("the sheet's dimensions must be positive"));

    for (const auto &frame : aFrameData) {
        if (!(frame.length > 0) || !std::isfinite(frame.length))
            throw exception(std::string(TAG).append("every frame needs a length greater than zero"));

        if (frame.x < 0 || frame.y < 0 || frame.w <= 0 || frame.h <= 0
            || frame.x > aTextureWidth - frame.w || frame.y > aTextureHeight - frame.h)
            throw exception(std::string(TAG).append("a frame does not fit inside the sheet"));

        m_Starts.push_back(m_Length);

        m_Frames.push_back({
            .u = static_cast<normalized_texture_coordinate_type>(frame.x) / aTextureWidth,
            .v = static_cast<normalized_texture_coordinate_type>(frame.y) / aTextureHeight,
            .w = static_cast<normalized_texture_coordinate_type>(frame.w) / aTextureWidth,
            .h = static_cast<normalized_texture_coordinate_type>(frame.h) / aTextureHeight
        });

        m_Length += frame.length;
    }
}
