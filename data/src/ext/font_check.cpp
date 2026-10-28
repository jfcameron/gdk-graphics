// © Joseph Cameron - MIT License

#include "font_check.h"

#include <gdk/graphics/exception.h>

#include <algorithm>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
    constexpr std::size_t MOST_COMPOSITE_DEPTH = 16;
    constexpr std::size_t MOST_GPOS_WORK = std::size_t{1} << 22;

    [[noreturn]] void refuse(const std::string &aWhy) { throw exception("gdk::graphics::ext::font: " + aWhy); }

    class bytes final {
    public:
        bytes(const std::span<const unsigned char> aData, std::string aName)
        : m_Data(aData)
        , m_Name(std::move(aName))
        {}

        [[nodiscard]] std::size_t size() const { return m_Data.size(); }

        [[nodiscard]] bool has(const std::size_t aAt, const std::size_t aCount) const {
            return aAt <= m_Data.size() && aCount <= m_Data.size() - aAt;
        }

        void need(const std::size_t aAt, const std::size_t aCount) const {
            if (!has(aAt, aCount)) refuse("the font's " + m_Name + " is cut short, or points past its end");
        }

        [[nodiscard]] bytes from(const std::size_t aAt) const {
            need(aAt, 0);

            return {m_Data.subspan(aAt), m_Name};
        }

        [[nodiscard]] bytes sub(const std::size_t aAt, const std::size_t aCount) const {
            need(aAt, aCount);

            return {m_Data.subspan(aAt, aCount), m_Name};
        }

        [[nodiscard]] std::uint8_t u8(const std::size_t aAt) const {
            need(aAt, 1);

            return m_Data[aAt];
        }

        [[nodiscard]] std::uint16_t u16(const std::size_t aAt) const {
            need(aAt, 2);

            return static_cast<std::uint16_t>((m_Data[aAt] << 8) | m_Data[aAt + 1]);
        }

        [[nodiscard]] std::int16_t i16(const std::size_t aAt) const { return static_cast<std::int16_t>(u16(aAt)); }

        [[nodiscard]] std::uint32_t u32(const std::size_t aAt) const {
            need(aAt, 4);

            return (std::uint32_t{m_Data[aAt]} << 24) | (std::uint32_t{m_Data[aAt + 1]} << 16)
                | (std::uint32_t{m_Data[aAt + 2]} << 8) | std::uint32_t{m_Data[aAt + 3]};
        }

        [[nodiscard]] std::uint32_t offset(const std::size_t aAt, const std::size_t aSize) const {
            std::uint32_t value = 0;

            for (std::size_t i = 0; i < aSize; ++i) value = (value << 8) | u8(aAt + i);

            return value;
        }

    private:
        std::span<const unsigned char> m_Data;
        std::string m_Name;
    };

    constexpr std::uint32_t tag(const char aA, const char aB, const char aC, const char aD) {
        return (std::uint32_t(std::uint8_t(aA)) << 24) | (std::uint32_t(std::uint8_t(aB)) << 16)
            | (std::uint32_t(std::uint8_t(aC)) << 8) | std::uint32_t(std::uint8_t(aD));
    }

    [[nodiscard]] std::size_t start_of(const bytes &aFont, const std::size_t aFace) {
        const auto kind = aFont.u32(0);

        if (kind == tag('t', 't', 'c', 'f')) {
            const auto version = aFont.u32(4);

            if (version != 0x00010000 && version != 0x00020000) refuse("the font collection is of an unknown version");

            const std::size_t faces = aFont.u32(8);

            if (faces > aFont.size() / 4) refuse("the font collection claims more faces than it has room for");

            aFont.need(12, faces * 4);

            if (aFace >= faces)
                refuse("face " + std::to_string(aFace) + " was asked for, and the data holds " + std::to_string(faces));

            return aFont.u32(12 + aFace * 4);
        }

        const auto isFont = kind == 0x00010000 || kind == tag('t', 'r', 'u', 'e') || kind == tag('O', 'T', 'T', 'O')
            || kind == tag('t', 'y', 'p', '1') || kind == tag('1', 0, 0, 0);

        if (!isFont) refuse("data does not begin a truetype font");

        if (aFace) refuse("face " + std::to_string(aFace) + " was asked for, and the data holds 1");

        return 0;
    }

    [[nodiscard]] std::map<std::uint32_t, font_check::table> tables_of(const bytes &aFont, const std::size_t aStart) {
        const std::size_t count = aFont.u16(aStart + 4);

        aFont.need(aStart + 12, count * 16);

        std::map<std::uint32_t, font_check::table> out;

        for (std::size_t i = 0; i < count; ++i) {
            const auto record = aStart + 12 + i * 16;

            const font_check::table each{aFont.u32(record + 8), aFont.u32(record + 12)};

            if (!aFont.has(each.offset, each.length)) refuse("a table lies past the end of the font");

            if (!each.offset) refuse("a table lies at the start of the font, where its header is");

            out.emplace(aFont.u32(record), each);
        }

        return out;
    }

    void check_cmap(const bytes &aCmap) {
        const std::size_t count = aCmap.u16(2);

        aCmap.need(4, count * 8);

        std::optional<std::size_t> chosen;

        for (std::size_t i = 0; i < count; ++i) {
            const auto platform = aCmap.u16(4 + i * 8);
            const auto encoding = aCmap.u16(6 + i * 8);

            if ((platform == 3 && (encoding == 1 || encoding == 10)) || platform == 0)
                chosen = aCmap.u32(8 + i * 8);
        }

        if (!chosen) refuse("the font has no Unicode character map");

        const auto map = aCmap.from(*chosen);

        switch (const auto format = map.u16(0)) {
            case 0: {
                const std::size_t length = map.u16(2);

                if (length < 6) refuse("the font's character map is cut short");

                map.need(0, length);

                return;
            }

            case 4: {
                const std::size_t doubled = map.u16(6);

                if (!doubled || doubled % 2) refuse("the font's character map has no whole segments");

                const auto segments = doubled / 2;

                std::size_t power = 1, exponent = 0;

                while (power * 2 <= segments) { power *= 2; ++exponent; }

                if (map.u16(8) != power * 2 || map.u16(10) != exponent || map.u16(12) != segments * 2 - power * 2)
                    refuse("the font's character map searches its segments wrongly");

                map.need(0, 16 + segments * 8);

                for (std::size_t i = 0; i < segments; ++i) {
                    const std::size_t last = map.u16(14 + i * 2);
                    const std::size_t first = map.u16(16 + segments * 2 + i * 2);
                    const auto rangeAt = 16 + segments * 6 + i * 2;
                    const std::size_t range = map.u16(rangeAt);

                    const auto sentinel = first == 0xFFFF && last == 0xFFFF;

                    if (range && first <= last && !sentinel) map.need(rangeAt + range, (last - first + 1) * 2);
                }

                return;
            }

            case 6: map.need(10, std::size_t{map.u16(8)} * 2); return;

            case 12:
            case 13: {
                const std::size_t groups = map.u32(12);

                if (groups > map.size() / 12) refuse("the font's character map claims more groups than it holds");

                map.need(16, groups * 12);

                return;
            }

            default: refuse("the font's character map is in format " + std::to_string(format) + ", which isn't read");
        }
    }

    [[nodiscard]] std::vector<std::uint16_t> components_of(const bytes &aGlyph) {
        std::vector<std::uint16_t> out;

        std::size_t at = 10;

        for (bool more = true; more;) {
            const auto flags = aGlyph.u16(at);
            const auto glyph = aGlyph.u16(at + 2);

            at += 4;

            if (!(flags & 2)) refuse("a composite glyph is placed by matching points, which isn't read");

            at += (flags & 1) ? 4 : 2;

            if (flags & 8) at += 2;
            else if (flags & 64) at += 4;
            else if (flags & 128) at += 8;

            aGlyph.need(0, at);

            out.push_back(glyph);

            more = flags & 32;
        }

        return out;
    }

    //! whether stb_truetype can draw the glyph: it reads a point past the last when the last contour is one point,
    //! off the curve
    [[nodiscard]] bool check_simple_glyph(const bytes &aGlyph, const std::size_t aContours) {
        aGlyph.need(10, aContours * 2 + 2);

        std::size_t points = 0, lastStart = 0;

        for (std::size_t i = 0; i < aContours; ++i) {
            const std::size_t end = aGlyph.u16(10 + i * 2);

            if (i && end < points) refuse("a glyph's contours end out of order");

            lastStart = points;
            points = end + 1;
        }

        bool lastOffCurve = false;

        std::size_t at = 12 + aContours * 2 + aGlyph.u16(10 + aContours * 2);
        std::size_t xBytes = 0, yBytes = 0;

        for (std::size_t point = 0; point < points;) {
            const auto flags = aGlyph.u8(at++);

            std::size_t repeats = 1;

            if (flags & 8) repeats += aGlyph.u8(at++);

            repeats = std::min(repeats, points - point);

            if (point + repeats == points) lastOffCurve = !(flags & 1);

            xBytes += repeats * ((flags & 2) ? 1 : (flags & 16) ? 0 : 2);
            yBytes += repeats * ((flags & 4) ? 1 : (flags & 32) ? 0 : 2);

            point += repeats;
        }

        aGlyph.need(at, xBytes + yBytes);

        return !(aContours && lastStart + 1 == points && lastOffCurve);
    }

    //! how deep a composite nests, and whether stb_truetype can draw it: it reads a component's glyph as signed,
    //! so one from 0x8000 on is read from before the glyph locations
    struct composite_reach final {
        std::size_t depth = 0;
        bool drawable = true;
    };

    [[nodiscard]] composite_reach reach_of(const std::size_t aGlyph,
        const std::vector<std::vector<std::uint16_t>> &aComponents, const std::vector<bool> &aSimpleUndrawable,
        std::vector<std::optional<composite_reach>> &aReach, std::vector<bool> &aVisiting) {
        if (aReach[aGlyph]) return *aReach[aGlyph];

        if (aVisiting[aGlyph]) refuse("a composite glyph is made of itself");

        aVisiting[aGlyph] = true;

        composite_reach out;

        for (const auto each : aComponents[aGlyph]) {
            if (each >= 0x8000) { out.drawable = false; continue; }

            if (each >= aComponents.size()) continue;

            if (aSimpleUndrawable[each]) { out.drawable = false; continue; }

            const auto below = reach_of(each, aComponents, aSimpleUndrawable, aReach, aVisiting);

            out.depth = std::max(out.depth, below.depth);
            out.drawable = out.drawable && below.drawable;

            if (out.depth >= MOST_COMPOSITE_DEPTH) refuse("a composite glyph nests too deeply");
        }

        ++out.depth;

        aVisiting[aGlyph] = false;

        aReach[aGlyph] = out;

        return out;
    }

    [[nodiscard]] std::vector<bool> check_glyphs(const bytes &aLoca, const bytes &aGlyf, const std::size_t aGlyphs,
        const std::int16_t aFormat) {
        if (aFormat != 0 && aFormat != 1) refuse("the font's glyph locations are in an unknown format");

        const auto width = aFormat ? 4 : 2;

        aLoca.need(0, (aGlyphs + 1) * width);

        const auto location = [&](const std::size_t aGlyph) -> std::size_t {
            return aFormat ? aLoca.u32(aGlyph * 4) : std::size_t{aLoca.u16(aGlyph * 2)} * 2;
        };

        std::vector<std::vector<std::uint16_t>> components(aGlyphs);
        std::vector<bool> undrawable(aGlyphs, false);

        for (std::size_t glyph = 0; glyph < aGlyphs; ++glyph) {
            const auto first = location(glyph);
            const auto given = location(glyph + 1);
            const auto last = given < first ? aGlyf.size() : given;

            if (first > aGlyf.size() || last > aGlyf.size()) refuse("a glyph lies outside the font's glyph data");

            if (first == given) continue;

            const auto data = aGlyf.sub(first, last - first);

            data.need(0, 10);

            if (const auto contours = data.i16(0); contours >= 0)
                undrawable[glyph] = !check_simple_glyph(data, static_cast<std::size_t>(contours));
            else components[glyph] = components_of(data);
        }

        const auto simpleUndrawable = undrawable;

        std::vector<std::optional<composite_reach>> reach(aGlyphs);
        std::vector<bool> visiting(aGlyphs, false);

        for (std::size_t glyph = 0; glyph < aGlyphs; ++glyph)
            if (!components[glyph].empty())
                undrawable[glyph] = !reach_of(glyph, components, simpleUndrawable, reach, visiting).drawable;

        if (undrawable[0]) refuse("the font's notdef glyph is made of a glyph that cannot be read");

        return undrawable;
    }

    [[nodiscard]] bool kern_reads(const bytes &aKern) {
        if (!aKern.has(0, 4)) return false;

        if (aKern.u16(2) < 1) return true;

        if (!aKern.has(0, 18)) return false;

        if (aKern.u16(8) != 1) return true;

        return aKern.has(18, std::size_t{aKern.u16(10)} * 6);
    }

    [[nodiscard]] std::size_t highest_coverage(const bytes &aCoverage) {
        switch (aCoverage.u16(0)) {
            case 1: {
                const std::size_t count = aCoverage.u16(2);

                aCoverage.need(4, count * 2);

                return count;
            }

            case 2: {
                const std::size_t count = aCoverage.u16(2);

                aCoverage.need(4, count * 6);

                std::size_t highest = 0;

                for (std::size_t i = 0; i < count; ++i) {
                    const std::size_t first = aCoverage.u16(4 + i * 6);
                    const std::size_t last = aCoverage.u16(6 + i * 6);

                    if (last >= first) highest = std::max(highest, aCoverage.u16(8 + i * 6) + last - first + 1);
                }

                return highest;
            }

            default: return 0;
        }
    }

    void check_class_definition(const bytes &aClasses) {
        switch (aClasses.u16(0)) {
            case 1: aClasses.need(6, std::size_t{aClasses.u16(4)} * 2); return;
            case 2: aClasses.need(4, std::size_t{aClasses.u16(2)} * 6); return;
            default: return;
        }
    }

    [[nodiscard]] bool gpos_reads(const bytes &aGpos) {
        try {
            if (aGpos.u16(0) != 1 || aGpos.u16(2) != 0) return true;

            const auto lookups = aGpos.from(aGpos.u16(8));
            const std::size_t count = lookups.u16(0);

            std::size_t work = 0;

            for (std::size_t i = 0; i < count; ++i) {
                const auto lookup = lookups.from(lookups.u16(2 + i * 2));

                const auto type = lookup.u16(0);
                const std::size_t subtables = lookup.u16(4);

                lookup.need(6, subtables * 2);

                if (type != 2) continue;

                for (std::size_t s = 0; s < subtables; ++s) {
                    const auto table = lookup.from(lookup.u16(6 + s * 2));

                    const auto format = table.u16(0);
                    const auto covered = highest_coverage(table.from(table.u16(2)));

                    if (format == 1) {
                        if (table.u16(4) != 4 || table.u16(6) != 0) continue;

                        const std::size_t sets = table.u16(8);

                        table.need(10, covered * 2);

                        for (std::size_t index = 0; index < covered; ++index) {
                            if (++work > MOST_GPOS_WORK) return false;

                            const auto pairs = table.from(table.u16(10 + index * 2));

                            pairs.need(0, 2);

                            if (index < sets) pairs.need(2, std::size_t{pairs.u16(0)} * 4);
                        }
                    }
                    else if (format == 2) {
                        if (table.u16(4) != 4 || table.u16(6) != 0) continue;

                        check_class_definition(table.from(table.u16(8)));
                        check_class_definition(table.from(table.u16(10)));

                        table.need(16, std::size_t{table.u16(12)} * table.u16(14) * 2);
                    }
                }
            }

            return true;
        }
        catch (const exception &) {
            return false;
        }
    }

    struct cff_index final {
        std::size_t count = 0;
        std::size_t offset_size = 0;
        std::size_t offsets_at = 0;
        std::size_t end = 0;
    };

    [[nodiscard]] cff_index index_at(const bytes &aCff, const std::size_t aAt) {
        cff_index out;

        out.count = aCff.u16(aAt);

        if (!out.count) {
            out.end = aAt + 2;

            return out;
        }

        out.offset_size = aCff.u8(aAt + 2);

        if (out.offset_size < 1 || out.offset_size > 4) refuse("the font's CFF has an index of an unknown width");

        out.offsets_at = aAt + 3;

        aCff.need(out.offsets_at, (out.count + 1) * out.offset_size);

        std::size_t previous = 1;

        for (std::size_t i = 0; i <= out.count; ++i) {
            const std::size_t each = aCff.offset(out.offsets_at + i * out.offset_size, out.offset_size);

            if ((i == 0 && each != 1) || each < previous) refuse("the font's CFF has an index out of order");

            previous = each;
        }

        const auto dataAt = out.offsets_at + (out.count + 1) * out.offset_size;

        aCff.need(dataAt, previous - 1);

        out.end = dataAt + previous - 1;

        return out;
    }

    [[nodiscard]] bytes element_of(const bytes &aCff, const cff_index &aIndex, const std::size_t aElement) {
        const auto dataAt = aIndex.offsets_at + (aIndex.count + 1) * aIndex.offset_size - 1;

        const std::size_t first = aCff.offset(aIndex.offsets_at + aElement * aIndex.offset_size, aIndex.offset_size);
        const std::size_t last = aCff.offset(aIndex.offsets_at + (aElement + 1) * aIndex.offset_size, aIndex.offset_size);

        return aCff.sub(dataAt + first, last - first);
    }

    [[nodiscard]] std::map<int, std::vector<std::int64_t>> dictionary_of(const bytes &aDictionary) {
        std::map<int, std::vector<std::int64_t>> out;

        std::vector<std::int64_t> operands;

        for (std::size_t at = 0; at < aDictionary.size();) {
            const auto b0 = aDictionary.u8(at++);

            if (b0 >= 28) {
                if (b0 == 30) {
                    while (at < aDictionary.size()) {
                        const auto nibbles = aDictionary.u8(at++);

                        if ((nibbles & 0xF) == 0xF || (nibbles >> 4) == 0xF) break;
                    }

                    operands.push_back(0);
                }
                else if (b0 >= 32 && b0 <= 246) operands.push_back(b0 - 139);
                else if (b0 >= 247 && b0 <= 250) operands.push_back((b0 - 247) * 256 + aDictionary.u8(at++) + 108);
                else if (b0 >= 251 && b0 <= 254) operands.push_back(-(b0 - 251) * 256 - aDictionary.u8(at++) - 108);
                else if (b0 == 28) { operands.push_back(aDictionary.i16(at)); at += 2; }
                else if (b0 == 29) { operands.push_back(static_cast<std::int32_t>(aDictionary.u32(at))); at += 4; }
                else operands.push_back(0);

                continue;
            }

            const auto op = b0 == 12 ? 0x100 | aDictionary.u8(at++) : b0;

            out.emplace(op, std::move(operands));

            operands.clear();
        }

        return out;
    }

    [[nodiscard]] std::size_t place(const std::map<int, std::vector<std::int64_t>> &aDictionary, const int aKey,
        const std::size_t aOperand = 0) {
        const auto found = aDictionary.find(aKey);

        if (found == aDictionary.end() || found->second.size() <= aOperand) return 0;

        if (found->second[aOperand] < 0) refuse("the font's CFF has a negative offset");

        return static_cast<std::size_t>(found->second[aOperand]);
    }

    void check_private(const bytes &aCff, const std::map<int, std::vector<std::int64_t>> &aDictionary) {
        const auto size = place(aDictionary, 18, 0);
        const auto at = place(aDictionary, 18, 1);

        if (!size || !at) return;

        const auto subrs = place(dictionary_of(aCff.sub(at, size)), 19);

        if (subrs) static_cast<void>(index_at(aCff, at + subrs));
    }

    void check_cff(const bytes &aCff, const std::size_t aGlyphs) {
        const std::size_t header = aCff.u8(2);

        if (header < 4) refuse("the font's CFF header is cut short");

        const auto names = index_at(aCff, header);
        const auto tops = index_at(aCff, names.end);

        if (!tops.count) refuse("the font's CFF has no top dictionary");

        const auto strings = index_at(aCff, tops.end);

        static_cast<void>(index_at(aCff, strings.end));

        const auto top = dictionary_of(element_of(aCff, tops, 0));

        if (const auto found = top.find(0x100 | 6); found != top.end() && !found->second.empty()
            && found->second[0] != 2)
            refuse("the font's CFF charstrings are not Type 2");

        const auto charstrings = place(top, 17);

        if (!charstrings) refuse("the font's CFF has no charstrings");

        if (index_at(aCff, charstrings).count < aGlyphs) refuse("the font's CFF has fewer charstrings than glyphs");

        check_private(aCff, top);

        if (const auto fonts = place(top, 0x100 | 36)) {
            const auto selects = place(top, 0x100 | 37);

            if (!selects) refuse("the font's CFF names its font dictionaries but not which glyph uses which");

            const auto dictionaries = index_at(aCff, fonts);

            for (std::size_t i = 0; i < dictionaries.count; ++i)
                check_private(aCff, dictionary_of(element_of(aCff, dictionaries, i)));

            switch (aCff.u8(selects)) {
                case 0: aCff.need(selects + 1, aGlyphs); break;
                case 3: aCff.need(selects + 3, std::size_t{aCff.u16(selects + 1)} * 3 + 2); break;
                default: refuse("the font's CFF selects font dictionaries in an unknown format");
            }
        }
    }
}

font_check::checked font_check::check(const std::span<const unsigned char> aFont, const std::size_t aFace) {
    const bytes font(aFont, "header");

    checked out;

    out.start = start_of(font, aFace);

    const auto tables = tables_of(font, out.start);

    const auto required = [&](const std::uint32_t aTag, const char *aName) {
        const auto found = tables.find(aTag);

        if (found == tables.end()) refuse(std::string("the font has no ") + aName + " table");

        return bytes(aFont.subspan(found->second.offset, found->second.length), aName);
    };

    static_cast<void>(required(tag('h', 'm', 't', 'x'), "hmtx"));

    const auto head = required(tag('h', 'e', 'a', 'd'), "head");
    const auto hhea = required(tag('h', 'h', 'e', 'a'), "hhea");
    const auto maxp = required(tag('m', 'a', 'x', 'p'), "maxp");
    const auto hmtx = bytes(aFont.subspan(tables.at(tag('h', 'm', 't', 'x')).offset), "hmtx");

    head.need(0, 54);

    if (const auto units = head.u16(18); units < 16 || units > 16384)
        refuse("the font's em is " + std::to_string(units) + " units, outside 16 to 16384");

    hhea.need(0, 36);

    if (hhea.i16(4) == hhea.i16(6)) refuse("the font's ascent and descent are the same, so it has no height");

    out.glyphs = maxp.u16(4);

    if (!out.glyphs) refuse("the font has no glyphs");

    const std::size_t metrics = hhea.u16(34);

    if (!metrics) refuse("the font has no horizontal metrics");

    hmtx.need(0, metrics * 4);

    if (out.glyphs > metrics) hmtx.need(metrics * 4, (out.glyphs - metrics) * 2);

    check_cmap(required(tag('c', 'm', 'a', 'p'), "cmap"));

    if (tables.contains(tag('g', 'l', 'y', 'f'))) {
        out.undrawable = check_glyphs(required(tag('l', 'o', 'c', 'a'), "loca"), required(tag('g', 'l', 'y', 'f'), "glyf"),
            out.glyphs, head.i16(50));
    }
    else {
        const auto cff = tables.find(tag('C', 'F', 'F', ' '));

        if (cff == tables.end()) refuse("the font has neither glyf nor CFF outlines");

        check_cff(bytes(aFont.subspan(cff->second.offset, cff->second.length), "CFF"), out.glyphs);

        out.cff = cff->second;
    }

    if (const auto kern = tables.find(tag('k', 'e', 'r', 'n')); kern != tables.end())
        out.kern = kern_reads(bytes(aFont.subspan(kern->second.offset, kern->second.length), "kern"));

    if (const auto gpos = tables.find(tag('G', 'P', 'O', 'S')); gpos != tables.end())
        out.gpos = gpos_reads(bytes(aFont.subspan(gpos->second.offset, gpos->second.length), "GPOS"));

    return out;
}
