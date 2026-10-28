// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/font.h>

#include <cstddef>
#include <cstdint>
#include <span>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *aData, const std::size_t aSize) {
    try {
        gdk::graphics::ext::font subject(std::span<const unsigned char>(aData, aSize), 16.f,
            {.atlas_edge = 64, .max_atlas_edge = 256});

        for (const char32_t codepoint : {U'A', U'g', U'é', U'日', U'�', U'\U0001F600'})
            static_cast<void>(subject.try_get(codepoint));

        static volatile float sink = 0;

        sink = subject.kerning(U'A', U'V') + subject.line_height();
    }
    catch (const gdk::graphics::exception &) {}

    return 0;
}
