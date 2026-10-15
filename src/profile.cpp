// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/profile.h>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/model_data.h>

#include <algorithm>
#include <cctype>
#include <initializer_list>
#include <map>
#include <optional>
#include <string>
#include <vector>

using namespace gdk::graphics;

profile gdk::graphics::es2_minimum() {
    profile out{"es2_minimum", {}};

    out.most.texture_size = 64;
    out.most.non_power_of_two_textures = false;
    out.most.vertex_uniform_vectors = 128;
    out.most.fragment_uniform_vectors = 16;
    out.most.texture_units = 8;
    out.most.vertex_texture_units = 0;
    out.most.vertex_attributes = 8;
    out.most.varying_vectors = 8;

    return out;
}

profile gdk::graphics::webgl1_minimum() {
    auto out = es2_minimum();

    out.name = "webgl1_minimum";

    return out;
}

namespace {
    [[nodiscard]] bool is_power_of_two(const std::size_t aValue) {
        return aValue > 0 && (aValue & (aValue - 1)) == 0;
    }

    [[nodiscard]] std::string past(const profile &aProfile, const std::string &aWhat, const std::size_t aHas,
        const std::string &aLimit, const std::size_t aMost) {
        return aProfile.name + ": " + aWhat + " " + std::to_string(aHas) + ", past " + aProfile.name + "'s "
            + aLimit + " of " + std::to_string(aMost);
    }

    struct declared final {
        std::size_t uniformVectors = 0;
        std::size_t samplers = 0;
        std::size_t attributeVectors = 0;
        std::size_t varyingVectors = 0;
    };

    [[nodiscard]] std::string without_comments(const std::string_view aSource) {
        std::string out;
        out.reserve(aSource.size());

        for (std::size_t i = 0; i < aSource.size();) {
            if (aSource.compare(i, 2, "//") == 0) {
                while (i < aSource.size() && aSource[i] != '\n') ++i;
            }
            else if (aSource.compare(i, 2, "/*") == 0) {
                const auto end = aSource.find("*/", i + 2);

                i = end == std::string_view::npos ? aSource.size() : end + 2;
                out += ' ';
            }
            else out += aSource[i++];
        }

        return out;
    }

    [[nodiscard]] std::vector<std::string> words_of(const std::string &aText) {
        std::vector<std::string> out;
        std::string word;

        for (const auto c : aText) {
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') word += c;
            else {
                if (!word.empty()) out.push_back(std::move(word));
                word.clear();

                if (c == '[' || c == ']' || c == ',' || c == '=') out.emplace_back(1, c);
            }
        }

        if (!word.empty()) out.push_back(std::move(word));

        return out;
    }

    [[nodiscard]] std::optional<std::size_t> vectors_of(const std::string &aType) {
        static const std::map<std::string, std::size_t> KNOWN{
            {"float", 1}, {"int", 1}, {"bool", 1},
            {"vec2", 1}, {"vec3", 1}, {"vec4", 1},
            {"ivec2", 1}, {"ivec3", 1}, {"ivec4", 1},
            {"bvec2", 1}, {"bvec3", 1}, {"bvec4", 1},
            {"mat2", 2}, {"mat3", 3}, {"mat4", 4}};

        if (const auto found = KNOWN.find(aType); found != KNOWN.end()) return found->second;

        return std::nullopt;
    }

    [[nodiscard]] bool is_sampler(const std::string &aType) {
        return aType == "sampler2D" || aType == "samplerCube";
    }

    [[nodiscard]] std::optional<std::size_t> number_of(const std::string &aWord) {
        if (aWord.empty() || !std::all_of(aWord.begin(), aWord.end(), [](const char aC) {
            return std::isdigit(static_cast<unsigned char>(aC)) != 0;
        })) return std::nullopt;

        return static_cast<std::size_t>(std::stoull(aWord));
    }

    [[nodiscard]] declared declared_in(const std::string_view aSource, const std::string &aStage) {
        const auto source = without_comments(aSource);

        std::map<std::string, std::vector<std::string>> defines;

        std::map<std::string, std::size_t> sizes;

        std::size_t lineStart = 0;

        while (lineStart < source.size()) {
            auto lineEnd = source.find('\n', lineStart);

            if (lineEnd == std::string::npos) lineEnd = source.size();

            const auto line = words_of(source.substr(lineStart, lineEnd - lineStart));
            const auto hash = source.find_first_not_of(" \t", lineStart);

            if (hash < lineEnd && source[hash] == '#' && line.size() >= 2 && line[0] == "define")
                defines[line[1]] = std::vector<std::string>(line.begin() + 2, line.end());

            lineStart = lineEnd + 1;
        }

        declared out;

        std::string statement;
        std::vector<std::string> statements;

        for (std::size_t i = 0; i < source.size(); ++i) {
            const auto c = source[i];

            if (c == '#') {
                while (i < source.size() && source[i] != '\n') ++i;

                continue;
            }

            if (c == ';' || c == '{' || c == '}') {
                statements.push_back(std::move(statement));
                statement.clear();
            }
            else statement += c;
        }

        for (const auto &each : statements) {
            auto words = words_of(each);

            for (int pass = 0; pass < 4; ++pass) {
                std::vector<std::string> expanded;
                bool changed = false;

                for (const auto &word : words) {
                    if (const auto found = defines.find(word); found != defines.end()) {
                        expanded.insert(expanded.end(), found->second.begin(), found->second.end());
                        changed = true;
                    }
                    else expanded.push_back(word);
                }

                words = std::move(expanded);

                if (!changed) break;
            }

            std::size_t at = 0;

            const auto skip = [&](const std::initializer_list<const char *> aWords) {
                while (at < words.size() && std::any_of(aWords.begin(), aWords.end(), [&](const char *aWord) {
                    return words[at] == aWord;
                })) ++at;
            };

            skip({"invariant"});

            if (at + 1 < words.size() && words[at] == "const" && words[at + 1] == "int") {
                if (at + 4 < words.size() && words[at + 3] == "=")
                    if (const auto value = number_of(words[at + 4])) sizes[words[at + 2]] = *value;

                continue;
            }

            if (at >= words.size()) continue;

            const auto qualifier = words[at++];

            if (qualifier != "uniform" && qualifier != "attribute" && qualifier != "varying") continue;

            skip({"highp", "mediump", "lowp"});

            if (at >= words.size()) continue;

            const auto type = words[at++];
            const auto sampler = is_sampler(type);
            const auto vectors = vectors_of(type);

            if (!sampler && !vectors)
                throw exception("a " + aStage + " shader's " + qualifier + " of type " + type
                    + ", which a profile cannot count");

            while (at < words.size()) {
                if (words[at] == ",") { ++at; continue; }

                const auto name = words[at++];
                std::size_t count = 1;

                if (at < words.size() && words[at] == "[") {
                    const auto size = at + 1 < words.size() ? words[at + 1] : std::string();
                    const auto number = number_of(size);
                    const auto named = sizes.find(size);

                    if (!number && named == sizes.end())
                        throw exception("a " + aStage + " shader's " + qualifier + " " + name + "[" + size
                            + "], whose size a profile cannot count: a number, a #define or a const int");

                    count = number ? *number : named->second;

                    while (at < words.size() && words[at] != "]") ++at;

                    ++at;
                }

                if (qualifier == "uniform") {
                    if (sampler) out.samplers += count;
                    else out.uniformVectors += count * *vectors;
                }
                else if (qualifier == "attribute") out.attributeVectors += count * vectors.value_or(1);
                else out.varyingVectors += count * vectors.value_or(1);
            }
        }

        return out;
    }
}

void gdk::graphics::check_texture(const profile &aProfile, const std::size_t aWidth, const std::size_t aHeight) {
    check_texture(aProfile, aWidth, aHeight, false);
}

void gdk::graphics::check_texture(const profile &aProfile, const std::size_t aWidth, const std::size_t aHeight,
    const bool aClampedAndUnmipmapped) {
    const auto &most = aProfile.most;

    const auto too_big = [&](const std::size_t aHas, const std::string &aWay) {
        return exception(aProfile.name + ": a texture " + std::to_string(aHas) + " " + aWay + ", past "
            + aProfile.name + "'s largest texture of " + std::to_string(most.texture_size));
    };

    if (aWidth > most.texture_size) throw too_big(aWidth, "wide");
    if (aHeight > most.texture_size) throw too_big(aHeight, "tall");

    if (most.non_power_of_two_textures || aClampedAndUnmipmapped || (aWidth == 0 && aHeight == 0)) return;

    if (!is_power_of_two(aWidth))
        throw exception(aProfile.name + ": a texture " + std::to_string(aWidth)
            + " wide is not a power of two, which " + aProfile.name + " requires");

    if (!is_power_of_two(aHeight))
        throw exception(aProfile.name + ": a texture " + std::to_string(aHeight)
            + " tall is not a power of two, which " + aProfile.name + " requires");
}

void gdk::graphics::check_shader(const profile &aProfile, const std::string_view aVertexSource,
    const std::string_view aFragmentSource) {
    const auto &most = aProfile.most;
    const auto vertex = declared_in(aVertexSource, "vertex");
    const auto fragment = declared_in(aFragmentSource, "fragment");

    const auto refuse_past = [&](const std::size_t aHas, const std::size_t aMost, const std::string &aWhat,
        const std::string &aLimit) {
        if (aHas > aMost) throw exception(past(aProfile, aWhat, aHas, aLimit, aMost));
    };

    refuse_past(vertex.uniformVectors, most.vertex_uniform_vectors, "a vertex shader's uniform vectors",
        "vertex uniform vectors");
    refuse_past(fragment.uniformVectors, most.fragment_uniform_vectors, "a fragment shader's uniform vectors",
        "fragment uniform vectors");
    refuse_past(fragment.samplers, most.texture_units, "a fragment shader's samplers", "texture units");
    refuse_past(vertex.samplers, most.vertex_texture_units, "a vertex shader's samplers", "vertex texture units");
    refuse_past(vertex.attributeVectors, most.vertex_attributes, "a vertex shader's attribute vectors",
        "vertex attributes");
    refuse_past(vertex.varyingVectors, most.varying_vectors, "a vertex shader's varying vectors", "varying vectors");
}

void gdk::graphics::check_model(const profile &aProfile, const model_data &aModel) {
    if (const auto has = aModel.attributes().size(); has > aProfile.most.vertex_attributes)
        throw exception(past(aProfile, "a model's attributes", has, "vertex attributes",
            aProfile.most.vertex_attributes));
}
