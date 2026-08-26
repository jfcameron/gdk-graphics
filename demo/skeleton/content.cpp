// © Joseph Cameron - All Rights Reserved

#include "content.h"

#include <gdk/math.h>

#include <algorithm>
#include <cmath>
#include <string>

using namespace gdk;

using namespace gdk::graphics;

namespace demo {
    gait walking() { return {}; }

    gait running() {
        gait g;
        g.duration = 0.62f;
        g.hip_swing = 42.0f;
        g.knee_bend = 85.0f;
        g.ankle_level = 38.0f;
        g.shoulder_swing = 48.0f;
        g.elbow_fold = 62.0f;
        g.elbow_rest = 55.0f;
        g.hip_yaw = 10.0f;
        g.chest_yaw = 14.0f;
        g.lean = 14.0f;
        g.speed = 4.1f;

        return g;
    }

    gait idling() {
        gait g;
        g.duration = 3.4f;
        g.hip_swing = 0;
        g.knee_bend = 0;
        g.ankle_level = 0;
        g.ankle_swing = 0;
        g.shoulder_swing = 2.0f;
        g.elbow_fold = 0;
        g.elbow_rest = 10.0f;
        g.hip_yaw = 1.5f;
        g.chest_yaw = 2.5f;
        g.speed = 0;

        return g;
    }

    clip make_walk_clip(const skeleton &aSkeleton, const std::size_t aKeyCount) {
        return make_gait_clip(aSkeleton, walking(), aKeyCount);
    }

    clip make_gait_clip(const skeleton &aSkeleton, const gait &aGait, const std::size_t aKeyCount) {
        clip result;
        result.duration = aGait.duration;

        result.root_motion_per_cycle = vector3_type(0, 0, -aGait.speed * aGait.duration);

        const auto about_x = [](const float aRadians) {
            return quaternion_type::from_angle_axis(aRadians,
                vector3_type(1, 0, 0));
        };
        const auto about_y = [](const float aRadians) {
            return quaternion_type::from_angle_axis(aRadians,
                vector3_type(0, 1, 0));
        };

        for (std::size_t k = 0; k < aKeyCount; ++k) {
            const float t = static_cast<float>(k) / static_cast<float>(aKeyCount);
            const float phase = t * 2.0f * numbers::pi_f;

            auto p = pose::rest(aSkeleton);

            const float leftSwing = std::sin(phase);
            const float rightSwing = std::sin(phase + numbers::pi_f);

            p.local_rotations[humanoid::left_hip] = about_x(leftSwing * to_radians(aGait.hip_swing));
            p.local_rotations[humanoid::right_hip] = about_x(rightSwing * to_radians(aGait.hip_swing));

            const float leftBend = std::max(0.0f, std::sin(phase - to_radians(60.0f)));
            const float rightBend = std::max(0.0f, std::sin(phase + numbers::pi_f - to_radians(60.0f)));

            p.local_rotations[humanoid::left_knee] = about_x(-leftBend * to_radians(aGait.knee_bend));
            p.local_rotations[humanoid::right_knee] = about_x(-rightBend * to_radians(aGait.knee_bend));

            p.local_rotations[humanoid::left_foot] =
                about_x((leftBend * to_radians(aGait.ankle_level)) + (leftSwing * to_radians(aGait.ankle_swing)));
            p.local_rotations[humanoid::right_foot] =
                about_x((rightBend * to_radians(aGait.ankle_level)) + (rightSwing * to_radians(aGait.ankle_swing)));

            p.local_rotations[humanoid::left_shoulder] = about_x(rightSwing * to_radians(aGait.shoulder_swing));
            p.local_rotations[humanoid::right_shoulder] = about_x(leftSwing * to_radians(aGait.shoulder_swing));

            p.local_rotations[humanoid::left_elbow] =
                about_x(std::max(0.0f, rightSwing) * to_radians(aGait.elbow_fold) + to_radians(aGait.elbow_rest));
            p.local_rotations[humanoid::right_elbow] =
                about_x(std::max(0.0f, leftSwing) * to_radians(aGait.elbow_fold) + to_radians(aGait.elbow_rest));

            p.local_rotations[humanoid::hips] = about_y(leftSwing * to_radians(aGait.hip_yaw));
            p.local_rotations[humanoid::chest] = about_y(rightSwing * to_radians(aGait.chest_yaw));

            p.local_rotations[humanoid::spine] = about_x(to_radians(aGait.lean));

            result.keys.push_back({t * aGait.duration, std::move(p),
                vector3_type(0, 0, -aGait.speed * aGait.duration * t)});
        }

        return result;
    }

    clip make_wave_clip(const skeleton &aSkeleton, const std::size_t aKeyCount) {
        clip result;
        result.duration = 1.2f;

        const auto about_x = [](const float aRadians) {
            return quaternion_type::from_angle_axis(aRadians, vector3_type(1, 0, 0));
        };
        const auto about_z = [](const float aRadians) {
            return quaternion_type::from_angle_axis(aRadians, vector3_type(0, 0, 1));
        };

        for (std::size_t k = 0; k < aKeyCount; ++k) {
            const float t = static_cast<float>(k) / static_cast<float>(aKeyCount);
            const float phase = t * 2.0f * numbers::pi_f;

            auto p = pose::rest(aSkeleton);

            p.local_rotations[humanoid::right_shoulder] = about_z(to_radians(-155.0f));

            p.local_rotations[humanoid::right_elbow] =
                about_x(to_radians(30.0f)) * about_z(std::sin(phase) * to_radians(32.0f));

            result.keys.push_back({t * result.duration, std::move(p), vector3_type(0, 0, 0)});
        }

        return result;
    }

    skeleton make_humanoid() {
        skeleton s;
        s.bones.resize(humanoid::count);

        const auto set = [&](const std::size_t aBone, const int aParent, const std::string &aName,
            const float aX, const float aY, const float aZ) {
            s.bones[aBone] = {aParent, {aX, aY, aZ}, quaternion_type::identity, aName};
        };

        set(humanoid::hips, -1, "hips", 0,  1.05f, 0);
        set(humanoid::spine, humanoid::hips, "spine", 0,  0.22f, 0);
        set(humanoid::chest, humanoid::spine, "chest", 0,  0.24f, 0);
        set(humanoid::head, humanoid::chest, "head", 0,  0.28f, 0);

        set(humanoid::left_shoulder, humanoid::chest, "leftArm", -0.20f, 0.18f, 0);
        set(humanoid::left_elbow, humanoid::left_shoulder, "leftForeArm", -0.03f, -0.27f, 0);
        set(humanoid::left_hand, humanoid::left_elbow, "leftHand", 0,     -0.25f, 0);

        set(humanoid::right_shoulder, humanoid::chest, "rightArm", 0.20f, 0.18f, 0);
        set(humanoid::right_elbow, humanoid::right_shoulder, "rightForeArm", 0.03f, -0.27f, 0);
        set(humanoid::right_hand, humanoid::right_elbow, "rightHand", 0,     -0.25f, 0);

        set(humanoid::left_hip, humanoid::hips, "leftUpLeg", -0.11f, -0.08f, 0);
        set(humanoid::left_knee, humanoid::left_hip, "leftLeg", 0, -0.44f, 0);
        set(humanoid::left_foot, humanoid::left_knee, "leftFoot", 0, -0.42f, 0);

        set(humanoid::right_hip, humanoid::hips, "rightUpLeg", 0.11f, -0.08f, 0);
        set(humanoid::right_knee, humanoid::right_hip, "rightLeg", 0, -0.44f, 0);
        set(humanoid::right_foot, humanoid::right_knee, "rightFoot", 0, -0.42f, 0);

        return s;
    }
}
