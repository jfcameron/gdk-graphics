// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXT_ANIMATION_PROFILE_H
#define GDK_GRAPHICS_EXT_ANIMATION_PROFILE_H

#include <gdk/graphics/ext/animation/skeleton.h>

#include <string>
#include <string_view>
#include <vector>

namespace gdk::graphics::animation {
    struct rig_profile final {
        //! what this profile describes for messages. eg: "humanoid", "quadruped"
        std::string name;

        struct role final {
            //! the canonical name, and what a mapping is keyed on
            std::string name;

            /// \brief names a rig might use for this role instead.
            std::vector<std::string> aliases;

            /// \brief the role this one mirrors to, or empty if it mirrors to itself.
            std::string mirror;
        };

        std::vector<role> roles;

        /// \brief the roles that touch the ground, for foot planting.
        std::vector<std::string> ground_contacts;

        //! the role of that name, or nullptr
        [[nodiscard]] const role *find(std::string_view aName) const;
    };

    /// \brief a profile resolved against one particular skeleton.
    class rig_binding final {
    public:
        //! the bone filling a role, or -1
        [[nodiscard]] int bone_for(std::string_view aRole) const;

        //! the bones filling the profile's ground contacts, skipping any the rig does not have
        [[nodiscard]] std::vector<std::size_t> ground_contact_bones() const;

        //! how many of the profile's roles this rig actually fills
        [[nodiscard]] std::size_t placed() const;

        /// \brief how many bones the skeleton this was bound against has
        [[nodiscard]] std::size_t bone_count() const { return m_BoneCount; }

        [[nodiscard]] const rig_profile &profile() const { return m_Profile; }

        /// \param aBones for each of the profile's roles, the bone filling it, or -1
        /// \throws gdk::graphics::exception where aBones is not one per role, or names a bone past aBoneCount
        rig_binding(rig_profile aProfile, std::vector<int> aBones, const std::size_t aBoneCount);

    private:
        rig_profile m_Profile;

        std::vector<int> m_Bones;

        std::size_t m_BoneCount = 0;
    };

    /// \brief resolve a profile against a skeleton, by bone name and then by alias
    [[nodiscard]] rig_binding bind_profile(const rig_profile &aProfile, const skeleton &aSkeleton);

    /// \brief the built-in humanoid vocabulary
    [[nodiscard]] const rig_profile &humanoid_profile();

    /// \brief for each of the target's bones, the source bone filling the same role, or -1
    [[nodiscard]] std::vector<int> map_by_profile(const rig_binding &aSource,
        const rig_binding &aTarget);
}

#endif
