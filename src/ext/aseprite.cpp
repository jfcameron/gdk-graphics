// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/aseprite.h>

#include <gdk/graphics/exception.h>

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include <string_view>
#include <variant>

namespace gdk::graphics::ext {
    namespace {
        constexpr std::string_view TAG = "aseprite: ";

        [[nodiscard]] exception wrong(const std::string_view aWhat) {
            return exception(std::string(TAG).append(aWhat));
        }

        struct value;

        using object = std::vector<std::pair<std::string, value>>;
        using array = std::vector<value>;

        struct value final {
            std::variant<std::nullptr_t, bool, double, std::string, std::shared_ptr<array>,
                std::shared_ptr<object>> held;
        };

        constexpr int MOST_DEPTH = 32;

        class reading final {
        public:
            explicit reading(const std::string_view aText)
            : m_Text(aText) {}

            [[nodiscard]] value whole() {
                auto out = read(0);

                skip();

                if (m_At != m_Text.size()) throw wrong("something after the end of the json");

                return out;
            }

        private:
            void skip() {
                while (m_At < m_Text.size() && std::isspace(static_cast<unsigned char>(m_Text[m_At]))) ++m_At;
            }

            [[nodiscard]] char peek() {
                skip();

                return m_At < m_Text.size() ? m_Text[m_At] : '\0';
            }

            void expect(const char aCharacter) {
                if (peek() != aCharacter)
                    throw wrong(std::string("expected '") + aCharacter + "' in the json");

                ++m_At;
            }

            [[nodiscard]] bool word(const std::string_view aWord) {
                if (m_Text.compare(m_At, aWord.size(), aWord) != 0) return false;

                m_At += aWord.size();

                return true;
            }

            [[nodiscard]] std::string text() {
                expect('"');

                std::string out;

                while (m_At < m_Text.size() && m_Text[m_At] != '"') {
                    if (m_Text[m_At] != '\\') { out += m_Text[m_At++]; continue; }

                    if (++m_At >= m_Text.size()) throw wrong("a string in the json ends in an escape");

                    const char escaped = m_Text[m_At++];

                    switch (escaped) {
                        case 'n': out += '\n'; break;
                        case 't': out += '\t'; break;
                        case 'r': out += '\r'; break;
                        case 'b': out += '\b'; break;
                        case 'f': out += '\f'; break;
                        case '"': out += '"'; break;
                        case '\\': out += '\\'; break;
                        case '/': out += '/'; break;

                        case 'u': {
                            if (m_At + 4 > m_Text.size()) throw wrong("a short escape in the json");

                            unsigned code = 0;

                            const auto got = std::from_chars(m_Text.data() + m_At, m_Text.data() + m_At + 4,
                                code, 16);

                            if (got.ec != std::errc() || got.ptr != m_Text.data() + m_At + 4)
                                throw wrong("an escape in the json that is not four digits");

                            m_At += 4;

                            // as utf-8, which is what everything downstream of this holds
                            if (code < 0x80) out += static_cast<char>(code);
                            else if (code < 0x800) {
                                out += static_cast<char>(0xc0 | (code >> 6));
                                out += static_cast<char>(0x80 | (code & 0x3f));
                            }
                            else {
                                out += static_cast<char>(0xe0 | (code >> 12));
                                out += static_cast<char>(0x80 | ((code >> 6) & 0x3f));
                                out += static_cast<char>(0x80 | (code & 0x3f));
                            }

                            break;
                        }

                        default: throw wrong(std::string("an escape in the json this does not know: \\")
                            + escaped);
                    }
                }

                expect('"');

                return out;
            }

            [[nodiscard]] double number() {
                skip();

                const auto piece = std::string(m_Text.substr(m_At,
                    std::min<std::size_t>(64, m_Text.size() - m_At)));

                char *end = nullptr;

                const double out = std::strtod(piece.c_str(), &end);

                const auto read = static_cast<std::size_t>(end - piece.c_str());

                if (!read) throw wrong("expected a number in the json");

                m_At += read;

                if (!std::isfinite(out)) throw wrong("a number in the json that is not finite");

                return out;
            }

            [[nodiscard]] value read(const int aDepth) {
                if (aDepth > MOST_DEPTH) throw wrong("json nested deeper than is allowed");

                switch (peek()) {
                    case '{': {
                        ++m_At;

                        auto out = std::make_shared<object>();

                        if (peek() == '}') { ++m_At; return {out}; }

                        for (;;) {
                            auto key = text();

                            expect(':');

                            out->emplace_back(std::move(key), read(aDepth + 1));

                            if (peek() == ',') { ++m_At; continue; }

                            break;
                        }

                        expect('}');

                        return {out};
                    }

                    case '[': {
                        ++m_At;

                        auto out = std::make_shared<array>();

                        if (peek() == ']') { ++m_At; return {out}; }

                        for (;;) {
                            out->push_back(read(aDepth + 1));

                            if (peek() == ',') { ++m_At; continue; }

                            break;
                        }

                        expect(']');

                        return {out};
                    }

                    case '"': return {text()};

                    case 't': if (word("true")) return {true}; break;
                    case 'f': if (word("false")) return {false}; break;
                    case 'n': if (word("null")) return {nullptr}; break;

                    default: return {number()};
                }

                throw wrong("something in the json this cannot read");
            }

            std::string_view m_Text;
            std::size_t m_At = 0;
        };

        [[nodiscard]] const value *within(const value &aValue, const std::string_view aKey) {
            const auto *const pObject = std::get_if<std::shared_ptr<object>>(&aValue.held);

            if (!pObject || !*pObject) return nullptr;

            for (const auto &[key, held] : **pObject)
                if (key == aKey) return &held;

            return nullptr;
        }

        [[nodiscard]] int whole(const value &aValue, const std::string_view aKey, const std::string_view aWhat,
            const std::optional<int> aUnless = {}) {
            const auto *const pFound = within(aValue, aKey);

            const auto *const pNumber = pFound ? std::get_if<double>(&pFound->held) : nullptr;

            if (!pNumber) {
                if (aUnless) return *aUnless;

                throw wrong(std::string("a sheet whose ").append(aWhat).append(" is not a number"));
            }

            if (std::floor(*pNumber) != *pNumber || std::abs(*pNumber) > 1e9)
                throw wrong(std::string("a sheet whose ").append(aWhat).append(" is not a whole number"));

            return static_cast<int>(*pNumber);
        }

        [[nodiscard]] std::string words(const value &aValue, const std::string_view aKey,
            const std::string &aUnless = {}) {
            const auto *const pFound = within(aValue, aKey);

            const auto *const pText = pFound ? std::get_if<std::string>(&pFound->held) : nullptr;

            return pText ? *pText : aUnless;
        }

        [[nodiscard]] std::array<int, 4> rectangle(const value &aValue, const std::string_view aKey,
            const std::string_view aWhat) {
            const auto *const pFound = within(aValue, aKey);

            if (!pFound) throw wrong(std::string("a sheet with no ").append(aWhat));

            return {whole(*pFound, "x", aWhat, 0), whole(*pFound, "y", aWhat, 0),
                whole(*pFound, "w", aWhat), whole(*pFound, "h", aWhat)};
        }

        [[nodiscard]] aseprite_direction direction_of(const std::string &aSaid) {
            if (aSaid.empty() || aSaid == "forward") return aseprite_direction::forward;
            if (aSaid == "reverse") return aseprite_direction::reverse;
            if (aSaid == "pingpong") return aseprite_direction::ping_pong;
            if (aSaid == "pingpong_reverse") return aseprite_direction::ping_pong_reverse;

            throw wrong("a tag that plays a way this does not know: " + aSaid);
        }

        [[nodiscard]] aseprite_frame frame_of(const value &aValue) {
            aseprite_frame out;

            const auto where = rectangle(aValue, "frame", "frame rectangle");

            out.x = where[0];
            out.y = where[1];
            out.w = where[2];
            out.h = where[3];

            const auto *const pWithin = within(aValue, "spriteSourceSize");

            if (pWithin) {
                const auto sat = rectangle(aValue, "spriteSourceSize", "sprite source size");

                out.offsetX = sat[0];
                out.offsetY = sat[1];
            }

            if (const auto *const pSize = within(aValue, "sourceSize")) {
                out.sourceWidth = whole(*pSize, "w", "source size");
                out.sourceHeight = whole(*pSize, "h", "source size");
            }
            else {
                out.sourceWidth = out.w;
                out.sourceHeight = out.h;
            }

            out.milliseconds = whole(aValue, "duration", "frame duration");

            if (out.w < 1 || out.h < 1) throw wrong("a frame of no texels");
            if (out.milliseconds < 1) throw wrong("a frame that shows for no time at all");

            return out;
        }
    }

    const aseprite_tag *aseprite_sheet::tag(const std::string_view aName) const {
        const auto found = std::find_if(tags.begin(), tags.end(),
            [aName](const aseprite_tag &each) { 
                return each.name == aName; 
            });

        return found == tags.end() ? nullptr : &*found;
    }

    const aseprite_slice *aseprite_sheet::slice(const std::string_view aName) const {
        const auto found = std::find_if(slices.begin(), slices.end(),
            [aName](const aseprite_slice &each) { return each.name == aName; });

        return found == slices.end() ? nullptr : &*found;
    }

    aseprite_sheet read_aseprite(const std::span<const std::byte> aBytes) {
        static constexpr auto MAX_FRAMES = 65536;

        if (aBytes.empty()) throw wrong("nothing to read");

        reading json(std::string_view(reinterpret_cast<const char *>(aBytes.data()), aBytes.size()));

        const auto read = json.whole();

        aseprite_sheet out;

        const auto *const pFrames = within(read, "frames");

        if (!pFrames) throw wrong("not an aseprite sheet: it has no frames");

        if (const auto *const pList = std::get_if<std::shared_ptr<array>>(&pFrames->held)) {
            if ((*pList)->size() > MAX_FRAMES) throw wrong("a sheet of more frames than are allowed");

            for (const auto &each : **pList) out.frames.push_back(frame_of(each));
        }
        else if (const auto *const pKeyed = std::get_if<std::shared_ptr<object>>(&pFrames->held)) {
            if ((*pKeyed)->size() > MAX_FRAMES) throw wrong("a sheet of more frames than are allowed");

            for (const auto &[name, each] : **pKeyed) out.frames.push_back(frame_of(each));
        }
        else throw wrong("a sheet whose frames are neither a list nor a table");

        if (out.frames.empty()) throw wrong("a sheet with no frames in it");

        const auto *const pMeta = within(read, "meta");

        if (!pMeta) throw wrong("not an aseprite sheet: it has no meta");

        out.image = words(*pMeta, "image");

        if (const auto *const pSize = within(*pMeta, "size")) {
            out.width = whole(*pSize, "w", "image width");
            out.height = whole(*pSize, "h", "image height");
        }

        if (out.width < 1 || out.height < 1) throw wrong("a sheet whose image has no size");

        for (const auto &each : out.frames)
            if (each.x < 0 || each.y < 0 || each.x + each.w > out.width || each.y + each.h > out.height)
                throw wrong("a frame that is not inside the image");

        if (const auto *const pTags = within(*pMeta, "frameTags")) {
            const auto *const pList = std::get_if<std::shared_ptr<array>>(&pTags->held);

            if (!pList) throw wrong("a sheet whose tags are not a list");

            if ((*pList)->size() > MAX_FRAMES) throw wrong("a sheet of more tags than are allowed");

            for (const auto &each : **pList) {
                aseprite_tag tag;

                tag.name = words(each, "name");
                tag.from = whole(each, "from", "tag's first frame");
                tag.to = whole(each, "to", "tag's last frame");
                tag.direction = direction_of(words(each, "direction"));

                const auto repeat = words(each, "repeat");

                if (!repeat.empty()) {
                    int times = 0;

                    const auto got = std::from_chars(repeat.data(), repeat.data() + repeat.size(), times);

                    if (got.ec != std::errc()) throw wrong("a tag that says it plays " + repeat + " times");

                    tag.repeat = times;
                }

                if (tag.from < 0 || tag.to < tag.from
                    || static_cast<std::size_t>(tag.to) >= out.frames.size())
                    throw wrong("a tag whose frames the sheet does not have: " + tag.name);

                out.tags.push_back(std::move(tag));
            }
        }

        if (const auto *const pSlices = within(*pMeta, "slices")) {
            const auto *const pList = std::get_if<std::shared_ptr<array>>(&pSlices->held);

            if (!pList) throw wrong("a sheet whose slices are not a list");

            for (const auto &each : **pList) {
                const auto name = words(each, "name");

                const auto *const pKeys = within(each, "keys");

                const auto *const pKeyList = pKeys ? std::get_if<std::shared_ptr<array>>(&pKeys->held) : nullptr;

                if (!pKeyList) continue;

                for (const auto &key : **pKeyList) {
                    aseprite_slice slice;

                    slice.name = name;
                    slice.frame = whole(key, "frame", "slice's frame", 0);

                    const auto bounds = rectangle(key, "bounds", "slice bounds");

                    slice.x = bounds[0];
                    slice.y = bounds[1];
                    slice.w = bounds[2];
                    slice.h = bounds[3];

                    if (const auto *const pPivot = within(key, "pivot"))
                        slice.pivot = std::pair{whole(*pPivot, "x", "slice pivot", 0),
                            whole(*pPivot, "y", "slice pivot", 0)};

                    out.slices.push_back(std::move(slice));
                }
            }
        }

        return out;
    }

    std::vector<sprite_animation::frame> frames_of(const aseprite_sheet &aSheet, const aseprite_tag &aTag) {
        if (aTag.from < 0 || aTag.to < aTag.from || static_cast<std::size_t>(aTag.to) >= aSheet.frames.size())
            throw wrong("a tag whose frames the sheet does not have: " + aTag.name);

        std::vector<int> order;

        for (int i = aTag.from; i <= aTag.to; ++i) order.push_back(i);

        if (aTag.direction == aseprite_direction::reverse
            || aTag.direction == aseprite_direction::ping_pong_reverse)
            std::reverse(order.begin(), order.end());

        if (aTag.direction == aseprite_direction::ping_pong
            || aTag.direction == aseprite_direction::ping_pong_reverse)
            for (std::size_t i = order.size() - 1; i-- > 1;) order.push_back(order[i]);

        std::vector<sprite_animation::frame> out;

        for (const auto which : order) {
            const auto &each = aSheet.frames[static_cast<std::size_t>(which)];

            out.push_back({each.x, each.y, each.w, each.h,
                static_cast<sprite_animation::time_type>(each.milliseconds) / 1000.0f});
        }

        return out;
    }

    std::vector<sprite_animation::frame> frames_of(const aseprite_sheet &aSheet) {
        aseprite_tag whole;

        whole.name = "the whole sheet";
        whole.to = static_cast<int>(aSheet.frames.size()) - 1;

        return frames_of(aSheet, whole);
    }

    sprite_animation animation_of(const aseprite_sheet &aSheet, const std::string_view aTag) {
        const auto *const pTag = aSheet.tag(aTag);

        if (!pTag) throw wrong(std::string("a sheet with no tag called ").append(aTag));

        const auto frames = frames_of(aSheet, *pTag);

        return sprite_animation(frames, aSheet.width, aSheet.height,
            pTag->repeat > 0 ? sprite_animation::play_mode::once : sprite_animation::play_mode::loop);
    }

    sprite_animation animation_of(const aseprite_sheet &aSheet) {
        const auto frames = frames_of(aSheet);

        return sprite_animation(frames, aSheet.width, aSheet.height);
    }
}

