// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/animation/profile.h>

#include <algorithm>
#include <cctype>

namespace gdk::graphics::animation {
    namespace {
        [[nodiscard]] std::string normalised(const std::string &aName) {
            std::string result;

            for (const auto c : aName)
                if (std::isalnum(static_cast<unsigned char>(c)))
                    result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

            return result;
        }
    }

    const rig_profile::role *rig_profile::find(const std::string &aName) const {
        for (const auto &r : roles)
            if (r.name == aName) return &r;

        return nullptr;
    }

    int rig_binding::bone_for(const std::string &aRole) const {
        for (std::size_t i = 0; i < m_Profile.roles.size(); ++i)
            if (m_Profile.roles[i].name == aRole) return m_Bones[i];

        return -1;
    }

    std::vector<std::size_t> rig_binding::ground_contact_bones() const {
        std::vector<std::size_t> bones;

        for (const auto &role : m_Profile.ground_contacts) {
            const auto bone = bone_for(role);

            if (bone >= 0) bones.push_back(static_cast<std::size_t>(bone));
        }

        return bones;
    }

    std::size_t rig_binding::placed() const {
        return static_cast<std::size_t>(std::count_if(m_Bones.begin(), m_Bones.end(),
            [](const int aBone) { return aBone >= 0; }));
    }

    rig_binding bind_profile(const rig_profile &aProfile, const skeleton &aSkeleton) {
        std::vector<std::string> names;
        names.reserve(aSkeleton.bones.size());

        for (const auto &bone : aSkeleton.bones) names.push_back(normalised(bone.name));

        std::vector<int> bones(aProfile.roles.size(), -1);

        for (std::size_t i = 0; i < aProfile.roles.size(); ++i) {
            const auto match = [&](const std::string &aCandidate) {
                const auto wanted = normalised(aCandidate);
                const auto found = std::find(names.begin(), names.end(), wanted);

                if (found == names.end()) return false;

                bones[i] = static_cast<int>(found - names.begin());

                return true;
            };

            if (match(aProfile.roles[i].name)) continue;

            for (const auto &alias : aProfile.roles[i].aliases)
                if (match(alias)) break;
        }

        return {aProfile, std::move(bones), aSkeleton.bones.size()};
    }

    const rig_profile &humanoid_profile() {
        static const rig_profile profile = [] {
            rig_profile p;
            p.name = "humanoid";

            p.roles = {
                {"hips",           {"hip", "pelvis", "root"}, ""},
                {"spine",          {"abdomen", "lowerback"}, ""},

                {"chest",          {"spine1", "spine2", "spine3", "thorax", "upperchest"}, ""},
                {"head",           {"neck1", "neck"}, ""},

                {"leftArm", {"lupperarm", "lshldr", "leftupperarm", "larm"}, "rightArm"},
                {"leftForeArm", {"lforearm", "leftlowerarm", "lelbow"}, "rightForeArm"},
                {"leftHand", {"lhand", "leftwrist", "lwrist"}, "rightHand"},

                {"rightArm", {"rupperarm", "rshldr", "rightupperarm", "rarm"}, "leftArm"},
                {"rightForeArm", {"rforearm", "rightlowerarm", "relbow"}, "leftForeArm"},
                {"rightHand", {"rhand", "rightwrist", "rwrist"}, "leftHand"},

                {"leftUpLeg", {"lthigh", "leftthigh", "lfemur", "lhipjoint"}, "rightUpLeg"},
                {"leftLeg", {"lshin", "leftshin", "ltibia", "leftlowerleg"}, "rightLeg"},
                {"leftFoot", {"lfoot", "leftankle", "lankle"}, "rightFoot"},

                {"rightUpLeg", {"rthigh", "rightthigh", "rfemur", "rhipjoint"}, "leftUpLeg"},
                {"rightLeg", {"rshin", "rightshin", "rtibia", "rightlowerleg"}, "leftLeg"},
                {"rightFoot", {"rfoot", "rightankle", "rankle"}, "leftFoot"}};

            p.ground_contacts = {"leftFoot", "rightFoot"};

            return p;
        }();

        return profile;
    }

    std::vector<int> map_by_profile(const rig_binding &aSource, const rig_binding &aTarget) {
        std::vector<int> mapping(aTarget.bone_count(), -1);

        for (const auto &role : aTarget.profile().roles) {
            const auto targetBone = aTarget.bone_for(role.name);
            const auto sourceBone = aSource.bone_for(role.name);

            if (targetBone >= 0 && sourceBone >= 0)
                mapping[static_cast<std::size_t>(targetBone)] = sourceBone;
        }

        return mapping;
    }
}
