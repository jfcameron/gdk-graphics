// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/animation/skinning.h>

#include <gdk/math.h>

#include <algorithm>
#include <cmath>

using namespace gdk;

namespace gdk::graphics::animation {
    void skin(std::span<const vector3_type> aBindPositions,
        std::span<const vertex_weights> aWeights,
        std::span<const matrix4x4_type> aPalette, model_data &aTarget) {
        auto &components = aTarget.get_attribute_data("a_Position").components();

        for (std::size_t v = 0; v < aBindPositions.size(); ++v) {
            const auto &w = aWeights[v];

            const auto a = aPalette[w.first] * aBindPositions[v];
            const auto b = aPalette[w.second] * aBindPositions[v];

            const auto blended = lerp(b, a, w.first_weight);

            components[v * 3 + 0] = blended.x;
            components[v * 3 + 1] = blended.y;
            components[v * 3 + 2] = blended.z;
        }
    }

    namespace {
        void append_box(const vector3_type &aFrom, const vector3_type &aTo,
            const float aHalfWidth, const std::size_t aBone, skinned_mesh &aMesh,
            std::vector<float> &aPositions, std::vector<float> &aUvs) {
            const auto along = aTo - aFrom;
            const auto length = along.length();

            if (length <= 0) return;

            const auto forward = along / length;

            const auto seed = std::abs(forward.y) > 0.9f
                ? vector3_type(1, 0, 0)
                : vector3_type(0, 1, 0);

            const auto side = forward.cross_product(seed).normal() * aHalfWidth;
            const auto up = forward.cross_product(side).normal() * aHalfWidth;

            const vector3_type corner[8] = {
                aFrom - side - up, aFrom + side - up, aFrom + side + up, aFrom - side + up,
                aTo   - side - up, aTo   + side - up, aTo   + side + up, aTo   - side + up};

            static const int faces[6][4] = {
                {0, 1, 2, 3},   
                {5, 4, 7, 6},  
                {4, 5, 1, 0}, {5, 6, 2, 1}, {6, 7, 3, 2}, {7, 4, 0, 3}};

            for (const auto &face : faces) {
                const int order[6] = {face[0], face[1], face[2], face[0], face[2], face[3]};
                const float uv[6][2] = {{0, 0}, {1, 0}, {1, 1}, {0, 0}, {1, 1}, {0, 1}};

                for (int i = 0; i < 6; ++i) {
                    const auto &v = corner[order[i]];

                    aMesh.bind_positions.push_back(v);
                    aMesh.weights.push_back({aBone, aBone, 1.0f});   

                    aPositions.insert(aPositions.end(), {0.0f, 0.0f, 0.0f});   
                    aUvs.insert(aUvs.end(), {uv[i][0], uv[i][1]});
                }
            }
        }
    }

    skinned_mesh make_bone_boxes(const skeleton &aSkeleton, const float aHalfWidth) {
        skinned_mesh mesh;

        const auto world = aSkeleton.bind_world_transforms();

        std::vector<float> positions;
        std::vector<float> uvs;

        std::vector<bool> hasChild(aSkeleton.bones.size(), false);
        for (const auto &b : aSkeleton.bones) if (b.parent >= 0)
            hasChild[static_cast<std::size_t>(b.parent)] = true;

        for (std::size_t i = 0; i < aSkeleton.bones.size(); ++i) {
            const auto parent = aSkeleton.bones[i].parent;
            if (parent < 0) continue;

            append_box(world[static_cast<std::size_t>(parent)].translation(),
                world[i].translation(), aHalfWidth, static_cast<std::size_t>(parent),
                mesh, positions, uvs);
        }

        for (std::size_t i = 0; i < aSkeleton.bones.size(); ++i) {
            if (hasChild[i]) continue;

            const auto at = world[i].translation();
            const auto tip = at + vector3_type(0, aHalfWidth * 1.6f, 0);

            append_box(at, tip, aHalfWidth * 1.3f, i, mesh, positions, uvs);
        }

        mesh.data = model_data({
            {"a_Position", {positions, 3}},
            {"a_UV", {uvs, 2}}});

        return mesh;
    }
}
