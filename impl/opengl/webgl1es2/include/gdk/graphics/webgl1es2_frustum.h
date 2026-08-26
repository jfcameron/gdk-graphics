// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_FRUSTUM_H
#define GDK_GFX_WEBGL1ES2_FRUSTUM_H

#include <gdk/graphics/types.h>

#include <array>
#include <cmath>

namespace gdk::graphics {
    /// \brief the six planes of a camera's view volume, and a test for whether a sphere is outside
    class frustum final {
    public:
        /// \param aViewProjection projection * view, the same product an entity's mvp is built from
        explicit frustum(const matrix4x4_type &aViewProjection) {
            const auto row = [&aViewProjection](const std::size_t aRow, const std::size_t aColumn) {
                return aViewProjection.get(aColumn, aRow);
            };

            // Three axes, six planes: row 3 plus row i, then row 3 minus row i.
            for (std::size_t i = 0; i < 3; ++i) {
                m_Planes[i * 2 + 0] = plane{
                    row(3, 0) + row(i, 0), row(3, 1) + row(i, 1),
                    row(3, 2) + row(i, 2), row(3, 3) + row(i, 3)};

                m_Planes[i * 2 + 1] = plane{
                    row(3, 0) - row(i, 0), row(3, 1) - row(i, 1),
                    row(3, 2) - row(i, 2), row(3, 3) - row(i, 3)};
            }

            // Normalised so that a plane's distance function returns world units, which is what makes
            // comparing against a radius meaningful.
            for (auto &p : m_Planes) {
                const auto length = std::sqrt(p.a * p.a + p.b * p.b + p.c * p.c);

                if (length <= 0) continue;

                p.a /= length; p.b /= length; p.c /= length; p.d /= length;
            }
        }

        /// \brief whether a world-space sphere is entirely outside the view volume
        [[nodiscard]] bool excludes(const vector3_type &aCentre,
            const floating_point_type aRadius) const {
            for (const auto &p : m_Planes)
                if (p.a * aCentre.x + p.b * aCentre.y + p.c * aCentre.z + p.d < -aRadius) return true;

            return false;
        }

    private:
        struct plane final {
            floating_point_type a, b, c, d;
        };

        std::array<plane, 6> m_Planes{};
    };
}

#endif
