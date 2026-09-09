// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/gltf.h>

#include <cgltf/cgltf.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>
#include <memory>
#include <stdexcept>
#include <array>
#include <limits>
#include <string>
#include <unordered_map>

namespace gdk::graphics {
    namespace {
        using gltf_handle = std::unique_ptr<cgltf_data, void (*)(cgltf_data *)>;

        [[nodiscard]] const char *describe(const cgltf_result aResult) {
            switch (aResult) {
                case cgltf_result_data_too_short: return "the document is truncated";
                case cgltf_result_unknown_format: return "not a glTF or glb document";
                case cgltf_result_invalid_json: return "the json is malformed";
                case cgltf_result_invalid_gltf: return "the document violates the glTF schema";
                case cgltf_result_out_of_memory: return "out of memory";
                case cgltf_result_legacy_gltf: return "glTF 1.0, which is a different format";
                case cgltf_result_file_not_found: return "it references a file that was not supplied";
                default: return "it could not be read";
            }
        }

        struct local_transform final {
            animation::vector3_type position;
            animation::quaternion_type rotation = animation::quaternion_type::identity;
        };

        [[nodiscard]] local_transform rest_of(const cgltf_node &aNode) {
            local_transform out;

            if (aNode.has_translation)
                out.position = {aNode.translation[0], aNode.translation[1], aNode.translation[2]};

            if (aNode.has_rotation) {
                out.rotation.x = aNode.rotation[0];
                out.rotation.y = aNode.rotation[1];
                out.rotation.z = aNode.rotation[2];
                out.rotation.w = aNode.rotation[3];
            }

            if (aNode.has_matrix) {
                animation::matrix4x4_type m;
                m.set_to_identity();

                for (int column = 0; column < 4; ++column)
                    for (int row = 0; row < 4; ++row)
                        m.set(column, row, aNode.matrix[column * 4 + row]);

                out.position = m.translation();
                out.rotation = m.rotation();
            }

            return out;
        }

        [[nodiscard]] std::size_t depth_of(const cgltf_node *aNode) {
            std::size_t depth = 0;

            for (const auto *n = aNode->parent; n; n = n->parent) ++depth;

            return depth;
        }

        [[nodiscard]] std::vector<float> floats_of(const cgltf_accessor &aAccessor) {
            const auto components = cgltf_num_components(aAccessor.type);

            std::vector<float> values(aAccessor.count * components);

            if (!cgltf_accessor_unpack_floats(&aAccessor, values.data(), values.size()))
                throw std::runtime_error("gltf: an accessor could not be read as floats");

            return values;
        }
    }

    namespace {
        struct converted_skin final {
            animation::skeleton rig;
            std::vector<std::size_t> remap;
        };

        [[nodiscard]] converted_skin convert(const cgltf_skin &aSkin, const cgltf_data &aDocument) {
            const auto count = aSkin.joints_count;

            std::vector<std::size_t> order(count);
            for (std::size_t i = 0; i < count; ++i) order[i] = i;

            std::stable_sort(order.begin(), order.end(),
                [&](const std::size_t a, const std::size_t b) {
                    return depth_of(aSkin.joints[a]) < depth_of(aSkin.joints[b]);
                });

            converted_skin out;
            out.remap.assign(count, 0);

            for (std::size_t bone = 0; bone < count; ++bone) out.remap[order[bone]] = bone;

            std::unordered_map<const cgltf_node *, std::size_t> boneOf;
            for (std::size_t joint = 0; joint < count; ++joint)
                boneOf.emplace(aSkin.joints[joint], out.remap[joint]);

            out.rig.bones.resize(count);

            for (std::size_t joint = 0; joint < count; ++joint) {
                const auto *node = aSkin.joints[joint];
                auto &bone = out.rig.bones[out.remap[joint]];

                const auto rest = rest_of(*node);

                bone.local_position = rest.position;
                bone.local_rotation = rest.rotation;
                bone.name = node->name ? node->name : "";
                bone.parent = -1;

                for (const auto *p = node->parent; p; p = p->parent) {
                    const auto found = boneOf.find(p);

                    if (found != boneOf.end()) { bone.parent = static_cast<int>(found->second); break; }
                }
            }

            if (aSkin.inverse_bind_matrices) {
                const auto values = floats_of(*aSkin.inverse_bind_matrices);

                if (values.size() == count * 16) {
                    out.rig.supplied_inverse_binds.resize(count);

                    for (std::size_t joint = 0; joint < count; ++joint) {
                        auto &m = out.rig.supplied_inverse_binds[out.remap[joint]];
                        m.set_to_identity();

                        for (int column = 0; column < 4; ++column)
                            for (int row = 0; row < 4; ++row)
                                m.set(column, row, values[joint * 16 + column * 4 + row]);
                    }
                }
            }

            (void)aDocument;

            return out;
        }

        [[nodiscard]] std::vector<animation::vertex_weights> weights_of(
            const cgltf_primitive &aPrimitive, const std::vector<std::size_t> &aRemap) {
            const cgltf_accessor *joints = nullptr;
            const cgltf_accessor *weights = nullptr;

            for (cgltf_size a = 0; a < aPrimitive.attributes_count; ++a) {
                const auto &attribute = aPrimitive.attributes[a];
                const std::string name = attribute.name ? attribute.name : "";

                if (name == "JOINTS_0") joints = attribute.data;
                if (name == "WEIGHTS_0") weights = attribute.data;
            }

            if (!joints || !weights) return {};

            const auto weightValues = floats_of(*weights);

            std::vector<animation::vertex_weights> out(joints->count);

            for (cgltf_size v = 0; v < joints->count; ++v) {
                cgltf_uint indices[4]{};
                cgltf_accessor_read_uint(joints, v, indices, 4);

                std::size_t first = 0, second = 1;

                for (std::size_t i = 2; i < 4; ++i) {
                    if (weightValues[v * 4 + i] > weightValues[v * 4 + first]) first = i;
                    else if (weightValues[v * 4 + i] > weightValues[v * 4 + second]) second = i;
                }

                if (weightValues[v * 4 + second] > weightValues[v * 4 + first])
                    std::swap(first, second);

                const auto a = weightValues[v * 4 + first];
                const auto b = weightValues[v * 4 + second];
                const auto total = a + b;

                const auto remapped = [&](const std::size_t slot) {
                    const auto index = static_cast<std::size_t>(indices[slot]);

                    return index < aRemap.size() ? aRemap[index] : std::size_t{0};
                };

                out[v].first = remapped(first);
                out[v].second = remapped(second);
                out[v].first_weight = total > 0.0f ? a / total : 1.0f;
            }

            return out;
        }
    }

    namespace {
        [[nodiscard]] animation::clip convert(const cgltf_animation &aAnimation,
            const cgltf_skin &aSkin, const std::vector<std::size_t> &aRemap,
            const animation::skeleton &aRig) {
            std::unordered_map<const cgltf_node *, std::size_t> boneOf;

            for (std::size_t joint = 0; joint < aSkin.joints_count; ++joint)
                boneOf.emplace(aSkin.joints[joint], aRemap[joint]);

            std::vector<float> times;

            for (cgltf_size c = 0; c < aAnimation.channels_count; ++c) {
                const auto *sampler = aAnimation.channels[c].sampler;

                if (!sampler || !sampler->input) continue;

                if (sampler->interpolation == cgltf_interpolation_type_cubic_spline)
                    throw std::runtime_error(
                        "gltf: CUBICSPLINE interpolation is not supported; resample to LINEAR");

                for (const auto t : floats_of(*sampler->input)) times.push_back(t);
            }

            std::sort(times.begin(), times.end());
            times.erase(std::unique(times.begin(), times.end()), times.end());

            animation::clip out;

            out.loops = false;

            if (times.empty()) return out;

            out.duration = std::max(times.back(), 0.0001f);

            for (const auto time : times)
                out.keys.push_back({time, animation::pose::rest(aRig), {}});

            for (cgltf_size c = 0; c < aAnimation.channels_count; ++c) {
                const auto &channel = aAnimation.channels[c];
                const auto *sampler = channel.sampler;

                if (!sampler || !channel.target_node) continue;

                const auto bone = boneOf.find(channel.target_node);

                if (bone == boneOf.end()) continue;

                const auto isRoot = aRig.bones[bone->second].parent < 0;

                if (channel.target_path != cgltf_animation_path_type_rotation
                    && !(channel.target_path == cgltf_animation_path_type_translation && isRoot))
                    continue;

                for (std::size_t k = 0; k < times.size(); ++k) {
                    if (channel.target_path == cgltf_animation_path_type_rotation) {
                        float q[4]{0, 0, 0, 1};
                        cgltf_accessor_read_float(sampler->output, 0, q, 4);

                        const auto input = floats_of(*sampler->input);
                        const auto output = floats_of(*sampler->output);

                        std::size_t i = 0;
                        while (i + 1 < input.size() && input[i + 1] < times[k]) ++i;

                        const auto step = sampler->interpolation == cgltf_interpolation_type_step;
                        const auto span = i + 1 < input.size() ? input[i + 1] - input[i] : 0.0f;
                        const auto u = (!step && span > 0)
                            ? std::max(0.0f, std::min(1.0f, (times[k] - input[i]) / span))
                            : 0.0f;

                        const auto at = [&](const std::size_t index) {
                            animation::quaternion_type value;
                            value.x = output[index * 4 + 0];
                            value.y = output[index * 4 + 1];
                            value.z = output[index * 4 + 2];
                            value.w = output[index * 4 + 3];

                            return value;
                        };

                        const auto a = at(i);
                        const auto b = i + 1 < input.size() ? at(i + 1) : a;

                        out.keys[k].p.local_rotations[bone->second] =
                            aRig.bones[bone->second].local_rotation.inverse_unit() * slerp(a, b, u);
                    }
                    else {
                        const auto input = floats_of(*sampler->input);
                        const auto output = floats_of(*sampler->output);

                        std::size_t i = 0;
                        while (i + 1 < input.size() && input[i + 1] < times[k]) ++i;

                        const auto span = i + 1 < input.size() ? input[i + 1] - input[i] : 0.0f;
                        const auto u = span > 0
                            ? std::max(0.0f, std::min(1.0f, (times[k] - input[i]) / span))
                            : 0.0f;

                        const auto at = [&](const std::size_t index) {
                            return animation::vector3_type(output[index * 3 + 0],
                                output[index * 3 + 1], output[index * 3 + 2]);
                        };

                        const auto a = at(i);
                        const auto b = i + 1 < input.size() ? at(i + 1) : a;

                        out.keys[k].root_position = a + (b - a) * u;
                    }
                }
            }

            out.root_motion_per_cycle = out.keys.back().root_position - out.keys.front().root_position;

            return out;
        }
    }

    const gltf_attribute_names &default_gltf_attribute_names() {
        static const gltf_attribute_names names{
            {"POSITION", "a_Position"},
            {"NORMAL", "a_Normal"},
            {"TEXCOORD_0", "a_UV"}};

        return names;
    }

    gltf_content read_gltf(const std::string_view aData, const gltf_attribute_names &aNames) {
        cgltf_options options{};
        cgltf_data *raw = nullptr;

        {
            const auto result = cgltf_parse(&options, aData.data(), aData.size(), &raw);

            if (result != cgltf_result_success)
                throw std::runtime_error(std::string("gltf: ") + describe(result));
        }

        const gltf_handle document(raw, cgltf_free);

        {
            const auto result = cgltf_load_buffers(&options, document.get(), nullptr);

            if (result != cgltf_result_success)
                throw std::runtime_error(std::string("gltf: ") + describe(result)
                    + " (external buffers are not read; supply a self-contained glb)");
        }

        {
            const auto result = cgltf_validate(document.get());

            if (result != cgltf_result_success)
                throw std::runtime_error(std::string("gltf: ") + describe(result)
                    + " (the document does not describe itself consistently)");
        }

        gltf_content content;

        for (cgltf_size i = 0; i < document->images_count; ++i) {
            const auto &image = document->images[i];

            gltf_image out;

            out.name = image.name ? image.name : "";
            out.mime_type = image.mime_type ? image.mime_type : "";

            if (image.buffer_view && image.buffer_view->buffer
                && image.buffer_view->buffer->data) {
                const auto &view = *image.buffer_view;

                if (view.offset > view.buffer->size
                    || view.size > view.buffer->size - view.offset)
                    throw std::runtime_error("gdk::graphics::read_gltf: this document's image \""
                        + out.name + "\" claims more bytes than the buffer holding it has");

                const auto *pStart = static_cast<const std::byte *>(view.buffer->data)
                    + view.offset;

                out.bytes.assign(pStart, pStart + view.size);
            }
            else if (image.uri) {
                const std::string_view uri(image.uri);

                const auto comma = uri.find(',');

                if (uri.rfind("data:", 0) != 0 || comma == std::string_view::npos
                    || uri.find(";base64") == std::string_view::npos)
                    throw std::runtime_error("gdk::graphics::read_gltf: this document's image \""
                        + out.name + "\" names a file rather than carrying its own bytes. Supply "
                        "a self-contained glb");

                void *pDecoded = nullptr;

                const auto encoded = uri.substr(comma + 1);

                const auto bound = encoded.size() / 4 * 3;

                const auto result = cgltf_load_buffer_base64(&options, bound, encoded.data(),
                    &pDecoded);

                if (result != cgltf_result_success)
                    throw std::runtime_error("gdk::graphics::read_gltf: this document's image \""
                        + out.name + "\" has a data uri that will not decode");

                const auto *pStart = static_cast<const std::byte *>(pDecoded);

                out.bytes.assign(pStart, pStart + bound);

                options.memory.free_func(options.memory.user_data, pDecoded);
            }

            content.images.push_back(std::move(out));
        }

        std::vector<std::vector<std::size_t>> remaps;

        for (cgltf_size k = 0; k < document->skins_count; ++k) {
            auto converted = convert(document->skins[k], *document);

            content.skeletons.push_back(std::move(converted.rig));
            remaps.push_back(std::move(converted.remap));
        }

        for (cgltf_size a = 0; a < document->animations_count && document->skins_count; ++a)
            content.animations.push_back(convert(document->animations[a], document->skins[0],
                remaps[0], content.skeletons[0]));

        for (cgltf_size n = 0; n < document->nodes_count; ++n) {
            const auto &node = document->nodes[n];

            gltf_node out;

            out.name = node.name ? node.name : "";
            out.parent_name = node.parent && node.parent->name ? node.parent->name : "";
            out.draws = node.mesh != nullptr;

            {
                cgltf_float column[16];

                cgltf_node_transform_local(&node, column);

                for (int row = 0; row < 4; ++row)
                    for (int col = 0; col < 4; ++col)
                        out.local_transform[static_cast<std::size_t>(row * 4 + col)]
                            = static_cast<float>(column[col * 4 + row]);
            }

            content.nodes.push_back(std::move(out));
        }

        for (cgltf_size m = 0; m < document->meshes_count; ++m) {
            const auto &mesh = document->meshes[m];

            const cgltf_node *pNode = nullptr;

            for (cgltf_size n = 0; n < document->nodes_count && !pNode; ++n)
                if (document->nodes[n].mesh == &mesh) pNode = &document->nodes[n];

            const std::string meshName = mesh.name ? mesh.name : "";
            const std::string nodeName = pNode && pNode->name ? pNode->name : "";

            const std::string parentName = pNode && pNode->parent && pNode->parent->name
                ? pNode->parent->name
                : "";

            std::array<float, 16> local{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

            if (pNode) {
                cgltf_float column[16];

                cgltf_node_transform_local(pNode, column);

                for (int row = 0; row < 4; ++row)
                    for (int col = 0; col < 4; ++col)
                        local[static_cast<std::size_t>(row * 4 + col)]
                            = static_cast<float>(column[col * 4 + row]);
            }

            for (cgltf_size p = 0; p < mesh.primitives_count; ++p) {
                const auto &primitive = mesh.primitives[p];

                model_data::attribute_collection_type attributes;

                for (cgltf_size a = 0; a < primitive.attributes_count; ++a) {
                    const auto &attribute = primitive.attributes[a];

                    const auto mapped = aNames.find(attribute.name ? attribute.name : "");

                    if (mapped == aNames.end() || !attribute.data) continue;

                    attributes.emplace(mapped->second,
                        attribute_data(floats_of(*attribute.data),
                            static_cast<std::size_t>(cgltf_num_components(attribute.data->type))));
                }

                if (attributes.empty()) continue;

                std::vector<model_data::index_value_type> indices;

                if (primitive.indices) {
                    indices.reserve(primitive.indices->count);

                    for (cgltf_size i = 0; i < primitive.indices->count; ++i) {
                        const auto index = cgltf_accessor_read_index(primitive.indices, i);

                        if (index > std::numeric_limits<model_data::index_value_type>::max())
                            throw std::runtime_error("gdk::graphics::read_gltf: this document "
                                "indexes a vertex past what a model can address. Split the mesh");

                        indices.push_back(
                            static_cast<model_data::index_value_type>(index));
                    }
                }

                gltf_mesh out;
                out.data = indices.empty()
                    ? model_data(std::move(attributes))
                    : model_data(std::move(attributes), std::move(indices));
                out.mesh_name = meshName;
                out.node_name = nodeName;
                out.parent_name = parentName;
                out.local_transform = local;

                if (primitive.material && primitive.material->name)
                    out.material_name = primitive.material->name;

                if (primitive.material && primitive.material->has_pbr_metallic_roughness) {
                    const auto *pImage = primitive.material
                        ->pbr_metallic_roughness.base_color_texture.texture;

                    if (pImage && pImage->image)
                        out.base_color_image = static_cast<int>(pImage->image - document->images);
                }

                for (cgltf_size n = 0; n < document->nodes_count && out.skeleton < 0; ++n) {
                    const auto &node = document->nodes[n];

                    if (node.mesh != &mesh || !node.skin) continue;

                    out.skeleton = static_cast<int>(node.skin - document->skins);
                }

                if (out.skeleton >= 0)
                    out.weights = weights_of(primitive, remaps[static_cast<std::size_t>(out.skeleton)]);

                content.meshes.push_back(std::move(out));
            }
        }

        return content;
    }
}
