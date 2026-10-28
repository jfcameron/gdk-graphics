// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXT_ANIMATION_RETARGET_H
#define GDK_GRAPHICS_EXT_ANIMATION_RETARGET_H

#include <gdk/graphics/ext/animation/profile.h>
#include <gdk/graphics/ext/animation/skeleton.h>

#include <string>
#include <vector>

namespace gdk::graphics::animation {
    using bone_mapping = std::vector<int>;

    /// \brief transfer one pose from the source rig to the target rig
    [[nodiscard]] pose retarget(const skeleton &aSource, const pose &aSourcePose,
        const skeleton &aTarget, const bone_mapping &aMapping);

    //! transfer a whole clip, key times and duration preserved
    [[nodiscard]] clip retarget(const skeleton &aSource, const clip &aSourceClip,
        const skeleton &aTarget, const bone_mapping &aMapping);

    /// \brief how many bones a mapping actually places
    [[nodiscard]] std::size_t mapped_bone_count(const bone_mapping &aMapping);

    /// \brief the same pose performed by the other side of the body, mirrored across the rig's YZ plane
    ///
    /// Each bone takes its mirror's turn from bind, reflected, so the rest pose mirrors to itself whatever the
    /// bones' bind frames are. \throws gdk::graphics::exception for a binding made against another skeleton
    [[nodiscard]] pose mirror_pose(const skeleton &aSkeleton, const rig_binding &aBinding,
        const pose &aPose);

    /// \brief mirror a whole clip, root motion included
    [[nodiscard]] clip mirror_clip(const skeleton &aSkeleton, const rig_binding &aBinding,
        const clip &aClip);
}

#endif
