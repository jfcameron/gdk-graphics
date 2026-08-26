// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/animation/skeleton.h>

#include <gdk/math.h>

#include <algorithm>
#include <cmath>
#include <stdexcept>

using namespace gdk;

namespace gdk::graphics::animation {
    std::vector<matrix4x4_type> skeleton::bind_world_transforms() const {
        std::vector<matrix4x4_type> world(bones.size());

        for (std::size_t i = 0; i < bones.size(); ++i) {
            matrix4x4_type local;
            local.set_to_identity();
            local.set_rotation(bones[i].local_rotation);
            local.set_translation(bones[i].local_position);

            world[i] = bones[i].parent < 0
                ? local
                : world[static_cast<std::size_t>(bones[i].parent)] * local;
        }

        return world;
    }

    std::vector<matrix4x4_type> skeleton::inverse_bind_transforms() const {
        if (supplied_inverse_binds.size() == bones.size()) return supplied_inverse_binds;

        auto world = bind_world_transforms();

        for (auto &m : world) m = m.inversed();

        return world;
    }

    skeleton skeleton::with_oriented_bone_frames() const {
        const auto world = bind_world_transforms();

        std::vector<quaternion_type> worldRotation(bones.size(),
            quaternion_type::identity);

        for (std::size_t i = 0; i < bones.size(); ++i) {
            std::size_t child = bones.size();
            for (std::size_t c = i + 1; c < bones.size(); ++c)
                if (bones[c].parent == static_cast<int>(i)) { child = c; break; }

            if (child == bones.size()) {
                worldRotation[i] = bones[i].parent < 0
                    ? quaternion_type::identity
                    : worldRotation[static_cast<std::size_t>(bones[i].parent)];

                continue;
            }

            const auto along = world[child].translation() - world[i].translation();

            if (along.is_effectively_zero()) {
                worldRotation[i] = quaternion_type::identity;

                continue;
            }

            const auto y = along.normal();

            auto reference = vector3_type(0, 0, 1);
            if (std::abs(y.dot_product(reference)) > 0.99f)
                reference = vector3_type(1, 0, 0);

            const auto x = reference.cross_product(y).normal();
            const auto z = x.cross_product(y);

            matrix4x4_type frame;
            frame.set_to_identity();
            frame.set(0, 0, x.x); frame.set(0, 1, x.y); frame.set(0, 2, x.z);
            frame.set(1, 0, y.x); frame.set(1, 1, y.y); frame.set(1, 2, y.z);
            frame.set(2, 0, z.x); frame.set(2, 1, z.y); frame.set(2, 2, z.z);

            worldRotation[i] = frame.rotation();
        }

        skeleton result = *this;

        for (std::size_t i = 0; i < bones.size(); ++i) {
            matrix4x4_type desired;
            desired.set_to_identity();
            desired.set_rotation(worldRotation[i]);
            desired.set_translation(world[i].translation());

            if (bones[i].parent < 0) {
                result.bones[i].local_position = desired.translation();
                result.bones[i].local_rotation = worldRotation[i];

                continue;
            }

            const auto p = static_cast<std::size_t>(bones[i].parent);

            matrix4x4_type parentDesired;
            parentDesired.set_to_identity();
            parentDesired.set_rotation(worldRotation[p]);
            parentDesired.set_translation(world[p].translation());

            const auto local = parentDesired.inversed() * desired;

            result.bones[i].local_position = local.translation();
            result.bones[i].local_rotation = local.rotation();
        }

        return result;
    }

    vertical_extent bind_vertical_extent(const skeleton &aSkeleton) {
        const auto world = aSkeleton.bind_world_transforms();

        if (world.empty()) return {};

        vertical_extent extent{world.front().translation().y, world.front().translation().y};

        for (const auto &t : world) {
            extent.lowest = std::min(extent.lowest, t.translation().y);
            extent.highest = std::max(extent.highest, t.translation().y);
        }

        return extent;
    }

    int find_bone(const skeleton &aSkeleton, const std::string &aName) {
        for (std::size_t i = 0; i < aSkeleton.bones.size(); ++i)
            if (aSkeleton.bones[i].name == aName) return static_cast<int>(i);

        return -1;
    }

    std::vector<std::string> duplicate_bone_names(const skeleton &aSkeleton) {
        std::vector<std::string> names;

        for (const auto &bone : aSkeleton.bones)
            if (!bone.name.empty()) names.push_back(bone.name);

        std::sort(names.begin(), names.end());

        std::vector<std::string> duplicates;

        for (std::size_t i = 1; i < names.size(); ++i)
            if (names[i] == names[i - 1]
                && (duplicates.empty() || duplicates.back() != names[i]))
                duplicates.push_back(names[i]);

        return duplicates;
    }

    float limb_half_width(const skeleton &aSkeleton) {
        return bind_vertical_extent(aSkeleton).height() * 0.032f;
    }

    pose pose::rest(const skeleton &aSkeleton) {
        pose p;
        p.local_rotations.assign(aSkeleton.bones.size(), quaternion_type::identity);

        return p;
    }

    pose blend(const pose &aFrom, const pose &aTo, const float aT) {
        if (aFrom.local_rotations.size() != aTo.local_rotations.size())
            throw std::invalid_argument("blend: the poses disagree about bone count");

        pose result;
        result.local_rotations.reserve(aFrom.local_rotations.size());

        for (std::size_t i = 0; i < aFrom.local_rotations.size(); ++i)
            result.local_rotations.push_back(slerp(aFrom.local_rotations[i],
                aTo.local_rotations[i], aT));

        return result;
    }

    bone_mask mask_for_subtree(const skeleton &aSkeleton, const std::size_t aRoot,
        const float aWeight) {
        if (aRoot >= aSkeleton.bones.size())
            throw std::invalid_argument("mask_for_subtree: no such bone");

        bone_mask mask(aSkeleton.bones.size(), 0.0f);
        mask[aRoot] = aWeight;

        for (std::size_t i = 0; i < aSkeleton.bones.size(); ++i) {
            const auto parent = aSkeleton.bones[i].parent;

            if (parent >= 0 && mask[static_cast<std::size_t>(parent)] > 0 && i != aRoot)
                mask[i] = mask[static_cast<std::size_t>(parent)];
        }

        return mask;
    }

    pose blend(const pose &aFrom, const pose &aTo, const bone_mask &aMask) {
        if (aFrom.local_rotations.size() != aTo.local_rotations.size()
            || aMask.size() != aFrom.local_rotations.size())
            throw std::invalid_argument("blend: the poses and the mask disagree about bone count");

        pose result;
        result.local_rotations.reserve(aFrom.local_rotations.size());

        for (std::size_t i = 0; i < aFrom.local_rotations.size(); ++i)
            result.local_rotations.push_back(slerp(aFrom.local_rotations[i],
                aTo.local_rotations[i], aMask[i]));

        return result;
    }

    pose difference(const pose &aBase, const pose &aTarget) {
        if (aBase.local_rotations.size() != aTarget.local_rotations.size())
            throw std::invalid_argument("difference: the poses disagree about bone count");

        pose result;
        result.local_rotations.reserve(aBase.local_rotations.size());

        for (std::size_t i = 0; i < aBase.local_rotations.size(); ++i)
            result.local_rotations.push_back(aBase.local_rotations[i].inverse_unit()
                * aTarget.local_rotations[i]);

        return result;
    }

    pose add(const pose &aBase, const pose &aDelta, const bone_mask &aMask) {
        if (aBase.local_rotations.size() != aDelta.local_rotations.size()
            || aMask.size() != aBase.local_rotations.size())
            throw std::invalid_argument("add: the poses and the mask disagree about bone count");

        pose result;
        result.local_rotations.reserve(aBase.local_rotations.size());

        for (std::size_t i = 0; i < aBase.local_rotations.size(); ++i)
            result.local_rotations.push_back(aBase.local_rotations[i]
                * slerp(quaternion_type::identity, aDelta.local_rotations[i], aMask[i]));

        return result;
    }

    pose add(const pose &aBase, const pose &aDelta, const float aWeight) {
        return add(aBase, aDelta, bone_mask(aBase.local_rotations.size(), aWeight));
    }

    pose sample(const clip &aClip, const float aTime) {
        if (aClip.keys.empty()) throw std::invalid_argument("sample: the clip has no keys");

        if (!aClip.loops) {
            if (aTime <= aClip.keys.front().time) return aClip.keys.front().p;
            if (aTime >= aClip.keys.back().time) return aClip.keys.back().p;
        }

        const float wrapped = aClip.loops
            ? std::fmod(std::fmod(aTime, aClip.duration) + aClip.duration, aClip.duration)
            : aTime;

        for (std::size_t i = 0; i + 1 < aClip.keys.size(); ++i) {
            const auto &a = aClip.keys[i];
            const auto &b = aClip.keys[i + 1];

            if (wrapped < b.time) {
                const float span = b.time - a.time;

                return blend(a.p, b.p, span > 0 ? (wrapped - a.time) / span : 0.0f);
            }
        }

        const auto &last = aClip.keys.back();
        const float span = aClip.duration - last.time;

        return blend(last.p, aClip.keys.front().p,
            span > 0 ? (wrapped - last.time) / span : 0.0f);
    }

    vector3_type sample_root_position(const clip &aClip, const float aTime) {
        if (aClip.keys.empty())
            throw std::invalid_argument("sample_root_position: the clip has no keys");

        if (!aClip.loops) {
            if (aTime <= aClip.keys.front().time) return aClip.keys.front().root_position;
            if (aTime >= aClip.keys.back().time) return aClip.keys.back().root_position;
        }

        const float wrapped = aClip.loops
            ? std::fmod(std::fmod(aTime, aClip.duration) + aClip.duration, aClip.duration)
            : aTime;

        for (std::size_t i = 0; i + 1 < aClip.keys.size(); ++i) {
            const auto &a = aClip.keys[i];
            const auto &b = aClip.keys[i + 1];

            if (wrapped < b.time) {
                const float span = b.time - a.time;
                const float u = span > 0 ? (wrapped - a.time) / span : 0.0f;

                return a.root_position + (b.root_position - a.root_position) * u;
            }
        }

        const auto &last = aClip.keys.back();
        const float span = aClip.duration - last.time;
        const float u = span > 0 ? (wrapped - last.time) / span : 0.0f;
        const auto end = aClip.keys.front().root_position + aClip.root_motion_per_cycle;

        return last.root_position + (end - last.root_position) * u;
    }

    vector3_type root_delta(const clip &aClip, const float aFrom, const float aTo) {
        if (!aClip.loops)
            return sample_root_position(aClip, aTo) - sample_root_position(aClip, aFrom);

        const auto laps = std::floor(aTo / aClip.duration) - std::floor(aFrom / aClip.duration);

        return sample_root_position(aClip, aTo) - sample_root_position(aClip, aFrom)
            + aClip.root_motion_per_cycle * laps;
    }

    std::vector<matrix4x4_type> pose_world_transforms(const skeleton &aSkeleton,
        const pose &aPose) {
        std::vector<matrix4x4_type> world(aSkeleton.bones.size());

        for (std::size_t i = 0; i < aSkeleton.bones.size(); ++i) {
            matrix4x4_type local;
            local.set_to_identity();
            local.set_rotation(aSkeleton.bones[i].local_rotation * aPose.local_rotations[i]);
            local.set_translation(aSkeleton.bones[i].local_position);

            world[i] = aSkeleton.bones[i].parent < 0
                ? local
                : world[static_cast<std::size_t>(aSkeleton.bones[i].parent)] * local;
        }

        return world;
    }

    matrix4x4_type socket_transform(const std::vector<matrix4x4_type> &aWorld,
        const socket &aSocket) {
        if (aSocket.bone >= aWorld.size())
            throw std::invalid_argument("socket_transform: no such bone");

        return aWorld[aSocket.bone] * aSocket.offset;
    }

    quaternion_type bone_world_rotation(const skeleton &aSkeleton, const pose &aPose,
        const std::size_t aBone) {
        if (aPose.local_rotations.size() != aSkeleton.bones.size())
            throw std::invalid_argument(
                "bone_world_rotation: the pose does not match the skeleton");

        if (aBone >= aSkeleton.bones.size())
            throw std::invalid_argument("bone_world_rotation: no such bone");

        std::vector<std::size_t> chain;

        for (auto b = static_cast<int>(aBone); b >= 0;
            b = aSkeleton.bones[static_cast<std::size_t>(b)].parent)
            chain.push_back(static_cast<std::size_t>(b));

        auto q = quaternion_type::identity;

        for (auto it = chain.rbegin(); it != chain.rend(); ++it)
            q = q * (aSkeleton.bones[*it].local_rotation * aPose.local_rotations[*it]);

        return q;
    }

    std::vector<matrix4x4_type> skinning_palette(const skeleton &aSkeleton,
        const pose &aPose, const std::vector<matrix4x4_type> &aInverseBind) {
        auto palette = pose_world_transforms(aSkeleton, aPose);

        for (std::size_t i = 0; i < palette.size(); ++i) palette[i] = palette[i] * aInverseBind[i];

        return palette;
    }
}
