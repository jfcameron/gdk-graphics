// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/gltf.h>

#include <cstddef>
#include <cstdint>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *aData, const std::size_t aSize) {
    try {
        const auto content = gdk::graphics::read_gltf(
            std::string_view(reinterpret_cast<const char *>(aData), aSize));

        std::size_t seen = 0;

        for (const auto &mesh : content.meshes) {
            seen += mesh.data.attributes().size();

            seen += mesh.data.indexed() ? mesh.data.indexes().size() : 0;

            seen += mesh.node_name.size() + mesh.parent_name.size() + mesh.material_name.size();
        }

        for (const auto &image : content.images) seen += image.bytes.size();

        for (const auto &node : content.nodes) seen += node.name.size();

        static volatile std::size_t sink = 0;

        sink = seen;
    }
    catch (const std::exception &) {}

    return 0;
}
