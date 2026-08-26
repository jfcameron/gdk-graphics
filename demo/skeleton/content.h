// © Joseph Cameron - All Rights Reserved

#ifndef GDK_DEMO_CONTENT_H
#define GDK_DEMO_CONTENT_H

#include <gdk/graphics/ext/animation/skeleton.h>

#include <cstddef>

/// \file the rig and the animations used by the demo
namespace demo {
    using namespace gdk::graphics::animation;

    [[nodiscard]] clip make_walk_clip(const skeleton &aSkeleton, const std::size_t aKeyCount);

    struct gait final {
        float duration = 1.0f;         
        float hip_swing = 28.0f;
        float knee_bend = 55.0f;
        float ankle_level = 30.0f;
        float ankle_swing = 10.0f;
        float shoulder_swing = 22.0f;
        float elbow_fold = 30.0f;
        float elbow_rest = 8.0f;
        float hip_yaw = 7.0f;
        float chest_yaw = 9.0f;
        float lean = 0.0f;             

        float speed = 1.35f;
    };

    [[nodiscard]] gait walking();
    [[nodiscard]] gait running();

    [[nodiscard]] gait idling();

    [[nodiscard]] clip make_wave_clip(const skeleton &aSkeleton, const std::size_t aKeyCount);

    //! the generator behind `make_walk_clip`, which is `make_gait_clip(s, walking(), n)`
    [[nodiscard]] clip make_gait_clip(const skeleton &aSkeleton, const gait &aGait,
        const std::size_t aKeyCount);

    struct humanoid final {
        enum bone_index : std::size_t {
            hips = 0, spine, chest, head,
            left_shoulder, left_elbow, left_hand,
            right_shoulder, right_elbow, right_hand,
            left_hip, left_knee, left_foot,
            right_hip, right_knee, right_foot,
            count
        };
    };

    [[nodiscard]] skeleton make_humanoid();
}

#endif
