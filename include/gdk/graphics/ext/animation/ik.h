// © Joseph Cameron - All Rights Reserved

#ifndef GDK_DEMO_IK_H
#define GDK_DEMO_IK_H

#include <gdk/graphics/ext/animation/skeleton.h>

#include <functional>
#include <vector>
#include <span>

/// \file two-bone inverse kinematics
namespace gdk::graphics::animation {
    [[nodiscard]] pose with_limb_reaching(const skeleton &aSkeleton, const pose &aPose,
        const std::size_t aUpper, const vector3_type &aTarget, const vector3_type &aPoleDirection);

    [[nodiscard]] pose with_limb_reaching_in_world(const skeleton &aSkeleton, const pose &aPose,
        const std::size_t aUpper, const vector3_type &aWorldTarget,
        const vector3_type &aWorldPoleDirection, const vector3_type &aCharacterPosition,
        const quaternion_type &aCharacterRotation);

    struct foot_arc final {
        float lowest = 0;
        float highest = 0;

        [[nodiscard]] float range() const { return highest - lowest; }
    };

    [[nodiscard]] foot_arc measure_foot_arc(const skeleton &aSkeleton, const clip &aClip,
        const std::size_t aFoot);

    [[nodiscard]] pose with_bone_oriented(const skeleton &aSkeleton, const pose &aPose,
        const std::size_t aBone, const quaternion_type &aWorldRotation);

    [[nodiscard]] pose with_bone_oriented_in_world(const skeleton &aSkeleton, const pose &aPose,
        const std::size_t aBone, const quaternion_type &aWorldRotation,
        const quaternion_type &aCharacterRotation);

    [[nodiscard]] float stance_weight(const foot_arc &aArc, const float aFootHeight,
        const float aBand = 0.25f);

    /// Spans rather than `const std::vector &`: these are read, not kept, so the container the caller
    /// holds them in is not this function's business. A caller with a `std::array` or a subrange no
    /// longer has to copy into a vector to call it.
    [[nodiscard]] float pelvis_lift(std::span<const matrix4x4_type> aWorld,
        std::span<const std::size_t> aFeet, const vector3_type &aCharacterPosition,
        const quaternion_type &aCharacterRotation,
        const std::function<float(float, float)> &aGroundHeight);
}

#endif
