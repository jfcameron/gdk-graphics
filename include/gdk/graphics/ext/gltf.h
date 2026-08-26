// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_EXT_GLTF_H
#define GDK_GRAPHICS_EXT_GLTF_H

#include <gdk/graphics/ext/animation/skeleton.h>
#include <gdk/graphics/ext/animation/skinning.h>
#include <gdk/graphics/model_data.h>

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

/// \file reading glTF 2.0.
namespace gdk::graphics {
    /// \brief one drawable piece of a glTF document 
    struct gltf_mesh final {
        model_data data;

        /// \brief per-vertex influences, empty when the primitive is not skinned.
        std::vector<animation::vertex_weights> weights;

        //! index into `gltf_content::skeletons`, or -1 when the primitive is not skinned
        int skeleton = -1;
    };

    struct gltf_content final {
        std::vector<gltf_mesh> meshes;
        std::vector<animation::skeleton> skeletons;
        std::vector<animation::clip> animations;
    };

    using gltf_attribute_names = std::unordered_map<std::string, std::string>;

    [[nodiscard]] const gltf_attribute_names &default_gltf_attribute_names();

    /// \brief read a glTF document held in memory
    [[nodiscard]] gltf_content read_gltf(const std::string_view aData,
        const gltf_attribute_names &aNames = default_gltf_attribute_names());
}

#endif

