// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/ext/aseprite.h>

#include <cstddef>
#include <cstdint>
#include <span>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *aData, const std::size_t aSize) {
    try {
        const auto sheet = gdk::graphics::ext::read_aseprite(
            std::span(reinterpret_cast<const std::byte *>(aData), aSize));

        std::size_t seen = sheet.image.size() + sheet.frames.size();

        for (const auto &tag : sheet.tags) {
            seen += tag.name.size();

            seen += gdk::graphics::ext::frames_of(sheet, tag).size();
        }

        for (const auto &slice : sheet.slices) seen += slice.name.size() + (slice.pivot ? 1 : 0);

        seen += gdk::graphics::ext::frames_of(sheet).size();

        static volatile std::size_t sink = 0;

        sink = seen;
    }
    catch (const std::exception &) {}

    return 0;
}
