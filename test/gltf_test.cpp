// © Joseph Cameron - All Rights Reserved

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <jfc/catch.hpp>

#include <gdk/graphics/ext/gltf.h>

using namespace gdk::graphics;

namespace {
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

        REQUIRE(gdk::to_degrees(bind.angle()) == Approx(90.0f).margin(0.01));

        const auto composedFirst = bind * clip.keys.front().p.local_rotations[1];

        REQUIRE(gdk::to_degrees(composedFirst.angle()) == Approx(0.0f).margin(0.01));

        const auto composedLast = bind * clip.keys.back().p.local_rotations[1];

        REQUIRE(gdk::to_degrees(composedLast.angle()) == Approx(90.0f).margin(0.01));
        REQUIRE(composedLast.x == Approx(0.70710678f).margin(1e-4));
        REQUIRE(composedLast.y == Approx(0.0f).margin(1e-4));
        REQUIRE(composedLast.z == Approx(0.0f).margin(1e-4));

        REQUIRE(clip.keys.back().p.local_rotations[0].angle() == Approx(0.0f).margin(1e-5));
    }
}

TEST_CASE("gdk::graphics::read_gltf", "[gdk::graphics]")
{
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
        REQUIRE_THROWS_AS(read_gltf("not a glb at all"), std::runtime_error);
        REQUIRE_THROWS_AS(read_gltf(document.substr(0, 20)), std::runtime_error);
    }

    SECTION("a document naming an external buffer is rejected, not read from disk")
    {
        const auto external = glb(R"({"asset":{"version":"2.0"},
            "buffers":[{"uri":"elsewhere.bin","byteLength":4}]})", "");

        REQUIRE_THROWS_AS(read_gltf(external), std::runtime_error);
    }
}
