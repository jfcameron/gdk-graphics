// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/animation/retarget.h>

#include <gdk/math.h>

#include <algorithm>
#include <stdexcept>

using namespace gdk;

namespace gdk::graphics::animation {
    namespace {
        [[nodiscard]] std::vector<quaternion_type> bind_world_rotations(const skeleton &aSkeleton) {
            std::vector<quaternion_type> world(aSkeleton.bones.size());

            for (std::size_t i = 0; i < aSkeleton.bones.size(); ++i)
                world[i] = aSkeleton.bones[i].parent < 0
                    ? aSkeleton.bones[i].local_rotation
                    : world[static_cast<std::size_t>(aSkeleton.bones[i].parent)]
                        * aSkeleton.bones[i].local_rotation;

            return world;
        }

        [[nodiscard]] std::vector<quaternion_type> pose_world_rotations(const skeleton &aSkeleton,
            const pose &aPose) {
            std::vector<quaternion_type> world(aSkeleton.bones.size());

            for (std::size_t i = 0; i < aSkeleton.bones.size(); ++i) {
                const auto local = aSkeleton.bones[i].local_rotation * aPose.local_rotations[i];

                world[i] = aSkeleton.bones[i].parent < 0
                    ? local
                    : world[static_cast<std::size_t>(aSkeleton.bones[i].parent)] * local;
            }

            return world;
        }

    }

    pose retarget(const skeleton &aSource, const pose &aSourcePose, const skeleton &aTarget,
        const bone_mapping &aMapping) {
        if (aSourcePose.local_rotations.size() != aSource.bones.size())
            throw std::invalid_argument("retarget: the pose does not belong to the source skeleton");

        if (aMapping.size() != aTarget.bones.size())
            throw std::invalid_argument("retarget: the mapping does not match the target skeleton");

        const auto sourceBind = bind_world_rotations(aSource);
        const auto sourceWorld = pose_world_rotations(aSource, aSourcePose);
        const auto targetBind = bind_world_rotations(aTarget);

        auto result = pose::rest(aTarget);

        std::vector<quaternion_type> targetWorld(aTarget.bones.size());

        for (std::size_t i = 0; i < aTarget.bones.size(); ++i) {
            const auto parent = aTarget.bones[i].parent;

            const auto parentWorld = parent < 0
                ? quaternion_type::identity
                : targetWorld[static_cast<std::size_t>(parent)];

            const auto source = aMapping[i];

            if (source < 0) {
                targetWorld[i] = parentWorld * aTarget.bones[i].local_rotation;

                continue;
            }

            const auto s = static_cast<std::size_t>(source);

            if (s >= aSource.bones.size())
                throw std::invalid_argument("retarget: the mapping names a bone the source lacks");

            const auto delta = sourceWorld[s] * sourceBind[s].inverse_unit();
            const auto wanted = delta * targetBind[i];

            result.local_rotations[i] =
                aTarget.bones[i].local_rotation.inverse_unit() * parentWorld.inverse_unit() * wanted;

            targetWorld[i] = wanted;
        }

        return result;
    }

    clip retarget(const skeleton &aSource, const clip &aSourceClip, const skeleton &aTarget,
        const bone_mapping &aMapping) {
        clip result;
        result.duration = aSourceClip.duration;
        result.loops = aSourceClip.loops;

        result.root_motion_per_cycle = aSourceClip.root_motion_per_cycle;

        for (const auto &key : aSourceClip.keys)
            result.keys.push_back({key.time, retarget(aSource, key.p, aTarget, aMapping),
                key.root_position});

        return result;
    }

    namespace {
        [[nodiscard]] quaternion_type reflected(const quaternion_type &aRotation) {
            quaternion_type result;

            result.x = aRotation.x;
            result.y = -aRotation.y;
            result.z = -aRotation.z;
            result.w = aRotation.w;

            return result;
        }
    }

    pose mirror_pose(const skeleton &aSkeleton, const rig_binding &aBinding, const pose &aPose) {
        if (aPose.local_rotations.size() != aSkeleton.bones.size())
            throw std::invalid_argument("mirror_pose: the pose does not match the skeleton");

        std::vector<std::size_t> source(aSkeleton.bones.size());

        for (std::size_t i = 0; i < source.size(); ++i) source[i] = i;

        for (const auto &role : aBinding.profile().roles) {
            const auto here = aBinding.bone_for(role.name);

            if (here < 0 || role.mirror.empty()) continue;

            const auto there = aBinding.bone_for(role.mirror);

            if (there >= 0) source[static_cast<std::size_t>(here)] =
                static_cast<std::size_t>(there);
        }

        const auto world = pose_world_rotations(aSkeleton, aPose);

        auto result = pose::rest(aSkeleton);

        std::vector<quaternion_type> mirrored(aSkeleton.bones.size());

        for (std::size_t i = 0; i < aSkeleton.bones.size(); ++i) {
            const auto parent = aSkeleton.bones[i].parent;

            const auto parentWorld = parent < 0
                ? quaternion_type::identity
                : mirrored[static_cast<std::size_t>(parent)];

            mirrored[i] = reflected(world[source[i]]);

            result.local_rotations[i] = aSkeleton.bones[i].local_rotation.inverse_unit()
                * parentWorld.inverse_unit() * mirrored[i];
        }

        return result;
    }

    clip mirror_clip(const skeleton &aSkeleton, const rig_binding &aBinding, const clip &aClip) {
        clip result;
        result.duration = aClip.duration;
        result.loops = aClip.loops;

        result.root_motion_per_cycle = {-aClip.root_motion_per_cycle.x,
            aClip.root_motion_per_cycle.y, aClip.root_motion_per_cycle.z};

        for (const auto &key : aClip.keys)
            result.keys.push_back({key.time, mirror_pose(aSkeleton, aBinding, key.p),
                {-key.root_position.x, key.root_position.y, key.root_position.z}});

        return result;
    }

    std::size_t mapped_bone_count(const bone_mapping &aMapping) {
        return static_cast<std::size_t>(std::count_if(aMapping.begin(), aMapping.end(),
            [](const int aSource) { return aSource >= 0; }));
    }
}
