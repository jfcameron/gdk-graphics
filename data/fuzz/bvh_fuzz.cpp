// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/animation/bvh.h>

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *aData, const std::size_t aSize) {
    try {
        const auto content = gdk::graphics::animation::read_bvh(
            std::string_view(reinterpret_cast<const char *>(aData), aSize));

        std::ostringstream written;

        gdk::graphics::animation::write_bvh(written, content.rig, content.animation, 2, content.frame_time);

        static volatile std::size_t sink = 0;

        sink = written.str().size();
    }
    catch (const gdk::graphics::exception &) {}

    return 0;
}
