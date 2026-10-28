// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/png.h>

#include <cstddef>
#include <cstdint>
#include <span>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *aData, const std::size_t aSize) {
    try {
        const auto [view, pData] = gdk::graphics::ext::make_from_png(std::span<const unsigned char>(aData, aSize));

        static volatile std::size_t sink = 0;

        sink = view.width * view.height + pData->size();
    }
    catch (const gdk::graphics::exception &) {}

    return 0;
}
