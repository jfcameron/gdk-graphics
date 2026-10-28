// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/animation/bvh.h>
#include <gdk/graphics/ext/animation/ik.h>
#include <gdk/graphics/ext/animation/profile.h>
#include <gdk/graphics/ext/animation/retarget.h>
#include <gdk/graphics/ext/animation/skeleton.h>
#include <gdk/graphics/ext/animation/skinning.h>

#include <gdk/math/constants.h>

#include <cmath>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

using namespace gdk::graphics;
using namespace gdk::graphics::animation;

namespace {
    [[nodiscard]] quaternion_type turned(const vector3_type &aAxis, const float aDegrees) {
        return quaternion_type::from_angle_axis(gdk::math::to_radians(aDegrees), aAxis);
    }

    //! the angle of the turn from one rotation to another, in degrees, on [0, 180]
    [[nodiscard]] float degrees_between(const quaternion_type &aFrom, const quaternion_type &aTo) {
        const auto angle = gdk::math::to_degrees((aFrom.inverse_unit() * aTo).angle());

        return std::min(angle, 360.0f - angle);
    }

    [[nodiscard]] skeleton::bone bone_at(const int aParent, const vector3_type &aPosition, const std::string &aName,
        const quaternion_type &aRotation = quaternion_type::identity) {
        skeleton::bone out;
        out.parent = aParent;
        out.local_position = aPosition;
        out.local_rotation = aRotation;
        out.name = aName;

        return out;
    }

    //! hips, a spine above them, and an arm to each side of the spine: shoulder, elbow, hand
    [[nodiscard]] skeleton upper_body() {
        skeleton out;

        out.bones = {
            bone_at(-1, {0, 1, 0}, "hips"),
            bone_at(0, {0, 0.5f, 0}, "spine"),
            bone_at(1, {0.2f, 0, 0}, "leftArm"),
            bone_at(2, {0.3f, 0, 0}, "leftForeArm"),
            bone_at(3, {0.3f, 0, 0}, "leftHand"),
            bone_at(1, {-0.2f, 0, 0}, "rightArm"),
            bone_at(5, {-0.3f, 0, 0}, "rightForeArm"),
            bone_at(6, {-0.3f, 0, 0}, "rightHand")};

        return out;
    }

    [[nodiscard]] clip two_keys(const skeleton &aRig, const float aFirst, const float aSecond, const float aDuration,
        const std::size_t aBone, const float aFirstDegrees, const float aSecondDegrees) {
        clip out;
        out.duration = aDuration;
        out.loops = true;

        auto a = pose::rest(aRig);
        auto b = pose::rest(aRig);

        a.local_rotations[aBone] = turned({0, 0, 1}, aFirstDegrees);
        b.local_rotations[aBone] = turned({0, 0, 1}, aSecondDegrees);

        out.keys = {{aFirst, a, {}}, {aSecond, b, {}}};

        return out;
    }

    [[nodiscard]] float approximately(const float aValue) { return std::round(aValue * 1000.0f) / 1000.0f; }
}

TEST_CASE("a skeleton composes each bone's place from its parents'", "[animation][skeleton]") {
    const auto rig = upper_body();

    const auto world = rig.bind_world_transforms();

    REQUIRE(world[4].translation().x == Approx(0.8f));
    REQUIRE(world[4].translation().y == Approx(1.5f));

    SECTION("and its inverse binds undo it") {
        const auto inverse = rig.inverse_bind_transforms();

        for (std::size_t i = 0; i < rig.bones.size(); ++i) {
            const auto identity = world[i] * inverse[i];

            for (std::size_t c = 0; c < 4; ++c)
                for (std::size_t r = 0; r < 4; ++r)
                    REQUIRE(identity.get(c, r) == Approx(c == r ? 1.0f : 0.0f).margin(1e-5));
        }
    }

    SECTION("names are found, and repeated ones reported") {
        REQUIRE(find_bone(rig, "leftHand") == 4);
        REQUIRE(find_bone(rig, "tail") == -1);

        auto twice = rig;

        twice.bones[7].name = "leftHand";

        REQUIRE(duplicate_bone_names(twice) == std::vector<std::string>{"leftHand"});
    }

    SECTION("a mask covers a bone and everything beneath it") {
        const auto mask = mask_for_subtree(rig, 2, 0.5f);

        REQUIRE(mask == bone_mask{0, 0, 0.5f, 0.5f, 0.5f, 0, 0, 0});
    }
}

TEST_CASE("a skeleton whose bones do not come after their parents is refused, never walked", "[animation][skeleton]") {
    auto loop = upper_body();

    loop.bones[1].parent = 3;

    REQUIRE_THROWS_AS(loop.bind_world_transforms(), exception);
    REQUIRE_THROWS_AS(pose_world_transforms(loop, pose::rest(loop)), exception);
    REQUIRE_THROWS_AS(bone_world_rotation(loop, pose::rest(loop), 4), exception);

    SECTION("and a pose that does not fit it is refused") {
        const auto rig = upper_body();

        pose shortPose;
        shortPose.local_rotations.assign(3, quaternion_type::identity);

        REQUIRE_THROWS_AS(pose_world_transforms(rig, shortPose), exception);
        REQUIRE_THROWS_AS(skinning_palette(rig, pose::rest(rig), std::vector<matrix4x4_type>(2)), exception);
    }
}

TEST_CASE("bone frames turned to point at their children leave the rest pose where it was", "[animation][skeleton]") {
    auto rig = upper_body();

    rig.bones[2].local_rotation = turned({0, 0, 1}, 30);

    const auto oriented = rig.with_oriented_bone_frames();

    const auto before = rig.bind_world_transforms();
    const auto after = oriented.bind_world_transforms();

    for (std::size_t i = 0; i < rig.bones.size(); ++i) {
        REQUIRE(after[i].translation().x == Approx(before[i].translation().x).margin(1e-5));
        REQUIRE(after[i].translation().y == Approx(before[i].translation().y).margin(1e-5));
        REQUIRE(after[i].translation().z == Approx(before[i].translation().z).margin(1e-5));
    }

    SECTION("and the palette at rest moves nothing, inverse binds supplied or not") {
        auto supplied = rig;

        supplied.supplied_inverse_binds = rig.inverse_bind_transforms();

        for (const auto &each : {oriented, supplied.with_oriented_bone_frames()}) {
            const auto palette = skinning_palette(each, pose::rest(each), each.inverse_bind_transforms());

            for (const auto &m : palette) {
                const auto moved = m * vector3_type(2, 0.5f, 0);

                REQUIRE(moved.x == Approx(2.0f).margin(1e-4));
                REQUIRE(moved.y == Approx(0.5f).margin(1e-4));
                REQUIRE(moved.z == Approx(0.0f).margin(1e-4));
            }
        }
    }
}

TEST_CASE("poses blend, differ and add bone by bone", "[animation][pose]") {
    const auto rig = upper_body();

    auto raised = pose::rest(rig);
    raised.local_rotations[2] = turned({0, 0, 1}, 80);

    REQUIRE(degrees_between(quaternion_type::identity, blend(pose::rest(rig), raised, 0.5f).local_rotations[2])
        == Approx(40.0f).margin(0.01));

    const auto delta = difference(pose::rest(rig), raised);

    REQUIRE(degrees_between(add(pose::rest(rig), delta, 1.0f).local_rotations[2], raised.local_rotations[2])
        == Approx(0.0f).margin(0.01));

    REQUIRE_THROWS_AS(blend(raised, pose{}, 0.5f), exception);
}

TEST_CASE("a clip is sampled between its keys, and a looping one across its end", "[animation][clip]") {
    const auto rig = upper_body();

    SECTION("between two keys") {
        const auto subject = two_keys(rig, 0, 1, 2, 2, 0, 90);

        REQUIRE(degrees_between(quaternion_type::identity, sample(subject, 0.5f).local_rotations[2])
            == Approx(45.0f).margin(0.01));
    }

    SECTION("before its first key, a looping clip is still coming back from its last") {
        const auto subject = two_keys(rig, 1, 2, 2, 2, 0, 90);

        const auto angle = degrees_between(quaternion_type::identity, sample(subject, 0.5f).local_rotations[2]);

        REQUIRE(angle == Approx(45.0f).margin(0.01));
        REQUIRE(sample(subject, 0.5f).local_rotations[2].z >= 0.0f);
    }

    SECTION("a clip that does not loop holds its ends") {
        auto subject = two_keys(rig, 1, 2, 2, 2, 0, 90);

        subject.loops = false;

        REQUIRE(degrees_between(quaternion_type::identity, sample(subject, 0.0f).local_rotations[2])
            == Approx(0.0f).margin(0.01));
        REQUIRE(degrees_between(quaternion_type::identity, sample(subject, 5.0f).local_rotations[2])
            == Approx(90.0f).margin(0.01));
    }

    SECTION("a clip with no keys, or a looping one with no length, is refused") {
        REQUIRE_THROWS_AS(sample(clip{}, 0.0f), exception);

        auto still = two_keys(rig, 0, 1, 0, 2, 0, 90);

        REQUIRE_THROWS_AS(sample(still, 0.5f), exception);
    }
}

TEST_CASE("a looping clip's root moves on smoothly across the end of each cycle", "[animation][clip]") {
    const auto rig = upper_body();

    auto subject = two_keys(rig, 0.5f, 1.5f, 2, 2, 0, 0);

    subject.keys[0].root_position = {0, 0, 0};
    subject.keys[1].root_position = {1, 0, 0};
    subject.root_motion_per_cycle = {2, 0, 0};

    REQUIRE(sample_root_position(subject, 1.0f).x == Approx(0.5f));

    const auto justBefore = root_delta(subject, 0.0f, 1.999f);
    const auto across = root_delta(subject, 1.999f, 2.001f);

    REQUIRE(across.x == Approx(0.002f).margin(1e-4));
    REQUIRE(justBefore.x + across.x == Approx(root_delta(subject, 0.0f, 2.001f).x).margin(1e-4));
    REQUIRE(root_delta(subject, 0.0f, 4.0f).x == Approx(4.0f).margin(1e-4));
}

TEST_CASE("skinning moves each vertex by its bones' palette", "[animation][skinning]") {
    const std::vector<vector3_type> bind{{1, 0, 0}, {0, 1, 0}};

    matrix4x4_type moved;
    moved.set_to_identity();
    moved.set_translation({0, 0, 2});

    const std::vector<matrix4x4_type> palette{matrix4x4_type::identity, moved};

    model_data target({{"a_Position", attribute_data({0, 0, 0, 0, 0, 0}, 3)}});

    skin(bind, std::vector<vertex_weights>{{1, 1, 1.0f}, {1, 0, 0.5f}}, palette, target);

    const auto &out = target.get_attribute_data("a_Position").components();

    REQUIRE(out[2] == Approx(2.0f));
    REQUIRE(out[5] == Approx(1.0f));

    SECTION("and refuses what it cannot read") {
        REQUIRE_THROWS_AS(skin(bind, std::vector<vertex_weights>{{0, 0, 1.0f}}, palette, target), exception);
        REQUIRE_THROWS_AS(skin(bind, std::vector<vertex_weights>{{0, 0, 1.0f}, {5, 0, 1.0f}}, palette, target),
            exception);

        REQUIRE_THROWS_AS(skin(bind, std::vector<vertex_weights>{{0, 0, 1.0f}, {0, 0, 1.0f}}, {}, target), exception);

        model_data flat({{"a_Position", attribute_data({0, 0, 0, 0}, 2)}});

        REQUIRE_THROWS_AS(skin(bind, std::vector<vertex_weights>{{0, 0, 1.0f}, {0, 0, 1.0f}}, palette, flat),
            exception);
    }
}

TEST_CASE("a bone box faces outward", "[animation][skinning]") {
    const auto mesh = make_bone_boxes(upper_body(), 0.05f);

    const auto &positions = mesh.bind_positions;

    REQUIRE(positions.size() % 36 == 0);

    for (std::size_t box = 0; box < positions.size(); box += 36) {
        vector3_type centre{0, 0, 0};

        for (std::size_t v = 0; v < 36; ++v) centre = centre + positions[box + v] / 36.0f;

        for (std::size_t t = box; t < box + 36; t += 3) {
            const auto normal = (positions[t + 1] - positions[t]).cross_product(positions[t + 2] - positions[t]);
            const auto outward = (positions[t] + positions[t + 1] + positions[t + 2]) / 3.0f - centre;

            REQUIRE(normal.dot_product(outward) > 0);
        }
    }
}

TEST_CASE("a limb reaches for what it can reach, and toward what it cannot", "[animation][ik]") {
    const auto rig = upper_body();

    const vector3_type target{0.4f, 1.9f, 0.1f};

    const auto reaching = with_limb_reaching(rig, pose::rest(rig), 2, target, {0, 0, 1});

    const auto hand = pose_world_transforms(rig, reaching)[4].translation();

    REQUIRE(hand.distance_from(target) < 0.01f);

    SECTION("out of reach, the limb straightens toward it") {
        const vector3_type far{3, 1.5f, 0};

        const auto straining = with_limb_reaching(rig, pose::rest(rig), 2, far, {0, 0, 1});

        const auto world = pose_world_transforms(rig, straining);

        const auto toHand = (world[4].translation() - world[2].translation()).normal();
        const auto toTarget = (far - world[2].translation()).normal();

        REQUIRE(toHand.dot_product(toTarget) == Approx(1.0f).margin(1e-3));
    }

    SECTION("a bone is turned to face what it was told") {
        const auto wanted = turned({1, 0, 0}, 30);

        const auto oriented = with_bone_oriented(rig, pose::rest(rig), 3, wanted);

        REQUIRE(degrees_between(bone_world_rotation(rig, oriented, 3), wanted) == Approx(0.0f).margin(0.01));
    }

    SECTION("a bone with no tip to bend toward is refused") {
        REQUIRE_THROWS_AS(with_limb_reaching(rig, pose::rest(rig), 3, target, {0, 0, 1}), exception);
    }
}

TEST_CASE("a foot is planted by how near it is to the lowest it goes", "[animation][ik]") {
    const foot_arc arc{0.0f, 1.0f};

    REQUIRE(stance_weight(arc, 0.0f) == Approx(1.0f));
    REQUIRE(stance_weight(arc, 0.5f) == Approx(0.0f));
    REQUIRE(stance_weight(foot_arc{}, 3.0f) == Approx(1.0f));

    const std::vector<matrix4x4_type> world{matrix4x4_type::identity};
    const std::vector<std::size_t> feet{0};

    REQUIRE(pelvis_lift(world, feet, {0, 0, 0}, quaternion_type::identity,
        [](float, float) { return 0.25f; }) == Approx(0.25f));
}

TEST_CASE("a pose is carried onto another rig by role", "[animation][retarget]") {
    const auto source = upper_body();

    auto target = upper_body();

    for (auto &bone : target.bones) bone.local_position = bone.local_position * 2.0f;

    auto raised = pose::rest(source);
    raised.local_rotations[2] = turned({0, 0, 1}, 60);

    const auto mapping = map_by_profile(bind_profile(humanoid_profile(), source),
        bind_profile(humanoid_profile(), target));

    REQUIRE(mapped_bone_count(mapping) == 8);

    const auto carried = retarget(source, raised, target, mapping);

    REQUIRE(degrees_between(bone_world_rotation(target, carried, 2), bone_world_rotation(source, raised, 2))
        == Approx(0.0f).margin(0.01));

    SECTION("a mapping of the wrong length is refused") {
        REQUIRE_THROWS_AS(retarget(source, raised, target, bone_mapping(3, -1)), exception);
    }
}

TEST_CASE("a mirrored pose is performed by the other side", "[animation][retarget]") {
    auto rig = upper_body();

    rig.bones[2].local_rotation = turned({0, 1, 0}, 20);

    const auto binding = bind_profile(humanoid_profile(), rig);

    SECTION("the rest pose mirrors to itself, whatever the bones' bind frames") {
        const auto mirrored = mirror_pose(rig, binding, pose::rest(rig));

        for (const auto &each : mirrored.local_rotations)
            REQUIRE(degrees_between(quaternion_type::identity, each) == Approx(0.0f).margin(0.01));
    }

    SECTION("on a rig that is its own mirror image, the left arm's raise becomes the right arm's") {
        const auto symmetric = upper_body();

        auto raised = pose::rest(symmetric);
        raised.local_rotations[2] = turned({0, 0, 1}, 50);

        const auto mirrored = mirror_pose(symmetric, bind_profile(humanoid_profile(), symmetric), raised);

        const auto world = pose_world_transforms(symmetric, mirrored);
        const auto original = pose_world_transforms(symmetric, raised);

        REQUIRE(world[7].translation().x == Approx(-original[4].translation().x).margin(1e-4));
        REQUIRE(world[7].translation().y == Approx(original[4].translation().y).margin(1e-4));
    }

    SECTION("a binding made for another skeleton is refused") {
        auto bigger = rig;

        bigger.bones.push_back(bone_at(7, {0, 0, 0}, "extra"));

        REQUIRE_THROWS_AS(mirror_pose(rig, bind_profile(humanoid_profile(), bigger), pose::rest(rig)), exception);
    }
}

TEST_CASE("a rig binding is one bone per role, each on the skeleton", "[animation][profile]") {
    const auto &profile = humanoid_profile();

    REQUIRE_THROWS_AS(rig_binding(profile, std::vector<int>(3, -1), 8), exception);
    REQUIRE_THROWS_AS(rig_binding(profile, std::vector<int>(profile.roles.size(), 9), 8), exception);
    REQUIRE_NOTHROW(rig_binding(profile, std::vector<int>(profile.roles.size(), -1), 8));

    SECTION("bones are found by role, by alias, ignoring case and punctuation") {
        skeleton rig;
        rig.bones = {bone_at(-1, {}, "Pelvis"), bone_at(0, {}, "L_Thigh")};

        const auto binding = bind_profile(profile, rig);

        REQUIRE(binding.bone_for("hips") == 0);
        REQUIRE(binding.bone_for("leftUpLeg") == 1);
        REQUIRE(binding.placed() == 2);
    }
}

namespace {
    const std::string WALK = R"(HIERARCHY
ROOT Hips
{
  OFFSET 0 1 0
  CHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation
  JOINT Leg
  {
    OFFSET 0 -0.5 0
    CHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation
    End Site
    {
      OFFSET 0 -0.5 0
    }
  }
}
MOTION
Frames: 2
Frame Time: 0.5
10 20 30 0 0 0  7 8 9 90 0 0
11 21 31 0 0 0  7 8 9 0 0 0
)";
}

TEST_CASE("a BVH file is read as a skeleton and a clip", "[animation][bvh]") {
    const auto content = read_bvh(WALK);

    REQUIRE(content.rig.bones.size() == 3);
    REQUIRE(content.rig.bones[2].name == "Leg_End");
    REQUIRE(content.frame_time == Approx(0.5f));
    REQUIRE(content.animation.keys.size() == 2);

    REQUIRE(degrees_between(quaternion_type::identity, content.animation.keys[0].p.local_rotations[1])
        == Approx(90.0f).margin(0.01));

    SECTION("only the root's position channels move the root") {
        const auto &root = content.animation.keys[0].root_position;

        REQUIRE(root.x == Approx(10.0f));
        REQUIRE(root.y == Approx(20.0f));
        REQUIRE(root.z == Approx(30.0f));
    }

    SECTION("numbers read the same whatever the locale") {
        struct comma final : std::numpunct<char> {
            char do_decimal_point() const override { return ','; }
        };

        const auto previous = std::locale::global(std::locale(std::locale::classic(), new comma));

        const auto again = read_bvh(WALK);

        std::locale::global(previous);

        REQUIRE(again.frame_time == Approx(0.5f));
    }
}

TEST_CASE("a BVH file is refused rather than followed too far", "[animation][bvh]") {
    const auto with_motion = [](const std::string &aFrames, const std::string &aTime, const std::string &aChannels) {
        return "HIERARCHY ROOT a { OFFSET 0 0 0 CHANNELS " + aChannels + " } MOTION Frames: " + aFrames
            + " Frame Time: " + aTime + " 0 0 0";
    };

    REQUIRE_NOTHROW(read_bvh(with_motion("1", "0.1", "3 Xrotation Yrotation Zrotation")));

    REQUIRE_THROWS_AS(read_bvh(with_motion("50000000", "0.1", "3 Xrotation Yrotation Zrotation")), exception);
    REQUIRE_THROWS_AS(read_bvh(with_motion("nan", "0.1", "3 Xrotation Yrotation Zrotation")), exception);
    REQUIRE_THROWS_AS(read_bvh(with_motion("1", "0", "3 Xrotation Yrotation Zrotation")), exception);
    REQUIRE_THROWS_AS(read_bvh(with_motion("1", "0.1", "-1")), exception);
    REQUIRE_THROWS_AS(read_bvh(with_motion("1", "0.1", "0")), exception);
    REQUIRE_THROWS_AS(read_bvh(with_motion("1", "inf", "3 Xrotation Yrotation Zrotation")), exception);

    SECTION("joints nested too deep") {
        std::string deep = "HIERARCHY ROOT a { OFFSET 0 0 0 CHANNELS 1 Xrotation ";

        for (int i = 0; i < 100; ++i) deep += "JOINT b { OFFSET 0 0 0 ";
        for (int i = 0; i < 100; ++i) deep += "} ";

        deep += "} MOTION Frames: 1 Frame Time: 0.1 0";

        REQUIRE_THROWS_AS(read_bvh(deep), exception);
    }
}

TEST_CASE("a skeleton and clip written as BVH read back the same", "[animation][bvh]") {
    skeleton rig;

    rig.bones = {
        bone_at(-1, {0, 1, 0}, "root"),
        bone_at(0, {0.5f, 0, 0}, "a"),
        bone_at(0, {-0.5f, 0, 0}, "b"),
        bone_at(1, {0.5f, 0, 0}, "a_child"),
        bone_at(-1, {3, 0, 0}, "other")};

    clip motion;
    motion.duration = 1;
    motion.loops = false;

    auto first = pose::rest(rig);
    first.local_rotations[3] = turned({0, 0, 1}, 40);
    first.local_rotations[2] = turned({1, 0, 0}, -25);

    motion.keys = {{0, first, {0, 0, 0}}, {1, first, {0, 0, 0}}};

    std::ostringstream written;

    write_bvh(written, rig, motion, 2, 0.5f);

    const auto read = read_bvh(written.str());

    for (const auto &name : {"root", "a", "b", "a_child"}) {
        INFO("bone " << name);

        const auto here = find_bone(rig, name);
        const auto there = find_bone(read.rig, name);

        REQUIRE(there >= 0);

        REQUIRE(degrees_between(first.local_rotations[static_cast<std::size_t>(here)],
            read.animation.keys[0].p.local_rotations[static_cast<std::size_t>(there)]) == Approx(0.0f).margin(0.01));
    }

    REQUIRE(approximately(read.rig.bones[0].local_position.y) == 1.0f);

    SECTION("a skeleton with no bones, or a clip of another, is refused") {
        std::ostringstream ignored;

        REQUIRE_THROWS_AS(write_bvh(ignored, skeleton{}, motion, 1, 0.5f), exception);

        auto shorter = rig;

        shorter.bones.pop_back();

        REQUIRE_THROWS_AS(write_bvh(ignored, shorter, motion, 1, 0.5f), exception);
    }
}
