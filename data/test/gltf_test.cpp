// © Joseph Cameron - MIT License

#include <bit>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <string>
#include <vector>

#include <jfc/catch.hpp>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/gltf.h>

using namespace gdk::graphics;

namespace {
    [[nodiscard]] bool big_endian() {
        if constexpr (std::endian::native == std::endian::little) return false;

        WARN("glTF is not read on a big endian machine; what is checked here is that it is refused");

        return true;
    }

    void put32(std::string &aOut, const std::uint32_t aValue) {
        for (int i = 0; i < 4; ++i) aOut.push_back(static_cast<char>((aValue >> (i * 8)) & 0xff));
    }

    void putf(std::string &aOut, const float aValue) {
        char bytes[4];
        std::memcpy(bytes, &aValue, 4);
        aOut.append(bytes, 4);
    }

    [[nodiscard]] std::string glb(std::string aJson, std::string aBin) {
        while (aJson.size() % 4) aJson.push_back(' ');
        while (aBin.size() % 4) aBin.push_back('\0');

        std::string out;
        const auto total = static_cast<std::uint32_t>(
            12 + 8 + aJson.size() + (aBin.empty() ? 0 : 8 + aBin.size()));

        out += "glTF";
        put32(out, 2);
        put32(out, total);
        put32(out, static_cast<std::uint32_t>(aJson.size()));
        put32(out, 0x4E4F534A);
        out += aJson;

        if (!aBin.empty()) {
            put32(out, static_cast<std::uint32_t>(aBin.size()));
            put32(out, 0x004E4942);
            out += aBin;
        }

        return out;
    }

    [[nodiscard]] std::string triangle() {
        std::string bin;

        for (const float v : {0.f,0.f,0.f,  1.f,0.f,0.f,  0.f,2.f,0.f}) putf(bin, v);
        for (const float v : {0.f,0.f,      1.f,0.f,      0.f,1.f})     putf(bin, v);

        return glb(R"({
          "asset":{"version":"2.0"},
          "buffers":[{"byteLength":60}],
          "bufferViews":[
            {"buffer":0,"byteOffset":0, "byteLength":36},
            {"buffer":0,"byteOffset":36,"byteLength":24}],
          "accessors":[
            {"bufferView":0,"componentType":5126,"count":3,"type":"VEC3"},
            {"bufferView":1,"componentType":5126,"count":3,"type":"VEC2"}],
          "meshes":[{"primitives":[{"attributes":{"POSITION":0,"TEXCOORD_0":1}}]}]
        })", bin);
    }
}

namespace {
    [[nodiscard]] std::string skinned() {
        std::string bin;

        for (const float v : {0.f,0.f,0.f,  0.f,1.f,0.f}) putf(bin, v);

        for (int vertex = 0; vertex < 2; ++vertex) {
            const std::uint16_t joints[4] = {static_cast<std::uint16_t>(vertex), 0, 0, 0};
            for (const auto j : joints) { bin.push_back(char(j & 0xff)); bin.push_back(char(j >> 8)); }
        }
        for (int vertex = 0; vertex < 2; ++vertex)
            for (const float w : {1.f, 0.f, 0.f, 0.f}) putf(bin, w);

        for (const float t : {0.f, 1.f}) putf(bin, t);
        for (const float q : {0.f,0.f,0.f,1.f,  0.70710678f,0.f,0.f,0.70710678f}) putf(bin, q);

        return glb(R"({
          "asset":{"version":"2.0"},
          "buffers":[{"byteLength":112}],
          "bufferViews":[
            {"buffer":0,"byteOffset":0,  "byteLength":24},
            {"buffer":0,"byteOffset":24, "byteLength":16},
            {"buffer":0,"byteOffset":40, "byteLength":32},
            {"buffer":0,"byteOffset":72, "byteLength":8},
            {"buffer":0,"byteOffset":80, "byteLength":32}],
          "accessors":[
            {"bufferView":0,"componentType":5126,"count":2,"type":"VEC3"},
            {"bufferView":1,"componentType":5123,"count":2,"type":"VEC4"},
            {"bufferView":2,"componentType":5126,"count":2,"type":"VEC4"},
            {"bufferView":3,"componentType":5126,"count":2,"type":"SCALAR"},
            {"bufferView":4,"componentType":5126,"count":2,"type":"VEC4"}],
          "nodes":[
            {"name":"root","children":[1],"translation":[0,0,0]},
            {"name":"tip","translation":[0,1,0],"rotation":[0,0,0.70710678,0.70710678]},
            {"name":"body","mesh":0,"skin":0}],
          "skins":[{"joints":[1,0]}],
          "meshes":[{"primitives":[{"attributes":{"POSITION":0,"JOINTS_0":1,"WEIGHTS_0":2}}]}],
          "animations":[{
            "samplers":[{"input":3,"output":4,"interpolation":"LINEAR"}],
            "channels":[{"sampler":0,"target":{"node":1,"path":"rotation"}}]}]
        })", bin);
    }
}

TEST_CASE("gdk::graphics::read_gltf skins and animations", "[gdk::graphics]")
{
    if (big_endian()) return;

    const auto content = read_gltf(skinned());

    SECTION("a skin becomes a skeleton with parents before children")
    {
        REQUIRE(content.skeletons.size() == 1);

        const auto &rig = content.skeletons.front();

        REQUIRE(rig.bones.size() == 2);

        REQUIRE(rig.bones[0].name == "root");
        REQUIRE(rig.bones[1].name == "tip");
        REQUIRE(rig.bones[0].parent == -1);
        REQUIRE(rig.bones[1].parent == 0);

        REQUIRE(rig.bones[1].local_position.y == 1.0f);
    }

    SECTION("vertex influences are renumbered through the same reordering")
    {
        REQUIRE(content.meshes.size() == 1);

        const auto &mesh = content.meshes.front();

        REQUIRE(mesh.skeleton == 0);
        REQUIRE(mesh.weights.size() == 2);

        REQUIRE(mesh.weights[0].first == 1);
        REQUIRE(mesh.weights[1].first == 0);
    }

    SECTION("an animation becomes a clip of whole poses")
    {
        REQUIRE(content.animations.size() == 1);

        const auto &clip = content.animations.front();

        REQUIRE(clip.keys.size() == 2);
        REQUIRE(clip.duration == 1.0f);
        REQUIRE(!clip.loops);

        const auto &bind = content.skeletons.front().bones[1].local_rotation;

        REQUIRE(gdk::math::to_degrees(bind.angle()) == Approx(90.0f).margin(0.01));

        const auto composedFirst = bind * clip.keys.front().p.local_rotations[1];

        REQUIRE(gdk::math::to_degrees(composedFirst.angle()) == Approx(0.0f).margin(0.01));

        const auto composedLast = bind * clip.keys.back().p.local_rotations[1];

        REQUIRE(gdk::math::to_degrees(composedLast.angle()) == Approx(90.0f).margin(0.01));
        REQUIRE(composedLast.x == Approx(0.70710678f).margin(1e-4));
        REQUIRE(composedLast.y == Approx(0.0f).margin(1e-4));
        REQUIRE(composedLast.z == Approx(0.0f).margin(1e-4));

        REQUIRE(clip.keys.back().p.local_rotations[0].angle() == Approx(0.0f).margin(1e-5));
    }
}

TEST_CASE("gdk::graphics::read_gltf", "[gdk::graphics]")
{
    if (big_endian()) return;

    const auto document = triangle();

    SECTION("a primitive becomes a model_data, with attributes renamed")
    {
        const auto content = read_gltf(document);

        REQUIRE(content.meshes.size() == 1);

        const auto &mesh = content.meshes.front().data;
        const auto &position = mesh.get_attribute_data("a_Position");
        const auto &uv = mesh.get_attribute_data("a_UV");

        REQUIRE(position.components().size() == 9);
        REQUIRE(uv.components().size() == 6);
        REQUIRE(position.number_of_components_per_attribute() == 3);
        REQUIRE(uv.number_of_components_per_attribute() == 2);

        REQUIRE(position.components()[0] == 0.0f);
        REQUIRE(position.components()[3] == 1.0f);
        REQUIRE(position.components()[7] == 2.0f);
    }

    SECTION("an attribute with no mapping is skipped rather than guessed at")
    {
        const gltf_attribute_names positionsOnly{{"POSITION", "a_Position"}};

        const auto content = read_gltf(document, positionsOnly);

        REQUIRE(content.meshes.size() == 1);
        REQUIRE(content.meshes.front().data.attributes().size() == 1);
    }

    SECTION("malformed documents are rejected rather than half read")
    {
        REQUIRE_THROWS_AS(read_gltf("not a glb at all"), gdk::graphics::exception);
        REQUIRE_THROWS_AS(read_gltf(document.substr(0, 20)), gdk::graphics::exception);
    }

    SECTION("a document naming an external buffer is rejected, not read from disk")
    {
        const auto external = glb(R"({"asset":{"version":"2.0"},
            "buffers":[{"uri":"elsewhere.bin","byteLength":4}]})", "");

        REQUIRE_THROWS_AS(read_gltf(external), gdk::graphics::exception);
    }
}

TEST_CASE("a glTF document's names come back with its geometry", "[gdk::graphics]") {
    if (big_endian()) return;

    const auto document = [] {
        std::string bin;

        for (const float v : {0.f,0.f,0.f,  1.f,0.f,0.f,  0.f,2.f,0.f}) putf(bin, v);
        for (const float v : {0.f,0.f,      1.f,0.f,      0.f,1.f})     putf(bin, v);

        return glb(R"({
          "asset":{"version":"2.0"},
          "buffers":[{"byteLength":60}],
          "bufferViews":[
            {"buffer":0,"byteOffset":0, "byteLength":36},
            {"buffer":0,"byteOffset":36,"byteLength":24}],
          "accessors":[
            {"bufferView":0,"componentType":5126,"count":3,"type":"VEC3"},
            {"bufferView":1,"componentType":5126,"count":3,"type":"VEC2"}],
          "nodes":[{"name":"wall_stone","mesh":0}],
          "meshes":[
            {"name":"Cube.003","primitives":[
              {"attributes":{"POSITION":0,"TEXCOORD_0":1}},
              {"attributes":{"POSITION":0,"TEXCOORD_0":1}}]},
            {"primitives":[{"attributes":{"POSITION":0,"TEXCOORD_0":1}}]}]
        })", bin);
    }();

    const auto content = read_gltf(document);

    REQUIRE(content.meshes.size() == 3);

    SECTION("**the node's name and the mesh's name are different facts, and both come back**")
    {
        REQUIRE(content.meshes[0].node_name == "wall_stone");
        REQUIRE(content.meshes[0].mesh_name == "Cube.003");
    }

    SECTION("**a mesh of several primitives repeats its name, once per primitive**")
    {
        REQUIRE(content.meshes[1].node_name == content.meshes[0].node_name);
        REQUIRE(content.meshes[1].mesh_name == content.meshes[0].mesh_name);
    }

    SECTION("**what nothing names has no name, rather than inheriting one**")
    {
        REQUIRE(content.meshes[2].mesh_name.empty());
        REQUIRE(content.meshes[2].node_name.empty());
    }
}

TEST_CASE("a glTF node's parent, material and placement come back with it", "[gdk::graphics]") {
    if (big_endian()) return;

    const auto document = [] {
        std::string bin;

        for (const float v : {0.f,0.f,0.f,  1.f,0.f,0.f,  0.f,2.f,0.f}) putf(bin, v);
        for (const float v : {0.f,0.f,      1.f,0.f,      0.f,1.f})     putf(bin, v);

        return glb(R"({
          "asset":{"version":"2.0"},
          "buffers":[{"byteLength":60}],
          "bufferViews":[
            {"buffer":0,"byteOffset":0, "byteLength":36},
            {"buffer":0,"byteOffset":36,"byteLength":24}],
          "accessors":[
            {"bufferView":0,"componentType":5126,"count":3,"type":"VEC3"},
            {"bufferView":1,"componentType":5126,"count":3,"type":"VEC2"}],
          "materials":[{"name":"accent"}],
          "nodes":[
            {"name":"pillar","mesh":0,"children":[1]},
            {"name":"collider","mesh":1,"translation":[0,0.5,0]}],
          "meshes":[
            {"name":"pillar_mesh","primitives":[
              {"attributes":{"POSITION":0,"TEXCOORD_0":1},"material":0}]},
            {"name":"collider_mesh_data","primitives":[
              {"attributes":{"POSITION":0,"TEXCOORD_0":1}}]}]
        })", bin);
    }();

    const auto content = read_gltf(document);

    REQUIRE(content.meshes.size() == 2);

    SECTION("**a child knows its parent by name, since a parent may carry no mesh at all**")
    {
        REQUIRE(content.meshes[0].parent_name.empty());
        REQUIRE(content.meshes[1].parent_name == "pillar");
    }

    SECTION("**a material comes back as its name, not as its parameters**")
    {
        REQUIRE(content.meshes[0].material_name == "accent");

        REQUIRE(content.meshes[1].material_name.empty());
    }

    SECTION("**a node moved in object mode carries the offset in its transform**")
    {
        REQUIRE(content.meshes[1].data.attributes().at("a_Position").components()[1]
            == Approx(0.0f));

        REQUIRE(content.meshes[1].local_transform[7] == Approx(0.5f));

        REQUIRE(content.meshes[1].local_transform[3] == Approx(0.0f));
        REQUIRE(content.meshes[1].local_transform[11] == Approx(0.0f));
    }

    SECTION("**and an unmoved node is identity rather than whatever was there**")
    {
        const auto &identity = content.meshes[0].local_transform;

        for (std::size_t i = 0; i < 16; ++i)
            REQUIRE(identity[i] == Approx(i % 5 == 0 ? 1.0f : 0.0f));
    }
}

TEST_CASE("a primitive's indices come back with it", "[gdk::graphics]") {
    if (big_endian()) return;

    const auto indexed = [] {
        std::string bin;

        for (const float v : {0.f,0.f,0.f, 1.f,0.f,0.f, 1.f,1.f,0.f, 0.f,1.f,0.f}) putf(bin, v);

        for (const std::uint16_t i : std::initializer_list<std::uint16_t>{0, 1, 2, 0, 2, 3}) {
            bin.push_back(static_cast<char>(i & 0xff));
            bin.push_back(static_cast<char>(i >> 8));
        }

        return glb(R"({
          "asset":{"version":"2.0"},
          "buffers":[{"byteLength":60}],
          "bufferViews":[
            {"buffer":0,"byteOffset":0, "byteLength":48},
            {"buffer":0,"byteOffset":48,"byteLength":12}],
          "accessors":[
            {"bufferView":0,"componentType":5126,"count":4,"type":"VEC3"},
            {"bufferView":1,"componentType":5123,"count":6,"type":"SCALAR"}],
          "meshes":[{"primitives":[{"attributes":{"POSITION":0},"indices":1}]}]
        })", bin);
    }();

    const auto content = read_gltf(indexed);

    REQUIRE(content.meshes.size() == 1);

    SECTION("**an indexed primitive stays indexed, and keeps its winding**")
    {
        REQUIRE(content.meshes.front().data.indexed());

        const auto &indexes = content.meshes.front().data.indexes();

        REQUIRE(indexes.size() == 6);

        REQUIRE(indexes[0] == 0);
        REQUIRE(indexes[1] == 1);
        REQUIRE(indexes[2] == 2);
        REQUIRE(indexes[3] == 0);
        REQUIRE(indexes[4] == 2);
        REQUIRE(indexes[5] == 3);
    }

    SECTION("**and a primitive with none is left unindexed rather than given some**")
    {
        REQUIRE_FALSE(read_gltf(triangle()).meshes.front().data.indexed());
    }
}

TEST_CASE("a glTF document's images come back encoded", "[gdk::graphics]") {
    if (big_endian()) return;

    const std::string picture = "\x89PNG\r\n\x1a\n";

    SECTION("**an image in the binary chunk is handed back byte for byte**")
    {
        std::string bin;

        for (const float v : {0.f,0.f,0.f, 1.f,0.f,0.f, 0.f,1.f,0.f}) putf(bin, v);

        bin += picture;

        const auto document = glb(R"({
          "asset":{"version":"2.0"},
          "buffers":[{"byteLength":44}],
          "bufferViews":[
            {"buffer":0,"byteOffset":0, "byteLength":36},
            {"buffer":0,"byteOffset":36,"byteLength":8}],
          "accessors":[{"bufferView":0,"componentType":5126,"count":3,"type":"VEC3"}],
          "images":[{"name":"brick","bufferView":1,"mimeType":"image/png"}],
          "textures":[{"source":0}],
          "materials":[{"name":"stone","pbrMetallicRoughness":{
            "baseColorTexture":{"index":0}}}],
          "meshes":[{"primitives":[
            {"attributes":{"POSITION":0},"material":0}]}]
        })", bin);

        const auto content = read_gltf(document);

        REQUIRE(content.images.size() == 1);
        REQUIRE(content.images.front().name == "brick");
        REQUIRE(content.images.front().mime_type == "image/png");

        REQUIRE(content.images.front().bytes.size() == picture.size());

        REQUIRE(std::memcmp(content.images.front().bytes.data(), picture.data(), picture.size())
            == 0);
    }

    SECTION("**and a primitive says which image its material paints with**")
    {
        std::string bin;

        for (const float v : {0.f,0.f,0.f, 1.f,0.f,0.f, 0.f,1.f,0.f}) putf(bin, v);

        bin += picture;

        const auto document = glb(R"({
          "asset":{"version":"2.0"},
          "buffers":[{"byteLength":44}],
          "bufferViews":[
            {"buffer":0,"byteOffset":0, "byteLength":36},
            {"buffer":0,"byteOffset":36,"byteLength":8}],
          "accessors":[{"bufferView":0,"componentType":5126,"count":3,"type":"VEC3"}],
          "images":[{"name":"brick","bufferView":1,"mimeType":"image/png"}],
          "textures":[{"source":0}],
          "materials":[{"name":"stone","pbrMetallicRoughness":{
            "baseColorTexture":{"index":0}}}],
          "meshes":[{"primitives":[
            {"attributes":{"POSITION":0},"material":0}]}]
        })", bin);

        const auto content = read_gltf(document);

        REQUIRE(content.meshes.front().base_color_image == 0);
        REQUIRE(read_gltf(triangle()).meshes.front().base_color_image == -1);
    }

    SECTION("**an image naming a file is refused, the way an external buffer is**")
    {
        const auto external = glb(R"({
          "asset":{"version":"2.0"},
          "images":[{"name":"brick","uri":"../../../etc/passwd"}]
        })", "");

        try {
            const auto ignored = read_gltf(external);

            FAIL("expected a document naming a file to be refused");
        }
        catch (const gdk::graphics::exception &aException) {
            const std::string said = aException.what();

            INFO("said: " << said);

            REQUIRE(said.find("names a file") != std::string::npos);
        }
    }
}

namespace {
    [[nodiscard]] std::string base64(const std::string &aBytes) {
        static constexpr char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        std::string out;

        for (std::size_t i = 0; i < aBytes.size(); i += 3) {
            std::uint32_t chunk = 0;

            const auto have = std::min<std::size_t>(3, aBytes.size() - i);

            for (std::size_t j = 0; j < 3; ++j)
                chunk = (chunk << 8) | (j < have ? static_cast<unsigned char>(aBytes[i + j]) : 0u);

            for (std::size_t j = 0; j < 4; ++j)
                out.push_back(j <= have ? ALPHABET[(chunk >> (18 - 6 * j)) & 0x3F] : '=');
        }

        return out;
    }

    [[nodiscard]] std::string positions_and(const std::string &aRest, const std::string &aExtraBin = {}) {
        std::string bin;

        for (const float v : {0.f,0.f,0.f,  1.f,0.f,0.f,  0.f,1.f,0.f,  1.f,1.f,0.f}) putf(bin, v);

        bin += aExtraBin;

        return glb(R"({
          "asset":{"version":"2.0"},
          "buffers":[{"byteLength":)" + std::to_string(bin.size()) + R"(}],
          "bufferViews":[{"buffer":0,"byteOffset":0,"byteLength":48})" + aRest, bin);
    }
}

TEST_CASE("a glTF image carried as a data uri comes back byte for byte, however it is padded", "[gdk::graphics]") {
    if (big_endian()) return;

    for (const std::string payload : {"a", "ab", "abc", "abcd", "abcde", "abcdef"}) {
        INFO("payload " << payload);

        const auto document = glb(R"({"asset":{"version":"2.0"},
            "images":[{"name":"inline","uri":"data:image/png;base64,)" + base64(payload) + R"("}]})", "");

        const auto content = read_gltf(document);

        REQUIRE(content.images.size() == 1);
        REQUIRE(std::string(reinterpret_cast<const char *>(content.images[0].bytes.data()),
            content.images[0].bytes.size()) == payload);
    }
}

TEST_CASE("a glTF accessor of the wrong type for what reads it is refused, not read past", "[gdk::graphics]") {
    if (big_endian()) return;

    const auto animated = [](const std::string &aOutputType, const std::string &aInputType) {
        std::string bin;

        for (const float t : {0.f, 1.f, 0.f, 0.f}) putf(bin, t);
        for (int i = 0; i < 8; ++i) putf(bin, 0.f);

        return glb(R"({
          "asset":{"version":"2.0"},
          "buffers":[{"byteLength":48}],
          "bufferViews":[{"buffer":0,"byteOffset":0,"byteLength":16},{"buffer":0,"byteOffset":16,"byteLength":32}],
          "accessors":[
            {"bufferView":0,"componentType":5126,"count":)" + std::string(aInputType == "SCALAR" ? "2" : "1")
                + R"(,"type":")" + aInputType + R"("},
            {"bufferView":1,"componentType":5126,"count":2,"type":")" + aOutputType + R"("}],
          "nodes":[{"name":"root"}],
          "skins":[{"joints":[0]}],
          "animations":[{
            "samplers":[{"input":0,"output":1}],
            "channels":[{"sampler":0,"target":{"node":0,"path":"rotation"}}]}]
        })", bin);
    };

    REQUIRE_NOTHROW(read_gltf(animated("VEC4", "SCALAR")));

    SECTION("a rotation that is not four components") {
        REQUIRE_THROWS_AS(read_gltf(animated("VEC3", "SCALAR")), exception);
        REQUIRE_THROWS_AS(read_gltf(animated("VEC2", "SCALAR")), exception);
    }

    SECTION("times that are not scalars") {
        REQUIRE_THROWS_AS(read_gltf(animated("VEC4", "VEC4")), exception);
    }

    SECTION("weights that are not four components") {
        std::string extra;

        for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) { extra.push_back(0); extra.push_back(0); }
        for (int i = 0; i < 4; ++i) putf(extra, 1.f);

        const auto document = positions_and(R"(,
            {"buffer":0,"byteOffset":48,"byteLength":32},{"buffer":0,"byteOffset":80,"byteLength":16}],
          "accessors":[
            {"bufferView":0,"componentType":5126,"count":4,"type":"VEC3"},
            {"bufferView":1,"componentType":5123,"count":4,"type":"VEC4"},
            {"bufferView":2,"componentType":5126,"count":4,"type":"SCALAR"}],
          "nodes":[{"name":"bone"},{"name":"body","mesh":0,"skin":0}],
          "skins":[{"joints":[0]}],
          "meshes":[{"primitives":[{"attributes":{"POSITION":0,"JOINTS_0":1,"WEIGHTS_0":2}}]}]
        })", extra);

        REQUIRE_THROWS_AS(read_gltf(document), exception);
    }
}

TEST_CASE("a glTF document cannot make the reader take more than it should", "[gdk::graphics]") {
    if (big_endian()) return;

    SECTION("a count far past the document's size is refused before anything is made") {
        const auto document = glb(R"({"asset":{"version":"2.0"},
            "accessors":[{"componentType":5126,"count":400000000,"type":"VEC3"}],
            "meshes":[{"primitives":[{"attributes":{"POSITION":0}}]}]})", "");

        REQUIRE_THROWS_AS(read_gltf(document), exception);
    }

    SECTION("a skin with no joints is refused") {
        const auto document = glb(R"({"asset":{"version":"2.0"},"nodes":[{"name":"a"}],"skins":[{"joints":[]}]})", "");

        REQUIRE_THROWS_AS(read_gltf(document), exception);
    }

    SECTION("an index past the primitive's vertices is refused") {
        std::string extra;

        for (const std::uint16_t i : std::initializer_list<std::uint16_t>{0, 1, 9}) { extra.push_back(char(i & 0xff)); extra.push_back(char(i >> 8)); }

        const auto document = positions_and(R"(,{"buffer":0,"byteOffset":48,"byteLength":6}],
          "accessors":[
            {"bufferView":0,"componentType":5126,"count":4,"type":"VEC3"},
            {"bufferView":1,"componentType":5123,"count":3,"type":"SCALAR"}],
          "meshes":[{"primitives":[{"attributes":{"POSITION":0},"indices":1}]}]
        })", extra);

        REQUIRE_THROWS_AS(read_gltf(document), exception);
    }
}

TEST_CASE("a glTF vertex is weighted to its two strongest joints", "[gdk::graphics]") {
    if (big_endian()) return;

    std::string extra;

    for (const std::uint16_t j : std::initializer_list<std::uint16_t>{0, 1, 2, 3}) { extra.push_back(char(j & 0xff)); extra.push_back(char(j >> 8)); }
    for (int padding = 0; padding < 3; ++padding) for (int i = 0; i < 8; ++i) extra.push_back(0);
    for (const float w : {0.3f, 0.1f, 0.6f, 0.f}) putf(extra, w);
    for (int padding = 0; padding < 3; ++padding) for (int i = 0; i < 4; ++i) putf(extra, i == 0 ? 1.f : 0.f);

    const auto document = positions_and(R"(,
        {"buffer":0,"byteOffset":48,"byteLength":32},{"buffer":0,"byteOffset":80,"byteLength":64}],
      "accessors":[
        {"bufferView":0,"componentType":5126,"count":4,"type":"VEC3"},
        {"bufferView":1,"componentType":5123,"count":4,"type":"VEC4"},
        {"bufferView":2,"componentType":5126,"count":4,"type":"VEC4"}],
      "nodes":[{"name":"a","children":[1]},{"name":"b","children":[2]},{"name":"c","children":[3]},{"name":"d"},
        {"name":"body","mesh":0,"skin":0}],
      "skins":[{"joints":[0,1,2,3]}],
      "meshes":[{"primitives":[{"attributes":{"POSITION":0,"JOINTS_0":1,"WEIGHTS_0":2}}]}]
    })", extra);

    const auto content = read_gltf(document);

    REQUIRE(content.meshes.size() == 1);

    const auto &weights = content.meshes[0].weights[0];

    REQUIRE(weights.first == 2);
    REQUIRE(weights.second == 0);
    REQUIRE(weights.first_weight == Approx(0.6f / 0.9f));
}

TEST_CASE("a glTF primitive drawn as a strip or a fan comes back as a triangle list", "[gdk::graphics]") {
    if (big_endian()) return;

    const auto drawnAs = [](const int aMode) {
        return positions_and(R"(],
          "accessors":[{"bufferView":0,"componentType":5126,"count":4,"type":"VEC3"}],
          "meshes":[{"primitives":[{"attributes":{"POSITION":0},"mode":)" + std::to_string(aMode) + R"(}]}]
        })");
    };

    SECTION("a strip alternates its winding, as glTF says") {
        const auto content = read_gltf(drawnAs(5));

        REQUIRE(content.meshes[0].data.indexes() == std::vector<model_data::index_value_type>{0, 1, 2, 2, 1, 3});
    }

    SECTION("a fan turns about its first vertex") {
        const auto content = read_gltf(drawnAs(6));

        REQUIRE(content.meshes[0].data.indexes() == std::vector<model_data::index_value_type>{0, 1, 2, 0, 2, 3});
    }

    SECTION("points and lines are refused, as a model draws triangles") {
        for (const auto mode : {0, 1, 2, 3}) REQUIRE_THROWS_AS(read_gltf(drawnAs(mode)), exception);
    }
}

TEST_CASE("a glTF mesh drawn by several nodes comes back once for each", "[gdk::graphics]") {
    if (big_endian()) return;

    const auto document = positions_and(R"(],
      "accessors":[{"bufferView":0,"componentType":5126,"count":4,"type":"VEC3"}],
      "nodes":[{"name":"left","mesh":0,"translation":[-1,0,0]},{"name":"right","mesh":0,"translation":[1,0,0]}],
      "meshes":[{"name":"crate","primitives":[{"attributes":{"POSITION":0}}]}]
    })");

    const auto content = read_gltf(document);

    REQUIRE(content.meshes.size() == 2);
    REQUIRE(content.meshes[0].node_name == "left");
    REQUIRE(content.meshes[1].node_name == "right");
    REQUIRE(content.meshes[0].mesh_name == "crate");
    REQUIRE(content.meshes[0].local_transform[3] == -1.0f);
    REQUIRE(content.meshes[1].local_transform[3] == 1.0f);
}

TEST_CASE("a glTF root bone carries the armature it hangs from", "[gdk::graphics]") {
    if (big_endian()) return;

    const auto document = glb(R"({"asset":{"version":"2.0"},
        "nodes":[{"name":"Armature","children":[1],"translation":[0,0,5],"rotation":[0,0,0.70710678,0.70710678]},
            {"name":"hips","translation":[1,0,0]}],
        "skins":[{"joints":[1]}]})", "");

    const auto content = read_gltf(document);
    const auto &hips = content.skeletons.at(0).bones.at(0);

    REQUIRE(hips.local_position.x == Approx(0.0f).margin(1e-5));
    REQUIRE(hips.local_position.y == Approx(1.0f).margin(1e-5));
    REQUIRE(hips.local_position.z == Approx(5.0f).margin(1e-5));
    REQUIRE(hips.local_rotation.z == Approx(0.70710678f).margin(1e-5));
}

TEST_CASE("a glTF document is refused on a big endian machine, rather than misread", "[gdk::graphics]") {
    if constexpr (std::endian::native == std::endian::little) return;

    REQUIRE_THROWS_AS(read_gltf(triangle()), exception);
}
