// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/animation/ik.h>

#include <gdk/math.h>

#include <algorithm>
#include <cmath>
#include <stdexcept>

using namespace gdk;

namespace gdk::graphics::animation {
    namespace {
        [[nodiscard]] std::size_t first_child_of(const skeleton &aSkeleton, const std::size_t aBone) {
            for (std::size_t i = aBone + 1; i < aSkeleton.bones.size(); ++i)
                if (aSkeleton.bones[i].parent == static_cast<int>(aBone)) return i;

            return aSkeleton.bones.size();
        }

        [[nodiscard]] quaternion_type aim(const quaternion_type &aParentWorld,
            const quaternion_type &aBindLocal, const vector3_type &aCurrentWorldDirection,
            const vector3_type &aDesiredWorldDirection, const quaternion_type &aCurrentLocal) {
            const auto from = aCurrentWorldDirection.normal();
            const auto to = aDesiredWorldDirection.normal();

            const auto dot = std::max(-1.0f, std::min(1.0f, from.dot_product(to)));

            if (dot > 0.999999f) return aCurrentLocal;

            auto axis = from.cross_product(to);

            if (axis.is_effectively_zero()) {
                axis = std::abs(from.x) < 0.9f
                    ? from.cross_product(vector3_type(1, 0, 0))
                    : from.cross_product(vector3_type(0, 1, 0));
            }

            const auto correction = quaternion_type::from_angle_axis(std::acos(dot), axis.normal());

            const auto worldLocal = aParentWorld * aBindLocal;

            return worldLocal.inverse_unit() * correction * worldLocal * aCurrentLocal;
        }
    }

    pose with_bone_oriented(const skeleton &aSkeleton, const pose &aPose, const std::size_t aBone,
        const quaternion_type &aWorldRotation) {
        if (aBone >= aSkeleton.bones.size())
            throw std::invalid_argument("with_bone_oriented: no such bone");

        if (aPose.local_rotations.size() != aSkeleton.bones.size())
            throw std::invalid_argument("with_bone_oriented: the pose does not match the skeleton");

        const auto parent = aSkeleton.bones[aBone].parent;

        const auto parentWorld = parent < 0
            ? quaternion_type::identity
            : bone_world_rotation(aSkeleton, aPose, static_cast<std::size_t>(parent));

        auto result = aPose;

        result.local_rotations[aBone] =
            (parentWorld * aSkeleton.bones[aBone].local_rotation).inverse_unit() * aWorldRotation;

        return result;
    }

    pose with_bone_oriented_in_world(const skeleton &aSkeleton, const pose &aPose,
        const std::size_t aBone, const quaternion_type &aWorldRotation,
        const quaternion_type &aCharacterRotation) {
        return with_bone_oriented(aSkeleton, aPose, aBone,
            aCharacterRotation.inverse_unit() * aWorldRotation);
    }

    foot_arc measure_foot_arc(const skeleton &aSkeleton, const clip &aClip,
        const std::size_t aFoot) {
        if (aClip.keys.empty())
            throw std::invalid_argument("measure_foot_arc: the clip has no keys");

        if (aFoot >= aSkeleton.bones.size())
            throw std::invalid_argument("measure_foot_arc: no such bone");

        auto height = [&](const pose &aPose) {
            return pose_world_transforms(aSkeleton, aPose)[aFoot].translation().y;
        };

        foot_arc arc{height(aClip.keys.front().p), height(aClip.keys.front().p)};

        for (const auto &key : aClip.keys) {
            const auto y = height(key.p);

            arc.lowest = std::min(arc.lowest, y);
            arc.highest = std::max(arc.highest, y);
        }

        return arc;
    }

    float stance_weight(const foot_arc &aArc, const float aFootHeight, const float aBand) {
        if (aArc.range() < 0.0001f) return 1.0f;

        const auto above = (aFootHeight - aArc.lowest) / (aArc.range() * std::max(aBand, 0.0001f));

        return std::max(0.0f, std::min(1.0f, 1.0f - above));
    }

    float pelvis_lift(std::span<const matrix4x4_type> aWorld,
        std::span<const std::size_t> aFeet, const vector3_type &aCharacterPosition,
        const quaternion_type &aCharacterRotation,
        const std::function<float(float, float)> &aGroundHeight) {
        float lift = 0;

        for (const auto foot : aFeet) {
            if (foot >= aWorld.size())
                throw std::invalid_argument("pelvis_lift: no such bone");

            const auto placed = (aCharacterRotation * aWorld[foot].translation())
                + aCharacterPosition;

            lift = std::max(lift, aGroundHeight(placed.x, placed.z) - placed.y);
        }

        return lift;
    }

    pose with_limb_reaching_in_world(const skeleton &aSkeleton, const pose &aPose,
        const std::size_t aUpper, const vector3_type &aWorldTarget,
        const vector3_type &aWorldPoleDirection, const vector3_type &aCharacterPosition,
        const quaternion_type &aCharacterRotation) {
        const auto intoRig = aCharacterRotation.inverse_unit();

        return with_limb_reaching(aSkeleton, aPose, aUpper,
            intoRig * (aWorldTarget - aCharacterPosition), intoRig * aWorldPoleDirection);
    }

    pose with_limb_reaching(const skeleton &aSkeleton, const pose &aPose, const std::size_t aUpper,
        const vector3_type &aTarget, const vector3_type &aPoleDirection) {
        if (aPose.local_rotations.size() != aSkeleton.bones.size())
            throw std::invalid_argument("with_limb_reaching: the pose does not match the skeleton");

        if (aUpper >= aSkeleton.bones.size())
            throw std::invalid_argument("with_limb_reaching: no such bone");

        const auto middle = first_child_of(aSkeleton, aUpper);

        if (middle == aSkeleton.bones.size())
            throw std::invalid_argument("with_limb_reaching: that bone has no child to bend at");

        const auto tip = first_child_of(aSkeleton, middle);

        if (tip == aSkeleton.bones.size())
            throw std::invalid_argument("with_limb_reaching: that limb has no tip");

        auto result = aPose;

        for (int pass = 0; pass < 2; ++pass) {
            const auto world = pose_world_transforms(aSkeleton, result);

            const auto root = world[aUpper].translation();
            const auto joint = world[middle].translation();
            const auto end = world[tip].translation();

            const auto upperLength = (joint - root).length();
            const auto lowerLength = (end - joint).length();

            const auto toTarget = aTarget - root;
            const auto distance = toTarget.length();

            if (distance < 0.0001f || upperLength < 0.0001f || lowerLength < 0.0001f) break;

            const auto reach = std::min(distance, upperLength + lowerLength - 0.0001f);

            const auto cosRoot = std::max(-1.0f, std::min(1.0f,
                (upperLength * upperLength + reach * reach - lowerLength * lowerLength)
                    / (2.0f * upperLength * reach)));

            const auto rootAngle = std::acos(cosRoot);

            const auto along = toTarget.normal();

            auto pole = aPoleDirection - along * aPoleDirection.dot_product(along);

            if (pole.is_effectively_zero()) {
                pole = (joint - root) - along * (joint - root).dot_product(along);

                if (pole.is_effectively_zero()) break;
            }

            pole = pole.normal();

            const auto desiredJoint = root
                + (along * std::cos(rootAngle) + pole * std::sin(rootAngle)) * upperLength;

            {
                const auto worldRotations = [&](const std::size_t aBone) {
                    auto q = quaternion_type::identity;
                    auto b = static_cast<int>(aBone);

                    std::vector<std::size_t> chain;
                    while (b >= 0) { chain.push_back(static_cast<std::size_t>(b));
                        b = aSkeleton.bones[static_cast<std::size_t>(b)].parent; }

                    for (auto it = chain.rbegin(); it != chain.rend(); ++it)
                        q = q * (aSkeleton.bones[*it].local_rotation * result.local_rotations[*it]);

                    return q;
                };

                const auto upperParent = aSkeleton.bones[aUpper].parent < 0
                    ? quaternion_type::identity
                    : worldRotations(static_cast<std::size_t>(aSkeleton.bones[aUpper].parent));

                result.local_rotations[aUpper] = aim(upperParent, aSkeleton.bones[aUpper].local_rotation,
                    joint - root, desiredJoint - root, result.local_rotations[aUpper]);
            }

            {
                const auto afterUpper = pose_world_transforms(aSkeleton, result);

                const auto newJoint = afterUpper[middle].translation();
                const auto newEnd = afterUpper[tip].translation();

                auto q = quaternion_type::identity;
                auto b = static_cast<int>(aSkeleton.bones[middle].parent);

                std::vector<std::size_t> chain;
                while (b >= 0) { chain.push_back(static_cast<std::size_t>(b));
                    b = aSkeleton.bones[static_cast<std::size_t>(b)].parent; }

                for (auto it = chain.rbegin(); it != chain.rend(); ++it)
                    q = q * (aSkeleton.bones[*it].local_rotation * result.local_rotations[*it]);

                result.local_rotations[middle] = aim(q, aSkeleton.bones[middle].local_rotation,
                    newEnd - newJoint, aTarget - newJoint, result.local_rotations[middle]);
            }
        }

        return result;
    }
}
