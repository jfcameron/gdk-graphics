// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_LIMITS_H
#define GDK_GFX_LIMITS_H

#include <cstddef>
#include <limits>

namespace gdk::graphics {
    /// \brief used to (optionally) enforce limits on graphical types used across graphics apis
    ///
    struct limits final {
        //! no limit at all
        static constexpr std::size_t UNRESTRICTED = std::numeric_limits<std::size_t>::max();

        //! the widest and tallest a texture may be, in texels (`GL_MAX_TEXTURE_SIZE`)
        std::size_t texture_size = UNRESTRICTED;

        /// \brief whether a texture may be other than a power of two wide and tall with any filter
        /// and wrapping
        ///
        /// ES 2.0 and WebGL 1 take one only unmipmapped and clamped on both axes, which a
        /// webgl1es2_texture made so is allowed whatever this says; anything repeating or mipmapped
        /// must be a power of two there.
        bool non_power_of_two_textures = true;

        //! uniform vectors a vertex shader may declare (`GL_MAX_VERTEX_UNIFORM_VECTORS`)
        std::size_t vertex_uniform_vectors = UNRESTRICTED;

        //! uniform vectors a fragment shader may declare (`GL_MAX_FRAGMENT_UNIFORM_VECTORS`)
        std::size_t fragment_uniform_vectors = UNRESTRICTED;

        //! samplers a fragment shader may declare (`GL_MAX_TEXTURE_IMAGE_UNITS`)
        std::size_t texture_units = UNRESTRICTED;

        //! samplers a vertex shader may declare (`GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS`)
        std::size_t vertex_texture_units = UNRESTRICTED;

        //! attribute vectors a vertex shader, or a model, may have (`GL_MAX_VERTEX_ATTRIBS`)
        std::size_t vertex_attributes = UNRESTRICTED;

        //! varying vectors between the stages (`GL_MAX_VARYING_VECTORS`)
        std::size_t varying_vectors = UNRESTRICTED;

        [[nodiscard]] bool operator==(const limits &) const = default;
    };

    //! **the least of two limits each**: what fits both
    [[nodiscard]] constexpr limits narrowest(const limits &aA, const limits &aB) {
        const auto least = [](const std::size_t aX, const std::size_t aY) { return aX < aY ? aX : aY; };

        limits out;
        out.texture_size = least(aA.texture_size, aB.texture_size);
        out.non_power_of_two_textures = aA.non_power_of_two_textures && aB.non_power_of_two_textures;
        out.vertex_uniform_vectors = least(aA.vertex_uniform_vectors, aB.vertex_uniform_vectors);
        out.fragment_uniform_vectors = least(aA.fragment_uniform_vectors, aB.fragment_uniform_vectors);
        out.texture_units = least(aA.texture_units, aB.texture_units);
        out.vertex_texture_units = least(aA.vertex_texture_units, aB.vertex_texture_units);
        out.vertex_attributes = least(aA.vertex_attributes, aB.vertex_attributes);
        out.varying_vectors = least(aA.varying_vectors, aB.varying_vectors);

        return out;
    }
}

#endif
