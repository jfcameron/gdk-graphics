// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/png.h>

#include <stb/stb_image.h>

#include <limits>
#include <memory>
#include <string>

using namespace gdk::graphics;
using namespace gdk::graphics::texture_data;

namespace {
    [[nodiscard]] int channels_of(const texture_data::format aFormat) {
        switch (aFormat) {
            case texture_data::format::grey: return 1;
            case texture_data::format::rg: return 2;
            case texture_data::format::rgb: return 3;
            case texture_data::format::rgba: return 4;
        }

        throw exception("make_from_png: an unknown texture format");
    }

    [[nodiscard]] std::size_t big_endian_u32(const std::span<const encoded_byte> aBytes, const std::size_t aAt) {
        return (std::size_t{aBytes[aAt]} << 24) | (std::size_t{aBytes[aAt + 1]} << 16)
            | (std::size_t{aBytes[aAt + 2]} << 8) | std::size_t{aBytes[aAt + 3]};
    }

    //! stb_image grows its buffer to what a chunk claims before reading it, so a chunk claiming more than the file
    //! holds would have it ask for that much
    void check_chunks(const std::span<const encoded_byte> aPNG) {
        constexpr std::size_t SIGNATURE = 8, HEADER = 8, CRC = 4;

        for (std::size_t at = SIGNATURE; at + HEADER <= aPNG.size();) {
            const auto length = big_endian_u32(aPNG, at);

            if (length > aPNG.size() - at - HEADER || aPNG.size() - at - HEADER - length < CRC)
                throw exception("make_from_png: a chunk claims " + std::to_string(length)
                    + " bytes, more than the file has");

            if (aPNG[at + 4] == 'I' && aPNG[at + 5] == 'E' && aPNG[at + 6] == 'N' && aPNG[at + 7] == 'D') return;

            at += HEADER + length + CRC;
        }
    }
}

std::pair<view, std::shared_ptr<channel_data>> gdk::graphics::ext::make_from_png(
    std::span<const encoded_byte> aPNGData, const texture_data::format aFormat) {
    const auto channels = channels_of(aFormat);

    if (aPNGData.empty()) throw exception("make_from_png: there is nothing to decode");

    check_chunks(aPNGData);

    if (aPNGData.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw exception("make_from_png: " + std::to_string(aPNGData.size()) + " bytes is past what can be decoded");

    const auto *const pBytes = reinterpret_cast<const stbi_uc *>(aPNGData.data());
    const auto length = static_cast<int>(aPNGData.size());

    int width = 0, height = 0, given = 0;

    if (!stbi_info_from_memory(pBytes, length, &width, &height, &given))
        throw exception(std::string("make_from_png: not a png: ") + stbi_failure_reason());

    if (static_cast<std::size_t>(width) > MOST_PNG_EDGE || static_cast<std::size_t>(height) > MOST_PNG_EDGE)
        throw exception("make_from_png: a png " + std::to_string(width) + " by " + std::to_string(height)
            + " is past the " + std::to_string(MOST_PNG_EDGE) + " a side this decodes");

    const std::unique_ptr<stbi_uc, void (*)(void *)> decoded(
        stbi_load_from_memory(pBytes, length, &width, &height, &given, channels), stbi_image_free);

    if (!decoded) throw exception(std::string("make_from_png: failed to decode: ") + stbi_failure_reason());

    const auto size = static_cast<std::size_t>(width) * static_cast<std::size_t>(height)
        * static_cast<std::size_t>(channels);

    auto pChannels = std::make_shared<channel_data>(decoded.get(), decoded.get() + size);

    const view out{
        .width = static_cast<std::size_t>(width),
        .height = static_cast<std::size_t>(height),
        .format = aFormat,
        .data = pChannels->data()};

    return {out, std::move(pChannels)};
}
