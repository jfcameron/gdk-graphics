// © Joseph Cameron - All Rights Reserved

#ifndef GDK_DEMO_SKINNING_H
#define GDK_DEMO_SKINNING_H

#include <gdk/graphics/ext/animation/skeleton.h>

#include <gdk/graphics/model_data.h>

#include <cstddef>
#include <vector>
#include <span>

namespace gdk::graphics::animation {
    /// \brief what a vertex is attached to: bone indices and how much each one moves it.
    struct vertex_weights final {
        std::size_t first = 0;
        std::size_t second = 0;
        float first_weight = 1.0f;
    };

    /// \brief write posed positions into aTarget, from bind-pose positions and weights
    ///
    /// Spans rather than `const std::vector &`: these are read, not kept, so the container the caller
    /// holds them in is not this function's business. A caller with a `std::array` or a subrange no
    /// longer has to copy into a vector to call it.
    void skin(std::span<const vector3_type> aBindPositions,
        std::span<const vertex_weights> aWeights,
        std::span<const matrix4x4_type> aPalette,
        gdk::graphics::model_data &aTarget);

    /// \brief a mesh built from one box per bone segment, rigidly weighted
    struct skinned_mesh final {
        gdk::graphics::model_data data;
        std::vector<vector3_type> bind_positions;
        std::vector<vertex_weights> weights;
    };

    /// \brief a box per bone segment, plus a cube at each leaf joint
    [[nodiscard]] skinned_mesh make_bone_boxes(const skeleton &aSkeleton, const float aHalfWidth);
}

#endif
