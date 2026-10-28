// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_PROFILE_H
#define GDK_GRAPHICS_PROFILE_H

#include <gdk/graphics/limits.h>

#include <cstddef>
#include <string>
#include <string_view>

namespace gdk::graphics {
    class model_data;

    /// \brief a target's limits
    ///
    /// What any graphics API has: a largest texture, sampler and attribute counts, and the checks below.
    struct profile final {
        //! said in every refusal: `webgl1_minimum`, `adreno_330`
        std::string name;

        //! the most of each property this target takes
        graphics::limits most;

        [[nodiscard]] bool operator==(const profile &) const = default;
    };

    /// \name The checks a context makes, for anything else that wants them
    ///@{
    //
        /// \brief a texture aWidth by aHeight
        /// \throws gdk::graphics::exception past the profile's largest texture, or not a power of two
        /// where the profile takes only those
        void check_texture(const profile &aProfile, std::size_t aWidth, std::size_t aHeight);

        /// \brief the same, for a texture that is clamped on both axes and never mipmapped if
        /// aClampedAndUnmipmapped: ES 2.0 takes any size for one of those, so the profile does too
        void check_texture(const profile &aProfile, std::size_t aWidth, std::size_t aHeight,
            bool aClampedAndUnmipmapped);

        //! \throws gdk::graphics::exception for a model with more attributes than the profile's vertex attributes
        void check_model(const profile &aProfile, const model_data &aModel);

        [[nodiscard]] std::string past_limit(const profile &aProfile, std::string_view aWhat, std::size_t aHas,
            std::string_view aLimit, std::size_t aMost);
    ///@}
}

#endif
