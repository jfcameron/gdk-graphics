// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_EXT_GLTF_H
#define GDK_GRAPHICS_EXT_GLTF_H

#include <gdk/graphics/ext/animation/skeleton.h>
#include <gdk/graphics/ext/animation/skinning.h>
#include <gdk/graphics/model_data.h>

#include <array>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace gdk::graphics {
    /// \brief one drawable piece of a glTF document 
    struct gltf_mesh final {
        model_data data;

        /// \brief the name of the glTF mesh this came from, empty if it is unnamed
        std::string mesh_name;

        /// \brief the name of the node that draws this mesh, empty if no node does
        std::string node_name;

        /// \brief the name of the parent node, empty if this is the root node
        std::string parent_name;

        /// \brief the name of the glTF material this primitive draws with, empty if it has none
        std::string material_name;

        /// \brief the image this primitive's material paints with, or -1 for none
        int base_color_image = -1;

        /// \brief this node's transform **relative to its parent**, row major, identity by default
        std::array<float, 16> local_transform{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1};

        /// \brief per-vertex influences, empty if the primitive is not skinned.
        std::vector<animation::vertex_weights> weights;

        //! index into `gltf_content::skeletons`, or -1 when the primitive is not skinned
        int skeleton = -1;
    };

    /// \brief a node of the document
    struct gltf_node final {
        std::string name;

        //! empty if root node
        std::string parent_name;

        //! relative to the parent 
        std::array<float, 16> local_transform{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1};

        //! whether it draws, and so whether it also appears in \ref gltf_content::meshes
        bool draws = false;
    };

    /// \brief an encoded image in the document
    struct gltf_image final {
        std::string name;

        //! "image/png", "image/jpeg", or empty when the document did not say
        std::string mime_type;

        //! the file as it sits in the document, PNG or JPEG bytes and not channel data
        std::vector<std::byte> bytes;
    };

    struct gltf_content final {
        //! meshes carried by the document
        std::vector<gltf_mesh> meshes;

        //! images carried by the document
        std::vector<gltf_image> images;

        //! every node, drawing or not
        std::vector<gltf_node> nodes;
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

