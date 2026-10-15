// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_PROFILE_H
#define GDK_GFX_PROFILE_H

#include <gdk/graphics/limits.h>

#include <cstddef>
#include <string>
#include <string_view>

namespace gdk::graphics {
    class model_data;

    /// \brief a target's limits, for a context to hold what it makes to
    ///
    struct profile final {
        //! said in every refusal: `webgl1_minimum`, `adreno_330`
        std::string name;

        //! the most of each property this target takes
        graphics::limits most;

        [[nodiscard]] bool operator==(const profile &) const = default;
    };

    //! **what every OpenGL ES 2.0 device has**: the spec's guaranteed minimums
    [[nodiscard]] profile es2_minimum();

    //! **what every WebGL 1 browser has**: WebGL 1 guarantees the ES 2.0 minimums, so the same limits
    [[nodiscard]] profile webgl1_minimum();

    /// \name The checks a context makes, for anything else that wants them
    ///@{
    //
        /// \brief a texture aWidth by aHeight
        /// \throws gdk::graphics::exception past the profile's largest texture, or not a power of two
        /// where the profile takes only those
        void check_texture(const profile &aProfile, std::size_t aWidth, std::size_t aHeight);

        /// \brief a shader program, counted from its GLSL's declarations: uniform vectors in each stage,
        /// attributes, varyings, and samplers in each stage
        void check_shader(const profile &aProfile, std::string_view aVertexSource,
            std::string_view aFragmentSource);

        //! \throws gdk::graphics::exception for a model with more attributes than the profile's vertex attributes
        void check_model(const profile &aProfile, const model_data &aModel);
    ///@}
}

#endif
