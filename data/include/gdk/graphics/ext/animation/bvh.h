// © Joseph Cameron - All Rights Reserved

#ifndef GDK_DEMO_BVH_H
#define GDK_DEMO_BVH_H

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
    [[nodiscard]] bvh_content read_bvh(const std::string_view aText, const float aScale = 1.0f);

    /// \brief write a skeleton and clip as BVH
    void write_bvh(std::ostream &aStream, const skeleton &aSkeleton, const clip &aClip,
        const std::size_t aFrameCount, const float aFrameTime);
}

#endif
