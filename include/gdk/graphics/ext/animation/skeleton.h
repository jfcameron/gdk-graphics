// © Joseph Cameron - All Rights Reserved

#ifndef GDK_DEMO_SKELETON_H
#define GDK_DEMO_SKELETON_H

#include <gdk/graphics/ext/animation/animation_types.h>

#include <cstddef>
#include <string>
#include <vector>

namespace gdk::graphics::animation {
    /// \brief a chain of bones in bind pose.
    struct skeleton final {
        struct bone final {
            int parent = -1;
            vector3_type local_position;
            quaternion_type local_rotation;
            std::string name;
        };

        std::vector<bone> bones;
        std::vector<matrix4x4_type> supplied_inverse_binds;

        //! world-space transform of each bone in bind pose
        [[nodiscard]] std::vector<matrix4x4_type> bind_world_transforms() const;

        /// \brief the same skeleton with each bone's local frame pointing +Y at its first child
        [[nodiscard]] skeleton with_oriented_bone_frames() const;

        /// \brief the inverse of each bind-pose world transform
        [[nodiscard]] std::vector<matrix4x4_type> inverse_bind_transforms() const;
    };

    /// \brief the vertical span of a rig in bind pose
    struct vertical_extent final {
        float lowest = 0;
        float highest = 0;

        [[nodiscard]] float height() const { return highest - lowest; }
    };

    [[nodiscard]] vertical_extent bind_vertical_extent(const skeleton &aSkeleton);

    /// \brief how thick a limb is on this rig, as a proportion of it
    [[nodiscard]] float limb_half_width(const skeleton &aSkeleton);

    /// \brief the index of a bone by name, or -1
    [[nodiscard]] int find_bone(const skeleton &aSkeleton, const std::string &aName);

    /// \brief every name carried by more than one bone, sorted, without repeats
    [[nodiscard]] std::vector<std::string> duplicate_bone_names(const skeleton &aSkeleton);

    /// \brief one bone rotation per bone, relative to its parent.
    struct pose final {
        std::vector<quaternion_type> local_rotations;

        [[nodiscard]] static pose rest(const skeleton &aSkeleton);
    };

    /// \brief interpolate between two poses, bone by bone
    [[nodiscard]] pose blend(const pose &aFrom, const pose &aTo, const float aT);

    /// \brief how much of a layer each bone takes: 0 keeps the base, 1 takes the layer entirely.
    using bone_mask = std::vector<float>;

    /// \brief a mask covering one bone and everything beneath it
    [[nodiscard]] bone_mask mask_for_subtree(const skeleton &aSkeleton, const std::size_t aRoot,
        const float aWeight = 1.0f);

    /// \brief blend per bone rather than uniformly -- the upper body from one clip, the legs another
    [[nodiscard]] pose blend(const pose &aFrom, const pose &aTo, const bone_mask &aMask);

    /// \brief the per-bone rotation that takes `aBase` to `aTarget`
    [[nodiscard]] pose difference(const pose &aBase, const pose &aTarget);

    /// \brief apply a delta on top of a base pose, scaled per bone
    [[nodiscard]] pose add(const pose &aBase, const pose &aDelta, const bone_mask &aMask);

    //! apply a delta uniformly, for a gesture that covers the whole body
    [[nodiscard]] pose add(const pose &aBase, const pose &aDelta, const float aWeight);

    /// \brief a looping sequence of keyframed poses
    struct clip final {
        struct key final {
            float time = 0;
            pose p;
            vector3_type root_position;
        };

        std::vector<key> keys;
        float duration = 1.0f;
        vector3_type root_motion_per_cycle;
        bool loops = true;
    };

    /// \brief the pose at a time, looping
    [[nodiscard]] pose sample(const clip &aClip, const float aTime);

    //! the root's position within one cycle, wrapping like `sample` and ignoring laps
    [[nodiscard]] vector3_type sample_root_position(const clip &aClip, const float aTime);

    /// \brief how far the root travelled between two times, laps included
    [[nodiscard]] vector3_type root_delta(const clip &aClip, const float aFrom, const float aTo);

    //! world-space transform of each bone under a pose
    [[nodiscard]] std::vector<matrix4x4_type> pose_world_transforms(
        const skeleton &aSkeleton, const pose &aPose);

    /// \brief a thing attached to a bone: which bone, and where it sits in that bone's space.
    struct socket final {
        std::size_t bone = 0;
        matrix4x4_type offset;
    };

    /// \brief where a socket's attachment ends up, given the pose's world transforms
    [[nodiscard]] matrix4x4_type socket_transform(const std::vector<matrix4x4_type> &aWorld,
        const socket &aSocket);

    /// \brief the world rotation a bone has under a pose
    [[nodiscard]] quaternion_type bone_world_rotation(const skeleton &aSkeleton, const pose &aPose,
        const std::size_t aBone);

    /// \brief the matrices that take a bind-pose vertex to its posed position
    [[nodiscard]] std::vector<matrix4x4_type> skinning_palette(
        const skeleton &aSkeleton, const pose &aPose,
        const std::vector<matrix4x4_type> &aInverseBind
    );
}

#endif

