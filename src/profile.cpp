// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/profile.h>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/model_data.h>

#include <algorithm>
#include <string>
#include <string_view>

using namespace gdk::graphics;

namespace {
    [[nodiscard]] bool is_power_of_two(const std::size_t aValue) {
        return aValue > 0 && (aValue & (aValue - 1)) == 0;
    }
}

void gdk::graphics::check_texture(const profile &aProfile, const std::size_t aWidth, const std::size_t aHeight) {
    check_texture(aProfile, aWidth, aHeight, false);
}

void gdk::graphics::check_texture(const profile &aProfile, const std::size_t aWidth, const std::size_t aHeight,
    const bool aClampedAndUnmipmapped) {
    const auto &most = aProfile.most;

    const auto too_big = [&](const std::size_t aHas, std::string_view aWay) {
        return exception(aProfile.name + ": a texture " + std::to_string(aHas) + " " + std::string(aWay) + ", past "
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

void gdk::graphics::check_model(const profile &aProfile, const model_data &aModel) {
    if (const auto has = aModel.attributes().size(); has > aProfile.most.vertex_attributes)
        throw exception(past_limit(aProfile, "a model's attributes", has, "vertex attributes",
            aProfile.most.vertex_attributes));
}

std::string gdk::graphics::past_limit(const profile &aProfile, const std::string_view aWhat, const std::size_t aHas,
    const std::string_view aLimit, const std::size_t aMost) {
    return aProfile.name + ": " + std::string(aWhat) + " " + std::to_string(aHas) + ", past " + aProfile.name + "'s "
        + std::string(aLimit) + " of " + std::to_string(aMost);
}
