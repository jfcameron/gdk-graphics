// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/gltf.h>

#include <cgltf/cgltf.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <span>
#include <string>
#include <unordered_map>

namespace gdk::graphics {
    namespace {
        constexpr std::size_t MOST_ACCESSOR_VALUES = std::size_t{1} << 24;

        using gltf_handle = std::unique_ptr<cgltf_data, void (*)(cgltf_data *)>;

        [[noreturn]] void refuse(const std::string &aWhy) { throw exception("gltf: " + aWhy); }

        void *allocate(void *, const cgltf_size aSize) { return std::malloc(aSize); }

        void release(void *, void *pMemory) { std::free(pMemory); }

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

        [[nodiscard]] const char *name_of(const cgltf_type aType) {
            switch (aType) {
                case cgltf_type_scalar: return "SCALAR";
                case cgltf_type_vec2: return "VEC2";
                case cgltf_type_vec3: return "VEC3";
                case cgltf_type_vec4: return "VEC4";
                case cgltf_type_mat2: return "MAT2";
                case cgltf_type_mat3: return "MAT3";
                case cgltf_type_mat4: return "MAT4";
                default: return "an unknown type";
            }
        }

        void require_type(const cgltf_accessor &aAccessor, const cgltf_type aType, const std::string &aWhat) {
            if (aAccessor.type != aType)
                refuse(aWhat + " is " + name_of(aAccessor.type) + ", where it must be " + name_of(aType));
        }

        struct local_transform final {
            animation::vector3_type position;
            animation::quaternion_type rotation = animation::quaternion_type::identity;
            animation::vector3_type scale = {1, 1, 1};
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

            if (aNode.has_scale) out.scale = {aNode.scale[0], aNode.scale[1], aNode.scale[2]};

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

        [[nodiscard]] animation::vector3_type placed(const local_transform &aParent,
            const animation::vector3_type &aPosition) {
            const animation::vector3_type scaled{aPosition.x * aParent.scale.x, aPosition.y * aParent.scale.y,
                aPosition.z * aParent.scale.z};

            return aParent.position + aParent.rotation * scaled;
        }

        //! the transform of every node above aNode, composed: where a root bone's armature put it
        [[nodiscard]] local_transform ancestry_of(const cgltf_node &aNode) {
            std::vector<const cgltf_node *> chain;

            for (const auto *p = aNode.parent; p; p = p->parent) {
                if (chain.size() > MOST_ACCESSOR_VALUES) refuse("a node's parents form a loop");

                chain.push_back(p);
            }

            local_transform out;

            for (auto each = chain.rbegin(); each != chain.rend(); ++each) {
                const auto local = rest_of(**each);

                out.position = placed(out, local.position);
                out.rotation = out.rotation * local.rotation;
                out.scale = {out.scale.x * local.scale.x, out.scale.y * local.scale.y, out.scale.z * local.scale.z};
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

            if (!components) refuse("an accessor has no type a reader can take");

            if (aAccessor.count > MOST_ACCESSOR_VALUES / components)
                refuse("an accessor of " + std::to_string(aAccessor.count) + " " + name_of(aAccessor.type)
                    + "s is past the " + std::to_string(MOST_ACCESSOR_VALUES) + " values this reader takes");

            std::vector<float> values(aAccessor.count * components);

            if (cgltf_accessor_unpack_floats(&aAccessor, values.data(), values.size()) != values.size())
                refuse("an accessor could not be read as floats");

            return values;
        }

        [[nodiscard]] std::vector<std::byte> decoded_base64(const cgltf_options &aOptions, std::string_view aEncoded,
            const std::string &aName) {
            while (!aEncoded.empty() && aEncoded.back() == '=') aEncoded.remove_suffix(1);

            const auto size = aEncoded.size() * 3 / 4;

            if (!size) return {};

            void *pDecoded = nullptr;

            if (cgltf_load_buffer_base64(&aOptions, size, aEncoded.data(), &pDecoded) != cgltf_result_success)
                refuse("this document's image \"" + aName + "\" has a data uri that will not decode");

            const std::unique_ptr<void, void (*)(void *)> owned(pDecoded, std::free);

            const auto *pStart = static_cast<const std::byte *>(pDecoded);

            return {pStart, pStart + size};
        }
    }

    namespace {
        struct converted_skin final {
            animation::skeleton rig;
            std::vector<std::size_t> remap;
        };

        [[nodiscard]] converted_skin convert(const cgltf_skin &aSkin) {
            const auto count = aSkin.joints_count;

            if (!count) refuse("a skin has no joints");

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
                if (!boneOf.emplace(aSkin.joints[joint], out.remap[joint]).second)
                    refuse("a skin names one joint twice");

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

                if (bone.parent < 0) {
                    const auto above = ancestry_of(*node);

                    bone.local_position = placed(above, rest.position);
                    bone.local_rotation = above.rotation * rest.rotation;
                }
            }

            if (aSkin.inverse_bind_matrices) {
                require_type(*aSkin.inverse_bind_matrices, cgltf_type_mat4, "a skin's inverse bind matrices");

                const auto values = floats_of(*aSkin.inverse_bind_matrices);

                if (values.size() != count * 16)
                    refuse("a skin of " + std::to_string(count) + " joints has "
                        + std::to_string(values.size() / 16) + " inverse bind matrices");

                out.rig.supplied_inverse_binds.resize(count);

                for (std::size_t joint = 0; joint < count; ++joint) {
                    auto &m = out.rig.supplied_inverse_binds[out.remap[joint]];
                    m.set_to_identity();

                    for (int column = 0; column < 4; ++column)
                        for (int row = 0; row < 4; ++row)
                            m.set(column, row, values[joint * 16 + column * 4 + row]);
                }
            }

            return out;
        }

        [[nodiscard]] std::vector<animation::vertex_weights> weights_of(
            const cgltf_primitive &aPrimitive, std::span<const std::size_t> aRemap) {
            const cgltf_accessor *joints = nullptr;
            const cgltf_accessor *weights = nullptr;

            for (cgltf_size a = 0; a < aPrimitive.attributes_count; ++a) {
                const auto &attribute = aPrimitive.attributes[a];
                const std::string name = attribute.name ? attribute.name : "";

                if (name == "JOINTS_0") joints = attribute.data;
                if (name == "WEIGHTS_0") weights = attribute.data;
            }

            if (!joints || !weights) return {};

            require_type(*joints, cgltf_type_vec4, "JOINTS_0");
            require_type(*weights, cgltf_type_vec4, "WEIGHTS_0");

            if (joints->count != weights->count) refuse("JOINTS_0 and WEIGHTS_0 have different counts");

            const auto weightValues = floats_of(*weights);

            std::vector<animation::vertex_weights> out(joints->count);

            for (cgltf_size v = 0; v < joints->count; ++v) {
                cgltf_uint indices[4]{};
                cgltf_accessor_read_uint(joints, v, indices, 4);

                const auto weight = [&](const std::size_t aSlot) { return weightValues[v * 4 + aSlot]; };

                std::array<std::size_t, 4> slots{0, 1, 2, 3};

                std::stable_sort(slots.begin(), slots.end(),
                    [&](const std::size_t a, const std::size_t b) { return weight(a) > weight(b); });

                const auto bone_of = [&](const std::size_t aSlot) {
                    const auto index = static_cast<std::size_t>(indices[aSlot]);

                    if (index >= aRemap.size())
                        refuse("a vertex is weighted to joint " + std::to_string(index) + " of a skin of "
                            + std::to_string(aRemap.size()));

                    return aRemap[index];
                };

                const auto a = weight(slots[0]);
                const auto b = weight(slots[1]);
                const auto total = a + b;

                out[v].first = bone_of(slots[0]);
                out[v].second = b > 0 ? bone_of(slots[1]) : out[v].first;
                out[v].first_weight = total > 0.0f ? a / total : 1.0f;
            }

            return out;
        }
    }

    namespace {
        struct channel_samples final {
            std::vector<float> input;
            std::vector<float> output;
            bool step = false;
        };

        [[nodiscard]] channel_samples samples_of(const cgltf_animation_sampler &aSampler, const cgltf_type aOutput,
            const std::string &aWhat) {
            require_type(*aSampler.input, cgltf_type_scalar, aWhat + "'s times");
            require_type(*aSampler.output, aOutput, aWhat + "'s values");

            if (aSampler.output->count < aSampler.input->count)
                refuse(aWhat + " has fewer values than times");

            return {floats_of(*aSampler.input), floats_of(*aSampler.output),
                aSampler.interpolation == cgltf_interpolation_type_step};
        }

        struct between final {
            std::size_t from = 0;
            std::size_t to = 0;
            float u = 0;
        };

        //! where aTime falls among aInput's keys, from aCursor on: times are visited in order
        [[nodiscard]] between find(const channel_samples &aSamples, std::size_t &aCursor, const float aTime) {
            const auto &input = aSamples.input;

            while (aCursor + 1 < input.size() && input[aCursor + 1] < aTime) ++aCursor;

            const auto i = aCursor;
            const auto next = i + 1 < input.size() ? i + 1 : i;
            const auto span = input[next] - input[i];

            const auto u = (!aSamples.step && span > 0)
                ? std::max(0.0f, std::min(1.0f, (aTime - input[i]) / span))
                : 0.0f;

            return {i, next, u};
        }

        [[nodiscard]] animation::clip convert(const cgltf_animation &aAnimation,
            const cgltf_skin &aSkin, std::span<const std::size_t> aRemap,
            const animation::skeleton &aRig) {
            std::unordered_map<const cgltf_node *, std::size_t> boneOf;

            for (std::size_t joint = 0; joint < aSkin.joints_count; ++joint)
                boneOf.emplace(aSkin.joints[joint], aRemap[joint]);

            std::vector<float> times;

            for (cgltf_size c = 0; c < aAnimation.channels_count; ++c) {
                const auto *sampler = aAnimation.channels[c].sampler;

                if (!sampler || !sampler->input || !sampler->output) continue;

                if (sampler->interpolation == cgltf_interpolation_type_cubic_spline)
                    refuse("CUBICSPLINE interpolation is not supported; resample to LINEAR");

                require_type(*sampler->input, cgltf_type_scalar, "an animation's times");

                for (const auto t : floats_of(*sampler->input)) {
                    if (!std::isfinite(t)) refuse("an animation has a time that is not a number");

                    times.push_back(t);
                }
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

                if (!sampler || !sampler->input || !sampler->output || !channel.target_node) continue;

                const auto bone = boneOf.find(channel.target_node);

                if (bone == boneOf.end()) continue;

                const auto isRoot = aRig.bones[bone->second].parent < 0;

                const auto rotates = channel.target_path == cgltf_animation_path_type_rotation;
                const auto moves = channel.target_path == cgltf_animation_path_type_translation && isRoot;

                if (!rotates && !moves) continue;

                const auto samples = samples_of(*sampler, rotates ? cgltf_type_vec4 : cgltf_type_vec3,
                    rotates ? "a rotation channel" : "a translation channel");

                if (samples.input.empty()) continue;

                const auto above = isRoot ? ancestry_of(*channel.target_node) : local_transform{};

                std::size_t cursor = 0;

                for (std::size_t k = 0; k < times.size(); ++k) {
                    const auto at = find(samples, cursor, times[k]);

                    if (rotates) {
                        const auto quaternion = [&](const std::size_t index) {
                            animation::quaternion_type value;
                            value.x = samples.output[index * 4 + 0];
                            value.y = samples.output[index * 4 + 1];
                            value.z = samples.output[index * 4 + 2];
                            value.w = samples.output[index * 4 + 3];

                            return value;
                        };

                        out.keys[k].p.local_rotations[bone->second] =
                            aRig.bones[bone->second].local_rotation.inverse_unit()
                            * (above.rotation * slerp(quaternion(at.from), quaternion(at.to), at.u));
                    }
                    else {
                        const auto position = [&](const std::size_t index) {
                            return animation::vector3_type(samples.output[index * 3 + 0],
                                samples.output[index * 3 + 1], samples.output[index * 3 + 2]);
                        };

                        const auto a = position(at.from);
                        const auto b = position(at.to);

                        out.keys[k].root_position = placed(above, a + (b - a) * at.u);
                    }
                }
            }

            out.root_motion_per_cycle = out.keys.back().root_position - out.keys.front().root_position;

            return out;
        }
    }

    namespace {
        [[nodiscard]] std::array<float, 16> local_transform_of(const cgltf_node *pNode) {
            std::array<float, 16> local{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

            if (!pNode) return local;

            cgltf_float column[16];

            cgltf_node_transform_local(pNode, column);

            for (int row = 0; row < 4; ++row)
                for (int col = 0; col < 4; ++col)
                    local[static_cast<std::size_t>(row * 4 + col)] = static_cast<float>(column[col * 4 + row]);

            return local;
        }

        //! a primitive's triangles as a list, whatever the primitive drew them as
        [[nodiscard]] std::vector<model_data::index_value_type> triangle_list_of(const cgltf_primitive &aPrimitive,
            const std::size_t aVertexCount) {
            std::vector<std::size_t> given;

            if (aPrimitive.indices) {
                require_type(*aPrimitive.indices, cgltf_type_scalar, "a primitive's indices");

                if (aPrimitive.indices->count > MOST_ACCESSOR_VALUES)
                    refuse("a primitive of " + std::to_string(aPrimitive.indices->count) + " indices");

                given.reserve(aPrimitive.indices->count);

                for (cgltf_size i = 0; i < aPrimitive.indices->count; ++i)
                    given.push_back(cgltf_accessor_read_index(aPrimitive.indices, i));
            }
            else if (aPrimitive.type != cgltf_primitive_type_triangles) {
                given.resize(aVertexCount);

                for (std::size_t i = 0; i < aVertexCount; ++i) given[i] = i;
            }

            std::vector<std::size_t> listed;

            switch (aPrimitive.type) {
                case cgltf_primitive_type_triangles: listed = std::move(given); break;

                case cgltf_primitive_type_triangle_strip:
                    for (std::size_t i = 0; i + 2 < given.size(); ++i) {
                        const auto even = i % 2 == 0;

                        listed.insert(listed.end(), {given[even ? i : i + 1], given[even ? i + 1 : i], given[i + 2]});
                    }
                    break;

                case cgltf_primitive_type_triangle_fan:
                    for (std::size_t i = 1; i + 1 < given.size(); ++i)
                        listed.insert(listed.end(), {given[0], given[i], given[i + 1]});
                    break;

                default: refuse("a primitive draws points or lines, where a model draws triangles");
            }

            std::vector<model_data::index_value_type> out;

            out.reserve(listed.size());

            for (const auto index : listed) {
                if (index >= aVertexCount)
                    refuse("a primitive indexes vertex " + std::to_string(index) + " of "
                        + std::to_string(aVertexCount));

                if (index > std::numeric_limits<model_data::index_value_type>::max())
                    refuse("a primitive indexes a vertex past what a model can address. Split the mesh");

                out.push_back(static_cast<model_data::index_value_type>(index));
            }

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
        if constexpr (std::endian::native != std::endian::little)
            refuse("a document is read in the machine's byte order by cgltf, and glTF is little endian; this machine "
                "is not");

        cgltf_options options{};
        options.memory.alloc_func = allocate;
        options.memory.free_func = release;

        cgltf_data *raw = nullptr;

        if (const auto result = cgltf_parse(&options, aData.data(), aData.size(), &raw);
            result != cgltf_result_success)
            refuse(describe(result));

        const gltf_handle document(raw, cgltf_free);

        if (const auto result = cgltf_load_buffers(&options, document.get(), nullptr);
            result != cgltf_result_success)
            refuse(std::string(describe(result)) + " (external buffers are not read; supply a self-contained glb)");

        if (const auto result = cgltf_validate(document.get()); result != cgltf_result_success)
            refuse(std::string(describe(result)) + " (the document does not describe itself consistently)");

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
                    refuse("this document's image \"" + out.name
                        + "\" claims more bytes than the buffer holding it has");

                const auto *pStart = static_cast<const std::byte *>(view.buffer->data)
                    + view.offset;

                out.bytes.assign(pStart, pStart + view.size);
            }
            else if (image.uri) {
                const std::string_view uri(image.uri);

                const auto comma = uri.find(',');

                if (uri.rfind("data:", 0) != 0 || comma == std::string_view::npos
                    || uri.substr(0, comma).find(";base64") == std::string_view::npos)
                    refuse("this document's image \"" + out.name + "\" names a file rather than carrying its own "
                        "bytes. Supply a self-contained glb");

                out.bytes = decoded_base64(options, uri.substr(comma + 1), out.name);
            }

            content.images.push_back(std::move(out));
        }

        std::vector<std::vector<std::size_t>> remaps;

        for (cgltf_size k = 0; k < document->skins_count; ++k) {
            auto converted = convert(document->skins[k]);

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
            out.local_transform = local_transform_of(&node);

            content.nodes.push_back(std::move(out));
        }

        for (cgltf_size m = 0; m < document->meshes_count; ++m) {
            const auto &mesh = document->meshes[m];

            std::vector<const cgltf_node *> drawnBy;

            for (cgltf_size n = 0; n < document->nodes_count; ++n)
                if (document->nodes[n].mesh == &mesh) drawnBy.push_back(&document->nodes[n]);

            if (drawnBy.empty()) drawnBy.push_back(nullptr);

            const std::string meshName = mesh.name ? mesh.name : "";

            for (const auto *pNode : drawnBy) {
                const std::string nodeName = pNode && pNode->name ? pNode->name : "";

                const std::string parentName = pNode && pNode->parent && pNode->parent->name
                    ? pNode->parent->name
                    : "";

                const auto local = local_transform_of(pNode);

                for (cgltf_size p = 0; p < mesh.primitives_count; ++p) {
                    const auto &primitive = mesh.primitives[p];

                    model_data::attribute_collection_type attributes;

                    std::size_t vertexCount = 0;

                    for (cgltf_size a = 0; a < primitive.attributes_count; ++a) {
                        const auto &attribute = primitive.attributes[a];

                        const auto mapped = aNames.find(attribute.name ? attribute.name : "");

                        if (mapped == aNames.end() || !attribute.data) continue;

                        vertexCount = attribute.data->count;

                        attributes.emplace(mapped->second,
                            attribute_data(floats_of(*attribute.data),
                                static_cast<std::size_t>(cgltf_num_components(attribute.data->type))));
                    }

                    if (attributes.empty()) continue;

                    auto indices = triangle_list_of(primitive, vertexCount);

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

                    if (pNode && pNode->skin) {
                        out.skeleton = static_cast<int>(pNode->skin - document->skins);
                        out.weights = weights_of(primitive, remaps[static_cast<std::size_t>(out.skeleton)]);
                    }

                    content.meshes.push_back(std::move(out));
                }
            }
        }

        return content;
    }
}
