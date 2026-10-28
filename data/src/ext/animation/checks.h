// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXT_ANIMATION_CHECKS_H
#define GDK_GRAPHICS_EXT_ANIMATION_CHECKS_H

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/animation/skeleton.h>

#include <string>

namespace gdk::graphics::animation::checks {
    //! \throws exception where a bone's parent does not come before it, which every walk down a skeleton relies on
    inline void parents_come_first(const skeleton &aSkeleton, const char *aWho) {
        for (std::size_t i = 0; i < aSkeleton.bones.size(); ++i)
            if (const auto parent = aSkeleton.bones[i].parent; parent >= static_cast<int>(i) || parent < -1)
                throw exception(std::string(aWho) + ": bone " + std::to_string(i) + "'s parent does not come before it");
    }

    //! \throws exception where the pose is not one bone rotation per bone
    inline void pose_fits(const skeleton &aSkeleton, const pose &aPose, const char *aWho) {
        if (aPose.local_rotations.size() != aSkeleton.bones.size())
            throw exception(std::string(aWho) + ": the pose does not match the skeleton");
    }
}

#endif
