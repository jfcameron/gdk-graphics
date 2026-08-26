// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/animation/bvh.h>

#include <gdk/math.h>

#include <functional>
#include <cstring>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <stdexcept>

using namespace gdk;

namespace gdk::graphics::animation {
    namespace {
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

        /// \brief a position in the document, handing out whitespace-delimited tokens
        struct cursor final {
            std::string_view text;
            std::size_t at = 0;

            [[nodiscard]] std::string_view next() {
                const auto space = [](const char c) {
                    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f'
                        || c == '\v';
                };

                while (at < text.size() && space(text[at])) ++at;

                if (at >= text.size()) throw std::runtime_error("bvh: unexpected end of document");

                const auto start = at;

                while (at < text.size() && !space(text[at])) ++at;

                return text.substr(start, at - start);
            }
        };

        [[nodiscard]] std::string_view next_token(cursor &aCursor) { return aCursor.next(); }

        void expect(cursor &aCursor, const std::string_view aExpected) {
            const auto token = next_token(aCursor);

            if (token != aExpected)
                throw std::runtime_error("bvh: expected \"" + std::string(aExpected)
                    + "\", found \"" + std::string(token) + "\"");
        }

        [[nodiscard]] float next_number(cursor &aCursor) {
            const auto token = next_token(aCursor);

            char buffer[64];

            if (token.size() >= sizeof(buffer))
                throw std::runtime_error("bvh: expected a number, found \"" + std::string(token)
                    + "\"");

            std::memcpy(buffer, token.data(), token.size());
            buffer[token.size()] = '\0';

            char *end = nullptr;
            const auto value = std::strtof(buffer, &end);

            if (end != buffer + token.size())
                throw std::runtime_error("bvh: expected a number, found \"" + std::string(token)
                    + "\"");

            return value;
        }

        [[nodiscard]] channel channel_from(const std::string_view aName) {
            if (aName == "Xposition") return channel::x_position;
            if (aName == "Yposition") return channel::y_position;
            if (aName == "Zposition") return channel::z_position;
            if (aName == "Xrotation") return channel::x_rotation;
            if (aName == "Yrotation") return channel::y_rotation;
            if (aName == "Zrotation") return channel::z_rotation;

            throw std::runtime_error("bvh: unknown channel \"" + std::string(aName) + "\"");
        }

        void parse_joint(cursor &aCursor, const int aParent, const float aScale,
            bvh_content &aOut, std::vector<joint_channels> &aChannels, std::size_t &aChannelCount,
            const bool aIsEndSite) {
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
                    const auto count = static_cast<std::size_t>(next_number(aCursor));

                    for (std::size_t i = 0; i < count; ++i)
                        aChannels[self].entries.emplace_back(channel_from(next_token(aCursor)),
                            aChannelCount++);
                }
                else if (token == "JOINT") {
                    parse_joint(aCursor, static_cast<int>(self), aScale, aOut, aChannels,
                        aChannelCount, false);
                }
                else if (token == "End") {
                    expect(aCursor, "Site");
                    parse_joint(aCursor, static_cast<int>(self), aScale, aOut, aChannels,
                        aChannelCount, true);
                }
                else throw std::runtime_error("bvh: unexpected token \"" + std::string(token)
                    + "\" in a joint");
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

        parse_joint(aCursor, -1, aScale, result, channels, channelCount, false);

        expect(aCursor, "MOTION");
        expect(aCursor, "Frames:");

        const auto frames = static_cast<std::size_t>(next_number(aCursor));

        expect(aCursor, "Frame");
        expect(aCursor, "Time:");

        result.frame_time = next_number(aCursor);
        result.animation.duration = result.frame_time * static_cast<float>(frames);

        for (std::size_t f = 0; f < frames; ++f) {
            std::vector<float> values(channelCount);
            for (auto &v : values) v = next_number(aCursor);

            auto p = pose::rest(result.rig);
            auto rootPosition = vector3_type(0, 0, 0);

            for (std::size_t j = 0; j < result.rig.bones.size(); ++j) {
                auto rotation = quaternion_type::identity;

                for (const auto &[which, index] : channels[j].entries) {
                    const auto radians = to_radians(values[index]);

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

                        case channel::x_position: rootPosition.x = values[index] * aScale; break;
                        case channel::y_position: rootPosition.y = values[index] * aScale; break;
                        case channel::z_position: rootPosition.z = values[index] * aScale; break;
                    }
                }

                p.local_rotations[j] = rotation;
            }

            result.animation.keys.push_back({result.frame_time * static_cast<float>(f),
                std::move(p), rootPosition});
        }

        if (result.animation.keys.empty())
            throw std::runtime_error("bvh: the file declares no frames");

        result.animation.root_motion_per_cycle = result.animation.keys.back().root_position
            - result.animation.keys.front().root_position;

        result.animation.loops = false;

        return result;
    }

    void write_bvh(std::ostream &aStream, const skeleton &aSkeleton, const clip &aClip,
        const std::size_t aFrameCount, const float aFrameTime) {
        std::vector<std::vector<std::size_t>> children(aSkeleton.bones.size());
        for (std::size_t i = 0; i < aSkeleton.bones.size(); ++i)
            if (aSkeleton.bones[i].parent >= 0)
                children[static_cast<std::size_t>(aSkeleton.bones[i].parent)].push_back(i);

        const std::function<void(std::size_t, int)> write_joint =
            [&](const std::size_t aBone, const int aDepth) {
                const std::string pad(static_cast<std::size_t>(aDepth) * 2, ' ');
                const auto &o = aSkeleton.bones[aBone].local_position;

                aStream << pad << (aDepth == 0 ? "ROOT" : "JOINT") << " "
                    << (aSkeleton.bones[aBone].name.empty()
                        ? "bone" + std::to_string(aBone) : aSkeleton.bones[aBone].name) << "\n";
                aStream << pad << "{\n";
                aStream << pad << "  OFFSET " << o.x << " " << o.y << " " << o.z << "\n";
                aStream << pad << "  CHANNELS 3 Yrotation Xrotation Zrotation\n";

                for (const auto child : children[aBone]) write_joint(child, aDepth + 1);

                if (children[aBone].empty()) {
                    aStream << pad << "  End Site\n" << pad << "  {\n";
                    aStream << pad << "    OFFSET 0 0.1 0\n" << pad << "  }\n";
                }

                aStream << pad << "}\n";
            };

        aStream << "HIERARCHY\n";
        write_joint(0, 0);

        aStream << "MOTION\n";
        aStream << "Frames: " << aFrameCount << "\n";
        aStream << "Frame Time: " << aFrameTime << "\n";

        for (std::size_t f = 0; f < aFrameCount; ++f) {
            const auto p = sample(aClip, aClip.duration * static_cast<float>(f)
                / static_cast<float>(aFrameCount));

            for (std::size_t j = 0; j < aSkeleton.bones.size(); ++j) {
                const auto e = p.local_rotations[j].to_euler();

                aStream << to_degrees(e.y) << " " << to_degrees(e.x) << " " << to_degrees(e.z);
                aStream << (j + 1 == aSkeleton.bones.size() ? "\n" : " ");
            }
        }
    }
}
