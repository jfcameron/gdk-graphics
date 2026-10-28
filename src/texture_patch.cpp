// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>
#include <gdk/graphics/texture_patch.h>

#include <string>

void gdk::graphics::check_patch(const texture_data::view &aPatch, const std::size_t aOffsetX,
    const std::size_t aOffsetY, const std::size_t aWidth, const std::size_t aHeight,
    const std::optional<texture_data::format> aFormat) {
    if (!aFormat || aPatch.format != *aFormat)
        throw exception("a texture patch must be in the texture's own format");

    if (aOffsetX > aWidth || aPatch.width > aWidth - aOffsetX || aOffsetY > aHeight || aPatch.height > aHeight - aOffsetY)
        throw exception("a texture patch " + std::to_string(aPatch.width) + " by " + std::to_string(aPatch.height)
            + " at " + std::to_string(aOffsetX) + ", " + std::to_string(aOffsetY) + " is not wholly within a texture "
            + std::to_string(aWidth) + " by " + std::to_string(aHeight));
}
