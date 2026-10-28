// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/animation/bvh.h>

#include <gdk/math/constants.h>

#include <cmath>
#include <functional>
#include <locale>
#include <ostream>
#include <sstream>

using namespace gdk;

namespace gdk::graphics::animation {
    namespace {
        constexpr std::size_t MOST_JOINT_DEPTH = 64;
        constexpr std::size_t MOST_CHANNELS_PER_JOINT = 6;

        [[noreturn]] void refuse(const std::string &aWhy) { throw exception("bvh: " + aWhy); }

        enum class channel { 
            x_position, 
            y_position, 
            z_position, 
            x_rotation, 
            y_rotation, 
            z_rotation 
        };

        struct joint_channels final {
            std::vector<std::pair<channel, std::size_t>> entries;
        };

        struct cursor final {
            std::string_view text;
            std::size_t at = 0;

            [[nodiscard]] std::string_view next() {
                const auto space = [](const char c) {
                    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f'
                        || c == '\v';
                };

                while (at < text.size() && space(text[at])) ++at;

                if (at >= text.size()) refuse("unexpected end of document");

                const auto start = at;

                while (at < text.size() && !space(text[at])) ++at;

                return text.substr(start, at - start);
            }
        };

        [[nodiscard]] std::string_view next_token(cursor &aCursor) { return aCursor.next(); }

        void expect(cursor &aCursor, const std::string_view aExpected) {
            const auto token = next_token(aCursor);

            if (token != aExpected)
                refuse("expected \"" + std::string(aExpected) + "\", found \"" + std::string(token) + "\"");
        }

        [[nodiscard]] float next_number(cursor &aCursor) {
            const auto token = next_token(aCursor);

            std::istringstream stream{std::string(token)};
            stream.imbue(std::locale::classic());

            float value = 0;

            stream >> value;

            if (!stream || stream.peek() != std::char_traits<char>::eof() || !std::isfinite(value))
                refuse("expected a number, found \"" + std::string(token) + "\"");

            return value;
        }

        [[nodiscard]] std::size_t next_count(cursor &aCursor) {
            const auto token = next_token(aCursor);

            if (token.empty() || token.size() > 9 || token.find_first_not_of("0123456789") != std::string_view::npos)
                refuse("expected a whole number, found \"" + std::string(token) + "\"");

            return static_cast<std::size_t>(std::stoul(std::string(token)));
        }

        [[nodiscard]] channel channel_from(const std::string_view aName) {
            if (aName == "Xposition") return channel::x_position;
            if (aName == "Yposition") return channel::y_position;
            if (aName == "Zposition") return channel::z_position;
            if (aName == "Xrotation") return channel::x_rotation;
            if (aName == "Yrotation") return channel::y_rotation;
            if (aName == "Zrotation") return channel::z_rotation;

            refuse("unknown channel \"" + std::string(aName) + "\"");
        }

        void parse_joint(cursor &aCursor, const int aParent, const float aScale,
            bvh_content &aOut, std::vector<joint_channels> &aChannels, std::size_t &aChannelCount,
            const bool aIsEndSite, const std::size_t aDepth) {
            if (aDepth > MOST_JOINT_DEPTH)
                refuse("joints nest more than " + std::to_string(MOST_JOINT_DEPTH) + " deep");

            const auto name = aIsEndSite
                ? (aOut.rig.bones[static_cast<std::size_t>(aParent)].name + "_End")
                : std::string(next_token(aCursor));

            expect(aCursor, "{");
            expect(aCursor, "OFFSET");

            skeleton::bone bone;
            bone.parent = aParent;
            bone.local_position = {next_number(aCursor) * aScale, next_number(aCursor) * aScale,
                next_number(aCursor) * aScale};

            bone.name = name;

            const auto self = aOut.rig.bones.size();
            aOut.rig.bones.push_back(bone);
            aChannels.emplace_back();

            for (;;) {
                const auto token = next_token(aCursor);

                if (token == "}") return;

                if (token == "CHANNELS") {
                    const auto count = next_count(aCursor);

                    if (count > MOST_CHANNELS_PER_JOINT)
                        refuse("a joint has " + std::to_string(count) + " channels, where there are 6 kinds");

                    for (std::size_t i = 0; i < count; ++i)
                        aChannels[self].entries.emplace_back(channel_from(next_token(aCursor)),
                            aChannelCount++);
                }
                else if (token == "JOINT") {
                    parse_joint(aCursor, static_cast<int>(self), aScale, aOut, aChannels,
                        aChannelCount, false, aDepth + 1);
                }
                else if (token == "End") {
                    expect(aCursor, "Site");
                    parse_joint(aCursor, static_cast<int>(self), aScale, aOut, aChannels,
                        aChannelCount, true, aDepth + 1);
                }
                else refuse("unexpected token \"" + std::string(token) + "\" in a joint");
            }
        }
    }

    bvh_content read_bvh(const std::string_view aText, const float aScale) {
        cursor aCursor{aText, 0};

        bvh_content result;

        std::vector<joint_channels> channels;
        std::size_t channelCount = 0;

        expect(aCursor, "HIERARCHY");
        expect(aCursor, "ROOT");

        parse_joint(aCursor, -1, aScale, result, channels, channelCount, false, 0);

        for (auto token = next_token(aCursor); token != "MOTION"; token = next_token(aCursor)) {
            if (token != "ROOT") refuse("expected \"ROOT\" or \"MOTION\", found \"" + std::string(token) + "\"");

            parse_joint(aCursor, -1, aScale, result, channels, channelCount, false, 0);
        }

        expect(aCursor, "Frames:");

        const auto frames = next_count(aCursor);

        expect(aCursor, "Frame");
        expect(aCursor, "Time:");

        result.frame_time = next_number(aCursor);

        if (result.frame_time <= 0) refuse("a frame time must be more than nothing");

        if (!channelCount) refuse("the motion has no channels");

        if (frames > (aText.size() - aCursor.at) / (channelCount * 2) + 1)
            refuse(std::to_string(frames) + " frames of " + std::to_string(channelCount)
                + " channels is more than the document holds");
        result.animation.duration = result.frame_time * static_cast<float>(frames);

        for (std::size_t f = 0; f < frames; ++f) {
            std::vector<float> values(channelCount);
            for (auto &v : values) v = next_number(aCursor);

            auto p = pose::rest(result.rig);
            auto rootPosition = vector3_type(0, 0, 0);

            for (std::size_t j = 0; j < result.rig.bones.size(); ++j) {
                auto rotation = quaternion_type::identity;

                for (const auto &[which, index] : channels[j].entries) {
                    const auto radians = math::to_radians(values[index]);

                    switch (which) {
                        case channel::x_rotation:
                            rotation = rotation * quaternion_type::from_angle_axis(radians,
                                vector3_type(1, 0, 0));
                            break;
                        case channel::y_rotation:
                            rotation = rotation * quaternion_type::from_angle_axis(radians,
                                vector3_type(0, 1, 0));
                            break;
                        case channel::z_rotation:
                            rotation = rotation * quaternion_type::from_angle_axis(radians,
                                vector3_type(0, 0, 1));
                            break;

                        case channel::x_position: if (!j) rootPosition.x = values[index] * aScale; break;
                        case channel::y_position: if (!j) rootPosition.y = values[index] * aScale; break;
                        case channel::z_position: if (!j) rootPosition.z = values[index] * aScale; break;
                    }
                }

                p.local_rotations[j] = rotation;
            }

            result.animation.keys.push_back({result.frame_time * static_cast<float>(f),
                std::move(p), rootPosition});
        }

        if (result.animation.keys.empty()) refuse("the file declares no frames");

        result.animation.root_motion_per_cycle = result.animation.keys.back().root_position
            - result.animation.keys.front().root_position;

        result.animation.loops = false;

        return result;
    }

    void write_bvh(std::ostream &aStream, const skeleton &aSkeleton, const clip &aClip,
        const std::size_t aFrameCount, const float aFrameTime) {
        if (aSkeleton.bones.empty()) refuse("a skeleton with no bones cannot be written");

        for (const auto &key : aClip.keys)
            if (key.p.local_rotations.size() != aSkeleton.bones.size())
                refuse("the clip poses " + std::to_string(key.p.local_rotations.size()) + " bones, and the skeleton has "
                    + std::to_string(aSkeleton.bones.size()));

        std::vector<std::vector<std::size_t>> children(aSkeleton.bones.size());
        std::vector<std::size_t> roots;

        for (std::size_t i = 0; i < aSkeleton.bones.size(); ++i) {
            const auto parent = aSkeleton.bones[i].parent;

            if (parent < 0) roots.push_back(i);
            else if (static_cast<std::size_t>(parent) >= i) refuse("a bone's parent comes after it");
            else children[static_cast<std::size_t>(parent)].push_back(i);
        }

        std::vector<std::size_t> written;

        const auto locale = aStream.imbue(std::locale::classic());

        const std::function<void(std::size_t, std::size_t)> write_joint =
            [&](const std::size_t aBone, const std::size_t aDepth) {
                const std::string pad(aDepth * 2, ' ');
                const auto &o = aSkeleton.bones[aBone].local_position;

                written.push_back(aBone);

                aStream << pad << (aDepth == 0 ? "ROOT" : "JOINT") << " "
                    << (aSkeleton.bones[aBone].name.empty()
                        ? "bone" + std::to_string(aBone) : aSkeleton.bones[aBone].name) << "\n";
                aStream << pad << "{\n";
                aStream << pad << "  OFFSET " << o.x << " " << o.y << " " << o.z << "\n";
                aStream << pad << (aDepth == 0
                    ? "  CHANNELS 6 Xposition Yposition Zposition Yrotation Xrotation Zrotation\n"
                    : "  CHANNELS 3 Yrotation Xrotation Zrotation\n");

                for (const auto child : children[aBone]) write_joint(child, aDepth + 1);

                if (children[aBone].empty()) {
                    aStream << pad << "  End Site\n" << pad << "  {\n";
                    aStream << pad << "    OFFSET 0 0.1 0\n" << pad << "  }\n";
                }

                aStream << pad << "}\n";
            };

        aStream << "HIERARCHY\n";

        for (const auto root : roots) write_joint(root, 0);

        aStream << "MOTION\n";
        aStream << "Frames: " << aFrameCount << "\n";
        aStream << "Frame Time: " << aFrameTime << "\n";

        for (std::size_t f = 0; f < aFrameCount; ++f) {
            const auto time = aClip.duration * static_cast<float>(f) / static_cast<float>(aFrameCount);

            const auto p = sample(aClip, time);
            const auto root = sample_root_position(aClip, time);

            for (std::size_t i = 0; i < written.size(); ++i) {
                const auto bone = written[i];

                if (aSkeleton.bones[bone].parent < 0) aStream << root.x << " " << root.y << " " << root.z << " ";

                const auto e = p.local_rotations[bone].to_euler();

                aStream << math::to_degrees(e.y) << " " << math::to_degrees(e.x) << " " << math::to_degrees(e.z);
                aStream << (i + 1 == written.size() ? "\n" : " ");
            }
        }

        aStream.imbue(locale);
    }
}
