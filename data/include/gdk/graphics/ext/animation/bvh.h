// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXT_ANIMATION_BVH_H
#define GDK_GRAPHICS_EXT_ANIMATION_BVH_H

#include <gdk/graphics/ext/animation/skeleton.h>

#include <cstddef>
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

namespace gdk::graphics::animation {
    /// \brief a skeleton and an animation read from a Biovision Hierarchy file.
    struct bvh_content final {
        skeleton rig;
        clip animation;

        //! seconds per frame as declared by the file
        float frame_time = 1.0f / 30.0f;
    };

    /// \brief read a BVH document held in memory
    ///
    /// Every ROOT is read. The first root's position channels move the clip's root; any other joint's are read and
    /// left, as a pose turns bones but does not move them. An End Site becomes a bone of its own, named for its joint with "_End".
    /// Numbers are read the same in every locale.
    /// \throws gdk::graphics::exception for a malformed document, joints nested past 64 deep, or more frames
    /// declared than the document holds
    [[nodiscard]] bvh_content read_bvh(const std::string_view aText, const float aScale = 1.0f);

    /// \brief write a skeleton and clip as BVH: every root with its position channels, every other bone turned
    /// \throws gdk::graphics::exception for a skeleton with no bones, a bone whose parent comes after it, or a clip
    /// that poses a different number of bones
    void write_bvh(std::ostream &aStream, const skeleton &aSkeleton, const clip &aClip,
        const std::size_t aFrameCount, const float aFrameTime);
}

#endif
