// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/animation/bvh.h>
#include "content.h"
#include "simple_skin.h"

#include <gdk/graphics/ext/gltf.h>
#include <gdk/graphics/ext/animation/ik.h>
#include <gdk/graphics/ext/animation/profile.h>
#include <gdk/graphics/ext/animation/retarget.h>
#include <gdk/graphics/ext/animation/skinning.h>

#include <gdk/timing/game_loop.h>
#include <gdk/windowing/impl_glfw_window.h>
#include <gdk/graphics/context.h>
#include <gdk/math.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/scene.h>
#include <gdk/graphics/texture_data.h>
#include <gdk/graphics/webgl1es2_context.h>

#include <jfc/events/event.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace gdk;

using namespace gdk::graphics;
using namespace gdk::timing;

namespace {
    struct rig_bounds final {
        float height = 1;
        float lowest = 0;
    };

    [[nodiscard]] rig_bounds measure(const demo::skeleton &aSkeleton) {
        const auto extent = demo::bind_vertical_extent(aSkeleton);

        return {std::max(extent.height(), 0.001f), extent.lowest};
    }

    struct playback final {
        std::size_t current = 0;
        std::size_t previous = 0;

        float transition = 1.0f;     
        float until_switch = 4.0f;   

        float phase = 0;
        float previous_phase = 0;

        demo::vector3_type position;
        float facing = 0;

        float gesture = 0;
        float gesture_phase = 0;
        bool gesturing = false;
        float until_gesture = 6.0f;
    };

    void advance(playback &aPlayback, const std::vector<demo::clip> &aClips, const float aDeltaTime) {
        const float fadeSeconds = 0.3f;

        aPlayback.previous_phase = aPlayback.phase;
        aPlayback.phase += aDeltaTime / aClips[aPlayback.current].duration;
        aPlayback.phase -= std::floor(aPlayback.phase);

        aPlayback.transition = std::min(1.0f, aPlayback.transition + aDeltaTime / fadeSeconds);

        if (aClips.size() < 2) return;

        aPlayback.until_switch -= aDeltaTime;

        if (aPlayback.until_switch > 0) return;

        aPlayback.previous = aPlayback.current;
        aPlayback.current = (aPlayback.current + 1) % aClips.size();
        aPlayback.transition = 0;
        aPlayback.until_switch = 4.0f;
    }

    void advance_gesture(playback &aPlayback, const demo::clip &aWave, const float aDeltaTime) {
        aPlayback.until_gesture -= aDeltaTime;

        if (aPlayback.until_gesture <= 0) {
            aPlayback.gesturing = !aPlayback.gesturing;
            aPlayback.until_gesture = aPlayback.gesturing ? 3.6f : 5.0f;

            if (aPlayback.gesturing) aPlayback.gesture_phase = 0;
        }

        aPlayback.gesture_phase += aDeltaTime / aWave.duration;
        aPlayback.gesture_phase -= std::floor(aPlayback.gesture_phase);

        const float fade = 0.4f;
        aPlayback.gesture = std::max(0.0f, std::min(1.0f, aPlayback.gesture
            + (aPlayback.gesturing ? aDeltaTime / fade : -aDeltaTime / fade)));
    }

    [[nodiscard]] demo::vector3_type root_step(const playback &aPlayback,
        const std::vector<demo::clip> &aClips) {
        const auto step = [&](const demo::clip &aClip) {
            const float laps = aPlayback.phase < aPlayback.previous_phase ? 1.0f : 0.0f;

            return demo::root_delta(aClip, aPlayback.previous_phase * aClip.duration,
                (aPlayback.phase + laps) * aClip.duration);
        };

        const auto to = step(aClips[aPlayback.current]);

        if (aPlayback.transition >= 1.0f) return to;

        const auto from = step(aClips[aPlayback.previous]);
        const auto u = aPlayback.transition * aPlayback.transition * (3.0f - 2.0f * aPlayback.transition);

        return from + (to - from) * u;
    }

    [[nodiscard]] demo::pose pose_at(const playback &aPlayback, const std::vector<demo::clip> &aClips) {
        const auto &to = aClips[aPlayback.current];
        const auto sampled = demo::sample(to, aPlayback.phase * to.duration);

        if (aPlayback.transition >= 1.0f) return sampled;

        const auto &from = aClips[aPlayback.previous];

        const auto u = aPlayback.transition * aPlayback.transition * (3.0f - 2.0f * aPlayback.transition);

        return demo::blend(demo::sample(from, aPlayback.phase * from.duration), sampled, u);
    }

    [[nodiscard]] float ground_height(const float aX, const float aZ) {
        return 0.055f * std::sin(aX * 2.1f) + 0.045f * std::cos(aZ * 2.7f)
            + 0.030f * std::sin((aX + aZ) * 1.3f);
    }

    [[nodiscard]] gdk::graphics::model_data make_ground(const float aExtent, const std::size_t aCells) {
        std::vector<float> positions, uvs;

        const auto vertex = [&](const float x, const float z) {
            positions.insert(positions.end(), {x, ground_height(x, z), z});
            uvs.insert(uvs.end(), {x * 0.5f, z * 0.5f});
        };

        const float step = (aExtent * 2.0f) / static_cast<float>(aCells);

        for (std::size_t i = 0; i < aCells; ++i)
            for (std::size_t j = 0; j < aCells; ++j) {
                const float x = -aExtent + static_cast<float>(i) * step;
                const float z = -aExtent + static_cast<float>(j) * step;

                vertex(x, z);           vertex(x + step, z);    vertex(x, z + step);
                vertex(x + step, z);    vertex(x + step, z + step); vertex(x, z + step);
            }

        return gdk::graphics::model_data({{"a_Position", {positions, 3}}, {"a_UV", {uvs, 2}}});
    }

    [[nodiscard]] demo::pose plant_foot(const demo::skeleton &aSkeleton, const demo::pose &aPose,
        const std::size_t aHip, const std::size_t aFoot, const demo::vector3_type &aCharacter,
        const demo::quaternion_type &aHeading, const demo::foot_arc &aArc, const float aSole,
        const std::vector<demo::matrix4x4_type> &aWorld) {
        const auto &world = aWorld;

        const auto weight = demo::stance_weight(aArc, world[aFoot].translation().y);

        if (weight <= 0.001f) return aPose;

        const auto rigFoot = world[aFoot].translation();
        const auto worldFoot = (aHeading * rigFoot) + aCharacter;

        const auto ground = ground_height(worldFoot.x, worldFoot.z) + aSole;

        if (worldFoot.y >= ground) return aPose;

        return demo::blend(aPose, demo::with_limb_reaching_in_world(aSkeleton, aPose, aHip,
            demo::vector3_type(worldFoot.x, ground, worldFoot.z),
            aHeading * demo::vector3_type(0, 0, -1), aCharacter, aHeading), weight);
    }

    [[nodiscard]] texture_ptr_type make_checker(const context_ptr_type &aGraphics) {
        static const std::vector<texture_data::encoded_byte> pixels{
            0xf0, 0xf0, 0xf0, 0xff,   0x40, 0x50, 0x90, 0xff,
            0x40, 0x50, 0x90, 0xff,   0xf0, 0xf0, 0xf0, 0xff};

        texture_data::view view;
        view.format = texture::format::rgba;
        view.width = 2;
        view.height = 2;
        view.data = &pixels.front();

        return aGraphics->make_texture(view);
    }
}

namespace {
    struct loaded final {
        demo::skeleton rig;
        std::vector<demo::clip> clips;

        demo::skinned_mesh mesh;
        bool has_mesh = false;

        std::string described = "procedural";
    };

    [[nodiscard]] bool looks_like_gltf(const std::string_view aBytes) {
        return aBytes.compare(0, 4, "glTF") == 0
            || aBytes.find_first_not_of(" \t\r\n") == aBytes.find('{');
    }

    [[nodiscard]] loaded from_gltf(const std::string_view aBytes, const std::string &aWhat) {
        const auto content = gdk::graphics::read_gltf(aBytes);

        if (content.skeletons.empty()) throw std::runtime_error(aWhat + ": it carries no skin");

        loaded out;
        out.rig = content.skeletons.front();
        out.described = aWhat;

        for (const auto &clip : content.animations) out.clips.push_back(clip);

        if (out.clips.empty()) {
            demo::clip rest;
            rest.keys.push_back({0, demo::pose::rest(out.rig), {}});
            out.clips.push_back(std::move(rest));
        }

        for (const auto &mesh : content.meshes) {
            if (mesh.skeleton < 0 || mesh.weights.empty()) continue;

            out.mesh.data = mesh.data;
            out.mesh.weights = mesh.weights;

            const auto &components = mesh.data.get_attribute_data("a_Position").components();

            for (std::size_t v = 0; v + 2 < components.size(); v += 3)
                out.mesh.bind_positions.push_back({components[v], components[v + 1],
                    components[v + 2]});

            out.has_mesh = true;

            break;
        }

        return out;
    }
}

namespace {
    int run(int argc, char **argv) {
    const auto gltf = [&]() -> std::unique_ptr<loaded> {
        if (argc > 1 && std::string(argv[1]) == "gltf")
            return std::make_unique<loaded>(from_gltf(demo::SIMPLE_SKIN_GLTF,
                "SimpleSkin, compiled in"));

        if (argc < 2) return nullptr;

        std::ifstream file(argv[1], std::ios::binary);

        if (!file) return nullptr;

        const std::string bytes{std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()};

        if (!looks_like_gltf(bytes)) return nullptr;

        return std::make_unique<loaded>(from_gltf(bytes, argv[1]));
    }();

    const auto content = [&]() -> demo::bvh_content {
        if (argc < 2 || gltf) {
            demo::bvh_content procedural;

            procedural.rig = demo::make_humanoid().with_oriented_bone_frames();

            procedural.animation = demo::make_walk_clip(procedural.rig, 16);

            return procedural;
        }

        std::ifstream file(argv[1], std::ios::binary);

        if (!file) throw std::runtime_error(std::string("cannot open \"") + argv[1]
            + "\"\n\nusage:\n"
            "  skeleton_demo                       the procedural humanoid walk\n"
            "  skeleton_demo gltf                  the compiled-in glTF sample\n"
            "  skeleton_demo <file.glb|.gltf>      any self-contained glTF\n"
            "  skeleton_demo <file.bvh> [scale]    a motion capture, retargeted\n"
            "  skeleton_demo <file.bvh> 1 raw      the capture on its own rig\n\n"
            "demo/skeleton/assets/fetch.sh downloads a fully rigged and animated sample.");

        const std::string text{std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()};

        const auto scale = argc > 2 ? std::stof(argv[2]) : 1.0f;

        auto loaded = demo::read_bvh(text, scale);

        loaded.animation.loops = true;

        return loaded;
    }();

    const auto pWindow = windowing::impl_glfw_window::make("skeleton demo");

    jfc::events::event<float, float> update_event;

    const auto pGraphics = webgl1es2_context::make();
    const auto pScene = pGraphics->make_scene();

    const auto displayed = [&]() -> demo::bvh_content {
        const auto raw = argc > 3 && std::string(argv[3]) == "raw";

        if (argc < 2 || raw || gltf) return content;

        const auto source = demo::bind_profile(demo::humanoid_profile(), content.rig);
        const auto target = demo::bind_profile(demo::humanoid_profile(),
            demo::make_humanoid().with_oriented_bone_frames());

        const auto mapping = demo::map_by_profile(source, target);
        const auto mapped = demo::mapped_bone_count(mapping);

        if (mapped < 8) {
            std::cout << "recognised " << mapped << " of 16 joint names; playing the capture on its "
                "own rig instead\n";

            return content;
        }

        demo::bvh_content result;
        result.rig = demo::make_humanoid().with_oriented_bone_frames();
        result.animation = demo::retarget(content.rig, content.animation, result.rig, mapping);
        result.frame_time = content.frame_time;

        std::cout << "retargeted onto the demo's humanoid, " << mapped << " of 16 bones mapped\n";

        return result;
    }();

    const auto &skeleton = gltf ? gltf->rig : displayed.rig;

    const auto clips = [&]() {
        if (gltf) return gltf->clips;
        if (argc >= 2) return std::vector<demo::clip>{displayed.animation};

        return std::vector<demo::clip>{
            demo::make_gait_clip(skeleton, demo::idling(), 16),
            demo::make_gait_clip(skeleton, demo::walking(), 16),
            demo::make_gait_clip(skeleton, demo::running(), 16)};
    }();

    const auto bounds = measure(skeleton);

    const auto pCamera = [&]() {
        auto pCamera = pGraphics->make_camera();
        pCamera->set_clear_color(color::cornflower_blue);
        pScene->add(pCamera);

        update_event.subscribe([pCamera, &pWindow, bounds](float, float) {
            pCamera->set_perspective_projection(to_radians(60.0f), bounds.height * 0.01f,
                bounds.height * 20.0f, static_cast<floating_point_type>(pWindow->aspect_ratio()));
            pCamera->set_transform({0, bounds.height * 0.55f, bounds.height * 2.2f},
                quaternion_type::identity);
        });

        return pCamera;
    }();

    const auto inverseBind = skeleton.inverse_bind_transforms();

    const auto sole = demo::limb_half_width(skeleton);

    const auto binding = demo::bind_profile(demo::humanoid_profile(), skeleton);
    const auto groundContacts = binding.ground_contact_bones();

    const bool humanoidRig = binding.placed() == demo::humanoid_profile().roles.size();

    if (!humanoidRig)
        std::cout << "rig fills " << binding.placed() << " of "
            << demo::humanoid_profile().roles.size()
            << " humanoid roles; foot planting and the gesture layer are off\n";

    const auto footArcs = [&]() {
        std::vector<std::vector<demo::foot_arc>> arcs;

        if (!humanoidRig) return arcs;

        for (const auto &c : clips) {
            std::vector<demo::foot_arc> perFoot;

            for (const auto foot : groundContacts)
                perFoot.push_back(demo::measure_foot_arc(skeleton, c, foot));

            arcs.push_back(std::move(perFoot));
        }

        return arcs;
    }();

    auto mesh = gltf && gltf->has_mesh ? gltf->mesh : demo::make_bone_boxes(skeleton, sole);

    std::cout << skeleton.bones.size() << " bones, " << clips.size() << " clip"
        << (clips.size() == 1 ? "" : "s") << ", " << bounds.height << " units tall\n";

    const auto pMaterial = [&]() {
        auto pMaterial = pGraphics->make_material(pGraphics->make_alpha_cutoff_shader());
        pMaterial->set_texture("_Texture", make_checker(pGraphics));
        pMaterial->set_vector2("_UVScale", {1, 1});
        pMaterial->set_vector2("_UVOffset", {0, 0});

        return pMaterial;
    }();

    const auto pModel = pGraphics->make_model(model::usage_hint::dynamic, mesh.data);

    const auto pEntity = [&]() {
        auto pEntity = pGraphics->make_entity(pModel, pMaterial);
        pScene->add(pEntity);

        return pEntity;
    }();

    const auto pGround = [&]() {
        auto pGroundEntity = pGraphics->make_entity(
            pGraphics->make_model(model::usage_hint::upload_once, make_ground(3.0f, 48)), pMaterial);
        pScene->add(pGroundEntity);

        return pGroundEntity;
    }();

    playback state;

    const auto wave = humanoidRig ? demo::make_wave_clip(skeleton, 12) : demo::clip{};
    const auto restPose = demo::pose::rest(skeleton);
    const auto armMask = humanoidRig
        ? demo::mask_for_subtree(skeleton,
            static_cast<std::size_t>(binding.bone_for("rightArm")))
        : demo::bone_mask(skeleton.bones.size(), 0.0f);

    update_event.subscribe([&](float, const float deltaTime) {
        advance(state, clips, deltaTime);
        advance_gesture(state, wave, deltaTime);

        const auto step = root_step(state, clips);

        const float radius = 0.9f;
        state.facing += step.length() / radius;

        const auto heading = quaternion_type::from_angle_axis(state.facing,
            vector3_type(0, 1, 0));

        state.position = state.position + (heading * step);

        state.position.y = ground_height(state.position.x, state.position.z);

        auto posed = pose_at(state, clips);

        if (humanoidRig && state.gesture > 0) {
            auto weights = armMask;
            for (auto &w : weights) w *= state.gesture;

            auto damping = weights;
            for (auto &w : damping) w *= 0.75f;

            posed = demo::add(demo::blend(posed, restPose, damping),
                demo::difference(restPose, demo::sample(wave, state.gesture_phase * wave.duration)),
                weights);
        }
        auto stance = state.position + vector3_type(0, -bounds.lowest, 0);

        if (humanoidRig) {
        const auto world = demo::pose_world_transforms(skeleton, posed);

        stance.y += demo::pelvis_lift(world, groundContacts, stance, heading,
            [sole](const float x, const float z) { return ground_height(x, z) + sole; });

        const auto &arcs = footArcs[state.current];

        posed = plant_foot(skeleton, posed,
            static_cast<std::size_t>(binding.bone_for("leftUpLeg")), groundContacts[0],
            stance, heading, arcs[0], sole, world);
        posed = plant_foot(skeleton, posed,
            static_cast<std::size_t>(binding.bone_for("rightUpLeg")), groundContacts[1],
            stance, heading, arcs[1], sole, world);
        }

        const auto palette = demo::skinning_palette(skeleton, posed, inverseBind);

        demo::skin(mesh.bind_positions, mesh.weights, palette, mesh.data);

        pModel->upload(model::usage_hint::dynamic, mesh.data);

        pEntity->set_transform(stance, heading);
    });

    game_loop(frames_per_second{60}, [&](const game_loop::frame aFrame) {
        const auto time = static_cast<float>(aFrame.elapsed);
        const auto deltaTime = static_cast<float>(aFrame.delta);
        windowing::impl_glfw_window::poll_events();
        update_event.notify(time, deltaTime);
        pScene->draw(pWindow->window_size());
        pWindow->swap_buffers();

        return pWindow->should_close();
    }).run();

    return EXIT_SUCCESS;
    }
}

int main(int argc, char **argv) {
    try {
        return run(argc, argv);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << "\n";

        return EXIT_FAILURE;
    }
}
